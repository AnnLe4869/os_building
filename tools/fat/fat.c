#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

typedef uint8_t bool;
#define true 1;
#define false 0;

typedef struct 
{
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];

    // ... we don't care about code ...

} __attribute__((packed)) BootSector;

typedef struct 
{
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} __attribute__((packed)) DirectoryEntry;


BootSector g_BootSector;
uint8_t* g_Fat = NULL;
// an array of file/directory
DirectoryEntry* g_RootDirectory = NULL;
uint32_t g_RootDirectoryEnd;

bool readBootSector(FILE* disk){
    return fread(&g_BootSector, sizeof(g_BootSector), 1, disk);
}

/**
 * @brief read a number of sectors to the bufferOut
 * 
 * @param disk 
 * @param lba starting point to read 
 * @param count how many sector to read
 * @param bufferOut 
 * @return true 
 * @return false 
 */
bool readSectors(FILE* disk, uint32_t lba, uint32_t count, void* bufferOut){
    bool ok = true;
    ok = ok && (fseek(disk, lba * g_BootSector.BytesPerSector, SEEK_SET) == 0);
    ok = ok && (fread(bufferOut, g_BootSector.BytesPerSector, count, disk) == count);
    return ok;
}

bool readFat(FILE* disk){
    // there are 2 FAT, but we only read one
    // because the other is copy for redundancy
    g_Fat = (uint8_t*)malloc(g_BootSector.SectorsPerFat * g_BootSector.BytesPerSector);
    return readSectors(disk, g_BootSector.ReservedSectors, g_BootSector.SectorsPerFat, g_Fat);
}

bool readRootDirectory(FILE* disk){
    uint32_t lba = g_BootSector.ReservedSectors + g_BootSector.SectorsPerFat * g_BootSector.FatCount;
    uint32_t size = sizeof(DirectoryEntry) * g_BootSector.DirEntryCount;
    uint32_t sectors = (size / g_BootSector.BytesPerSector);
    if(size % g_BootSector.BytesPerSector > 0){
        // round up
        sectors++;
    }
    g_RootDirectoryEnd = lba + sectors;
    // allocate based on sectors count
    g_RootDirectory = (DirectoryEntry*)malloc(sectors * g_BootSector.BytesPerSector);

    return readSectors(disk, lba, sectors, g_RootDirectory);
}

DirectoryEntry* findFile(const char* name){
    for(uint32_t i = 0; i < g_BootSector.DirEntryCount; i++){
        if(memcmp(name, g_RootDirectory[i].Name, 11) == 0){
            return &g_RootDirectory[i];
        }

    }
    return NULL;
}

/**
 * @brief
 * 
 * @param fileEntry 
 * @param disk 
 * @param outputBuf 
 * @return true 
 * @return false 
 */
bool readFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* outputBuf){
    uint16_t currentCluster = fileEntry->FirstClusterLow;
    bool ok = true;

    do
    {
        // why subtract 2 here?
        uint32_t lba = g_RootDirectoryEnd + (currentCluster - 2)*g_BootSector.SectorsPerCluster;
        ok = ok && readSectors(disk, lba, g_BootSector.SectorsPerCluster, outputBuf);
        outputBuf += g_BootSector.SectorsPerCluster * g_BootSector.BytesPerSector;

        // look up the next cluster
        // cluster is just an index in the FAT
        uint32_t fatIndex = currentCluster * 3 / 2; // table index
        if (currentCluster % 2 == 0)
            // remove top bits
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) & 0x0FFF;
        else
            // get the upper bits
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) >> 4;
    } while (ok && currentCluster < 0x0FF8); // when no more chain

    return ok;
}


int main(int argc, char** argv){
    if(argc < 3){
        printf("Syntax: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");

    if(!disk){
        fprintf(stderr, "Cannot open disk image %s!", argv[1]);
        return -1;
    }

    if(!readBootSector(disk)){
        fprintf(stderr, "Failed to read boot sector\n");
        return -2;
    }

    if(!readFat(disk)){
        fprintf(stderr, "Could not read from FAT \n");
        free(g_Fat);
        return -3;
    }

    if(!readRootDirectory(disk)){
        fprintf(stderr, "Could not read from FAT and directory \n");
        free(g_Fat);
        free(g_RootDirectory);
        return -4;
    }


    DirectoryEntry* fileEntry = findFile(argv[2]);
    if(!fileEntry){
        fprintf(stderr, "Could not find file %s \n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        return -5;
    }

    uint8_t* buffer = (uint8_t*) malloc(fileEntry->Size * g_BootSector.BytesPerSector);
    if (!readFile(fileEntry, disk, buffer)) {
        fprintf(stderr, "Could not read file %s!\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        free(buffer);
        return -5;
    }

    for (size_t i = 0; i < fileEntry->Size; i++)
    {
        if (isprint(buffer[i])) fputc(buffer[i], stdout);
        else printf("<%02x>", buffer[i]);
    }
    printf("\n");


    free(buffer);
    free(g_Fat);
    free(g_RootDirectory);


    return 0;
}
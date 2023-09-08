#pragma once
#include "stdint.h"
#include "disk.h"

#pragma pack(push, 1)

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
} FAT_DirectoryEntry;

#pragma pack(pop)

typedef struct 
{
    int Handle;         // where it's located in the stored table OpenedFiles
    bool IsDirectory;
    uint32_t Position; // which sector it is in
    uint32_t Size;
} FAT_File;

enum FAT_Attributes
{
    FAT_ATTRIBUTE_READ_ONLY         = 0x01,
    FAT_ATTRIBUTE_HIDDEN            = 0x02,
    FAT_ATTRIBUTE_SYSTEM            = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID         = 0x08,
    FAT_ATTRIBUTE_DIRECTORY         = 0x10,
    FAT_ATTRIBUTE_ARCHIVE           = 0x20,
    FAT_ATTRIBUTE_LFN               = FAT_ATTRIBUTE_READ_ONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

/**
 * @brief read the boot sector, the FAT section and the root directory
 * 
 * @param disk 
 * @return true 
 * @return false 
 */
bool FAT_Initialize(DISK* disk);

/**
 * @brief open a file and read the detail of the file - not the content.
 * the path is the complete path, and we need to split the path into components
 * 
 * @param disk 
 * @param path 
 * @return FAT_File* 
 */
FAT_File far* FAT_Open(DISK* disk, const char* path);


/**
 * @brief read the content of a FAT_File 
 * and write the content to the buffer
 * 
 * @param disk 
 * @param file 
 * @param byteCount 
 * @param dataOut pointer to where to write the content to
 * @return uint32_t how many bytes we actually read from the FAT_File
 */
uint32_t FAT_Read(DISK* disk, FAT_File far* file, uint32_t byteCount, void* dataOut);


/**
 * @brief read an directory entry and fill out the directory structure
 * 
 * @param disk 
 * @param file 
 * @param dirEntry pointer to where to fill out the directory structure
 * @return true 
 * @return false 
 */
bool FAT_ReadEntry(DISK* disk, FAT_File far* file, FAT_DirectoryEntry* dirEntry);



/**
 * @brief release all resource associated with the file
 * 
 * @param file 
 */
void FAT_Close(FAT_File far* file);
#pragma once
#pragma pack (push, 1)

#include <cstdint>
#include <string>
#include "entries.hpp"

class BootRecord {
    private:
        //BIOS Parameter Block
        uint8_t bootJump[3];
        uint8_t oemID[8];
        uint16_t  bytesPerSector;
        uint8_t sectorsPerCluster;
        uint16_t reservedSectors;
        uint8_t FATCount;
        uint16_t rootDirEntries;
        uint16_t totalSectors;
        uint8_t mediaDescriptorType;
        uint16_t sectorsPerFAT;
        uint16_t sectorsPerTrack;
        uint16_t headCount;
        uint32_t hiddenSectorCount;
        uint32_t largeSectorCount;

        //Extended Boot Record
        uint8_t driveNumber;
        uint8_t flags;
        uint8_t signature;
        uint32_t volumeID;
        uint8_t volumeLabel[11];
        uint8_t systemID[8];
        uint8_t bootCode[448];
        uint16_t bootPartSignature;

    public:
        std::string getOEMID();

        uint16_t getBytesPerSector();

        uint8_t getSectorsPerCluster();

        uint16_t getSectorsPerTrack();

        uint16_t getHeadCount();

        std::string getSystemID();

        uint16_t getBootPartSignature();
        
        uint16_t getReservedSectors();

        uint32_t getTotalSectors();

        uint16_t getSectorsPerFAT();

        uint8_t getFATCount();

        uint16_t getRootDirEntries();

        uint16_t getRootDirSize();

        uint16_t getFirstDataSector();

        uint16_t getFirstFATSector();

        uint32_t getTotalDataSectors();

        uint16_t getFirstRootDirSector();

        uint16_t test();
};
#pragma pack (pop)

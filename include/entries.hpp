#pragma once
#include <cwchar>
#pragma pack (push, 1)

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

class DirEntry {
    private:
        uint8_t fileName[11];
        uint8_t attrs;
        uint8_t reservedWNT;
        uint8_t creationTimeSecs;
        uint16_t creationTime;
        uint16_t creationDate;
        uint16_t lastAccessedDate;
        uint16_t h16bFirstCluster; //Always zero in FAT12/16
        uint16_t lastModificationTime;
        uint16_t lastModificationDate;
        uint16_t l16bFirstCluster;
        uint32_t fileSizeB;

    public:
        uint8_t* getFileName();

        std::string getFileNameString();

        uint8_t getAttrs();

        uint16_t getCreationTime();

        std::string getCreationTimeStr();

        uint16_t getCreationDate();
        
        std::string getCreationDateStr();

        std::string getLastAccessedDate();

        uint16_t getL16BFirstCluster();

        uint32_t getFileSizeB();
};

class LongFNDirEntry {
    private:
        uint8_t entryOrder;
        uint16_t firstChars[5];
        uint8_t attribute;
        uint8_t longEntryType;
        uint8_t checksum;
        uint16_t intermChars[6];
        uint16_t alwaysZero;
        uint16_t finalChars[2];
    public:
        std::wstring getFileName();
};
#pragma pack (pop)

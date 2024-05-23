#pragma once
#pragma pack (push, 1)

#include <cstdint>

class PartitionTable {
    private:
        uint8_t driveAttrs;
        uint8_t partStartCHS[3];
        uint8_t partitionType;
        uint8_t lastSector[3];
        uint32_t partStartLBA;
        uint32_t noSectors;

    public:
        uint8_t* getPartStartCHS();
        uint8_t getPartitionType();
        uint32_t getNoSectors();
        uint32_t getPartStartLBA();
};
#pragma pack (pop)

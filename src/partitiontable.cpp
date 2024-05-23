#include "partitiontable.hpp"

uint8_t* PartitionTable::getPartStartCHS() {
    return partStartCHS;
}

uint8_t PartitionTable::getPartitionType() {
    return partitionType;
}

uint32_t PartitionTable::getNoSectors() {
    return noSectors;
}

uint32_t PartitionTable::getPartStartLBA() {
    return partStartLBA;
}

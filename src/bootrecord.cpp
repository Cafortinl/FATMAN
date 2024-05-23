#include "bootrecord.hpp"

std::string BootRecord::getOEMID() {
    return std::string(reinterpret_cast<char*>(oemID));
}

uint16_t BootRecord::getBytesPerSector() {
    return bytesPerSector;
}

uint8_t BootRecord::getSectorsPerCluster() {
    return sectorsPerCluster;
}

uint16_t BootRecord::getSectorsPerTrack() {
    return sectorsPerTrack;
}

uint16_t BootRecord::getHeadCount() {
    return headCount;
}

std::string BootRecord::getSystemID() {
    return std::string(reinterpret_cast<char*>(systemID));
}

uint16_t BootRecord::getBootPartSignature() {
    return bootPartSignature;
}

uint16_t BootRecord::getReservedSectors() {
    return reservedSectors;
}

uint32_t BootRecord::getTotalSectors() {
    return totalSectors == 0 ? largeSectorCount : (uint32_t) totalSectors;
}

uint16_t BootRecord::getSectorsPerFAT() {
    return sectorsPerFAT;
}

uint8_t BootRecord::getFATCount() {
    return FATCount;
}

uint16_t BootRecord::getRootDirEntries() {
    return rootDirEntries;
}

uint16_t BootRecord::getRootDirSize() {
    return ((rootDirEntries * sizeof(DirEntry)) + (bytesPerSector - 1)) / bytesPerSector;
}

uint16_t BootRecord::getFirstDataSector() {
    return reservedSectors + (FATCount * getSectorsPerFAT()) + getRootDirSize();
}

uint16_t BootRecord::getFirstFATSector() {
    return reservedSectors;
}

uint32_t BootRecord::getTotalDataSectors() {
    return getTotalSectors() - getFirstDataSector();
}

uint16_t BootRecord::getFirstRootDirSector() {
    return getFirstDataSector() - getRootDirSize();
}

uint16_t BootRecord::test() {
    return (reservedSectors - 1 + sectorsPerFAT * FATCount) * bytesPerSector;
}

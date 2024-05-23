#include "entries.hpp"

uint8_t* DirEntry::getFileName() {
    return fileName;
}

std::string DirEntry::getFileNameString() {
    return std::string(reinterpret_cast<char*>(&fileName));
}

uint8_t DirEntry::getAttrs() {
    /*
     *  
     */
    return attrs;
}

uint16_t DirEntry::getCreationTime() {
    return creationTime;
}

std::string DirEntry::getCreationTimeStr() {
    /*
     *  Hour: 5 bits
     *  Minutes: 6 bits
     *  Seconds: 5 bits
     */
    int hour, minute, second;
    hour = creationTime >> 11;
    minute = (creationTime >> 5) & 0x3f;
    second = 2 * (creationTime & 0x1f);
    
    char timeCStr[9];
    std::snprintf(timeCStr, 9,"%.2i:%.2i:%.2i", hour, minute, second);

    return std::string(timeCStr); 
}

uint16_t DirEntry::getCreationDate() {
    return creationDate;
}

std::string DirEntry::getCreationDateStr() {
    /*
     *  Year: 7 bits offset by 1980
     *  Month: 4 bits
     *  Day: 5 bits
     */
    int year, month, day;
    year = 1980 + (creationDate >> 9);
    month = (creationDate >> 5) & 0xf;
    day = creationDate & 0x1f;
    
    char dateCStr[13];
    std::snprintf(dateCStr, 13,"%.4i-%.2i-%.2i", year, month, day);

    return std::string(dateCStr); 
}

std::string DirEntry::getLastAccessedDate() {
    /*
     *  Year: 7 bits offset by 1980
     *  Month: 4 bits
     *  Day: 5 bits
     */
    int year, month, day;
    year = 1980 + (lastAccessedDate >> 9);
    month = (lastAccessedDate >> 5) & 0xf;
    day = lastAccessedDate & 0x1f;
    
    char dateCStr[13];
    std::snprintf(dateCStr, 13,"%.4i-%.2i-%.2i", year, month, day);

    return std::string(dateCStr); 
}

uint16_t DirEntry::getL16BFirstCluster() {
    return l16bFirstCluster;
}

uint32_t DirEntry::getFileSizeB() {
    return fileSizeB;
}

std::wstring LongFNDirEntry::getFileName() {
    std::wstring filename = L"";
    for (size_t i = 0; i < 5; i++) {
        filename += (char16_t)firstChars[i];
    }

    for (size_t i = 0; i < 6; i++) {
        filename += (char16_t)intermChars[i];
    }

    for (size_t i = 0; i < 2; i++) {
        filename += (char16_t)finalChars[i];
    }
    return filename;
}

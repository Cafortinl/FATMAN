/*
 *  author: cafortinl
 *  Info about the FAT file system found in:
 *      - https://wiki.osdev.org/FAT
 *      - https://www.compuphase.com/mbr_fat.htm
 *      - https://codeandlife.com/2012/04/02/simple-fat-and-sd-tutorial-part-1/
 *      - http://www.tavi.co.uk/phobos/fat.html
 */

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "bootrecord.hpp"
#include "entries.hpp"
#include "partitiontable.hpp"

#define SECTORSIZE 512

std::pair<std::string, int> currentDir;

struct offsets {
    int bootRecord;
    int FATStart;
    int rootDir;
    int dataStart;
} Offsets;

int clusterSize;
int rootDirEntries;

int findEmptyDirEntry(std::ifstream& df, int offset) {
    df.seekg(offset);
    while (df.peek() != 0x00 && df.peek() != EOF) {
        df.seekg((int) df.tellg() + sizeof(DirEntry));
    }
    return df.peek() != EOF ? (int) df.tellg() : -1;
}

int findEmptyFATEntry(std::ifstream& df) {
    df.seekg(Offsets.FATStart);
    uint8_t entry[2];
    int entryIdx = 0;
    do {
        df.read(reinterpret_cast<char*>(entry), 2);
        entryIdx++;
    } while (entry[0] != 0x00 || entry[1] != 0x00);

    return entryIdx - 1;
}

void readFile(std::ifstream& df, int cluster, int fileSize) {
    uint8_t clusterData[clusterSize];
    int bytesRead = 0;
    int rwSize;

    do {
        rwSize = clusterSize > (fileSize - bytesRead) ? (fileSize - bytesRead) : clusterSize;

        df.seekg(Offsets.dataStart + (cluster - 2) * clusterSize);
        df.read(reinterpret_cast<char*>(clusterData), rwSize);

        bytesRead += clusterSize;
        std::printf("%.*s", rwSize, reinterpret_cast<char*>(clusterData));
        
        df.seekg(Offsets.FATStart + (2 * cluster));
        df.read(reinterpret_cast<char*>(&cluster), 2);
    } while (bytesRead < fileSize && cluster != 0xffff);
}

void readEntry(std::ifstream& df, std::vector<DirEntry>& dirs) {
    int currOffset = (int) df.tellg();

    if (df.peek() != 0x00 && df.peek() != 0xe5) {
        df.seekg(currOffset + 11);
        uint8_t attr = df.peek();
        df.seekg(currOffset);

        if (attr == 0x0f) {
            /*
             *  TODO: Implement LongFileNameEntries
             */
            df.seekg(currOffset + sizeof(LongFNDirEntry));
        } else {
            DirEntry de;
            df.read(reinterpret_cast<char*>(&de), sizeof(DirEntry));
            dirs.push_back(de);
        }
    }
}

std::vector<DirEntry> readDirEntries(std::ifstream& df, int offset) {
    df.seekg(offset);

    std::vector<DirEntry> dirs;
    if (offset == Offsets.rootDir) {
        for (size_t i = 0; i < rootDirEntries; i++) {
            readEntry(df, dirs);
        }
    } else {
        while (df.peek() != 0x00) {
            readEntry(df, dirs);
        }
    }

    return dirs;
}

std::pair<int, int> findFile(std::ifstream& df, int curDirOffset, std::string fileName) {
    std::string fn;
    std::string fe;

    int pos = fileName.find_first_of(".");
    if (pos != std::string::npos && pos != 0) {
        int idx = fileName.find_first_of(".");
        fn = fileName.substr(0, idx);
        fe = fileName.substr(idx + 1);

        int origFNLn = fn.length();
        int origFELn = fe.length();

        for (int i = 0; i < 8 - origFNLn; i++) {
            fn += " ";
        }
        for (int i = 0; i < 3 - origFELn; i++) {
            fe += " ";
        }
        fileName = fn + fe + " ";
    } else {
        int size = fileName.size();
        for (int i = 0; i < 11 - size; i++) {
            fileName += " ";
        }
        fileName += (char) 16;
    }

    df.seekg(curDirOffset);
    std::vector<DirEntry> dirs = readDirEntries(df, curDirOffset);

    for (auto i = dirs.begin(); i != dirs.end(); i++) {
        if (i->getFileNameString() == fileName) {
            return std::make_pair(i->getL16BFirstCluster(), i->getFileSizeB());
        }
    }
    return std::make_pair(-1, -1);
}

bool initializeFS(std::ifstream& df, BootRecord& br) {
    //Offsetting df to partition table start
    df.seekg(0x1be);
    
    PartitionTable pt;
    for (int i = 0; i < 4; i++) {
        df.read(reinterpret_cast<char*>(&pt), sizeof(PartitionTable));
        if (pt.getPartStartLBA() != 0x00) break;
    }

    //Offsetting df to FAT Boot Record.
    Offsets.bootRecord = SECTORSIZE * pt.getPartStartLBA();
    df.seekg(Offsets.bootRecord);

    df.read(reinterpret_cast<char*>(&br), sizeof(BootRecord));

    clusterSize = br.getSectorsPerCluster() * br.getBytesPerSector();
    
    Offsets.FATStart = Offsets.bootRecord + (br.getReservedSectors() * br.getBytesPerSector());

    Offsets.rootDir = Offsets.FATStart
        + (br.getFATCount() * br.getSectorsPerFAT() * br.getBytesPerSector());

    Offsets.dataStart = Offsets.rootDir + (br.getRootDirSize() * br.getBytesPerSector());

    currentDir = std::make_pair("/", Offsets.rootDir);

    return Offsets.dataStart != 0 && Offsets.rootDir != 0 && Offsets.FATStart != 0 && Offsets.bootRecord != 0;
}

std::vector<std::string> split(std::string str, std::string delim) {
    std::vector<std::string> tokens;

    while (str.find(delim) != std::string::npos) {
        tokens.push_back(str.substr(0, str.find(delim)));

        if (str.find(delim) != std::string::npos)
            str = str.substr(str.find(delim, 0) + 1, str.length());
    }
    tokens.push_back(str);

    return tokens;
}

int exec(std::string input, std::ifstream& df) {
    std::vector<std::string> tokens = split(input, " ");

    if (tokens[0] == "echo") {
        for (size_t i  = 1; i < tokens.size(); i++) {
            std::cout << tokens[i] <<
                (i == tokens.size() - 1 ? "" : " ");
        }
    } else if (tokens[0] == "exit" && tokens.size() == 1) {
        exit(0);
    } else if (tokens[0] == "ls" && tokens.size() < 4) {
        std::vector<DirEntry> dirs = readDirEntries(
            df,
            currentDir.second
        );

        for(auto i = dirs.begin(); i != dirs.end(); i++) {
            std::cout << i->getFileName() << '\n';

            if (tokens.size() == 2 && tokens[1].find("l") != std::string::npos) {
                std::cout << "\tAttributes: " << std::hex << (int) i->getAttrs() << std::dec << '\n';
                std::cout << "\tCreated on: " << i->getCreationDateStr() 
                    << " at: " << i->getCreationTimeStr() << '\n';
                std::cout << "\tLast accessed: " << i->getLastAccessedDate() << '\n';
                std::cout << "\tStarting cluster: " << i->getL16BFirstCluster() << '\n';
                std::cout << "\tSize: " << i->getFileSizeB() << "B\n";
            }
        }
    } else if (tokens[0] == "cd" && tokens.size() < 3) {
        std::pair<int, int> cluster = findFile(df, currentDir.second, tokens[1]);
        if (cluster.first != -1) {
            int offset = cluster.first == 0 ? Offsets.rootDir : Offsets.dataStart + (cluster.first - 2) * clusterSize;
            currentDir = std::make_pair(offset == Offsets.rootDir ? "/" : tokens[1], offset);
        }
    } else if (tokens[0] == "cat" && tokens.size() < 4) {
        if (tokens[1] != ">") {
            std::pair<int, int> cluster = findFile(df, currentDir.second, tokens[1]);

            if (cluster.first != -1)
                readFile(df, cluster.first, cluster.second);
        } else {
            std::cout << "Not implemented yet.\n";
        }
    } else if (tokens[0] == "mkdir" && tokens.size() < 3) {
        //int off = findEmptyDirEntry(df, currentDir.second);
        //std::cout << "Res: " << std::hex << off << std::dec << '\n';
        //std::cout << "Free FAT: " << findEmptyFATEntry(df) << '\n';
        std::cout << "Not implemented yet.\n";
    } else {
        std::cout << "FATMAN - command not found: " << tokens[0] << '\n';
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << (argc > 2 ? "Too many arguments!\n" : "Not enough arguments!\n");
        std::cout << "Usage: " << argv[0] << " path_to_disk_image\n";
        exit(1);
    }

    std::ifstream df(argv[1], std::ios::binary);

    if (!df.is_open()) {
        std::cout << "Failed to open df: " << argv[1] << '\n';
        exit(2);
    }

    BootRecord br;
    if (!initializeFS(df, br)) {
        std::cout << "Error: could not find a valid patition in " << argv[1] << '\n';
        exit(3);
    }
    rootDirEntries = br.getRootDirEntries();
    
    bool running = true;

    std::string command;
    while (running) {
        std::cout << '[' << currentDir.first << "] $ ";
        std::getline(std::cin, command);
        exec(command, df);
        std::cout << '\n';
    }

    //readDirEntries(df, currentDir.second, br.getRootDirEntries());
    //readFile(df, 14, 353);
    df.close();
    return 0;
}

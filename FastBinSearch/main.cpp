#include "FastBinSearch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <chrono>

void* ReadFileMapping()
{
    std::string filename = R"(C:\Work.7z)";
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        return 0;
    }
    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    void* pData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);

    return pData;
}

int main() {

    std::string filename = R"(C:\Work.7z)";
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        return 0;
    }
    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t filesize = file.tellg();
    file.seekg(0, std::ios::beg);

    HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    void* pData = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);


    char data2[16] = { 0x12,0x34,0x55,0x66,0x67,0x68,0x99,0x12,0x99,0x99,0x66,0x67,0xA9,0x99,0xA1,0x87 };
    auto start = std::chrono::high_resolution_clock::now();
    FastSearchPattern sig1("BC56958DF147E2AB83409BFDDCA9E1536D4DF931??C591DEB7F5D226B305210135EF4E8174DEEBB95B165FAF628FD000??D3AB2C5C65BA5CD6244BDEB426382C7B4F149334EAABB3F0822C953609AA6E0FCB7EB1EA873009ED6534F7DB37C1F51A066B63A11769EE9E370120A6C6??942FED612AEC7C05DB39978A1CDB5FBFBCE1F60577F24DCE65CAA836C9A9C4C024018FA0F3306B33CD12A9C648216D??B2");
    auto searchResult = sig1.searchAll(pData, filesize);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;


    for (unsigned int n = 0; n < searchResult.size(); ++n) {
        std::cout << "offset:0x" << std::hex << searchResult[n] << std::endl;
    }

    int a = 0;
}
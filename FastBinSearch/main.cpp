#include "FastBinSearch.h"
#include <string>
#include <iostream>

int main() {

    //FILE* hFile;
    //fopen_s(&hFile,R"(C:\Work\≤‚ ‘.data)", "rb");
    //fseek(hFile, 0, SEEK_END);
    //long fileSize = ftell(hFile);
    //unsigned char* pBuffer = new unsigned char[fileSize];
    //fseek(hFile, 0, SEEK_SET);
    //fread(pBuffer, fileSize, 1, hFile);

    char data2[16] = { 0x12,0x34,0x55,0x66,0x67,0x68,0x99,0x12,0x99,0x99,0x66,0x67,0xA9,0x99,0xA1,0x87 };
    FastSearchPattern sig1("6667??99");
    auto searchResult = sig1.searchAll(data2, sizeof(data2));
    for (unsigned int n = 0; n < searchResult.size(); ++n) {
        std::cout << "offset:0x" << std::hex << searchResult[n] << std::endl;
    }

    int a = 0;
}
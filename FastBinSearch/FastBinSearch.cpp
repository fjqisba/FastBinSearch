#include "FastBinSearch.h"
#include <vector>
#include <immintrin.h> 

unsigned char gBinMap[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,		//123456789
		0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//ABCDEF
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 10, 11, 12, 13, 14, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0,	//abcdef
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

inline unsigned char strToBin(const char *pBuf)
{
	unsigned char* pData = (unsigned char*)pBuf;
	return (gBinMap[pData[0]] << 4) + gBinMap[pData[1]];
}

FastSearchPattern::FastSearchPattern(std::string pattern)
{
	const char* pStrPattern = pattern.c_str();
	int patSize = pattern.size();
	if (patSize < 2) {
		return;
	}
	sigLen = patSize / 2;
	int ruleCount = (sigLen + 14) / 16;
	ruleList.resize(ruleCount);
	int curRuleIdx = 0;

	//算出第一个字节
	firstSig = strToBin(pStrPattern);

	int startIndex = 2;
	const int numBytes = 32;
	while (startIndex + numBytes < patSize) {
		ruleList[curRuleIdx].mask = 0x0;
		ruleList[curRuleIdx].byteLen = 16;
		for (int i = 0; i < 16; ++i) {
			if (pattern[startIndex] == '?') {
				ruleList[curRuleIdx].mask |= (1 << i);
			}
			else {
				ruleList[curRuleIdx].bytes[i] = strToBin(pStrPattern + startIndex);
			}
			startIndex += 2;
		}
		curRuleIdx++;
	}

	//处理最后一块规则
	ruleList[curRuleIdx].mask = 0x0;
	ruleList[curRuleIdx].byteLen = (patSize - startIndex) / 2;
	for (int i = 0; i < numBytes / 2; ++i) {
		if (i < ruleList[curRuleIdx].byteLen) {
			if (pattern[startIndex] == '?') {
				ruleList[curRuleIdx].mask |= (1 << i);
			}
			else {
				ruleList[curRuleIdx].bytes[i] = strToBin(pStrPattern + startIndex);
			}
			startIndex = startIndex + 2;
		}
		else {
			ruleList[curRuleIdx].mask |= (1 << i);
		}
	}
}

FastSearchPattern::~FastSearchPattern()
{
	
}

std::vector<size_t> FastSearchPattern::searchAll(void* pBuffer, size_t bufSize)
{
	std::vector<size_t> retList;
	size_t searchEnd = bufSize - sigLen;
	__m128i sigHead = _mm_set1_epi8(firstSig);
	for (size_t i = 0; i <= searchEnd; i = i + 16) {
		unsigned char* pSearchBuf = (unsigned char*)pBuffer + i;
		__m128i startBuf = _mm_loadu_si128((__m128i*)pSearchBuf);
		__m128i curComp = _mm_cmpeq_epi8(sigHead, startBuf);
		unsigned long comMask = _mm_movemask_epi8(curComp);
		unsigned long idxCom;
		while (_BitScanForward(&idxCom, comMask)) {
			bool bFind = true;
			unsigned char* pComBuf = pSearchBuf + 1 + idxCom;
			for (unsigned int n = 0; n < ruleList.size(); ++n) {
				__m128i compareBuf1 = _mm_loadu_si128((__m128i*)(pComBuf + (n << 4)));
				__m128i compareBuf2 = _mm_loadu_si128((__m128i*)ruleList[n].bytes);
				__m128i xmm3 = _mm_cmpeq_epi8(compareBuf1, compareBuf2);
				unsigned long mask = _mm_movemask_epi8(xmm3);
				if ((mask | ruleList[n].mask) != 0xFFFF) {
					bFind = false;
					break;
				}
			}
			if (bFind) {
				retList.push_back(i + idxCom);
			}
			comMask &= comMask - 1;
		}
	}
	return retList;
}
// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "RDARScriptItemInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include "SecureStdio.h"
#include "UnicodeConvert.h"
#include "DNFFunctionLib.h"

char* NextToken(const char** ppszBuff, char chStringMark, char chDelimiter)
{
    static char s_szBuff[0x400];
    bool bDelSkip = false;
    char* ptr = s_szBuff;
    memset(s_szBuff, 0, 0x400);
    while (**ppszBuff != '\0')
    {
        if (**ppszBuff < '\0')
        {
            *ptr = **ppszBuff;
            ptr = ptr + 1;
            *ppszBuff = *ppszBuff + 1;
            *ptr = **ppszBuff;
            ptr = ptr + 1;
            *ppszBuff = *ppszBuff + 1;
        }
        else
        {
            if (**ppszBuff == chStringMark)
            {
                bDelSkip = (bool)(bDelSkip ^ 1);
            }
            else
            {
                if ((!bDelSkip) && (**ppszBuff == chDelimiter))
                {
                    break;
                }
                *ptr = **ppszBuff;
                ptr = ptr + 1;
            }
            *ppszBuff = *ppszBuff + 1;
        }
    }
    if (**ppszBuff != '\0')
    {
        *ppszBuff = *ppszBuff + 1;
    }
    return s_szBuff;
}

bool findFileWithServerTokenInAuction(TCHAR* fullPath)
{
    int MAX_PATH_ = 0x104;
    std::string fullPathStr = fullPath;
    std::string newFullPathStr;
    int endIndex = (int)fullPathStr.find_last_of("/");
    if (endIndex == -1)
    {
        endIndex = (int)fullPathStr.find_last_of("\\");
    }
    if (endIndex != -1)
    {
        newFullPathStr = fullPathStr.substr(0, endIndex + 1);
        newFullPathStr += "(R)";
        newFullPathStr += fullPathStr.substr(endIndex + 1, fullPathStr.size());
        ss_strcpy(fullPath, 0x104, newFullPathStr.c_str());
        if (access(fullPath, 0) == 0)
        {
            return true;
        }
        // ORIG 二进制实测：第二次 ss_strcpy 用 fullPathStr（还原原始路径），
        // 不是 newFullPathStr。
        ss_strcpy(fullPath, 0x104, fullPathStr.c_str());
    }
    return false;
}

CNRDItemInfoList::CNRDItemInfoList()
{
}

CNRDItemInfoList::~CNRDItemInfoList()
{
    Clear();
}

void CNRDItemInfoList::Clear()
{
    for (ItemInfoMap::iterator it = itemInfoMap_.begin(); it != itemInfoMap_.end(); ++it)
    {
        if (it->second != (STItemInfo*)0)
        {
            delete it->second;
            it->second = (STItemInfo*)0;
        }
    }
    itemInfoMap_.clear();
}

CNRDItemInfoList::STItemInfo* CNRDItemInfoList::GetItemInfo(int nIndex) const
{
    ItemInfoMap::const_iterator it = itemInfoMap_.find(nIndex);
    if (it == itemInfoMap_.end())
    {
        return (STItemInfo*)0;
    }
    return it->second;
}

bool CNRDItemInfoList::Load(const TCHAR* szFilePath, ConvertFunction convertor)
{
    TCHAR newFilePath[260];
    FILE* pFile;
    bool result;
    int fileSize;
    char* pTotalBuffer;
    char* pTotalPtr;
    char* pszStr;
    const char* pszToken;
    int intData;

    result = false;
    if (szFilePath == (TCHAR*)0)
    {
        result = false;
    }
    else
    {
        ss_strcpy(newFilePath, 0x104, szFilePath);
        findFileWithServerTokenInAuction(newFilePath);
        pFile = fopen(newFilePath, "rb");
        if (pFile == (FILE*)0)
        {
            result = false;
        }
        else
        {
            result = true;
            char szLineBuff[0x400] = {};
            fileSize = 0;
            fseek(pFile, 0, SEEK_END);
            fileSize = (int)ftell(pFile);
            fseek(pFile, 0, SEEK_SET);
            pTotalBuffer = new char[fileSize];
            fread(pTotalBuffer, 1, fileSize, pFile);
            pTotalPtr = pTotalBuffer;
            while (true)
            {
                while (true)
                {
                    if (*pTotalPtr == '\0' || fileSize <= pTotalPtr - pTotalBuffer)
                    {
                        goto cleanup;
                    }
                    char* pLineBuffPtr = szLineBuff;
                    for (; *pTotalPtr != '\n'; pTotalPtr = pTotalPtr + 1)
                    {
                        *pLineBuffPtr = *pTotalPtr;
                        pLineBuffPtr = pLineBuffPtr + 1;
                    }
                    pTotalPtr = pTotalPtr + 1;
                    *pLineBuffPtr = '\0';
                    pLineBuffPtr = pLineBuffPtr - 1;
                    if (*pLineBuffPtr == '\r')
                    {
                        *pLineBuffPtr = '\0';
                    }
                    if (strlen(szLineBuff) > 1)
                    {
                        break;
                    }
                    memset(szLineBuff, 0, 0x400);
                }
                pszStr = (char*)0;
                pszToken = szLineBuff;
                STItemInfo* pItemInfo = new CNRDItemInfoList::STItemInfo();
                if (pItemInfo == (STItemInfo*)0)
                {
                    break;
                }
                pszStr = NextToken(&pszToken, '`', ' ');
                pItemInfo->nItemIndex_ = atoi(pszStr);
                pszStr = NextToken(&pszToken, '`', ' ');
                pItemInfo->rarity_ = (ENUM_RARITY)atoi(pszStr);
                for (int i = 0; i < 0xb; i = i + 1)
                {
                    pszStr = NextToken(&pszToken, '`', ' ');
                    intData = atoi(pszStr);
                    pItemInfo->xaUsableCharacter_[i] = intData != 0;
                }
                pszStr = NextToken(&pszToken, '`', ' ');
                pItemInfo->nOriginalUsableLevel_ = atoi(pszStr);
                char localeName[0x100] = {};
                pszStr = NextToken(&pszToken, '`', ' ');
                (*convertor)(pszStr, localeName);
                pItemInfo->sName_ = localeName;
                memset(localeName, 0, 0x100);
                pszStr = NextToken(&pszToken, '`', ' ');
                (*convertor)(pszStr, localeName);
                pItemInfo->sEnglishName_ = localeName;
                pszStr = NextToken(&pszToken, '`', ' ');
                pItemInfo->category_ = (unsigned short)atoi(pszStr);
                if (GetItemInfo(pItemInfo->nItemIndex_) == (STItemInfo*)0 &&
                    0 < pItemInfo->nItemIndex_)
                {
                    itemInfoMap_.insert(std::make_pair(pItemInfo->nItemIndex_, pItemInfo));
                }
                memset(szLineBuff, 0, 0x400);
            }
cleanup:
            if (pTotalBuffer != (char*)0)
            {
                delete[] pTotalBuffer;
            }
            fclose(pFile);
        }
    }
    return result;
}

bool CNRDItemInfoList::Save(const TCHAR* pszFilePath)
{
    if (pszFilePath == (TCHAR*)0)
    {
        return false;
    }
    FILE* pFile = fopen(pszFilePath, "w");
    if (pFile == (FILE*)0)
    {
        return false;
    }
    for (ItemInfoMap::iterator it = itemInfoMap_.begin(); it != itemInfoMap_.end(); ++it)
    {
        int nIndex = it->first;
        STItemInfo* pItemInfo = it->second;
        if (pItemInfo != (STItemInfo*)0)
        {
            fprintf(pFile, "%d", nIndex);
            fputc(0x20, pFile);
            fprintf(pFile, "%d", pItemInfo->rarity_);
            fputc(0x20, pFile);
            for (int i = 0; i <= 0xa; i = i + 1)
            {
                fprintf(pFile, "%d",
                        (unsigned int)(pItemInfo->xaUsableCharacter_[i] ? 1 : 0));
                fputc(0x20, pFile);
            }
            fprintf(pFile, "%d", pItemInfo->nOriginalUsableLevel_);
            fputc(0x20, pFile);
            fprintf(pFile, "`%s`", toMbcs(pItemInfo->sName_));
            fputc(0x20, pFile);
            fprintf(pFile, "`%s`", toMbcs(pItemInfo->sEnglishName_));
            fputc(0x20, pFile);
            fprintf(pFile, "%d", (unsigned int)pItemInfo->category_);
            fputc(0xa, pFile);
        }
    }
    fclose(pFile);
    return true;
}

unsigned short CNRDItemInfoList::ConvertAvatarCategory(unsigned short category,
                                                       bool bEmblemAvatar) const
{
    if (category > 22999 && category <= 23000)
    {
        category -= 23000;
        if (bEmblemAvatar)
        {
            category += 0x4a38;
        }
        else
        {
            category += 0x3a98;
        }
    }
    else if (category > 24999 && category <= 25000)
    {
        category -= 25000;
        if (bEmblemAvatar)
        {
            category += 0x5208;
        }
        else
        {
            category += 0x4268;
        }
    }
    return category;
}

CNRDItemInfoList::STItemInfo::STItemInfo()
{
    nItemIndex_ = 0;
    rarity_ = RARITY_COMMON;
    memset(xaUsableCharacter_, 0, 0xb);
    nOriginalUsableLevel_ = 0;
    category_ = 0;
}

CNRDItemInfoList::STItemInfo::~STItemInfo()
{
}

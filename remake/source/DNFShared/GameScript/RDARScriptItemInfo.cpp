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
    int MAX_PATH_ = 0x104;
    if (szFilePath == (TCHAR*)0)
    {
        return false;
    }
    TCHAR newFilePath[260];
    ss_strcpy(newFilePath, 0x104, szFilePath);
    findFileWithServerTokenInAuction(newFilePath);
    FILE* pFile = fopen(newFilePath, "rb");
    if (pFile == (FILE*)0)
    {
        return false;
    }
    bool result = true;
    int intData = 0;
    char szLineBuff[0x400] = {};
    int fileSize = 0;
    fseek(pFile, 0, SEEK_END);
    fileSize = (int)ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    char* pTotalBuffer = new char[fileSize];
    fread(pTotalBuffer, 1, fileSize, pFile);
    char* pTotalPtr = pTotalBuffer;
    while (*pTotalPtr != '\0' && pTotalPtr - pTotalBuffer < fileSize)
    {
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
        if (strlen(szLineBuff) <= 1)
        {
            memset(szLineBuff, 0, 0x400);
            continue;
        }
        char* pszToken = (char*)0;
        const char* pszStr = szLineBuff;
        STItemInfo* pItemInfo = new CNRDItemInfoList::STItemInfo();
        if (pItemInfo == (STItemInfo*)0)
        {
            result = false;
            goto cleanup;
        }
        pszToken = NextToken(&pszStr, '`', ' ');
        pItemInfo->nItemIndex_ = atoi(pszToken);
        pszToken = NextToken(&pszStr, '`', ' ');
        pItemInfo->rarity_ = (ENUM_RARITY)atoi(pszToken);
        for (int i = 0; i < 0xb; i = i + 1)
        {
            pszToken = NextToken(&pszStr, '`', ' ');
            intData = atoi(pszToken);
            pItemInfo->xaUsableCharacter_[i] = intData != 0;
        }
        pszToken = NextToken(&pszStr, '`', ' ');
        pItemInfo->nOriginalUsableLevel_ = atoi(pszToken);
        char localeName[0x100] = {};
        (*convertor)(NextToken(&pszStr, '`', ' '), localeName);
        pItemInfo->sName_ = localeName;
        memset(localeName, 0, 0x100);
        (*convertor)(NextToken(&pszStr, '`', ' '), localeName);
        pItemInfo->sEnglishName_ = localeName;
        pszToken = NextToken(&pszStr, '`', ' ');
        pItemInfo->category_ = (unsigned short)atoi(pszToken);
        if (GetItemInfo(pItemInfo->nItemIndex_) != (STItemInfo*)0)
        {
        }
        else if (0 < pItemInfo->nItemIndex_)
        {
            itemInfoMap_.insert(std::make_pair(pItemInfo->nItemIndex_, pItemInfo));
        }
        memset(szLineBuff, 0, 0x400);
    }
cleanup:
            delete[] pTotalBuffer;
            fclose(pFile);
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

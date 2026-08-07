#include "DNFFunctionLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

void Char2Hex(unsigned char ch, char* szHex)
{
    static const char saucHex[] = "0123456789abcdef";
    *szHex = saucHex[(int)(unsigned int)ch >> 4];
    szHex[1] = saucHex[ch & 0xf];
    szHex[2] = '\0';
}

bool Hex2Char(const char* szHex, unsigned char& rch)
{
    if ((*szHex < '0') || ('9' < *szHex))
    {
        if ((*szHex < 'a') || ('f' < *szHex))
        {
            return false;
        }
        rch = *szHex + 0xa9;
    }
    else
    {
        rch = *szHex + 0xd0;
    }
    const char* pc = szHex + 1;
    if ((*pc < '0') || ('9' < *pc))
    {
        if ((*pc < 'a') || ('f' < *pc))
        {
            return false;
        }
        rch = rch << 4;
        rch = rch + *pc + 0xa9;
    }
    else
    {
        rch = rch << 4;
        rch = rch + *pc + 0xd0;
    }
    return true;
}

void DNFFLib::Binary2Hex(const unsigned char* pucBinStr, int iBinSize, char* pszHexStr)
{
    const unsigned char* p = pucBinStr;
    *pszHexStr = '\0';
    for (int i = 0; i < iBinSize; i++)
    {
        char szHex[3];
        Char2Hex(*p, szHex);
        strcat(pszHexStr, szHex);
        p = p + 1;
    }
}

bool DNFFLib::Hex2Binary(const char* pszHexStr, unsigned char* pucBinStr, int iBinSize)
{
    int i = 0;
    while (true)
    {
        if (iBinSize <= i)
        {
            return true;
        }
        unsigned char ch;
        bool ret = Hex2Char(pszHexStr, ch);
        if (!ret)
        {
            break;
        }
        *pucBinStr = ch;
        i = i + 1;
        pszHexStr = pszHexStr + 2;
        pucBinStr = pucBinStr + 1;
    }
    return false;
}

void DNFFLib::PrintTextFile(char* szOpenFileName, char* szText)
{
    FILE* fpStream = fopen(szOpenFileName, "a");
    if (fpStream != NULL)
    {
        fprintf(fpStream, szText);
        fclose(fpStream);
    }
}

void DNFFLib::fPrintTextFile(char* szOpenFileName, char* format, ...)
{
    char szInfoline[4096];
    memset(szInfoline, 0, 0x1000);
    va_list args;
    va_start(args, format);
    vsprintf(szInfoline, format, args);
    va_end(args);
    PrintTextFile(szOpenFileName, szInfoline);
}

int DNFFLib::ExplodeString(char* cStr, char* cSep, char** pArray, int iMax)
{
    int iTokenCnt = 0;
    if ((iMax < 1) || (cStr == NULL))
    {
        *pArray = cStr;
        iTokenCnt = 0;
    }
    else
    {
        size_t len = strlen(cStr);
        char* pToken = strtok(cStr, cSep);
        while (pToken != NULL)
        {
            pArray[iTokenCnt] = pToken;
            iTokenCnt = iTokenCnt + 1;
            if (iMax <= iTokenCnt)
            {
                size_t tlen = strlen(pToken);
                if (pToken + tlen == cStr + len)
                {
                    pArray[iTokenCnt] = NULL;
                    return iTokenCnt;
                }
                pArray[iTokenCnt] = pToken + tlen + 1;
                return iTokenCnt;
            }
            pToken = strtok(NULL, cSep);
        }
    }
    return iTokenCnt;
}

void DNFFLib::Sleep_Ext(int sec, int usec)
{
    timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
}

void DNFFLib::Make_Dir(char* szdir_name)
{
    DIR* p_dir = opendir(szdir_name);
    if (p_dir == NULL)
    {
        mkdir(szdir_name, 0x1ff);
    }
}

int DNFFLib::get_rand_int(int divide)
{
    if (divide < 0)
    {
        return 0;
    }
    if (divide == 0)
    {
        return rand();
    }
    int seed = rand();
    if (divide < seed)
    {
        int r = rand();
        return r % divide;
    }
    int i1 = seed * 0x41c64e6d + 0x3039;
    int i2 = i1 * 0x41c64e6d + 0x3039;
    int i3 = i2 * 0x41c64e6d + 0x3039;
    unsigned int result = ((((unsigned int)(i1 >> 0x1f) >> 0x10) + i1) >> 0x10 & 0x7ff) << 10 ^
                          ((((unsigned int)(i2 >> 0x1f) >> 0x10) + i2) >> 0x10 & 0x3ff) << 10 ^
                          ((((unsigned int)(i3 >> 0x1f) >> 0x10) + i3) >> 0x10 & 0x3ff);
    if ((unsigned int)divide < result)
    {
        result = result % (unsigned int)divide;
    }
    return result;
}

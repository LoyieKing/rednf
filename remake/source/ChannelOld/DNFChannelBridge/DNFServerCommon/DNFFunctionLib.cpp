#include "DNFFunctionLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdarg.h>

void Char2Hex(unsigned char ch, char* szHex)
{
    // ORIG DWARF: saucHex = unsigned char[] (non-const, .data); const 会改变
    // GCC 4.4 -O0 的寄存器分配（edx 直接装载 vs eax->edx 拷贝），须保持非 const。
    static unsigned char saucHex[] = "0123456789abcdef";
    *szHex = saucHex[(int)(unsigned int)ch >> 4];
    szHex[1] = saucHex[ch & 0xf];
    szHex[2] = '\0';
}

bool Hex2Char(const char* szHex, unsigned char& rch)
{
    if (('0' <= *szHex) && (*szHex <= '9'))
    {
        rch = *szHex - 0x30;
    }
    else if (('a' <= *szHex) && (*szHex <= 'f'))
    {
        rch = *szHex - 0x57;
    }
    else
    {
        return false;
    }
    szHex++;
    if (('0' <= *szHex) && (*szHex <= '9'))
    {
        rch = rch << 4;
        rch = rch + *szHex - 0x30;
    }
    else if (('a' <= *szHex) && (*szHex <= 'f'))
    {
        rch = rch << 4;
        rch = rch + *szHex - 0x57;
    }
    else
    {
        return false;
    }
    return true;
}

void DNFFLib::Binary2Hex(const unsigned char* pucBinStr, int iBinSize, char* pszHexStr)
{
    int i;
    const unsigned char* p = pucBinStr;
    *pszHexStr = '\0';
    for (i = 0; i < iBinSize; i++, p = p + 1)
    {
        char szHex[3];
        Char2Hex(*p, szHex);
        strcat(pszHexStr, szHex);
    }
}

bool DNFFLib::Hex2Binary(const char* pszHexStr, unsigned char* pucBinStr, int iBinSize)
{
    int i = 0;
    while (i < iBinSize)
    {
        unsigned char ch;
        if (!Hex2Char(pszHexStr, ch))
        {
            return false;
        }
        *pucBinStr = ch;
        i = i + 1;
        pszHexStr = pszHexStr + 2;
        pucBinStr = pucBinStr + 1;
    }
    return true;
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
        return 0;
    }
    // ORIG 声明序：pToken 先声明、pLast 后声明（与 auction 已对齐版本一致）。
    char* pToken;
    char* pLast;
    pLast = cStr + strlen(cStr);
    pToken = strtok(cStr, cSep);
    while (pToken != NULL)
    {
        pArray[iTokenCnt] = pToken;
        iTokenCnt = iTokenCnt + 1;
        if (iTokenCnt >= iMax)
        {
            char* pTail = pToken + strlen(pToken);
            if (pTail == pLast)
            {
                pArray[iTokenCnt] = NULL;
            }
            else
            {
                pArray[iTokenCnt] = pTail + 1;
            }
            break;
        }
        pToken = strtok(NULL, cSep);
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
    switch (divide)
    {
    case 0:
        return rand();
    default:
        break;
    }
    int r = rand();
    if (r > divide)
    {
        return rand() % divide;
    }
    r *= 0x41c64e6d;
    r += 0x3039;
    unsigned int result = (r / 65536) & 0x7ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    r *= 0x41c64e6d;
    r += 0x3039;
    result <<= 10;
    result ^= (r / 65536) & 0x3ff;
    if (result > (unsigned int)divide)
    {
        return result % divide;
    }
    return result;
}

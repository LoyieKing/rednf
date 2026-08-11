// Rebuilt from df_auction_r DWARF (DNFFunctionLibWrapper.cpp CU), 2026-08-08
#include <string.h>

#include "DNFFunctionLibWrapper.h"

void DNFFLibWrapper::Char2Hex(unsigned char ch, char* szHex)
{
    // ORIG：函数级静态 saucHex（_ZZN14DNFFLibWrapper8Char2HexEhPcE7saucHex），
    // 必须非 const unsigned char 才会出 movzbl T(%eax),%eax + mov %eax,%edx
    // 的寄存器形态（与 DNFFunctionLib.cpp 自由函数同款结论）。
    static unsigned char saucHex[] = "0123456789abcdef";
    szHex[0] = (char)saucHex[ch >> 4];
    szHex[1] = (char)saucHex[ch & 0xf];
    szHex[2] = '\0';
}

bool DNFFLibWrapper::Hex2Char(const char* szHex, unsigned char& rch)
{
    if ((szHex[0] > 0x2f) && (szHex[0] <= 0x39))
    {
        rch = szHex[0] - 0x30;
    }
    else if ((szHex[0] > 0x60) && (szHex[0] <= 0x66))
    {
        rch = szHex[0] - 0x57;
    }
    else
    {
        return false;
    }
    szHex = szHex + 1;
    if ((szHex[0] > 0x2f) && (szHex[0] <= 0x39))
    {
        // ORIG：先 rch <<= 4 回写，再 (rch + szHex[0]) - 0x30 回写（两次重载）。
        rch = (unsigned char)(rch << 4);
        rch = (unsigned char)(rch + szHex[0] - 0x30);
    }
    else if ((szHex[0] > 0x60) && (szHex[0] <= 0x66))
    {
        rch = (unsigned char)(rch << 4);
        rch = (unsigned char)(rch + szHex[0] - 0x57);
    }
    else
    {
        return false;
    }
    return true;
}

void DNFFLibWrapper::Binary2Hex(const unsigned char* pucBinStr, int iBinSize, char* pszHexStr)
{
    // ORIG DWARF：局部 i(38)/szHex(39)/pucBinStr1(40)；声明不初始化，
    // 体中按 pucBinStr1→pszHexStr[0]→i 序赋值（槽位 -0xc/-0x13/-0x10）。
    int i;
    char szHex[3];
    const unsigned char* pucBinStr1;
    pucBinStr1 = pucBinStr;
    pszHexStr[0] = '\0';
    i = 0;
    while (i < iBinSize)
    {
        Char2Hex(*pucBinStr1, szHex);
        strcat(pszHexStr, szHex);
        // ORIG：i 先自增、指针后自增。
        i = i + 1;
        pucBinStr1 = pucBinStr1 + 1;
    }
}

bool DNFFLibWrapper::Hex2Binary(const char* pszHexStr, unsigned char* pucBinStr, int iBinSize)
{
    int count = 0;
    while (count < iBinSize)
    {
        unsigned char value;
        if (Hex2Char(pszHexStr, value) == false)
        {
            return false;
        }
        *pucBinStr = value;
        // ORIG：count→pszHexStr→pucBinStr 的自增顺序。
        count = count + 1;
        pszHexStr = pszHexStr + 2;
        pucBinStr = pucBinStr + 1;
    }
    return true;
}

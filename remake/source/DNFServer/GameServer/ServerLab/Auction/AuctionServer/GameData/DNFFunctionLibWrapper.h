#ifndef DNFFUNCTIONLIBWRAPPER_H_
#define DNFFUNCTIONLIBWRAPPER_H_

class DNFFLibWrapper
{
public:
    static void Char2Hex(unsigned char ch, char* szHex);
    static bool Hex2Char(const char* szHex, unsigned char& rch);
    static void Binary2Hex(const unsigned char* pucBinStr, int iBinSize, char* pszHexStr);
    static bool Hex2Binary(const char* pszHexStr, unsigned char* pucBinStr, int iBinSize);
};

#endif // DNFFUNCTIONLIBWRAPPER_H_

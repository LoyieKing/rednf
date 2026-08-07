#ifndef DNFFUNCTIONLIB_H_
#define DNFFUNCTIONLIB_H_

#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>

void Char2Hex(unsigned char ch, char* szHex);
bool Hex2Char(char* szHex, unsigned char* rch);

class DNFFLib
{
public:
    static void Binary2Hex(const unsigned char* pucBinStr, int iBinSize, char* pszHexStr);
    static bool Hex2Binary(char* pszHexStr, unsigned char* pucBinStr, int iBinSize);
    static int ExplodeString(char* cStr, char* cSep, char** pArray, int iMax);
    static void PrintTextFile(char* szOpenFileName, char* szText);
    static void fPrintTextFile(char* szOpenFileName, char* format, ...);
    static void Make_Dir(char* szdir_name);
    static void Sleep_Ext(int sec, int usec);
    static int get_rand_int(int divide);
};

#endif // DNFFUNCTIONLIB_H_

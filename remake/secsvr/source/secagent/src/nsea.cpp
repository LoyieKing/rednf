/* nsea.cpp -- NSEA 流密码（AC 源码集 NSEA/NSEA.C 结构 + 自定义密钥派生）
   initSBoxes/initTempKey/initIV/permuteSBox 由二进制反汇编还原：
   - initTempKey：MSVC rand LCG（holdrand = holdrand*0x343fd+0x269ec3），
     种子 = (salt<<1 ^ key字节) 16 位，生成 1024 字节
   - setInitialSBox：填 [i/4] 重复 4 次（1024 字节）
   - permuteSBox：用 tempKey 置换（columnTable={0,256,512,768}）
   - encrypt/decrypt：AC 参考两轮（16 步）Feistel + CBC IV 处理 */

#include "inc/nsea.h"
#include "include/TenCrypt.h"

#include <string.h>

typedef unsigned char BYTE;
typedef unsigned int LONG;

static LONG rotTemp;

#define rotl(A, B)                                 \
    rotTemp = (LONG)(A) >> 24;                     \
    A = ((LONG)(A) << 8) | ((LONG)(B) >> 24);      \
    B = ((LONG)(B) << 8) | rotTemp

#define rotr(A, B)                                 \
    rotTemp = (LONG)(B) << 24;                     \
    B = ((LONG)(B) >> 8) | ((LONG)(A) << 24);      \
    A = ((LONG)(A) >> 8) | rotTemp

// mangled: _ZN5CNsea14setInitialSBoxEPh
void CNsea::setInitialSBox(unsigned char *sBox) {
    int i;
    int j;
    int index;

    for (i = 0; i < 256; i++) {
        for (j = 0; j < 4; j++)
            sBox[4 * i + j] = (unsigned char)i;
    }
}

// mangled: _ZN5CNsea11permuteSBoxEPhi
void CNsea::permuteSBox(unsigned char *sBox, int tempKeyIndex) {
    int columnTable[4];
    int srcIndex;
    int destIndex;
    unsigned char temp;

    columnTable[0] = 0x000;
    columnTable[1] = 0x100;
    columnTable[2] = 0x200;
    columnTable[3] = 0x300;
    for (srcIndex = 0; srcIndex < 1024; srcIndex++) {
        destIndex = tempKey[tempKeyIndex] + columnTable[srcIndex & 3];
        temp = sBox[srcIndex];
        sBox[srcIndex] = sBox[destIndex];
        sBox[destIndex] = temp;
        tempKeyIndex++;
    }
}

// mangled: _ZN5CNsea7encryptEPhS0_
void CNsea::encrypt(unsigned char *inData, unsigned char *outData) {
    unsigned int lLeft;
    unsigned int rLeft;
    unsigned int lRight;
    unsigned int rRight;

    lLeft = ((LONG *)inData)[0];
    rLeft = ((LONG *)inData)[1];
    lRight = ((LONG *)inData)[2];
    rRight = ((LONG *)inData)[3];
    lLeft ^= ivLLeft1;
    rLeft ^= ivRLeft1;
    lRight ^= ivLRight1;
    rRight ^= ivRRight1;

    lLeft ^= sBoxes1[(BYTE)lRight];   /* 1,1 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 1,2 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 1,3 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 1,4 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 1,5 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 1,6 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 1,7 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 1,8 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 2,1 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 2,2 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 2,3 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 2,4 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 2,5 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 2,6 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);
    lLeft ^= sBoxes1[(BYTE)lRight];   /* 2,7 */
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotl(lRight, rRight);
    lRight ^= sBoxes1[(BYTE)lLeft];   /* 2,8 */
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotl(lLeft, rLeft);

    ivLLeft1 = lLeft;
    ivRLeft1 = rLeft;
    ivLRight1 = lRight;
    ivRRight1 = rRight;
    ((LONG *)outData)[0] = lLeft;
    ((LONG *)outData)[1] = rLeft;
    ((LONG *)outData)[2] = lRight;
    ((LONG *)outData)[3] = rRight;
}

// mangled: _ZN5CNsea7decryptEPhS0_
void CNsea::decrypt(unsigned char *inData, unsigned char *outData) {
    unsigned int lLeft;
    unsigned int rLeft;
    unsigned int lRight;
    unsigned int rRight;

    lLeft = ((LONG *)inData)[0];
    rLeft = ((LONG *)inData)[1];
    lRight = ((LONG *)inData)[2];
    rRight = ((LONG *)inData)[3];

    rotr(lLeft, rLeft);                    /* 1,1 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 1,2 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 1,3 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 1,4 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 1,5 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 1,6 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 1,7 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 1,8 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 2,1 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 2,2 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 2,3 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 2,4 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 2,5 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 2,6 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];
    rotr(lLeft, rLeft);                    /* 2,7 */
    lRight ^= sBoxes1[(BYTE)lLeft];
    rRight ^= sBoxes2[(BYTE)rLeft];
    rotr(lRight, rRight);                  /* 2,8 */
    lLeft ^= sBoxes1[(BYTE)lRight];
    rLeft ^= sBoxes2[(BYTE)rRight];

    lLeft ^= ivLLeft2;
    rLeft ^= ivRLeft2;
    lRight ^= ivLRight2;
    rRight ^= ivRRight2;
    ivLLeft2 = ((LONG *)inData)[0];
    ivRLeft2 = ((LONG *)inData)[1];
    ivLRight2 = ((LONG *)inData)[2];
    ivRRight2 = ((LONG *)inData)[3];
    ((LONG *)outData)[0] = lLeft;
    ((LONG *)outData)[1] = rLeft;
    ((LONG *)outData)[2] = lRight;
    ((LONG *)outData)[3] = rRight;
}

// mangled: _ZN5CNsea6initIVEj
void CNsea::initIV(unsigned int salt) {
    unsigned char iv[16];
    int i;

    iv[0] = (unsigned char)(salt >> 24);
    iv[1] = (unsigned char)(salt >> 16);
    iv[2] = (unsigned char)(salt >> 8);
    iv[3] = (unsigned char)salt;
    for (i = 4; i < 16; i++)
        iv[i] = 0;
    ivLLeft1 = ((unsigned int *)iv)[0];
    ivLLeft2 = ((unsigned int *)iv)[0];
    ivRLeft1 = ((unsigned int *)iv)[1];
    ivRLeft2 = ((unsigned int *)iv)[1];
    ivLRight1 = ((unsigned int *)iv)[2];
    ivLRight2 = ((unsigned int *)iv)[2];
    ivRRight1 = ((unsigned int *)iv)[3];
    ivRRight2 = ((unsigned int *)iv)[3];
}

// mangled: _ZN5CNsea11initTempKeyEPhitS0_
void CNsea::initTempKey(unsigned char *key, int keyLength, unsigned short salt, unsigned char *tempKeyPtr) {
    int i;
    unsigned int holdrand;
    unsigned short seed;
    int j;

    seed = salt;
    if (keyLength > 0) {
        for (j = 0; j < keyLength; j++) {
            seed = (unsigned short)((seed << 1) ^ key[j]);
        }
    }
    holdrand = seed;
    for (i = 0; i < 1024; i++) {
        holdrand = holdrand * 0x343fd + 0x269ec3;
        tempKeyPtr[i] = (unsigned char)((holdrand >> 16) & 0xff);
    }
}

// mangled: _ZN5CNsea10initSBoxesEPhij
void CNsea::initSBoxes(unsigned char *key, int keyLength, unsigned int salt) {
    unsigned char *sBoxByte1;
    unsigned char *sBoxByte2;
    int i;
    unsigned int j;

    sBoxByte1 = (unsigned char *)sBoxes1;
    sBoxByte2 = (unsigned char *)sBoxes2;
    setInitialSBox(sBoxByte1);
    setInitialSBox(sBoxByte2);
    initTempKey(key, keyLength, (unsigned short)(salt & 0xffff), tempKey);
    initTempKey(key, keyLength, (unsigned short)(salt >> 16), tempKey + 0x400);
    permuteSBox(sBoxByte1, 0);
    permuteSBox(sBoxByte2, 0x400);
    initIV(salt);
    /* 第一轮后：用 key 覆盖 tempKey（长度前缀 + key + 零填充） */
    tempKey[0] = (unsigned char)(keyLength >> 8);
    tempKey[1] = (unsigned char)(keyLength & 0xff);
    for (i = 0; i < keyLength; i++)
        tempKey[2 + i] = key[i];
    for (j = 2 + keyLength; j < 0x800; j++)
        tempKey[j] = 0;
    /* 第二轮：重置 S 盒并用新 tempKey 置换 */
    setInitialSBox(sBoxByte1);
    setInitialSBox(sBoxByte2);
    permuteSBox(sBoxByte1, 0);
    permuteSBox(sBoxByte2, 0x400);
}

// mangled: _ZN5CNsea4NseaEPhbS0_j
void CNsea::Nsea(unsigned char *key, bool benc, unsigned char *buf, unsigned int buflen) {
    unsigned int nLen;
    ITsLocal *pLocal;
    unsigned int i;

    if (buflen != 0) {
        initSBoxes(key, 16, 0);
        initIV(0);
        nLen = buflen / 16;
        for (i = 0; i < nLen; i++) {
            if (benc)
                encrypt(buf + i * 16, buf + i * 16);
            else
                decrypt(buf + i * 16, buf + i * 16);
        }
        pLocal = CreateTsLocal();
        pLocal->Seattos(key, benc, buf + nLen * 16, buflen - nLen * 16);
    }
}

// mangled: _ZN5CNsea4InitEPvS0_
void CNsea::Init(void *pSys, void *pLocal) {
    m_pSys = pSys;
    m_pLocal = pLocal;
    tempKey = new unsigned char[0x800];
    sBoxes1 = new unsigned int[0x400];
    sBoxes2 = new unsigned int[0x400];
}

// mangled: _ZN5CNsea6UnInitEv
void CNsea::UnInit() {
    if (tempKey != 0) {
        delete[] tempKey;
        tempKey = 0;
    }
    if (sBoxes1 != 0) {
        delete[] sBoxes1;
        sBoxes1 = 0;
    }
    if (sBoxes2 != 0) {
        delete[] sBoxes2;
        sBoxes2 = 0;
    }
}

// mangled: _ZN5CNseaC1Ev / _ZN5CNseaC2Ev
CNsea::CNsea() {
    Init(0, 0);
}

// mangled: _ZN5CNseaD1Ev / _ZN5CNseaD2Ev
CNsea::~CNsea() {
    UnInit();
}

// mangled: _Z7TenNseaPhbS_j
void TenNsea(unsigned char *key, bool benc, unsigned char *buf,
             unsigned int buflen) {
    CNsea nsea;

    nsea.Nsea(key, benc, buf, buflen);
}

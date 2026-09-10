// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SHA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>

#include "SHA.h"

namespace nsl {

const uint CSHA::sm_K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint CSHA::sm_H256[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

#include "SHA/CSHA.inc"
{
    for (int i = 0; i < 8; i++)
    {
        m_auiBuf[i] = sm_H256[i];
    }
    m_auiBits[0] = 0;
    m_auiBits[1] = 0;
}

#include "SHA/AddData.inc"
{
    if (iDataLength < 1)
    {
        throw CCryptorException("AddData", 0x28, "FileCrypt ERROR: in CSHA::AddData(), Data Length should be > 0!");
    }

    uint uiT = m_auiBits[0];
    m_auiBits[0] = iDataLength * 8 + uiT;
    register bool bCarry = (m_auiBits[0] < uiT);
    if (bCarry)
    {
        m_auiBits[1] = m_auiBits[1] + 1;
    }
    m_auiBits[1] = m_auiBits[1] + (iDataLength >> 0x1d);

    uiT = (uiT >> 3) & 0x3f;
    if (uiT != 0)
    {
        unsigned char* pDest = m_aucIn + uiT;
        uiT = 0x40 - uiT;
        memcpy(pDest, pcData, uiT);
        Transform();
        pcData = pcData + uiT;
        iDataLength = iDataLength - uiT;
    }
    for (; iDataLength > 0x3f; iDataLength = iDataLength - 0x40)
    {
        memcpy(m_aucIn, pcData, 0x40);
        Transform();
        pcData = pcData + 0x40;
    }
    memcpy(m_aucIn, pcData, iDataLength);
    m_bAddData = true;
}

#include "SHA/FinalDigest.inc"
{
    if (m_bAddData != true)
    {
        throw CCryptorException("FinalDigest", 0x54, "FileCrypt ERROR: in CSHA::FinalDigest(), No data Added before call!");
    }

    uint uiCount = (m_auiBits[0] >> 3) & 0x3f;
    unsigned char* puc = m_aucIn + uiCount;
    *puc = 0x80;
    puc = puc + 1;
    uiCount = 0x3f - uiCount;
    if (uiCount < 8)
    {
        memset(puc, 0, uiCount);
        Transform();
        memset(m_aucIn, 0, 0x38);
    }
    else
    {
        memset(puc, 0, uiCount - 8);
    }
    Word2Bytes(m_auiBits[1], m_aucIn + 0x38);
    Word2Bytes(m_auiBits[0], m_aucIn + 0x3c);
    Transform();
    // ORIG：while 形（体内 i++ 先于 pcDigest += 4）。
    for (int i = 0; i < 8; )
    {
        Word2Bytes(m_auiBuf[i], (unsigned char*)pcDigest);
        i++;
        pcDigest = pcDigest + 4;
    }
    Reset();
}

#include "SHA/Reset.inc"
{
    for (int i = 0; i < 8; i++)
    {
        m_auiBuf[i] = sm_H256[i];
    }
    m_auiBits[0] = 0;
    m_auiBits[1] = 0;
    m_bAddData = false;
}

#include "SHA/Bytes2Word.inc"
{
    ruiWord = (unsigned int)pcBytes[3] | (unsigned int)(pcBytes[2] << 8) | (unsigned int)(pcBytes[1] << 0x10) | (unsigned int)(pcBytes[0] << 0x18);
}

#include "SHA/Word2Bytes.inc"
{
    pcBytes = pcBytes + 3;
    *pcBytes = (unsigned char)ruiWord;
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 8);
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 0x10);
    pcBytes = pcBytes - 1;
    *pcBytes = (unsigned char)(ruiWord >> 0x18);
}

#include "SHA/Transform.inc"
{
    uint W[64];
    uint n0, n1, n2, n3, n4, n5, n6, n7, T1;

    unsigned char* pcIn = m_aucIn;
    for (int i = 0; i < 0x10; i++)
    {
        Bytes2Word(pcIn, W[i]);
        pcIn = pcIn + 4;
    }
    for (int i = 0x10; i < 0x40; i++)
    {
        W[i] = sig1(W[i - 2]) + W[i - 7] + sig0(W[i - 0xf]) + W[i - 0x10];
    }

    n0 = m_auiBuf[0];
    n1 = m_auiBuf[1];
    n2 = m_auiBuf[2];
    n3 = m_auiBuf[3];
    n4 = m_auiBuf[4];
    n5 = m_auiBuf[5];
    n6 = m_auiBuf[6];
    n7 = m_auiBuf[7];

    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[0] + W[0];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[1] + W[1];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[2] + W[2];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[3] + W[3];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[4] + W[4];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[5] + W[5];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[6] + W[6];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[7] + W[7];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[8] + W[8];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[9] + W[9];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[10] + W[10];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[11] + W[11];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[12] + W[12];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[13] + W[13];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[14] + W[14];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[15] + W[15];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[16] + W[16];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[17] + W[17];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[18] + W[18];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[19] + W[19];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[20] + W[20];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[21] + W[21];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[22] + W[22];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[23] + W[23];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[24] + W[24];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[25] + W[25];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[26] + W[26];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[27] + W[27];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[28] + W[28];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[29] + W[29];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[30] + W[30];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[31] + W[31];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[32] + W[32];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[33] + W[33];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[34] + W[34];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[35] + W[35];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[36] + W[36];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[37] + W[37];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[38] + W[38];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[39] + W[39];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[40] + W[40];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[41] + W[41];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[42] + W[42];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[43] + W[43];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[44] + W[44];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[45] + W[45];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[46] + W[46];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[47] + W[47];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[48] + W[48];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[49] + W[49];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[50] + W[50];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[51] + W[51];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[52] + W[52];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[53] + W[53];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[54] + W[54];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[55] + W[55];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;
    T1 = SIG1(n4) + n7 + CH(n4,n5,n6) + sm_K256[56] + W[56];
    n7 = SIG0(n0) + T1 + MAJ(n0,n1,n2);
    n3 = n3 + T1;
    T1 = SIG1(n3) + n6 + CH(n3,n4,n5) + sm_K256[57] + W[57];
    n6 = SIG0(n7) + T1 + MAJ(n7,n0,n1);
    n2 = n2 + T1;
    T1 = SIG1(n2) + n5 + CH(n2,n3,n4) + sm_K256[58] + W[58];
    n5 = SIG0(n6) + T1 + MAJ(n6,n7,n0);
    n1 = n1 + T1;
    T1 = SIG1(n1) + n4 + CH(n1,n2,n3) + sm_K256[59] + W[59];
    n4 = SIG0(n5) + T1 + MAJ(n5,n6,n7);
    n0 = n0 + T1;
    T1 = SIG1(n0) + n3 + CH(n0,n1,n2) + sm_K256[60] + W[60];
    n3 = SIG0(n4) + T1 + MAJ(n4,n5,n6);
    n7 = n7 + T1;
    T1 = SIG1(n7) + n2 + CH(n7,n0,n1) + sm_K256[61] + W[61];
    n2 = SIG0(n3) + T1 + MAJ(n3,n4,n5);
    n6 = n6 + T1;
    T1 = SIG1(n6) + n1 + CH(n6,n7,n0) + sm_K256[62] + W[62];
    n1 = SIG0(n2) + T1 + MAJ(n2,n3,n4);
    n5 = n5 + T1;
    T1 = SIG1(n5) + n0 + CH(n5,n6,n7) + sm_K256[63] + W[63];
    n0 = SIG0(n1) + T1 + MAJ(n1,n2,n3);
    n4 = n4 + T1;

    m_auiBuf[0] = n0 + m_auiBuf[0];
    m_auiBuf[1] = n1 + m_auiBuf[1];
    m_auiBuf[2] = n2 + m_auiBuf[2];
    m_auiBuf[3] = n3 + m_auiBuf[3];
    m_auiBuf[4] = n4 + m_auiBuf[4];
    m_auiBuf[5] = n5 + m_auiBuf[5];
    m_auiBuf[6] = n6 + m_auiBuf[6];
    m_auiBuf[7] = n7 + m_auiBuf[7];
}

} // namespace nsl

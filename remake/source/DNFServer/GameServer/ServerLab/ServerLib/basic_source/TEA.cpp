// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TEA/<Func>.inc（不参与编译、内部不含 #include）。
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

#include "TEA.h"
#include "SHA.h"

namespace nsl {

const char CTEA::sm_chain0[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#include "TEA/CTEA.inc"
{
    m_blockSize = 8;
    m_keylength = 0x10;
}

#include "TEA/dtor_CTEA.inc"
{
}

#include "TEA/Initialize.inc"
{
    if (keydata == NULL)
    {
        throw CCryptorException("Initialize", 0x19, sm_szErrorMsg4);
    }
    if (keydatalength <= 0)
    {
        throw CCryptorException("Initialize", 0x1b, sm_szErrorMsg5);
    }
    if (iMode < 0 || iMode > 2)
    {
        throw CCryptorException("Initialize", 0x1d, sm_szErrorMsg2);
    }
    if (iPadding < 0 || iPadding > 2)
    {
        throw CCryptorException("Initialize", 0x1f, sm_szErrorMsg3);
    }

    m_iMode = iMode;
    m_iPadding = iPadding;

    char key[16];
    int i = 0;
    int pos = 0;
    while (i < m_keylength)
    {
        key[i] = keydata[pos];
        i = i + 1;
        pos = (pos + 1) % keydatalength;
    }

    // ORIG 声明序：bSameKey 先于 bSameChain（槽位 -0x1a/-0x19）。
    char bSameKey = 0;
    char bSameChain = 0;
    if (m_bInit)
    {
        if (memcmp(m_apchain0, chain, m_blockSize) == 0)
        {
            bSameChain = 1;
        }
        if (memcmp(m_apKey, key, m_keylength) == 0)
        {
            bSameKey = 1;
        }
    }
    if (bSameChain)
    {
        memcpy(m_apchain, m_apchain0, m_blockSize);
    }
    else
    {
        memcpy(m_apchain0, chain, m_blockSize);
        memcpy(m_apchain, chain, m_blockSize);
    }
    if (bSameKey == 0)
    {
        memcpy(m_apKey, key, m_keylength);
        BytesToWord((const unsigned char*)key, m_auiKey[0]);
        BytesToWord((const unsigned char*)key + 4, m_auiKey[1]);
        BytesToWord((const unsigned char*)key + 8, m_auiKey[2]);
        BytesToWord((const unsigned char*)key + 12, m_auiKey[3]);
        m_bInit = true;
    }
    return;
}

#include "TEA/ResetChain.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("ResetChain", 0x4d, sm_szErrorMsg1);
    }
    memcpy(m_apchain, m_apchain0, m_blockSize);
}

#include "TEA/Signature.inc"
{
    char acSigData[23] = {0};
    strcat(acSigData, "TEA");
    int iLen = strlen(acSigData);
    memcpy(acSigData + iLen, m_apKey, m_keylength);
    sprintf(acSigData + m_keylength + iLen, "%d%d", m_iMode, m_iPadding);
    CSHA oSHA;
    oSHA.AddData(acSigData, strlen(acSigData));
    oSHA.FinalDigest(pcSig);
}

#include "TEA/EncryptBlock.inc"
{
    // ORIG DWARF：v(98) 与 w(99) 两个数组——v 存输入字，w 存输出字。
    unsigned int v[2];
    unsigned int w[2];
    BytesToWord(pucIn, v[0]);
    BytesToWord(pucIn + 4, v[1]);
    register unsigned int y = v[0];
    register unsigned int z = v[1];
    register unsigned int sum = 0;
    register unsigned int delta = 0x9e3779b9;
    register unsigned int n = 0x20;
    while (n-- != 0)
    {
        y += (((z << 4) ^ (z >> 5)) + z) ^ (sum + m_auiKey[sum & 3]);
        sum += delta;
        z += (((y << 4) ^ (y >> 5)) + y) ^ (sum + m_auiKey[(sum >> 0xb) & 3]);
    }
    w[0] = y;
    w[1] = z;
    WordToBytes(w[0], pucOut);
    WordToBytes(w[1], pucOut + 4);
}

#include "TEA/DecryptBlock.inc"
{
    unsigned int v[2];
    unsigned int w[2];
    BytesToWord(pucIn, v[0]);
    BytesToWord(pucIn + 4, v[1]);
    register unsigned int y = v[0];
    register unsigned int z = v[1];
    register unsigned int sum = 0xc6ef3720;
    register unsigned int delta = 0x9e3779b9;
    register unsigned int n = 0x20;
    while (n-- != 0)
    {
        z -= (((y << 4) ^ (y >> 5)) + y) ^ (sum + m_auiKey[(sum >> 0xb) & 3]);
        sum -= delta;
        y -= (((z << 4) ^ (z >> 5)) + z) ^ (sum + m_auiKey[sum & 3]);
    }
    w[0] = y;
    w[1] = z;
    WordToBytes(w[0], pucOut);
    WordToBytes(w[1], pucOut + 4);
}

#include "TEA/Encrypt.inc"
{
    if (m_bInit != true)
    {
        return -1;
    }
    if (n == 0 || n % m_blockSize != 0)
    {
        return -1;
    }

    unsigned int i;
    const char* pin;
    char* presult;

    if (m_iMode == 1)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            Xor(m_apchain, pin);
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            memcpy(m_apchain, presult, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else if (m_iMode == 2)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            Xor(presult, pin);
            memcpy(m_apchain, presult, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            EncryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    return 1;
}

#include "TEA/Decrypt.inc"
{
    if (m_bInit != true)
    {
        return -1;
    }
    if (n == 0 || n % m_blockSize != 0)
    {
        return -1;
    }

    unsigned int i;
    const char* pin;
    char* presult;

    if (m_iMode == 1)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            Xor(presult, m_apchain);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else if (m_iMode == 2)
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            EncryptBlock((const unsigned char*)m_apchain, (unsigned char*)presult);
            Xor(presult, pin);
            memcpy(m_apchain, pin, m_blockSize);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    else
    {
        i = 0;
        pin = in;
        presult = result;
        for (; n / m_blockSize > i; i++)
        {
            DecryptBlock((const unsigned char*)pin, (unsigned char*)presult);
            pin = pin + m_blockSize;
            presult = presult + m_blockSize;
        }
    }
    return 1;
}

} // namespace nsl

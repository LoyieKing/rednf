// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Method/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "Method.h"

namespace nsl {

const char* IMethod::sm_szErrorMsg1 = "FileCrypt ERROR: Encryption/Decryption Object not Initialized!";
const char* IMethod::sm_szErrorMsg2 = "FileCrypt ERROR: Illegal Operation Mode!";
const char* IMethod::sm_szErrorMsg3 = "FileCrypt ERROR: Illegal Padding Mode!";
const char* IMethod::sm_szErrorMsg4 = "FileCrypt ERROR: No Key DataSpecified!";
const char* IMethod::sm_szErrorMsg5 = "FileCrypt ERROR: Key Data Length should be > 0!";
const char* IMethod::sm_szErrorMsg6 = "FileCrypt ERROR: Cannot open File ";
const char* IMethod::sm_szErrorMsg7 = "FileCrypt ERROR: The same File for Input and Output ";
const char* IMethod::sm_szErrorMsg8 = "FileCrypt ERROR: File ";
const char* IMethod::sm_szErrorMsg9 = " cannot be Correctly Decrypted!";
const char* IMethod::sm_szErrorMsg10 = " cannot be Correctly Decrypted!";

#include "Method/IMethod.inc"
{
    m_bInit = false;
}

#include "Method/dtor_IMethod.inc"
{
}

#include "Method/Xor.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("Xor", 0x22, sm_szErrorMsg1);
    }
    for (int i = 0; i < m_blockSize; i++)
    {
        *buff = *buff ^ *chain;
        buff = buff + 1;
        chain = chain + 1;
    }
}

#include "Method/SetMode.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetMode", 0x2b, sm_szErrorMsg1);
    }
    if (iMode < 0 || iMode >= 3)
    {
        throw CCryptorException("SetMode", 0x2d, sm_szErrorMsg2);
    }
    m_iMode = iMode;
}

#include "Method/SetPadding.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("SetPadding", 0x35, sm_szErrorMsg1);
    }
    if (iPadding < 0 || iPadding >= 3)
    {
        throw CCryptorException("SetPadding", 0x37, sm_szErrorMsg3);
    }
    m_iPadding = iPadding;
}

#include "Method/GetKeyLength.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetKeyLength", 0x3f, sm_szErrorMsg1);
    }
    return m_keylength;
}

#include "Method/GetBlockSize.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetBlockSize", 0x46, sm_szErrorMsg1);
    }
    return m_blockSize;
}

#include "Method/GetMode.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetMode", 0x4d, sm_szErrorMsg1);
    }
    return m_iMode;
}

#include "Method/GetPadding.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("GetPadding", 0x54, sm_szErrorMsg1);
    }
    return m_iPadding;
}

#include "Method/Pad.inc"
{
    if (m_bInit != true)
    {
        throw CCryptorException("Pad", 0x5c, sm_szErrorMsg1);
    }
    int iRes = iLength % m_blockSize;
    if (iRes != 0)
    {
        int iPadded = m_blockSize - iRes;
        char* pin = in + iLength;
        switch (m_iPadding)
        {
        case 0:
            // ORIG：while 形，体内 i++ 先于 pin++。
            for (int i = 0; i < iPadded; )
            {
                *pin = '\0';
                i++;
                pin = pin + 1;
            }
            break;
        case 1:
            for (int i = 0; i < iPadded; )
            {
                *pin = ' ';
                i++;
                pin = pin + 1;
            }
            break;
        case 2:
            for (int i = 0; i < iPadded; )
            {
                *pin = (char)iPadded;
                i++;
                pin = pin + 1;
            }
            break;
        }
        return iLength + iPadded;
    }
    return iLength;
}

#include "Method/BytesToWord.inc"
{
    ruiWord = 0;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x18);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 0x10);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | ((unsigned int)*pucBytes << 8);
    pucBytes = pucBytes + 1;
    ruiWord = ruiWord | (unsigned int)*pucBytes;
}

#include "Method/WordToBytes.inc"
{
    *pucBytes = (unsigned char)(uiWord >> 0x18);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 0x10);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)(uiWord >> 8);
    pucBytes = pucBytes + 1;
    *pucBytes = (unsigned char)uiWord;
}

} // namespace nsl

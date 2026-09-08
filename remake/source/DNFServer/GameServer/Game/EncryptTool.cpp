// EncryptTool.cpp — ORIG 还原（2026-08-28）。
// 对照 /tmp/df_game_r objdump：
//   C1/C2 0x0848e0ea、createEncKey 0x0848d5b0、setCryptKey 0x0848d94a、
//   getKeyIndex 0x0848d9a2、wrapEncrypt 0x0848d9b0。
// 加密类（用户规则 2026-08-16）只要求算法正确；实现尽量按 O0 指令形状对齐。

#include "EncryptTool.h"

#include <string.h>
#include <time.h>

#include "CRijndael.h"
#include "Encryption.h"

// ORIG 静态密钥表 @0x93ab8e0：16 项，每项 0x20 字节（strlen=16 的数字串）。
// C1 用 strlen+strncpy 按块拷贝。
static const char s_keyTable[16][0x20] = {
    "0214799992158780",
    "1214670882044781",
    "2214528773140782",
    "3214398662149783",
    "4214278552191784",
    "5219178442142785",
    "6214978332143986",
    "7314078222145287",
    "8204478112146708",
    "9215878002147999",
    "0214238552191784",
    "8219148442132785",
    "7214928332123986",
    "6314018222115287",
    "1204488112196708",
    "2215898002167999",
};

EncryptTool::EncryptTool()
{
    memset((char*)this, 0, 0x20);
    m_rijndael = new CRijndael;
    for (int i = 0; i <= 0xf; ++i)
    {
        memset(m_key[i], 0, 0x20);
        strncpy((char*)m_key[i], s_keyTable[i], strlen(s_keyTable[i]));
    }
}

void EncryptTool::createEncKey()
{
    unsigned int local_10 = time(0);
    unsigned int local_14 = local_10;
    m_keyIndex = local_14 & 0xf;
    local_14 = m_keyIndex;
    Encryption::SeedRand(local_14);

    m_key[m_keyIndex][0]  = (char)(m_key[0][9]  + m_keyIndex + 0x05);
    m_key[m_keyIndex][1]  = (char)(m_key[1][8]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][2]  = (char)(m_key[2][7]  + m_keyIndex + 0x04);
    m_key[m_keyIndex][3]  = (char)(m_key[3][6]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][4]  = (char)(m_key[4][5]  + m_keyIndex);
    m_key[m_keyIndex][5]  = (char)(m_key[5][4]  + m_keyIndex + 0x07);
    m_key[m_keyIndex][6]  = (char)(m_key[6][3]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][7]  = (char)(m_key[7][2]  + m_keyIndex + 0x06);
    m_key[m_keyIndex][8]  = (char)(m_key[8][1]  + m_keyIndex + 0x03);
    m_key[m_keyIndex][9]  = (char)(m_key[9][0]  + m_keyIndex + 0x02);
    m_key[m_keyIndex][10] = (char)(m_key[10][5] + m_keyIndex + 0x02);
    m_key[m_keyIndex][11] = (char)(m_key[11][4] + m_keyIndex);
    m_key[m_keyIndex][12] = (char)(m_key[12][3] + m_keyIndex + 0x07);
    m_key[m_keyIndex][13] = (char)(m_key[13][2] + m_keyIndex + 0x02);
    m_key[m_keyIndex][14] = (char)(m_key[14][1] + m_keyIndex + 0x06);
    m_key[m_keyIndex][15] = (char)(m_key[15][0] + m_keyIndex + 0x03);

    m_rijndael->Initialize((const char*)m_key[m_keyIndex], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}

void EncryptTool::setCryptKey(int index)
{
    m_rijndael->Initialize((const char*)m_key[index], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}

int EncryptTool::getKeyIndex()
{
    return m_keyIndex;
}

int EncryptTool::wrapEncrypt(char* src, int len, char* out)
{
    // [b28c-lane25 判词·N7] ORIG 0x0848da30：虚调用 `mov (%eax),%ecx; … call *%ecx`
    // vs 4.4 间接 call 目标恒 %eax（N7 已录，8 件实证）。助记符序列全等仅寄存器名差。
    int blockSize = m_rijndael->GetBlockSize(0);
    int padded = 0;
    if (len % blockSize != 0)
    {
        padded = (len / blockSize + 1) * blockSize;
    }
    else
    {
        padded = len;
    }
    m_rijndael->Pad(src, len, 0);
    m_rijndael->Encrypt(src, out, padded);
    out[padded] = '\0';
    return padded;
}

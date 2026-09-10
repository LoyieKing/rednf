// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EncryptTool/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "EncryptTool/EncryptTool.inc"
{
    memset((char*)this, 0, 0x20);
    m_rijndael = new CRijndael;
    for (int i = 0; i <= 0xf; ++i)
    {
        memset(m_key[i], 0, 0x20);
        strncpy((char*)m_key[i], s_keyTable[i], strlen(s_keyTable[i]));
    }
}

// [b28d-lane6] DIFF·豁免：加密/密钥生成类按用户规则只要求算法正确（不强制 identical）；258 vs 238，语义经 decrypt 往返验证。
// [P6AuditOldN.P6S2 2026-09-11 · 本波复核] n1_v DIFF 258/238 gap=+20（本片最大缺口）。
//   ORIG 0x0848d5b0 索引/常量计算段：`mov 0x200(%eax),%ecx; …; movzbl 0x9(%eax),%eax;
//   mov %eax,%edx; …; lea (%edx,%eax,1),%eax` vs 我方 `mov 0x200(%eax),%edx; …;
//   movzbl 0x9(%eax),%ecx; …; lea (%ecx,%eax,1),%eax; lea 0x5(%eax),%ecx; shl $0x5,%edx;
//   mov %cl,(%edx,%eax,1)`。新钥匙判定：P339（无槽序入口）/P59（无守卫）/真 switch
//   （无 cmp 链）/类型轴三式（掩码 cast 形未现）均不适用，属 4.1↔4.4 索引计算与
//   装载序；未落地，维持 N（未决）。
#include "EncryptTool/createEncKey.inc"
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

#include "EncryptTool/setCryptKey.inc"
{
    m_rijndael->Initialize((const char*)m_key[index], 0x20,
                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
}

#include "EncryptTool/getKeyIndex.inc"
{
    return m_keyIndex;
}

#include "EncryptTool/wrapEncrypt.inc"
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

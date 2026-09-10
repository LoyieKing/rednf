// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GreyImage/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ GreyImage 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 逐函数与 df_game_r 反编译比对（identical）。
#include "GreyImage.h"
#include <cstring>
#include <algorithm>

#include "GreyImage/GreyImage.inc"
{
    m_width = 0;
    m_height = 0;
    m_image = 0;
}

#include "GreyImage/dtor_GreyImage.inc"
{
    reset();
}

#include "GreyImage/reset.inc"
{
    // [b28c-35] ORIG 0x080df582 形：`je <nop 岛>; body; jmp <leave>; nop; leave; ret`
    // （4.1 空 else 占位岛，P223）。裸早返 `if (m_image == 0) return;` 复现该岛；
    // 原 `if (m_image) { … }` 形 4.4 jump-thread 掉 jmp+nop（19 vs 21 insn）。
    if (m_image == 0)
        return;
    m_width = 0;
    m_height = 0;
    delete m_image;   // ORIG reset 0x80df5ae 用标量 _ZdlPv（init 虽 _Znaj）
    m_image = 0;
}

#include "GreyImage/init.inc"
{
    reset();
    m_width = width;
    m_height = height;
    // ORIG 0x80df574：int×int 直接 imul mem 形（(size_t) cast 诱发寄存器中转）。
    m_image = new unsigned char[width * height];
    memset(m_image, fill, width * height);
}

#include "GreyImage/getWidth.inc"
{
    return m_width;
}

#include "GreyImage/getHeight.inc"
{
    return m_height;
}

#include "GreyImage/getImage.inc"
{
    return m_image;
}

#include "GreyImage/getImage_1.inc"
{
    return m_image;
}

#include "GreyImage/getAlpha.inc"
{
    return m_image[m_width * y + x];
}

#include "GreyImage/setAlpha.inc"
{
    m_image[m_width * y + x] = alpha;
}

#include "GreyImage/setAlphaAsMax.inc"
{
    // ORIG 0x80df636：像素地址与 alpha 各取 const uchar&，call std::max<unsigned_char>
    // 模板（-O0 外联），结果引用解引用写回像素。手写比较会内联成 cmp/jae（结构差）。
    unsigned char& p = m_image[m_width * y + x];
    p = std::max(p, alpha);
}

// [b28d-D9] N95：全部局部槽深恒差 8（4.4 槽基 ebp-8 vs 4.1 顶到 ebp）（ORIG 0x080df676 sz 0x0000012e）
#include "GreyImage/sampleAlpha.inc"
{
    int xi = x >> 16;
    int yi = y >> 16;
    if (xi < 0 || yi < 0 || xi >= m_width - 1 || yi >= m_height - 1)
    {
        return 0;
    }
    unsigned int fx = (unsigned int)x & 0xffff;
    unsigned int fy = (unsigned int)y & 0xffff;
    unsigned char a00 = getAlpha(xi, yi);
    unsigned char a10 = getAlpha(xi + 1, yi);
    unsigned char a01 = getAlpha(xi, yi + 1);
    unsigned char a11 = getAlpha(xi + 1, yi + 1);
    int top = (int)(((unsigned int)a00 * (0x10000 - fx) + (unsigned int)a10 * fx) >> 16);
    int bot = (int)(((unsigned int)a01 * (0x10000 - fx) + (unsigned int)a11 * fx) >> 16);
    return (int)(((unsigned int)top * (0x10000 - fy) + (unsigned int)bot * fy) >> 16);
}

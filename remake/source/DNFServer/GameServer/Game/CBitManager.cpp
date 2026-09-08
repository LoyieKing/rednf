// ============================================================================
// [b28d-38 判词] CBitManager::ShiftRight 0x0831c1.. DIFF：p167 CLEAR 0/0（无调用），
//  残差 N87（4.4 变长移位恒插 and $0x1f 掩码）+ N4（lea/shl 折叠）+ 帧槽差。
// CBitManager.cpp —— CBitManager 位流工具实现（ORIG 0x0831c09c..0x0831c3af）
//
// 语义来源：docs/class_func_reports/CBitManager/ 各 md 的 ## 汇编 + ## 反编译 C。
//
// 本 TU 自 WindowsPackFiles.cpp 拆出（batch-23 O_MiscC idx90，Main 批准）：
// ORIG 里 CBitManager 全族符号连续落在 0x0831c09c..0x0831c3af，而
// CObfuscator 在 0x08585888..，两者地址分段 ⇒ 原属不同编译单元。留在同一
// TU 内会引入与 ORIG 的人为偏差：4.4.7 见到同 TU 内 `CBitManager::~CBitManager()
// {}` 空体即推导 nothrow，从而对 CObfuscator::CObfuscator()（0x08585888）里的
// 临时 CBitManager 不发 EH landing pad（14 vs ORIG 29 insns）；拆出后
// WindowsPackFiles.cpp 只见声明，实测逐条复现 ORIG 29 条形（含 +0x2b..+0x41
// landing pad 与 -0x18(%ebp) 槽）。
// C++98 / 32 位 / O0；字段均为具名成员，无偏移宏。
// ============================================================================
#include "WindowsPackFiles.h"

#include <cstdlib>
#include <cstring>

// ===========================================================================
// CBitManager
// ===========================================================================
CBitManager::CBitManager()
    : m_data(0), m_buffer(0), m_size(0), m_field0c(0)
{
}                                                // 0x0831c09c

CBitManager::~CBitManager()
{
}                                                // 0x0831c0c8 空析构

void CBitManager::SetAll(int value, unsigned char* buf)     // 0x0831c0ce
{
    // ORIG 0x0831c0d4/0x0831c0db：i 声明置 0 + for 头再置 0（两条 movl $0 同
    // 槽）——原码即 `int i = 0; for (i = 0; ...)` 形，忠实保留。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        if (value == 0)
        {
            buf[i] = 0x00;
        }
        else
        {
            buf[i] = 0xff;
        }
    }
}

void CBitManager::ReadData(unsigned char* data, int size)   // 0x0831c116
{
    m_data = data;
    m_size = size;
    m_buffer = static_cast<unsigned char*>(malloc(size));
    memset(m_buffer, 0, size);
}

unsigned char CBitManager::GetBitFromLSB(int bits)          // 0x0831c160
{
    // ORIG 0x0831c160：栈上 8 字节掩码表**逐元素赋值**（非初始化列表——P60
    // 列表形变 .rodata 模板+rep movs，ORIG 为 8 条 movb 立即数）；下标 bits-1；
    // -0x10 dead int 零初始化槽（P31）；ShiftRight 第 3 实参 = mov (%eax) =
    // this+0 即 m_data（此前 m_buffer 系误转，0x0831c1b8 实证）。
    unsigned char table[7];
    table[0] = 0x01;
    table[1] = 0x03;
    table[2] = 0x07;
    table[3] = 0x0f;
    table[4] = 0x1f;
    table[5] = 0x3f;
    table[6] = 0x7f;
    unsigned char ret = 0;
    int unused = 0;
    (void)unused;
    unsigned char mask = table[bits - 1];
    ret = m_data[m_size - 1] & mask;
    ShiftRight(bits, m_data);
    return ret;
}

// [N 类标注] 本函数与 ShiftLeft（0x0831c266）各 we-bigger 4（57/61、58/62）：
// 逐块比对确认差异全在**变长移位的计数掩码**——4.1 对 `shr/shl %cl` 不插
// `and $0x1f`（ORIG 侧仅 `sub 0xc(%ebp),%edx` 后直接 `shl %cl`），4.4 对每一处
// 变长移位一律前置 `and $0x1f,%edx`（实测各 3 处），另 ORIG 多一条重复
// `movl $0x0,-0x10(%ebp)` 循环量 init。源码侧 `bits & 0x1f` 已按 ORIG 语义保留，
// 掩码条数不可控 ⇒ 范式库 N 类「跨版本 变长移位计数掩码」，语义等价，勿再耗轮次。
void CBitManager::ShiftRight(int bits, unsigned char* buf)  // 0x0831c1d6
{
    for (int i = m_size; i > 0; --i)
    {
        buf[i - 1] = static_cast<unsigned char>(buf[i - 1] >> (bits & 0x1f));
        if ((i - 1) != 0)
        {
            buf[i - 1] = static_cast<unsigned char>(
                (static_cast<unsigned int>(buf[i - 2])
                     << ((8 - (bits & 0x1f)) & 0x1f)) |
                buf[i - 1]);
        }
    }
}

void CBitManager::ShiftLeft(int bits, unsigned char* buf)   // 0x0831c266（同 [N 类标注]）
{
    // P191 配套：ORIG 0x0831c26f/276 双 `movl $0x0,-0x10` = `int i=0;`+`for(i=0;..)`
    // 双初始化；移位掩码实测两侧均无 and（旧 N 标注掩码差已失效，仅剩此条）。
    int i = 0;
    for (i = 0; i < m_size; ++i)
    {
        buf[i] = static_cast<unsigned char>(buf[i] << bits);
        if (i != m_size - 1)
        {
            buf[i] = static_cast<unsigned char>(
                buf[i] | (buf[i + 1] >> (8 - bits)));
        }
    }
}

unsigned char CBitManager::GetBit(int idx, int bitLo, int bitHi) // 0x0831c2fc
{
    // ORIG 0x0831c2fc：int 循环槽先置 0（-0x8）、acc/ret 置 0（-0x2/-0x1），
    // 栈掩码表逐元素赋值，(int)b<=bitHi setle 形循环。
    int b = 0;
    unsigned char acc = 0;
    unsigned char ret = 0;
    unsigned char masks[8];
    masks[0] = 0x01;
    masks[1] = 0x02;
    masks[2] = 0x04;
    masks[3] = 0x08;
    masks[4] = 0x10;
    masks[5] = 0x20;
    masks[6] = 0x40;
    masks[7] = 0x80;
    for (b = bitLo; b <= bitHi; ++b)
    {
        acc = static_cast<unsigned char>(acc | masks[b]);
    }
    ret = static_cast<unsigned char>(m_data[idx] & acc);
    return ret;
}

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// SBlock：ORIG 导出 (unsigned int, unsigned int) ctor W @0x08099ef6、
// 拷贝 ctor W @0x080aa2f8、operator^= W @0x080aa314；
// BlockToBytes W @0x080aa503 / BytesToBlock W @0x080aa3d0。
// 布局：两个 unsigned int（+0x00/+0x04，C1ERKS/op^=/Encrypt 读 0(%eax)/4(%eax) 实证），
// sizeof 8。
#ifndef DNF_GAME_SBLOCK_H_
#define DNF_GAME_SBLOCK_H_

class SBlock
{
public:
    SBlock(unsigned int a, unsigned int b)   // ORIG W @0x08099ef6：+0=a、+4=b
        : m_data0(a), m_data1(b)
    {
    }

    SBlock(SBlock const& o)                  // ORIG W @0x080aa2f8：逐字段拷贝
        : m_data0(o.m_data0), m_data1(o.m_data1)
    {
    }

    SBlock& operator^=(SBlock& o)      // ORIG W @0x080aa314：两字段异或，返回 this
    {
        m_data0 ^= o.m_data0;
        m_data1 ^= o.m_data1;
        return *this;
    }

    unsigned int m_data0;   // +0x00
    unsigned int m_data1;   // +0x04
};

// ORIG _Z4Bytej @0x080aa341（定义在 CBlowFish.cpp）：32 位直返低字节。
unsigned char Byte(unsigned int v);

// ORIG W @0x080aa503（objdump 实证）：先 +0x04 后 +0x00，每字节
// 「v = 字段[>>N]（具名局部 -0xc）; --buf; *buf = Byte(v)」。
// [b28d-32 AE] 重写后 83=83 逐条全等（仅 .o 重定位标签差） | ORIG 0x080aa503]
inline void BlockToBytes(SBlock const& block, unsigned char* buf)
{
    unsigned int v = block.m_data1;
    *--buf = Byte(v);
    v = block.m_data1 >> 8;
    *--buf = Byte(v);
    v = block.m_data1 >> 16;
    *--buf = Byte(v);
    v = block.m_data1 >> 24;
    *--buf = Byte(v);
    v = block.m_data0;
    *--buf = Byte(v);
    v = block.m_data0 >> 8;
    *--buf = Byte(v);
    v = block.m_data0 >> 16;
    *--buf = Byte(v);
    v = block.m_data0 >> 24;
    *--buf = Byte(v);
}

// ORIG W @0x080aa3d0（objdump 实证）：先清零 +0x00，逐字节
// 「v = *buf++; v <<= N; field = field | v」；+0x04 同法。
// [b28d-32 IDENTICAL] 重写后 103=103 逐条全等 | ORIG 0x080aa3d0]
inline void BytesToBlock(unsigned char const* buf, SBlock& block)
{
    unsigned int v;
    block.m_data0 = 0;
    v = *buf++;
    v <<= 24;
    block.m_data0 = block.m_data0 | v;
    v = *buf++;
    v <<= 16;
    block.m_data0 = block.m_data0 | v;
    v = *buf++;
    v <<= 8;
    block.m_data0 = block.m_data0 | v;
    v = *buf++;
    block.m_data0 = block.m_data0 | v;
    block.m_data1 = 0;
    v = *buf++;
    v <<= 24;
    block.m_data1 = block.m_data1 | v;
    v = *buf++;
    v <<= 16;
    block.m_data1 = block.m_data1 | v;
    v = *buf++;
    v <<= 8;
    block.m_data1 = block.m_data1 | v;
    v = *buf++;
    block.m_data1 = block.m_data1 | v;
}

#endif  // DNF_GAME_SBLOCK_H_

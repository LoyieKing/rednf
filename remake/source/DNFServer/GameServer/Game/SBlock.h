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

// ORIG W @0x080aa503：把块按大端序展开为 8 字节（先 +0x04 后 +0x00，
// 每次 shr 8 写向 buf 尾部）。
inline void BlockToBytes(SBlock const& block, unsigned char* buf)
{
    unsigned int hi = block.m_data1;
    buf[7] = (unsigned char)(hi);
    buf[6] = (unsigned char)(hi >> 8);
    buf[5] = (unsigned char)(hi >> 16);
    buf[4] = (unsigned char)(hi >> 24);
    unsigned int lo = block.m_data0;
    buf[3] = (unsigned char)(lo);
    buf[2] = (unsigned char)(lo >> 8);
    buf[1] = (unsigned char)(lo >> 16);
    buf[0] = (unsigned char)(lo >> 24);
}

// ORIG W @0x080aa3d0：从 8 字节大端序恢复块（+0x00 = b[0..3]、+0x04 = b[4..7]）。
inline void BytesToBlock(unsigned char const* buf, SBlock& block)
{
    block.m_data0 = ((unsigned int)buf[0] << 24) |
                    ((unsigned int)buf[1] << 16) |
                    ((unsigned int)buf[2] << 8) |
                    (unsigned int)buf[3];
    block.m_data1 = ((unsigned int)buf[4] << 24) |
                    ((unsigned int)buf[5] << 16) |
                    ((unsigned int)buf[6] << 8) |
                    (unsigned int)buf[7];
}

#endif  // DNF_GAME_SBLOCK_H_

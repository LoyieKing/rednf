#ifndef DNF_GAME_SKILL_USING_VALIDATE_INFO_H_
#define DNF_GAME_SKILL_USING_VALIDATE_INFO_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + DisPatcherCore.cpp@7086（源码定义复制）
// ORIG W ctor _ZN24SKILL_USING_VALIDATE_INFOC1Eihcc 0x08234756（int, uchar, char, char）。

// 布局按 ctor 逐字节（DisPatcherCore.cpp TU 局部镜像权威化）：
// +0x00 int job / +0x04 u8 slot / +0x05 s8 firstGrowType / +0x06 s8 secondGrowType；
// sizeof 8（+0x07 尾 pad，默认对齐）。
struct SKILL_USING_VALIDATE_INFO
{
    int m_job;               // +0x00
    unsigned char m_slot;    // +0x04
    char m_firstGrowType;    // +0x05
    char m_secondGrowType;   // +0x06

    SKILL_USING_VALIDATE_INFO(int job, unsigned char slot,
                              char firstGrowType, char secondGrowType)
    {   // ORIG 0x08234756：四成员顺序赋值
        m_job = job;
        m_slot = slot;
        m_firstGrowType = firstGrowType;
        m_secondGrowType = secondGrowType;
    }
};

typedef char skill_using_validate_info_size_check[(sizeof(SKILL_USING_VALIDATE_INFO) == 8) ? 1 : -1];

#endif  // DNF_GAME_SKILL_USING_VALIDATE_INFO_H_

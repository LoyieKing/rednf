// STGrowthType：ORIG 仅导出弱内联 ctor W @0x08486bce（写入 +0x0 = 0xff）；
// 成员布局无进一步证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STGROWTHTYPE_H_
#define DNF_GAME_STGROWTHTYPE_H_

class STGrowthType
{
public:
    unsigned char m_growthType;   // +0x0：ctor 置 0xff（ORIG 0x08486bce）

    STGrowthType() { m_growthType = 0xff; }   // ORIG W @0x08486bce（内联化）
};

#endif  // DNF_GAME_STGROWTHTYPE_H_

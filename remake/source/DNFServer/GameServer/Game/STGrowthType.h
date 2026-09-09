// STGrowthType：ORIG 仅导出弱内联 ctor W @0x08486bce（写入 +0x0 = 0xff）；
// 成员布局无进一步证据，sizeof 未证实故不加 static_assert。
#ifndef DNF_GAME_STGROWTHTYPE_H_
#define DNF_GAME_STGROWTHTYPE_H_

class STGrowthType
{
public:
    // +0x0：整字节视角（ctor 置 0xff，ORIG 0x08486bce `movb $0xff,(%eax)`）；
    // 位域视角依据 CDungeonSuccessNode::C1 0x08484fc1..0x08485011 的 RMW 掩码
    // （低 4 位掩码 0xfffffff0、bit4-6 掩码 0xffffff8f）与 `shl$4/sar$4`、`shl$5/sar$5`
    // 窄化序列 ⇒ 原码是 {4 位, 3 位, 1 位} 位域赋值，非手写掩码。
    // 位域底层类型 = **signed char**：ORIG 0x08484fcb/0x08484ff7 的 `shl$4; sar$4,%al`
    // 与 `shl$5; sar$5,%al` 是有符号窄化；unsigned 位域在 4.4 恒折 `and$0xf/and$0x7`
    // （探针 /tmp/b28d_s218_bf.cpp w1/w5 vs w2/w6）。值域 sub 0..5、grow 0..2 不受影响。
    union
    {
        unsigned char m_growthType;
        struct
        {
            signed char m_sub : 4;    // bit0-3：sub（JobKey::m_sub 来源）
            signed char m_grow : 3;   // bit4-6：grow
            signed char m_rsv : 1;    // bit7
        };
    };

    STGrowthType() { m_growthType = 0xff; }   // ORIG W @0x08486bce（内联化）
};

#endif  // DNF_GAME_STGROWTHTYPE_H_

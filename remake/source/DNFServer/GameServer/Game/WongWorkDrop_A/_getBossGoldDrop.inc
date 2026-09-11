// ---- _getBossGoldDrop — ORIG T 0x08535dc8 ----
// [b28d-solo-WongWorkDrop_A] verdict=AE：真语义 bug 修（首参 ORIG 为
// m_conditionRate[type].value + .rate，旧码误取 m_goldQuantity[2*type]）；
// m_conditionRateW[4][2] 二维视图复现 disp32 折叠、m_goldQuantityS[2] 结构体
// 数组视图复现 `add $K` + scaled4 折叠；正向 if(type<4) 包裹（P222）。
// [b28d-solo-WongWorkDrop_A] 真语义 bug 修正：ORIG 首参用
// m_conditionRate[type].value + m_conditionRate[type].rate（0x25938a/0x259388
// 两半字），旧码误取 m_goldQuantity[2*type]；且 ORIG 无 probSlot/baseSlot/
// goldLo/goldHi 具名局部（P0 寄存器直传），tier 是唯一局部（-0xc）。
// 分支形：`ja <尾部 mov $0>` 失败块在函数尾 ⇒ 正向 if(type<4){…return true}
// return false（P222 正向包裹）。
bool CMonsterDrop::_getBossGoldDrop(unsigned char type, unsigned short& outGold,
                                    unsigned short& outCount)
{
    if (type < 4)
    {
        int tier = 0;
        // ORIG：value 先物化 %ebx 跨 generateNumber 保活，实参和式
        // value + rate 各自独立重读（无局部）。
        if (static_cast<int>(m_conditionRateW[type][1]) >
            m_random.generateNumber(
                0, static_cast<int>(m_conditionRateW[type][1]) +
                       static_cast<int>(m_conditionRate[type].rate)))
            tier = 1;                                  // setg：value > roll → 高档

        outGold = static_cast<unsigned short>(
            m_random.generateNumber(0, m_goldQuantityS[tier].m_hi -
                                           m_goldQuantityS[tier].m_lo) +
            m_goldQuantityS[tier].m_lo);
        outCount = m_goldVolume[tier];                 // (tier+0x12c9d0)*2 → 步长2
        return true;
    }
    return false;
}
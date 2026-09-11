// ORIG stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t() 0x0836f2cc
// 指令序：word+0x1a4=1 → word+0x1a6=0 → word+0x1a8=0 → memset(this+0x12,0,0x191)
// （词写与 memset 区间不相交，顺序保持 ORIG 形态）。+0x11 与 +0x1aa 保持未初始化。
stFatigueBatteryConstantTable_t::stFatigueBatteryConstantTable_t()
{
    m_capLimit = 0;                        // movw $0x0,(%eax)
    m_chargeRate4 = 0.0f;                  // movl $0x0,0x4(%eax)（float 槽位清零）
    m_chargeDivisor8 = 0x28;               // movb $0x28,0x8(%eax)
    m_minChargeLevel9 = 0;                 // movb $0x0,0x9(%eax)
    m_fieldc = 0;                          // movl $0x0,0xc(%eax)
    m_endLevel10 = 0x46;                   // movb $0x46,0x10(%eax)
    m_activeSwitch1a4 = 1;                 // movw $0x1,0x1a4(%eax)
    m_bonusMultiplier1a6 = 0;              // movw $0x0,0x1a6(%eax)
    m_batteryThreshold1a8 = 0;             // movw $0x0,0x1a8(%eax)
    memset(&m_data11[1], 0, 0x191);        // this+0x12 起 0x191 字节
}
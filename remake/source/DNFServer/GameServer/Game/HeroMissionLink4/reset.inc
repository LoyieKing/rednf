// ---------------------------------------------------------------------------
// ORIG _ZN16HeroMissionValue5resetEv T 0x08164bb2（0x17，LinkFinal2 核销）：
// 仅 +0x04/+0x06 两个 ushort 清零（objdump 逐指令转写）。
// ---------------------------------------------------------------------------
void HeroMissionValue::reset()
{
    m_field4 = 0;   // ORIG movw $0x0,+0x04
    m_field6 = 0;   // ORIG movw $0x0,+0x06
}
// ORIG 0x085805a8 T：
//   参数布局 (uint mood, ulong unk[保留], uint count, CItem const*, result&)；
//   局部 short r：r<=0 直返；否则 r *= getFavorRatePerMood(getMood())（filds/fmuls/fistps）。
unsigned short CNPCDynamicInfo::giveGiftItem(
    unsigned int mood, unsigned long unk, unsigned int count,
    CItem const* item, ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const
{
    short r = m_script->giveGiftItem(mood, item, count, result);
    if (r > 0) {
        r = (short)((float)r * m_script->getFavorRatePerMood(getMood()));
    }
    return r;
}
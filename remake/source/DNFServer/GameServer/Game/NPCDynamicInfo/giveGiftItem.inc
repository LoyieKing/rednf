unsigned short CNPCDynamicInfo::giveGiftItem(
    unsigned int mood, unsigned long unk, unsigned int count,
    CItem const* item, ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const
{
    short r = m_script->giveGiftItem(mood, item, count, result);
    if (r > 0) {
        r = (short)((float)r * m_script->getFavorRatePerMood(getMood()));
    }
    return r;

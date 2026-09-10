void cUserHistoryLog::RandomOption(const Inven_Item& item, randomOptionReason::T reason)
{
    const unsigned char* opt =
        reinterpret_cast<const unsigned char*>(&item.m_random.m_random);
    m_user->LogHistory(
        "RandomOptionItem,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d",
        item.m_addInfo,
        opt[0], opt[1], opt[2], opt[3], opt[4], opt[5], opt[6],
        opt[7], opt[8], opt[9], opt[10], opt[11], opt[12], opt[13],
        (int)reason);

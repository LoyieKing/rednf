int CQuestShop::buyStatus(CUser* user, unsigned char status,
                          unsigned char count, short& result)
{
    int buyCount = getStatusBuyingCount(status);    // -0x18
    if (buyCount == -1)                             // +0x33 jne
        return 1;
    result = static_cast<short>(static_cast<unsigned short>(count) +
                                static_cast<unsigned short>(buyCount));
    const bool limited = checkBuyingStatus_LimitLevel(status, result,
                                                      user->get_charac_level());
    if (!limited)                                   // ORIG mov %al,-0x11 → xor $1
        return 0xe;                                 // +0x93
    const int beforeQp = getQp();                   // -0x10
    const int subRet = subQp(status, count);        // -0xc
    if (subRet != 0)                                // +0xca je → return subRet
        return subRet;
    incStatusBuyingCount(status, count);
    user->m_historyLog.QPSub(beforeQp, getQp(), static_cast<int>(status));
    return 0;

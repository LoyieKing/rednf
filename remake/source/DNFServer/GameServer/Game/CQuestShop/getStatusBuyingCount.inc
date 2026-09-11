// ---- ORIG T 0x85ef02e size 0xa0：读 m_buyingStatus[type] -----------------
short CQuestShop::getStatusBuyingCount(unsigned char status)
{
    const int type = findStatusType(status);      // -0xc
    if (type == 0x4d || type == -1)               // +0x2e/+0x34
        return -1;
    // ORIG 0x85ef02e：type 收窄 byte 后非 const find/end/operator->
    std::map<unsigned char, short>::iterator it =
        m_buyingStatus.find(static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())               // +0x84
        return it->second;                        // movzwl 0x2(%eax) 直返 16 位
    return 0;                                     // +0x99
}
void CQuestShop::incStatusBuyingCount(unsigned char status, unsigned char count)
{
    const int type = findStatusType(status);      // -0x28
    if (type == 0x4d)                             // +0x28 je → 直接返回
        return;
    alter();                                      // charac_expand::CData::alter
    // ORIG 0x85eef28：type 收窄 byte 后非 const find/end
    std::map<unsigned char, short>::iterator it = m_buyingStatus.find(
        static_cast<unsigned char>(type));
    if (it != m_buyingStatus.end())
    {
        it->second += static_cast<short>(count);  // ORIG lea 累加后 mov %dx,0x2(%eax)
        return;
    }
    // ORIG make_pair<IRiRKh>(type, count) 左值 + pair 移动 + insert
    m_buyingStatus.insert(std::make_pair(
        static_cast<const int&>(type),
        static_cast<const unsigned char&>(count)));

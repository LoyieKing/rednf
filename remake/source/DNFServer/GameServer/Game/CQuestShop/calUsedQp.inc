int CQuestShop::calUsedQp()
{
    unsigned char qp = 0;                                  // -0x19
    int used = 0;                                          // -0x18
    std::map<unsigned char, short>::iterator end =
        m_buyingStatus.end();                              // -0x20（ORIG 循环前缓存）
    for (std::map<unsigned char, short>::iterator it =
             m_buyingStatus.begin();                       // -0x24
         it != end; ++it)
    {
        int itemStatus = -1;                              // -0x14（循环内首声明）
        const int statusIdx = static_cast<int>(it->first); // -0x10
        const int count = static_cast<int>(it->second);    // cwtl -0xc
        if (statusIdx > 0x4c)                              // cmpl $0x4c → return -1
            return -1;
        itemStatus = static_cast<signed char>(
            G_CDataManager()->m_questShop.m_pad28[statusIdx]); // ORIG 每次重读 G_CDataManager
        qp = static_cast<unsigned char>(findStatusQp(itemStatus));
        if (qp == 0)                                       // +0xbc cmpb jne
            return -1;
        used += static_cast<int>(qp) * count;              // imul/add
    }
    return used;                                           // +0xd2 循环外

void CQuestShop::makePacket_BuyingStatusList(PacketGuard& packet, bool flag)
{
    // ORIG 0x85ef5dc：flag 分支选 map（if(!flag) 取反布局）
    std::map<unsigned char, short>* status;   // ORIG 非 const begin/end
    if (!flag)
        status = &m_buyingStatus;
    else
        status = &m_tagBuyingStatus;
    const unsigned char sz = static_cast<unsigned char>(status->size()); // -0x15
    packet.put_byte(static_cast<int>(sz));

    std::map<unsigned char, short>::iterator end = status->end(); // -0x20 循环前
    for (std::map<unsigned char, short>::iterator it =
             status->begin();                    // -0x24
         it != end; ++it)
    {
        int itemStatus = -1;                     // -0x14
        const int itemIndex = static_cast<int>(it->first); // -0x10
        int count = static_cast<int>(it->second); // -0xc
        if (itemIndex > 0x4c)                    // ORIG jle 跳计算分支（fall-through 超界）
        {
            itemStatus = -1;
            count = 0;
        }
        else
        {
            // ORIG 每次重读 G_CDataManager
            itemStatus = static_cast<signed char>(
                G_CDataManager()->m_questShop.m_pad28[itemIndex]);
        }
        packet.put_byte(itemStatus);
        packet.put_short(count);
    }
}
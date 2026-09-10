int Arad_GiftItem_Set::send_mail(CUser* user, int p2, int p3, int p4, int giftIdx)
{
    Data* data = find(giftIdx, p4, p3);
    if (data == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x15d, "[ERROR] send mail - Data None ");
        return 0;
    }

    ARAD_DATAS::AradMailInfo info;
    info.m_fieldC = p2;
    info.m_isSystemMail = (unsigned char)user->GetServerGroup();
    info.m_name = const_cast<char*>(data->m_str4.c_str());
    info.m_content = const_cast<char*>(data->m_str8.c_str());
    info.m_field14 = (int)data->m_str8.length();

    info.m_item.m_fieldC = data->m_item;
    info.m_item.m_field8 = 2;
    info.m_item.m_field4 = 1;
    info.m_item.m_fieldA = 0;
    info.m_item.m_itemAttr = 0;

    Inven_Item inven;

    CItem* item = G_CDataManager()->find_item(info.m_item.m_fieldC);
    if (item == 0)
    {
        LogManager::logFormat(1, "localjapan/Arad_Script_Loader.cpp",
                              __PRETTY_FUNCTION__,
                              0x175, "[ARAD_RYOSIKA] Item Null. (index:%d)",
                              info.m_item.m_fieldC);
        return 0;
    }

    item->make_item(inven);
    info.m_item.m_fieldA = inven.m_fieldb;
    return ARAD::send_mail(info);

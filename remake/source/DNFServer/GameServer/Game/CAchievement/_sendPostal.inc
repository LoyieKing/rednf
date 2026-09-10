void CAchievement::_sendPostal(int itemIdx, int addInfo, std::string sender,
                               std::string subject)
{
    Inven_Item item;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (pItem == 0)
    {
        return;
    }
    item.m_addInfo = itemIdx;
    pItem->make_item(item);
    item.set_add_info(addInfo);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender.c_str(), item, 0, (unsigned int)m_user->getCurCharacNo(),
        subject.c_str(), (int)subject.size(), 0, m_user->GetServerGroup(),
        false, false);

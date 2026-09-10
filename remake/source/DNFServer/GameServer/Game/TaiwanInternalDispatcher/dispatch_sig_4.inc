int InterInsertGiveBoxEventReward::dispatch_sig(CUser* user, char* buf, int len)
{
    char* msg = buf;
    if (msg == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1dc, 5);
        tr("[Taiwan, GiveMeBoxEvent] msg is null.");
        return 0x1dd;
    }
    // §4.5 正解：真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h 权威）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0xa5)->IsEventing(0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(EventGiveMeBox::kRewardItem_);
    if (item == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1e6, 5)(
            "[Taiwan, GiveMeBoxEvent] Invalid item index. (%u)",
            EventGiveMeBox::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    inv.m_addInfo = item->get_index();   // ORIG 0x08161b9c mov %eax,-0x73（inv+0x02）
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_givemebox2_title", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_givemebox2_mail", 0);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0xe,
        user->GetServerGroup(), false, false);
    return 0;

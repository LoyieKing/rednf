int InterCheckStayTimeEvent::dispatch_sig(CUser* user, char* buf, int len)
{
    char* msg = buf;
    if (msg == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0xc9, 5);
        tr("[Taiwan, StayEvent] SigStayTimeEvent is null.");
        return 0xca;
    }
    // ORIG 直调 vtable+0x34 槽（同 TaiwanInternalDispatcher.cpp 既有口径，保 ABI；无空检）。
    // §4.5 正解：真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h 权威）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9f)->IsEventing(0) != '\x01')
    {
        return 0;
    }
    CItem* item = G_CDataManager()->find_item(CEventStayTime::kRewardItem_);
    if (item == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xd3, 5)(
            "[Taiwan, StayEvent] Invalid item index. (%u)", CEventStayTime::kRewardItem_);
        return 0;
    }
    Inven_Item inv;
    inv.m_addInfo = item->get_index();   // ORIG 0x08160b54 mov %eax,-0x73（inv+0x02）
    item->make_item(inv);
    inv.set_add_info(1);
    const char* title = g_scriptStringManager_.findString(4, "tw_ccb_stress_title_01", 0);
    const char* mail = g_scriptStringManager_.findString(4, "tw_ccb_stress_mail_01", 0);
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, inv, 0, (unsigned int)user->getCurCharacNo(), mail, (int)strlen(mail), 0xe,
        user->GetServerGroup(), false, false);
    return 0;

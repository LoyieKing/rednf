// ===== InterInsertGiveBoxEventReward::dispatch_sig —— ORIG 0x08161a8a =====
// 同 InterCheckStayTimeEvent 模式：event 0xa5、道具 EventGiveMeBox::kRewardItem_、
// 脚本串 tw_givemebox2_title/tw_givemebox2_mail；buf==0 → 日志(0x1dc) 返回 0x1dd。
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N7+N18a 反向封顶（同 StayTime 形因；p167 16/16 CLEAR）
// [M2S6 2026-09-11 复核] 同 StayTime：p167 余项 = C1/C2 别名族（非缺口）；装载形 N18a 维持
// （探针 /tmp/m2s6_probe2.o）；语义（event 0xa5 / tw_givemebox2_* / 行号 0x1dc·0x1e6 / 返回 0x1dd）逐条核 ✓。
// [N1F 2026-09-11 复核] 同 StayTime：本条唯一残差 = `mov <A>,%eax; mov %eax,%ebx` 二段装载
//   （EventGiveMeBox::kRewardItem_ 同属 .h 声明类型轴，见上条探针三条证据）⇒ 封顶维持。
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
}
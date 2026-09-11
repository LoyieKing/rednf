// ===== InterCheckStayTimeEvent::dispatch_sig —— ORIG 0x08160a42 =====
// buf==0 → cMyTrace(0xc9) 日志 "[Taiwan, StayEvent] SigStayTimeEvent is null." 返回 0xca；
// GetRepeatEvent(0x9f) 经 vtable+0x34 槽（ORIG IsEventing）判活，!=1 返回 0；
// 奖励道具 CEventStayTime::kRewardItem_E（0x2920908d）find_item 失败 → 日志(0xd3) 返回 0；
// 否则 make_item 填 Inven_Item、set_add_info(1)，取标题/正文脚本串，经
// ReqDBSendNewSystemMail(…, 0xe, group, false, false) 发系统邮件，返回 0。
// [b28d-solo-TaiwanInternalDispatcher] DIFF·N7+N18a 反向封顶（虚调 make_item 目标 %eax vs
// ORIG %edx；kRewardItem_ 装载单段 vs ORIG 两段；p167 16/16 CLEAR、gap 0）
// [M2S6 2026-09-11 复核] p167 余项 = C1/C2 别名族（ORIG Inven_ItemC1≡C2@0x080cb854、
// cMyTraceC1≡C2@0x0854f718）⇒ 非缺口；kRewardItem_ 装载经探针 /tmp/m2s6_probe2.o 证 4.4 对
// 静态成员地址恒单段直达 callee-saved（4.1 走 %eax 中转两段），N18a 维持；语义逐条核 ✓。
// [N1F 2026-09-11 复核+探针] 本次以真后端（c6root 驱动 + cc1plus446）复测装载形，证据：
//   ORIG 0x08160ad3 字节实测 `a1 a0 00 b8 08` + `89 c3`（= `mov 0x08b800a0,%eax` →
//   `mov %eax,%ebx`，abs 形 a1 编码），我方 4.4 恒 `8b 1d <A>` 单段直落 %ebx ⇒ 差 1 条。
//   探针（同后端）三形：① 实参声明类型为 unsigned（`extern unsigned` 或 typedef) ⇒ 复现
//   ORIG 二段形（`mov <A>,%eax; mov %eax,%ebx`）逐字节同；② 显式 cast `(unsigned int)`
//   于 int 静态 ⇒ 前端折叠成单段（不生效）；③ `*(const unsigned int*)&X` ⇒ 走地址形
//   3 条（`mov $A,%eax; mov (%eax),%eax; mov %eax,%ebx`）更劣。
//   ⇒ 唯一源形是**声明类型**，而 kRewardItem_ 在 CEventStayTime.h（非本泳道写域）⇒ N18a 封顶。
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
}
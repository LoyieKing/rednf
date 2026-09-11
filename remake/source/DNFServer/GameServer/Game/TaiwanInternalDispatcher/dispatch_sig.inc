// ---- dispatch_sig ----
// ORIG T 0x08161012。第二参 buf 实际承载 Msg*（判空后读 +0x0/+0x4）。
// 流程：buf==0 → 日志 "interMsg is null." 返回 0x12f；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x139；
//   否则：SetEventCreateDnfReward(buf+4)；发 0x22d 包（rate + rewardable byte），返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 InterSelectCreateDnfEventInfo 同口径，
//   见其 dispatch_sig 注释；ORIG 槽序与 CGuildServerProxy.h 注释有出入，用直调保 ABI）。
// [b28d-solo-TaiwanInternalDispatcher] AE（Msg 视图+eventing 去槽+rewardable char+内联变参）
int InterSelectCreateDnfUserInfo::dispatch_sig(CUser* user, char* buf, int len)
{
    Msg* msg = reinterpret_cast<Msg*>(buf);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x12e,
                              "[Taiwan, Event] interMsg is null.");
        return 0x12f;
    }
    CEventCreateDnf* event =
        (CEventCreateDnf*)GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    // §4.5 真实虚调用 CEventBase::IsEventing(CUser*)@vtable+0x34（CGuildServerProxy.h
    // 权威）。与 ORIG `call *%edx` 相比 GCC 4.4 用 `call *%eax`（寄存器分配 N7 类差），
    // 实测 typedef 直调与该实调用指令数同为 148（+9 系函数既有 N82 帧差，非本转换引入）。
    if (event == 0 ||
        event->IsEventing(0) != '\x01')
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0x138,
                              "[Taiwan, Event] stayEvent=%s eventing:%d",
                              (event == 0) ? "nullptr" : "ok",
                              event->IsEventing(0));
        return 0x139;
    }
    char rewardable = 0;
    if (!msg->m_received && event->IsRewardCondition())
    {
        rewardable = 1;
    }
    user->SetEventCreateDnfReward(msg->m_received);
    PacketGuard pg;
    pg.put_header(0, 0x22d);
    pg.put_short((unsigned short)event->GetRate());
    pg.put_byte(rewardable);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
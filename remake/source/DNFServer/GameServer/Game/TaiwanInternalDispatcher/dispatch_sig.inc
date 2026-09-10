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

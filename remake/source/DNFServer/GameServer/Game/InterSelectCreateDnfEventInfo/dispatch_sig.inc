int InterSelectCreateDnfEventInfo::dispatch_sig(CUser* user, char* data, int len)
{
    // [b28d-56 判词·N] N7/P282：vcall 偏移已核（两处 vptr+0x34 = _ZTV+0x3c =
    // word[15]，P48 无 CSE 各自独立调用，P167 CLEAR 0 missing/0 extra）；残余为
    // 4.4.7 间接 call 目标恒 %eax vs ORIG %edx（N7，4.4.6 后端可翻，本 TU 未路由）
    // + 槽号平移 4（msg -0x14 vs -0x10）。真语义 bug 已修（m_rate/m_hour）。
    Msg* msg = reinterpret_cast<Msg*>(data);
    if (msg == 0)
    {
        LogManager::logFormat(1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
                              __PRETTY_FUNCTION__, 0xfd,
                              "[Taiwan, Event] interMsg is null.");
        return 0xfe;
    }
    CEventCreateDnf* event =
        (CEventCreateDnf*)GlobalData::s_event_manager->GetRepeatEvent(0xa0);
    typedef bool (*IsEventingFn)(void*, void*);
    // [b28d-56 判词·AE] ORIG 0x08160dbf 段逐条：`cmpl $0,event; je L1` +
    // `call *vptr+0x34; xor $1; test; je L2`（P214① 正条件 `||` 物化：L1 mov $1 /
    // L2 mov $0 → test）；未进行分支第二次虚调用**内联在 logFormat 实参位**
    // （P44/P92：ORIG 0x08160e0e `call *%edx; movzbl %al,%edx` 后直接压栈，
    // 无 fn/ev 具名局部）。第二次调用的 vtable 偏移同为 +0x34（P282：vptr+0x34
    // = _ZTV+0x3c = word[15]；两处独立调用，P48 无 CSE）。
    if (event == 0 ||
        !reinterpret_cast<IsEventingFn>(
            (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0))
    {
        // P0：ORIG 无 stay 具名槽（`mov $"nullptr",%eax; jmp; mov $"ok",%eax`
        // 直接物化进 %eax 后压 0x14(%esp)）⇒ 三目内联在实参位。
        LogManager::logFormat(
            1, "localtaiwan/Dispatcher/Taiwan_InterDispatcher.cpp",
            __PRETTY_FUNCTION__, 0x107,
            "[Taiwan, Event] stayEvent=%s eventing:%d",
            (event == 0) ? "nullptr" : "ok",
            (int)(unsigned char)reinterpret_cast<IsEventingFn>(
                (*reinterpret_cast<void***>(event))[0x34 / 4])(event, 0));
        return 0x108;
    }
    // [b28d-56 真语义 bug 修正] ORIG 0x08160e63 判/写的是 msg+0x00 = m_rate
    // （makeRequest 0x08160fb2 `msg+0 = param_1(rate)` 实证），旧码误用 m_hour
    // （msg+0x04）。SetEventInfo 实参序 (msg+4, msg+0, msg+8) = (m_hour, m_rate,
    // m_eventDay) 与现调用一致。
    if ((int)msg->m_rate < 0)
    {
        msg->m_rate = (int)((unsigned short)event->GetRate());
    }
    event->SetEventInfo(msg->m_hour, msg->m_rate, msg->m_eventDay);
    // P0：ORIG 无 cond/rate 具名槽（IsRewardCondition 结果留 %ebx、GetRate 直传），
    // 故内联为 ctor 实参。
    Reward reward((int)(unsigned short)event->GetRate(), event->IsRewardCondition());
    G_GameWorld()->ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(reward);
    return 0;

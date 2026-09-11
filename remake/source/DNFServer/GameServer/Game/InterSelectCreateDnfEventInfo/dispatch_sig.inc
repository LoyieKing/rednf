// ---- dispatch_sig ----
// ORIG 0x8160d5e。第二参 data 实际承载 Msg*（ORIG 汇编 0x10(%ebp)=data 判空；
// Ghidra C 的 param_3 标注为反编译器 thiscall 参数索引偏差，以汇编为准）。
// 流程：
//   data==0 → 日志 "interMsg is null."，返回 0xfe；
//   event = GetRepeatEvent(0xa0)；
//   if (event==0 || vtable+0x34 槽(event,0) != 1) 视为「事件未进行」：
//       再调槽取字节 + 日志 stayEvent=ok/nullptr eventing:%d，返回 0x108；
//   否则：hour<0 时 hour=GetRate()&0xffff；SetEventInfo(rate,hour,eventDay)；
//       Reward(GetRate()&0xffff, IsRewardCondition()) 经 ProcessAllUsers 分发，返回 0。
// 注：vtable+0x34 槽为 CEventBase 判活直调（与 Dispatchers.cpp BingoQuiz
// check_error 同口径；CGuildServerProxy.h 槽位注释与 ORIG 槽序有出入，用直调保 ABI）。
int InterSelectCreateDnfEventInfo::dispatch_sig(CUser* user, char* data, int len)
{
    // [b28d-56 判词·N] N7/P282：vcall 偏移已核（两处 vptr+0x34 = _ZTV+0x3c =
    // word[15]，P48 无 CSE 各自独立调用，P167 CLEAR 0 missing/0 extra）；残余为
    // 4.4.7 间接 call 目标恒 %eax vs ORIG %edx（N7，4.4.6 后端可翻，本 TU 未路由）
    // + 槽号平移 4（msg -0x14 vs -0x10）。真语义 bug 已修（m_rate/m_hour）。
    // [P1T2 2026-09-11 复验] 现存残余**仅**两处 call 目标寄存器（p167fix CLEAR、
    // 槽平移已不存在）：ORIG 0x08160ddd / 0x08160e0e 均
    // `mov (%eax),%edx; mov -0xc(%ebp),%eax; movl $0x0,0x4(%esp); mov %eax,(%esp);
    // call *%edx` vs 4.4.7 `mov (%eax),%eax; movl $0x0,0x4(%esp);
    // mov -0xc(%ebp),%edx; mov %edx,(%esp); call *%eax`。
    // A/B 实测：`-B cc1plus446bin` 反而 gap -1→-3（多一条 `mov %eax,%edx` 且
    // 实参序颠倒；`-B …/4.4.4/` 亦零变化）⇒ 旧注「4.4.6 后端可翻」被推翻，
    // 本件在本工具链下不可翻。
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
}
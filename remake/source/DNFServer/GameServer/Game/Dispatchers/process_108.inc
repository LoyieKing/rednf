int Dispatcher_SaveQuestNotify::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    saveQuestNotifyParam& res = static_cast<saveQuestNotifyParam&>(param);
    // ORIG 0x0825edfc：check_error 结果**无 err 槽**——`call; mov -0x14(%ebp),%edx;
    // mov %eax,0x4(%edx)` 直存 param+4，其后两判定都读回 `param->m_errorCode`（P0）。
    res.m_errorCode = check_error(user, msg);
    if (res.m_errorCode > 0)
    {
        return 0;
    }
    if (res.m_errorCode < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              __PRETTY_FUNCTION__,
                              0x2cb, "Dispatcher Check_Error No(%d)",
                              res.m_errorCode);
        return 0x2cc;
    }
    // err==0：ORIG 此处才物化 msg 指针槽 -0x10，并另立长度局部 -0xc（`movl $0x28,
    // -0xc(%ebp)` → `mov -0xc(%ebp),%edx; mov %edx,0x8(%esp)`，P0 槽往返）⇒ 具名 len。
    const MSG_SAVE_QUEST_NOTIFY& req =
        reinterpret_cast<const MSG_SAVE_QUEST_NOTIFY&>(msg);
    // ORIG 0x0825ee2x：`movl $0x28,-0xc(%ebp)` 之后 dst call，len 才在压参处
    // `mov -0xc(%ebp),%edx; mov %edx,0x8(%esp)` 从槽重读（不升格 callee-saved），
    // src 则在 call 前 `lea 0xd(%eax),%ebx` 跨 call 保活。
    // [实测两形] ① 普通 `int len`：55/57（len 升格 %ebx、src 被挤 %esi、帧 0x30+push esi）；
    // ② `volatile int len`：55/58（volatile 读仍升格 %ebx，反多 1 条装载）⇒ 两形均不闭合，
    // 残余为 4.1/4.4 outgoing 实参装载时刻差（P75/N-G），按 N 封顶，取较短的 ① 形。
    int len = 0x28;
    memcpy(user->getCurCharacQuestW()->m_authenData.m_authen,
           const_cast<char*>(req.m_0d), len);
    return 0;

bool CShutdowManager::WaitUntilExitThread()
{
    std::printf("WaitUntilExitThread (%d)\n", (int)(unsigned char)m_flag);
    // [b28d-27] P0/N96 清绑：ORIG 0x082a396e `movzbl (%eax),%eax; xor $1; test %al;
    // je <joins>` ⇒ 字段已 bool，裸 `if (!m_flag) { joins; return true; }` 即复现
    // （P248 档②；P214② 两个 return true 共享尾块）。
    // [ORIG 0x082a396e 直证] 形为 `movzbl; xor $1; test %al; je <joins>;
    // mov $0x1,%eax; jmp <end>; <joins>: …` ⇒ 原码是**否定守卫** `if (!m_flag)`
    // 且 else 臂（return true）独立成岛 ⇒ 写显式 else（if-return 双臂不共享尾块）。
    if (!m_flag)
    {
    // ORIG 0x82a396e 逐条转写：按地址序 join 7 个线程全局（s_dispatch_thr →
    // s_udp_thr → s_monitor_tcp_thr → s_db_thr → s_db_thr_for_logDB →
    // s_accept_thr → s_net_thr），每次 join 后 puts 对应描述串。
    // ORIG 直接 call ThreadInterface::join（非虚）；线程全局在 ORIG 以
    // ThreadInterface* 使用，重建声明为派生指针，此处 reinterpret 保持 this 值。
    reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->join();
    std::puts("dispatch thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->join();
    std::puts("udp thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->join();
    std::puts("monitor tcp thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->join();
    std::puts("db thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->join();
    std::puts("log db thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->join();
    std::puts("accept thread join");
    reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->join();
    std::puts("net thread join");
    return true;
    }
    else
    {
        return true;
    }

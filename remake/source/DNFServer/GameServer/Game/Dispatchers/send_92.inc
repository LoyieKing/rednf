void Dispatcher_FatigueAttendance::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081db392：param typed 指针槽 -0xc、字段全单级 disp
    // （`mov 0x4(%eax)`/`mov 0x8(%eax)`/…）——char*+N 反产 add 两步
    // 覆写（技巧14）。PacketTypes.h 冻结，本 TU 局部视图具名。
    struct FatigueSendView {
        char m_vptr[4];   // +0x00 ParamBase vptr
        int m_4;          // +0x04
        int m_8;          // +0x08
        int m_c;          // +0x0c
        int m_10;         // +0x10
    };
    FatigueSendView* pres = reinterpret_cast<FatigueSendView*>(&param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1ad);
    if (pres->m_4 == 0)
    {
        guard.put_byte(1);
        // ORIG 读 param+8 整型入栈，由 put_short 取低 16 位发出
        guard.put_short(pres->m_8);
        guard.put_int(pres->m_c);
        guard.put_int(pres->m_10);
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(static_cast<signed char>(pres->m_4));
        guard.put_short(pres->m_8);
    }
    guard.finalize(true);
    user->Send(guard);

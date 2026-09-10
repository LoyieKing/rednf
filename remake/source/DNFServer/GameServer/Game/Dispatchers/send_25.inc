void Dispatcher_Mercenary_Return::send(CUser* user, ParamBase& param)     // ORIG 0x081dda1c (86 insn)
{
    // ORIG 0x081dda53：param 槽 -0xc 统一基址，字段全单级 disp
    // （`movzbl 0x4(%eax)`/`movzbl 0x5+movsbl`/`mov 0x8(%eax)`）——
    // 裸偏移 char*+0x04 反产 add 两步覆写（技巧14）。+0x04..+0x08 经
    // 本 TU 局部视图具名（PacketTypes.h 冻结，仅 m_pad04[8] 占位）。
    struct MercRetMapView {
        char m_vptr[4];         // +0x00 ParamBase vptr
        unsigned char m_4;      // +0x04 bool 回执
        signed char m_5;        // +0x05 char 回显
        int m_8;                // +0x08
    };
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    MercRetMapView* mv = reinterpret_cast<MercRetMapView*>(&result);
    PacketGuard guard;
    guard.put_header(1, 0x1b9);
    guard.put_byte(mv->m_4 ? 1 : 0);                       // +0x04 bool（ORIG mov$1/jmp/mov$0 岛）
    guard.put_byte(static_cast<unsigned int>(mv->m_5));    // +0x05 char 回显
    guard.put_int(mv->m_8);                                // +0x08 int
    guard.put_int(result.m_result.m_0);                                     // +0x0c
    guard.put_int(result.m_result.m_4);                                     // +0x10
    guard.put_byte(static_cast<unsigned int>(
        static_cast<char>(result.m_result.m_8)));                           // +0x14
    guard.finalize(true);
    user->Send(guard);

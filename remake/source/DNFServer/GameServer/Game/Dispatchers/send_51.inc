void Dispatcher_Mercenary_Competition::send(CUser* user, ParamBase& param)    // ORIG 0x081dde20 (83 insn)
{
    // [G1Frest 2026-09-10] ORIG 0x081dde20 区：所有字段以 `mov 0x10(%ebp),%eax;
    //   movzbl 0x4(%eax),%eax` 直取 param 形参 —— **无** result / pResult 具名局部
    //   （旧码 -0x10 槽 + 帧 0x30，且偏移被拆成 `add $0x4,%eax` 两步）；
    //   `pResult[0x04] != 0` 作为实参时物化成 diamond（`je; mov $1,%eax; jmp; mov $0,%eax`）
    //   ⇒ 三目 `? 1 : 0` 形（旧码 setne 归一）。
    PacketGuard guard;
    guard.put_header(1, 0x1bb);
    guard.put_byte(((char*)&param)[0x04] != 0 ? 1 : 0);
    if (((char*)&param)[0x04] == 0)
    {
        guard.put_byte(0x15);                                                  // 失败错误码
    }
    else
    {
        guard.put_int(*reinterpret_cast<int*>((char*)&param + 0x08));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(((char*)&param)[0x0d])));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(((char*)&param)[0x0c])));
    }
    guard.finalize(true);
    user->Send(guard);

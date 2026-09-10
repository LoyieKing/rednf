void _NS_NonClient_Request(CUser* user)
{
    const int randInt = rand();
    PacketGuard packet;
    packet.put_header(0, 0x110);
    packet.put_int(randInt);
    packet.finalize(true);
    user->Send(packet);
    user->SetNonClientRandInt(randInt);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                (TIMER_MESSAGE)0x8c, 60,
                                (unsigned short)user->get_unique_id(), 0xffffffff);
    // [实测 2026-09-10] ORIG 的 `jmp <共享尾>`（同指令同序、仅块位互换）在本函数未能用
    // 裸 return / 块+return / goto 尾标签复现（探针 f6/f9 在小函数命中、本函数恒就地展开）
    // ⇒ 维持 N104 记法，待后续与同类件统一复算。

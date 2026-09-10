void CDropItem::execute()
{
    IDispatcher* dispatcher = CGameMasterMgr::GetDispatcher(0x32);
    if (dispatcher == 0)
        return;

    PacketGuard packet;
    packet.put_header(1, 0x32);
    packet.put_short(1);
    packet.put_short(1);
    packet.put_byte(0);
    packet.put_short(8);
    packet.put_int(1);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.finalize(false);
    packet.set_index(0xf);

    typedef int (*DispatchFn)(void*, CUser*, PacketBuf*);
    // ORIG 返回值经 bool 物化后丢弃（test/setne/test 无消费者）：
    // `while (call != 0) break;` 形复现（探针 /tmp/bl5/probe6.cpp q1 实证）。
    while ((*reinterpret_cast<DispatchFn*>(*(void**)dispatcher))(
               dispatcher, GetUser(), packet.get()) != 0)
        break;

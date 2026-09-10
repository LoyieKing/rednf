void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stStackableActionUse pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);
    int err = *reinterpret_cast<int*>(pbase + 0x4);               // +0x04
    short echoSlot = *reinterpret_cast<short*>(pbase + 0x8);      // +0x08
    char echoType = *reinterpret_cast<char*>(pbase + 0xa);        // +0x0a

    if (err != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);                 // class=1 cmd=0x204
        packet.put_byte(0);
        packet.put_byte(err & 0xff);
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);
        packet.put_byte(1);
        packet.put_short(static_cast<int>(echoSlot));
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }

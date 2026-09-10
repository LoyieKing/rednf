void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stStackableActionUse pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);       // ORIG -0xc 具名指针局部
    // [G1Frest 2026-09-10] ORIG 0x0825e4fa/0x0825e541/0x0825e559/0x0825e5fd/0x0825e614：
    //   每处使用都经 pbase 重新装载（pbase+4 / pbase+0xa / pbase+8），
    //   无 err/echoSlot/echoType 具名局部（旧码 -0x0c/-0x10/-0x14 三槽为发明，P0/P48）。
    if (*reinterpret_cast<int*>(pbase + 0x4) != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);                 // class=1 cmd=0x204
        packet.put_byte(0);
        packet.put_byte(*reinterpret_cast<int*>(pbase + 0x4) & 0xff);
        // ORIG `movzbl 0xa(%eax),%eax; movsbl %al,%eax` 两段 = 读 uchar 后按 char 提升
        packet.put_byte((int)(char)*reinterpret_cast<unsigned char*>(pbase + 0xa));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);
        packet.put_byte(1);
        // ORIG `movzwl 0x8(%eax),%eax; cwtl` 两段
        packet.put_short((int)(short)*reinterpret_cast<unsigned short*>(pbase + 0x8));
        packet.put_byte((int)(char)*reinterpret_cast<unsigned char*>(pbase + 0xa));
        packet.finalize(true);
        user->Send(packet);
    }

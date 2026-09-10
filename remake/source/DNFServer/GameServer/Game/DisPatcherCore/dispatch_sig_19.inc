int DisPatcher_MakePVP::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // ORIG 0x081f803e T（报告 DisPatcher_MakePVP/dispatch_sig.md 全文转写）：
    // ExtractPacket 失败→LineFunc(0x85b6)；房名 slang→SendCmdErrorPacket(0x35,0x9f)；
    // 否则 MakePVPRoom(user,msg,0x35, mode==0?2:6)。
    MSG_MAKE_PVP_ROOM msg;
    memset(&msg, 0, 0x3d);
    if (!ExtractPacket(packet, msg))
    {
        // [b28d-solo-DisPatcherCore] P0：ORIG 无 res 槽（`mov %eax,%ebx` 直承载
        // 返回值跨 string dtor），原具名局部多一对 store/load 并使全帧深 4。
        return LineFunc(0x85b6, __PRETTY_FUNCTION__, 0, 0);
    }
    {
        std::string name(msg.m_name);
        if (CSyncSlangFilter::GetInstance()->HasSlang(name))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x35, 0x9f);
            return 0;               // [Lane5] ORIG +0x70：独立 `mov $0,%ebx; jmp dtor`
        }
        else
        {
            // ORIG +0xf7..+0x140：两独立调用块——`test; je` 远跳 2-块、6-块紧跟
            // fall（技17 fall 块在前，技34 双常数直落立即数 spill）。
            if (msg.m_battleMode != 0)
            {
                MakePVPRoom(user, msg, (ENUM_CMDPACKET)0x35, (PVP_BATTLE_MODE)6);
            }
            else
            {
                MakePVPRoom(user, msg, (ENUM_CMDPACKET)0x35, (PVP_BATTLE_MODE)2);
            }
        }
        return 0;                   // dtor 后 mov %ebx,%eax（ORIG +0x167/+0x172）
    }
}

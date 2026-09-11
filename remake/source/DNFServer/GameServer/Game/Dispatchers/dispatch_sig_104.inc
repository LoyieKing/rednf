// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x08207d3a orig/ours=177/181 ndiff-blocks=23
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
int Dispatcher_RequestGuildSecede::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int local_38 = 0;                    // -0x34：目标模式（0=仅自己退会）
    char local_32[30];                   // -0x2e：目标角色名
    unsigned int local_14;               // -0x10：guildkey
    unsigned int local_10;               // -0xc：Secu Check 结果
    char cVar2;

    // [G2-104 2026-09-11 · P0/P212] ORIG 0x08207d3a `call get_state; cmp $0x2,%eax; jle`
    //   （寄存器直比、无槽）⇒ 原码无具名 state 局部；旧形多 `mov %eax,-0x18` + 帧 +0x18。
    //   0x08207d4d `cmp $0x7,%al; sete %al; test %al,%al; je` ⇒ getCurCharacVill 亦无槽。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xa29f, __PRETTY_FUNCTION__, 0, 0);
    }
    local_14 = user->get_charac_guildkey();
    if (local_14 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x22);
        return 0;
    }
    if ((char)user->getCurCharacVill() == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x13);
        return 0;
    }
    cVar2 = packet.get_int(local_38);
    if ((char)(cVar2 ^ 1) == 0)          // cVar2==1 继续
    {
        if (local_38 != 0 && local_38 <= 0x1d)   // jg 0x1d 越过 → (0,0x1d] 走目标分支
        {
            cVar2 = packet.get_str(local_32, 0x1e, local_38);
            if ((char)(cVar2 ^ 1) == 0)
            {
                cVar2 = user->IsSubGuildMaster();
                if (((char)(cVar2 ^ 1) != 0) &&
                    (cVar2 = user->IsGuildMaster(), (char)(cVar2 ^ 1) != 0))
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x24);
                    return 0;
                }
                cVar2 = GlobalData::s_power_manager->IsPowerWarEventOn();
                if (cVar2 != 0)
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x13);
                    return 0;
                }
            }
            else
            {
                return LineFunc(0xa2b8, __PRETTY_FUNCTION__, 0, 0);
            }
        }
        local_10 = (unsigned int)GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xd);
        if (local_10 == 0)
        {
            if (local_38 == 0)
            {
                user->deleteGuildExpBook();
            }
            unsigned int uVar5 = user->getCurCharacNo();
            ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
            CGuildServerProxy* this_00 =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
            this_00->SendRequestGuildSecede(local_14, uVar5, local_38, local_32);
        }
        else
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, (unsigned char)(local_10 & 0xff));
        }
    }
    else
    {
        return LineFunc(0xa2b3, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
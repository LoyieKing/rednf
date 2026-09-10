int Dispatcher_RequestGuildSecede::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int local_38 = 0;                    // -0x34：目标模式（0=仅自己退会）
    char local_32[30];                   // -0x2e：目标角色名
    unsigned int local_14;               // -0x10：guildkey
    unsigned int local_10;               // -0xc：Secu Check 结果
    char cVar2;
    int iVar3;

    iVar3 = user->get_state();
    if ((iVar3 <= 2) || (user->getCurCharacR() == 0))
    {
        LineFunc(0xa29f, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    local_14 = user->get_charac_guildkey();
    if (local_14 == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x9c, 0x22);
        return 0;
    }
    cVar2 = user->getCurCharacVill();
    if ((char)cVar2 == 7)
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
                LineFunc(0xa2b8, __PRETTY_FUNCTION__, 0, 0);
                return 0;
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
        LineFunc(0xa2b3, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    return 0;

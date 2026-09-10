int Dispatcher_UpgradeGuildCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char cVar2;
    int local_2c;                        // -0x2c：getGuildCargoUpradeInfo 结果
    unsigned int local_28;               // -0x28：info+8（扣减点数）
    int local_24;                        // -0x24：info+0xc（所需物品数量）
    unsigned int local_20;               // -0x20：info+4（所需 PowerWar 点数）
    bool bVar1;

    int iVar3 = user->get_state();
    if (!(iVar3 == 3 && user->getCurCharacR() != 0))
    {
        LineFunc(0xdc7c, __PRETTY_FUNCTION__, 0, 0);
        return 0;
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 100);
        return 0;
    }
    cVar2 = (char)user->IsExistGuildAgit();
    if (cVar2 != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb5);
        return 0;
    }
    cVar2 = (char)user->IsGuildMaster();
    if (cVar2 != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0x24);
        return 0;
    }
    int iVar5 = user->GetGuildCargoCapacity();
    // GuildParameterScript::getGuildCargoUpradeInfo（CDataManager::m_guildParameter +0xa50c）
    local_2c = (int)G_CDataManager()->m_guildParameter.getGuildCargoUpradeInfo(iVar5);
    if (local_2c == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 1);
        return 0;
    }
    local_28 = *(unsigned int*)(local_2c + 8);
    local_24 = *(int*)(local_2c + 0xc);
    local_20 = *(unsigned int*)(local_2c + 4);
    unsigned int local_2c_val = local_28;
    unsigned int uVar6 = (unsigned int)user->GetGuildPowerWarPoint();
    if (uVar6 < local_28)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb3);
        return 0;
    }
    Inven_Item local_99v;
    CInventory* pCVar7 = (CInventory*)user->getCurCharacInvenR();
    int local_20v = pCVar7->GetInvenData(0xcbd, local_99v);
    if ((local_24 != 0) && (local_20v == -1 || local_99v.m_addInfo2 < local_24))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb9);
        return 0;
    }
    int iVar3b = (int)user->GetGuildDBInfo();
    if (*(unsigned int*)(iVar3b + 0xa0) < local_20)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xba);
        return 0;
    }
    if (local_24 != 0)
    {
        pCVar7 = user->getCurCharacInvenW();
        cVar2 = (char)pCVar7->delete_item((INVEN_TYPE)1, local_20v, local_24, (eItemDelReason)0x20, 1);
        if (cVar2 != 1)
        {
            bVar1 = true;
        }
        else
        {
            bVar1 = false;
        }
    }
    else
    {
        bVar1 = false;
    }
    if (bVar1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb9);
        return 0;
    }
    PacketGuard local_5cv;
    local_5cv.put_header(0, 0xe);
    local_5cv.put_byte(0);
    local_5cv.put_short(1);
    pCVar7 = (CInventory*)user->getCurCharacInvenR();
    pCVar7->MakeItemPacket((INVEN_TYPE)1, local_20v, local_5cv);
    local_5cv.finalize(true);
    user->Send(local_5cv);
    user->DecreaseGuildPowerWarPoint(local_2c_val);
    int iVar3c = -(int)local_2c_val;
    unsigned int uVar8 = (unsigned int)user->get_charac_no(-1);
    unsigned int uVar6b = user->get_charac_guildkey();
    ENUM_SERVER_GROUP uVar4 = user->GetServerGroup();
    CGuildServerProxy* pCVar9 =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(uVar4);
    pCVar9->SendIncreaseDecreasePowerWarPoint(uVar6b, uVar8, iVar3c);
    Packet_Guild_Guild_Cargo_Upgrade local_4ev;
    local_4ev.m_guildKey = user->get_charac_guildkey();
    local_4ev.m_charNo = user->getCurCharacNo();
    local_4ev.m_curCapacity = user->GetGuildCargoCapacity();
    local_4ev.m_newCapacity = user->GetGuildCargoCapacity() + *(int*)(local_2c + 0x10);
    local_4ev.m_field1a = *(unsigned int*)(local_2c + 4);
    pCVar9 = GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    pCVar9->SendTcpPacket((char*)&local_4ev,
                          (unsigned int)*(unsigned short*)((char*)&local_4ev + 2));
    return 0;

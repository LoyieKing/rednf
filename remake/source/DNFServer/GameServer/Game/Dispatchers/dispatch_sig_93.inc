// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08222958 orig/ours=303/331 ndiff-blocks=47
//   帧差 sub $ac vs $d0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0xac,%esp | cmp    $0x3,%eax
int Dispatcher_UpgradeGuildCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char cVar2;
    STGuildStorageUpgradeInfo* local_2c;   // -0x2c：getGuildCargoUpradeInfo 结果

    unsigned int local_28;               // -0x28：info+8（扣减点数）
    int local_24;                        // -0x24：info+0xc（所需物品数量）
    unsigned int local_20;               // -0x20：info+4（所需 PowerWar 点数）
    bool bVar1;

    // ORIG 08222958 段：`call get_state; cmp $0x3,%eax; jne`（值留 %eax，无槽）⇒ 内联（P0）。
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xdc7c, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 100);
        return 0;
    }
    // ORIG 082229xx 段：`call IsExistGuildAgit; xor $0x1,%eax; test %al,%al; je`
    // （bool callee 裸取反，无槽）⇒ P168a① 形；旧 `(char)x != 1` 产 `mov %al,-0x4d; cmpb $1`。
    if (!user->IsExistGuildAgit())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0xb5);
        return 0;
    }
    if (!user->IsGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 0x24);
        return 0;
    }
    int iVar5 = user->GetGuildCargoCapacity();
    // GuildParameterScript::getGuildCargoUpradeInfo（CDataManager::m_guildParameter +0xa50c）
    local_2c = G_CDataManager()->m_guildParameter.getGuildCargoUpradeInfo(iVar5);

    if (local_2c == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x10d, 1);
        return 0;
    }
    local_28 = (unsigned int)local_2c->m_field8;
    local_24 = local_2c->m_fieldC;
    local_20 = (unsigned int)local_2c->m_field4;

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
    local_4ev.m_newCapacity = user->GetGuildCargoCapacity() + local_2c->m_field10;

    // [C2Dispatchers3a2 2026-09-11 ③a-2 人工核·负结论·偏移等价] ORIG 0x0822db3
    //   `mov -0x20(%ebp),%eax; mov %eax,-0x30(%ebp)` ⇒ 复用**栈副本 local_20**（ORIG `?D4:1`），
    //   旧文重读 `local_2c->m_field4`（我方 `?D4:2`）——同一成员 info+0x04、两侧偏移一致，非错位；
    //   改后 our 331→320、gap −28→−17。（ORIG 0x0822b8b `mov 0xa0(%eax)` vs 我方 0x1ed20
    //   `add $0xa0; mov (%eax)` = 门控③d 取址 lowering，等价。）
    local_4ev.m_field1a = (unsigned int)local_20;

    pCVar9 = GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    pCVar9->SendTcpPacket((char*)&local_4ev,
                          (unsigned int)local_4ev.packetSize);
    return 0;
}
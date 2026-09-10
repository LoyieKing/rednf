unsigned char CParty::CheckHaveHellPartyPassItem(CDungeon const* dungeon,
                                                ENUM_DUNGEON_TYPE type)
{
    if (m_hellWorldMap == 0)
    {
        return true;
    }
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7bba 段：`call; xor $0x1; test %al;
    // jne <mov $1 岛>` + `cmpl $0x1,0x10(%ebp); je <mov $0 岛>` ⇒ 原码是负极性
    // 短路单 return `!A || B`（P214①/P258），非具名 bool skip——我们的
    // -0x9 槽 + movb 写即该多出的局部（P0 槽-局部律）。
    if (!m_hellWorldMap->IsInHellDungeon() || type != 1)
    {
        return true;
    }

    std::vector<int> sealCondItems;
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al;
    // je <packet 块>` ⇒ 成功分支（m_sealDoorFlag=1）在 then、packet 块为
    // 直落 else（P59/P222 正向布局），勿写 `if (!call())`。
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c15 `call; test %al,%al; je`
    // （2 条，无 setne 物化）⇒ 被调返回 bool（P292），头已同步改 bool。
    if (CheckDestroyConditionSealDoor(dungeon, sealCondItems,
                                      m_hellWorldMap->m_field34))
    {
        m_sealDoorFlag = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_byte(0x1b);
        packet.put_byte(static_cast<int>(sealCondItems.size()));
        // [b28d-solo-CPartyDepsA2] ORIG 0x085a7c8e 段 begin/end/operator!= 均
        // 非 const 实例（无转换构造 call）⇒ P96/P270 用非 const iterator。
        std::vector<int>::iterator it = sealCondItems.begin();
        while (it != sealCondItems.end())
        {
            packet.put_short(*it);
            ++it;
        }
        packet.finalize(true);
        send_to_party(packet);
        m_sealDoorFlag = 0;
    }
    return m_sealDoorFlag;

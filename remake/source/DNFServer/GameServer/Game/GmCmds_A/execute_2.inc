void CBloodMonsterListCmd::execute()
{
    // [b28d-solo-GmCmds_A] P0/P167：ORIG 无 user 具名槽，3 处调用点各自重调
    // GetUser()（p167 MISSING GetUser×1）。
    if (GetUser()->get_state() != 5)
    {
        return;
    }

    // [b28d-solo-GmCmds_A] P200：ORIG `call GetPartyIndex; movswl %ax,%ebx` 直传
    // GetParty 实参（无 partyIndex 具名槽）。
    CParty* party = G_CGameManager()->GetParty(
        static_cast<int>(GetUser()->GetPartyIndex()));
    if (party == 0)
    {
        return;
    }

    std::string message("monster list : ");
    // [b28d-solo-GmCmds_A] P312：ORIG +0x0ae 对 buf[0xff] 发「对齐剥离+rep stos
    // +尾字节」慢路（无 memset call，p167 EXTRA memset）⇒ 数组零初始化。
    char buf[0xff] = {0};

    // CBattle_Field 为 CParty+0xb24 嵌入子对象（CParty.h 占位，全项目同口径视图）。
    // ORIG 0x84a9e??：GetMapInfo() 返回 MapInfo*，取其 +0x0c m_monsterMap。
    // [b28d-solo-2] P0：ORIG +0x119 把 GetMapInfo() 结果存入具名槽 -0x1c
    // （`mov %eax,-0x1c`），循环内每次 `mov -0x1c,%eax; lea 0xc(%eax),%edx`
    // 重算 m_monsterMap 地址 ⇒ 原码是 MapInfo* 具名局部，而非缓存的 map 指针。
    MapInfo* mapInfo = party->m_battleField.GetMapInfo();

    // [b28d-solo-GmCmds_A] P23：ORIG 条件块先载 receiver（`mov -0x1c,%eax;
    // lea 0xc(%eax),%edx`）再 lea sret。
    for (std::map<int, map_monster>::iterator it = mapInfo->m_monsterMap.begin();
         it != mapInfo->m_monsterMap.end(); ++it)
    {
        sprintf(buf, "(%d) ", it->first);
        message += buf;
    }

    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int(static_cast<int>(message.size()));
    // [b28d-47] P167 重载选错修正：ORIG 调 `put_str(char const*, int)`（PKci），
    // const_cast 使其选 `put_str(char*, int)`（Pci）。
    packet.put_str(message.c_str(), static_cast<int>(message.size()));
    packet.finalize(true);
    GetUser()->Send(packet);

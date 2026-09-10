int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8222270：战斗态+血轮地下城且非（bloodType<1 或物品等级==1）→ 血轮生成计时器
    if (user == 0)
    {
        return LineFunc(0xdbbf,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xdbc0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return LineFunc(0xdbc2,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->m_battleField.m_pDungeon == 0)
    {
        return 0;
    }
    // [G1Csig · P0 槽律] ORIG 0x08222270 段直证：`m_battleField` 基址以 lea 结果**直接**
    // 压栈（`mov %eax,(%esp); call BloodSpawnTimerManager`），无 CBattle_Field& 引用槽
    // （我方原 -0x14）；判定亦无具名 bool skip 槽（我方原 -0x9，ORIG 为 `xor $1,%al;
    // test %al,%al` 直用）⇒ 去具名改链式。
    MapInfo* mi = reinterpret_cast<CBattle_Field*>(&party->m_battleField)->GetMapInfo();
    // [Lane1Dispatchers 2026-09-10 · P292 字段版] ORIG 0x08222270 段为
    //   `movzbl 0x1(%eax),%eax; xor $0x1,%eax; test %al,%al; je <skip>` ——
    //   对 m_visited 的**布尔取反物化**（truthiness，无 cmp），非 `== 1` 数值比较。
    //   字段随 Lane27 广播由 char→bool（WarField.h:183，同宽度零布局变化）。
    if (!((party->m_battleField.m_pDungeon->m_bloodDungeonType < 1) ||
          (mi != 0 && mi->m_visited)))
    {
        reinterpret_cast<CBattle_Field*>(&party->m_battleField)
            ->BloodSpawnTimerManager(false, false);
    }
    return 0;

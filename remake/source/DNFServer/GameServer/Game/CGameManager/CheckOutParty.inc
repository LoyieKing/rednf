void CGameManager::CheckOutParty(CUser* user, bool flag)
{
    // 探针 /tmp/l4_cop_probe.cpp：f1 裸 (bool)(x^1) goto 形=xor;test;jne；
    // f2c (char)字段>0=movzbl;test;jle（P196 有符号）；f4 get_state()==2 裸形
    // =cmp;sete;test;je。三空块 4.1 [nop;jmp] 尾不可复现（N，同 deathTower 族）。
    if ((bool)(user->CheckInParty() ^ 1)) goto end;
    {
        CParty* party = GetParty(user->GetPartyIndex());
        if (party == 0) goto end;
        {
            bool bVar1 = false;
            CDungeon* dungeon = party->m_battleField.m_pDungeon;  // ORIG *(party+0xcac)（CBattle_Field 区）
            if (dungeon != 0)
            {
                if ((char)dungeon->m_bloodDungeonType > 0)
                    bVar1 = true;
                if (((CDungeon*)dungeon)->isTowerOfDespairDungeon())
                    bVar1 = true;
            }
            if (dungeon != 0)
            {
                if ((char)((CDungeon*)dungeon)->get_dimension_possible() > 0 ||
                    dungeon->m_89c != 0 ||
                    ((CDungeon*)dungeon)->isTournamentDungeon())
                    bVar1 = true;
            }
            party->m_battleField.m_dungeonClearTracer.Trace(
                __PRETTY_FUNCTION__);
            if (party->m_battleField.m_bloodState != 1)
            {
                if (party->get_state() == 2)
                {
                    bool bVar2 = false;
                    if (dungeon != 0 && dungeon->m_entryRestrictFlag != 0)
                        bVar2 = true;
                    bool bVar3 = false;
                    if (dungeon != 0 && dungeon->m_villageAttackEventFlag != 0)
                        bVar3 = true;
                    if (!bVar1 && bVar2 && bVar3)
                        user->giveup_panalty();
                }
            }
        }
        party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
    }
end:
    ;

bool CCreatureMgr::Evolute(int creatureId, int minLevel, bool force)
{
    if (m_equippedCreature != 0)
    {
        // b28d-solo-CreatureOnlyMgrRest [P226/P0]：ORIG 0x0833bedc
        // `cmp 0x10(%ebp),%eax; jge L1; cmpb $0,-0x2c; je L0; L1: mov $1;
        // jmp; L0: mov $0; test; je` = `||` 短路物化岛且 GetLevel() 无槽
        // ⇒ 语句表达式包条件（内不含具名变量）。
        if (({ m_equippedCreature->GetLevel() >= minLevel || force; }))
        {
            if (({ m_equippedCreature->IsEvolutionCreature(creatureId) || force; }))
                return m_equippedCreature->Evolute(m_user);

            if (({ m_equippedCreature->IsEvolutionCreatureMulty(creatureId) || force; }))
                return m_equippedCreature->EvoluteMulty(m_user, creatureId);

            // b28d-solo-CreatureOnlyMgrRest [P221]：ORIG 兄弟块槽向
            // then=-0x28 / else=-0x18（后声明占浅槽）；4.4 具名对象恒反向
            // ⇒ 改全表达式临时链复现。
            cMyTrace(__PRETTY_FUNCTION__, 0xdd6, 0)(
                "evolute error : different creature kind.");
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xddb, 0)(
                "evolute error : low creature level.");
        }
    }
    return false;

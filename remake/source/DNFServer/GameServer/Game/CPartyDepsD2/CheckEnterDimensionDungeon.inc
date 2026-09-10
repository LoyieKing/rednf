int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon, unsigned char& flag,
                                       bool checkMinParty)
{
    const int memberCount = get_member_count();
    // [b28d-solo-CPartyDepsD2] ORIG 0x0859f1f4 `movsbl %al,%eax; sub $0x1,%eax;
    // mov %eax,-0x10` ⇒ 源码为 (signed char)get_dimension_possible() - 1（同
    // CParty.cpp:1497 族）；券字节索引随之为 0xeb9+dimIdx。
    const int dimIdx = (signed char)dungeon->get_dimension_possible() - 1;

    for (int i = 0; i < 4; ++i)                          // cmpl $3/setle：i<=3
    {
        if (!_checkValidUser(i))
            continue;

        if (checkMinParty)
        {   // 人数下限：memberCount < min_partymem 且非 GM → 0xab
            // ORIG 0x0859f237 `movsbl %al,%eax` ⇒ (signed char) 语义（CParty.cpp:1509 同）。
            if ((memberCount < (signed char)dungeon->get_dimension_min_partymem()) &&
                !memberUser(this, i)->isGMUser())
                return 0xab;
        }

        // 等级下限 → 0xac（写出违规槽号）
        if (memberUser(this, i)->get_charac_level() < dungeon->get_min_level())
        {
            flag = (unsigned char)i;                     // mov %dl,(%eax)
            return 0xac;
        }

        // 维度入场券计数：*(char*)(characR + 0xeb9 + dimIdx) < 1
        // （单参重载 0x859f3fc 以 possible-1 + 0xeb9 寻址，同一字节）
        // [b28d-solo-CPartyDepsD2] P0/P89/P311：ORIG 0x0859f2e7 无 characR/
        // ticketCount 槽（`movzbl 0xeb9(%eax,%edx,1),%eax; test; jg`）⇒ 具名
        // 成员数组 m_demensionInout[dimIdx]（+0xeb9，CUserCharacInfo.h:484），
        // 与 CParty.cpp:1517 单参重载同形。
        if ((signed char)memberUser(this, i)->getCurCharacR()
                ->m_demensionInout[dimIdx] < '\x01' &&
            m_battleField.m_bloodState != 1 &&
            !memberUser(this, i)->isGMUser())
        {
            flag = (unsigned char)i;
            return 0xad;
        }
    }
    return 1;                                            // 全员通过

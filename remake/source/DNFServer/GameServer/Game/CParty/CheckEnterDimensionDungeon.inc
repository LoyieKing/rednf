int CParty::CheckEnterDimensionDungeon(CDungeon const* dungeon)
{
    // b28d-49 @0x859f3fc：DIFF(2)→修 `(signed char)` 两处。
    // ①0859f4b2 ORIG `movsbl %al,%eax`（get_dimension_min_partymem 返回值有符号扩展，
    //   头声明 unsigned char 不改，调用点按 P3 收窄）；②0859f518 ORIG `test %al,%al; jg`
    //   有符号比较（m_demensionInout 为 unsigned char 字段，原码按 signed char 判定；
    //   4.4 无 cast 时折 `jne`）。
    // ORIG 0x859f3fc：`if (gdp <= 0) return 0` 前置（`setle` 判定 + 独立 return0 岛），
    // 尾部 `return 0` 复用 leave 出口（ORIG +0x17f）。P0：gdp 结果 %al 直测无槽；
    // ORIG 未对 m_pDungeon 判空。
    if ((signed char)m_battleField.m_pDungeon->get_dimension_possible() <= 0)
        return 0;
    int memberCount = get_member_count();
    int dimIdx = (signed char)dungeon->get_dimension_possible() - 1;
    // ORIG 0859f476 `DimensionPartyCount::DimensionPartyCount(local_1b, (char)+0xcd8,
    // (char)+0xcc4, -1)` —— 用 3 参构造（非默认 ctor 后逐字段赋值）。
    DimensionPartyCount dpc((char)m_battleField.m_bloodState,
                            (char)m_battleField.m_dungeonDiff, -1);
    if (dungeon->get_dimension_member_count(dpc))
    {
        if (memberCount < dpc.m_2)
            return 0xab;
    }
    else
    {
        if (memberCount < (signed char)dungeon->get_dimension_min_partymem())
            return 0xab;
    }
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0859f4e7 `_checkValidUser; xor$0x1; test; jne` —— `!x continue` 形态。
        if (!_checkValidUser(i))
            continue;
        // P0：getCurCharacR 结果 %eax 直接索引 m_demensionInout[dimIdx]（+0xeb9），
        // 无 charac 缓存局部（ORIG 0859f510 `movzbl 0xeb9(%eax,%edx,1)`）。
        if ((signed char)m_member[i].m_pUser->getCurCharacR()->m_demensionInout[dimIdx] < '\x01' &&
            m_battleField.m_bloodState != 1 && !m_member[i].m_pUser->isGMUser())
            return 0xad;
    }
    return 0;

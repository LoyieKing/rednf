bool CBattle_Field::ConsistTournament(const CMap* pMap, std::list<_mapMonster>& mobList,
                                      std::map<short, std::vector<TournamentInfo> >& roundMap)
{
    if (pMap == NULL)
    {
        cMyTrace trace("bool CBattle_Field::ConsistTournament(const CMap*, "
                       "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                       0xb5a, 5);
        trace("CBattle_Field::ConsistTournament ERROR!! : pMap is NULL pointer");
        return false;
    }

    // CMap+0x114：m_tournamentMap（map<char, vector<TournamentMonster>>）。
    if (pMap->m_tournamentMap.empty())
    {
        cMyTrace trace("bool CBattle_Field::ConsistTournament(const CMap*, "
                       "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                       0xb61, 5);
        trace("CBattle_Field::ConsistTournament ERROR!! : tournamentMonster is empty");
        return false;
    }

    std::vector<TournamentMonster> monsters;
    const int memberCount = m_party->get_member_count();

    std::map<char, std::vector<TournamentMonster> >::const_iterator group =
        pMap->m_tournamentMap.find(static_cast<char>(memberCount));
    if (group == pMap->m_tournamentMap.end())
    {
        // ORIG 0x083036e9 `call get_member_count; mov %eax,%ebx` 在 cMyTrace ctor 之前
        // 求值并以 %ebx 跨 ctor 保活（无具名槽）⇒ 全表达式临时链（P227）。
        cMyTrace("bool CBattle_Field::ConsistTournament(const CMap*, "
                 "MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
                 0xb6c, 5)
            ("CBattle_Field::ConsistTournament ERROR!! : party member count is invalid(%d)",
             m_party->get_member_count());
        return false;
    }

    // ORIG 0x08303734：monsters = group->second（map 中该人数对应的怪物列表拷贝）。
    monsters = group->second;

    std::vector<TournamentInfo> roundInfo;
    // ORIG 0x0830377c `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax; xor $1` ⇒ 具名
    // bool 局部（P198①），勿直写 if(!call())。
    const bool basicOk = MakeBasicRoundInfo(memberCount, monsters, roundInfo);
    if (!basicOk)
        return false;

    std::sort(roundInfo.begin(), roundInfo.end());

    std::vector<TournamentInfo> firstGroup;
    ChoiceFirstGroup(roundInfo, firstGroup);
    ConsistTournamentRound(firstGroup, roundInfo, roundMap, mobList);
    return true;

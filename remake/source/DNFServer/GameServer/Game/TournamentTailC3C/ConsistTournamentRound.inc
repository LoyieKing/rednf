void CBattle_Field::ConsistTournamentRound(std::vector<TournamentInfo>& first,
    std::vector<TournamentInfo>& second,
    std::map<short, std::vector<TournamentInfo> >& roundMap,
    std::list<_mapMonster>& mobList)
{
    int randIdx = -1;
    randIdx = get_rand_int(0x10);

    std::map<short, TournamentInfo> tournament;

    TournamentInfo empty;
    empty.m_roundOrder = (short)randIdx;
    tournament.insert(std::make_pair(empty.m_roundOrder, empty));


    int peer = -1;
    if ((randIdx & 1) == 0)
        peer = randIdx + 1;
    else
        peer = randIdx - 1;

    std::random_shuffle(first.begin(), first.end());
    // [b28d-20] ORIG 0x08303bd2..0x08303c7c：begin() 仅调用一次（迭代器落 -0xd8 槽），
    // 后续 ptEv/deEv/ptEv/erase 全部复用该具名迭代器 —— 写 first.begin() 三次会多 3 组
    // begin()+sret 且槽号漂移（P0 槽往返 ⇒ 原码有具名 iterator it）。
    std::vector<TournamentInfo>::iterator it = first.begin();
    it->m_roundOrder = (short)peer;
    tournament.insert(std::make_pair(it->m_roundOrder, *it));
    first.erase(it);

    std::vector<int> indexes;
    for (int i = 0; i < 0x10; ++i)
    {
        // [b28d-20] ORIG 0x08303ca7/0x08303cb2 两个 je 落点不同（+0x200 / +0x203 两独立
        // nop 岛）⇒ P223 多岛细则形：两条正条件 goto + 尾标签串（标签序=守卫序）；
        // 合并 && 会被 4.4 jump-thread 成单落点。
        if (i == randIdx) goto L_skip1;
        if (i == peer) goto L_skip2;
        indexes.push_back(i);
    L_skip1:;
    L_skip2:;
    }
    std::random_shuffle(indexes.begin(), indexes.end());

    InsertTournamentInfo(indexes, first, tournament);
    InsertTournamentInfo(indexes, second, tournament);
    MakeRoundInfo(tournament, roundMap, mobList);

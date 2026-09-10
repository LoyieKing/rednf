void CBattle_Field::insert_tournamentRoundMonster()
{
    // [b28d-Lane27] ORIG 0x083029cc/0x083029d9：两条**独立** `test;je`（各带 nop 岛）
    // ⇒ 拆两条早退；第三层 `call;test %al,%al;je <epilogue>`（2 条、无 xor）⇒ 正向包裹
    // （`if(!f()) return;` 在 4.4 恒产 `xor $0x1; test; jne`，探针 /tmp/l27_probe2.cpp
    //   p1/p2 实证）。
    if (m_pDungeon == NULL)
        return;
    if (m_party == NULL)
        return;
    if (m_pDungeon->isTournamentDungeon())
    {
        std::multimap<int, map_monster>* tournamentMob =
            &GetCurrentMapInfo()->m_tournamentMob;                        // MapInfo+0xd0
        // [b28d-Lane27] ORIG 0x08302a0d 段：`mov 0x8(%ebp),%eax; mov %eax,(%esp); call
        // GetCurrentMapInfo; add $0xc,%eax; mov %eax,-0x14` ⇒ 是**成员指针**（MapInfo+0xc
        // 取址），不是 map 按值拷贝（拷贝会额外 map::C1/D1 + 深拷贝 node 链）。
        std::map<int, map_monster>* monsters = &GetCurrentMapInfo()->m_monsterMap;
        std::map<int, map_monster>::iterator it = monsters->begin();
        const int memberCount = m_party->get_member_count();

        for (int round = 1; round <= 4; ++round)
        {
            int n = 0;
            // ORIG 0x08302b43 段：`n<memberCount && it!=end` 的 4.1 物化岛
            // （`cmp;jge F` + `call ne; test; je F` + `mov $1/jmp/F: mov $0`）⇒ 用 P226
            // 语句表达式复现（`!(A&&B)` 形 4.4 会多一条 xor；直写 && 被折成直跳）。
            // ORIG 0x08302a5c `jmp 08302b43`（前置跳底部条件）+ 体在物理前 ⇒ 4.1 的
            // while 布局（非 do-while：do 形无前置 jmp）。
            while (({ n < memberCount && it != monsters->end(); }))
            {
                tournamentMob->insert(std::make_pair(round, it->second));
                ++n;
                it++;
            }
        }
    }

// ORIG passedMapMonsterKill 0x084adaf8：passed 列表每张图同样处理。
// b28d-solo-GmCmds_E：对齐 singleMapMonsterKill——双 bool 槽形、passed 指针
// 不落槽（getPassedMapInfoList 内联实参）、`m_dropCnt != 100` continue 形。
// p167 CLEAR（27/27），162=165（ratio 0.9541）。残差 = begin/end 调用点
// sret↔this 寄存器角色互换（4.4.7 恒 `mov %edx,%eax?`+sret=%eax/this=%edx，
// ORIG sret=%edx/this=%eax；N7/P230 后端敏感族）——本 TU 不在 dnf_game_cxx.sh
// 的 4.4.6 路由表内（禁改路由表）⇒ N 封顶。
// [round1-L29 2026-09-10 复核] 判据① CLEAR（38 vs 38）。我方 163 / ORIG 161 ⇒ 帧/push 族
//   （P332）⇒ N（ORIG 0x084ae7xx）。
void CMonsterDropTest::passedMapMonsterKill(CParty* party, DropStat& drop,
                                            GradeStat& grade)
{
    std::list<map_item> items;
    int killCnt = 100;
    std::vector<bool> flags;
    bool flagOn1 = true;                                     // ORIG -0xe/-0xd 双 bool 槽
    bool flagOn2 = true;
    flags.push_back(flagOn1);
    flags.push_back(flagOn2);

    for (unsigned int round = 0; round < static_cast<unsigned int>(m_count); round++)
    {
        for (std::vector<MapInfo>::iterator mit =
                 static_cast<std::vector<MapInfo>*>(
                     party->getPassedMapInfoList())->begin();
             mit != static_cast<std::vector<MapInfo>*>(
                        party->getPassedMapInfoList())->end();
             ++mit)
        {
            for (std::map<int, map_monster>::iterator it = mit->m_monsterMap.begin();
                 it != mit->m_monsterMap.end(); ++it)
            {
                if (it->second.m_dropCnt != 100)
                {
                    continue;
                }
                processKillMonster(party, *mit, it->second, flags, killCnt, items);
                getDungeonDropResult(items, drop, grade);
                items.clear();
            }
        }
    }
}
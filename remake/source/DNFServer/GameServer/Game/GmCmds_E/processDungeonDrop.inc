// ORIG processDungeonDrop(CParty*, map&, map2&) 0x084ad8dc：按 mode 分派击杀模拟。
// ORIG 决策树 `test;je→S;sub$2;cmp$1;ja→end;jmp→P` 为 switch case 0 +
// case 2..3 range 形（P46/P163 族），case 升序书写复现块序。
void CMonsterDropTest::processDungeonDrop(CParty* party, DropStat& drop,
                                          GradeStat& grade)
{
    switch (static_cast<unsigned int>(m_mode))
    {
    case 0:
        singleMapMonsterKill(party, drop, grade);
        break;
    case 2:
    case 3:
        passedMapMonsterKill(party, drop, grade);
        break;
    }
}
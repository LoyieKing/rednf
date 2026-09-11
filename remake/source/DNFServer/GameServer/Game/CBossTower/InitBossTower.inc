// ----------------------------------------------------------------------------
// ORIG T 0x08142f32 _ZN8WongWork10CBossTower13InitBossTowerEP6CPartyi：
// m_dungeon(+0x9f0) = find_dungeon(dungeonIdx)；party/dungeon 判空 → 0；
// m_party(+0x04) = party；m_stageMax(+0xae4) = getRandomTowerMaxStage()；
// m_prepareFinishFlag(+0xafc) = 0；返回 1。
// [batch-19 N18a 封顶 ndiff=5] 唯一结构残差 = ORIG +0x57 三段冗余 regmove
// （`call`→`mov %eax,%edx`→`mov 0x8(%ebp),%eax`→`mov %edx,0xae4(%eax)`）；
// 同函数 +0x1a 的 m_dungeon 落槽处 ORIG 只 3 条（`mov this,%edx;mov %eax,`
// `0x9f0(%edx)`）⇒ 4.1 在同一 store 模板上两种分配，源码侧无法定向。
// 实测否决：① `register int` 无约束 → ndiff 11（4.4 选 %ebx ⇒ 多
// push/pop + `add $0x14,%esp` 帧形全变）；② `register int asm("edx")` →
// ndiff 5（复现 `mov %eax,%edx` 但反出 `mov %edx,%eax; mov this,%edx`
// 三步，净 0 且违 P78）。维持直赋值。
// ----------------------------------------------------------------------------
bool CBossTower::InitBossTower(CParty* party, int dungeonIdx)
{
    m_dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (party == 0 || m_dungeon == 0)
        return 0;

    m_party = party;
    m_stageMax = m_dungeon->getRandomTowerMaxStage();
    m_prepareFinishFlag = 0;
    return 1;
}
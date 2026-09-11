// ORIG T 0x08143ebc：
//   CBossStage::handleKillMonster 为假 → 0；怪物清空检查：ORIG 对 this+0xa00
//   的 std::map<int,map_monster> 取 empty()（_M_node_count@map+0x14 即
//   this+0xa14），为空时 onFinishStage()；且 (uint)m_stageMax(+0xae4) <=
//   (uint)m_stageCur(+0xae0) 时 _onPrepareFinishTower(true)；返回 1。
// [N 封顶] 残差三处：① 调用点 CBossStage::handleKillMonster 后 ORIG `mov
// %al,-0x9;movzbl;xor$1;test/je` bool 槽物化，4.4 对 int callee 恒
// `test%eax;sete;test`（P197-④/N18b）；bool 化被调已实测致其大函数帧
// 0x3bc→0x3cc 全槽漂移回归，不可行；② ORIG 0x8143f38 `mov 0xae0(%eax),%edx`
// 直落 vs 4.4 两步（N18a）；③ ORIG 帧 0x44 vs 0x34（bool 槽差，随①）。
// [b28d-54 N] ORIG 0x08143ebc：CBossStage::handleKillMonster 保持 int（bool 化
//   实测致其大 body 全帧槽漂移）⇒ 调用点 test/setne（P197④）。本波已翻正
//   m_stageCur 同型化单段装载 + bool killed 槽形；余 setne 为 N。
// [b28d-solo-CBossTower] AE（52:52，Main 代落 bool 化后翻正）：CBossStage.h:45
//   handleKillMonster int→bool（ORIG 0x08143f03 `call; mov %al,-0x9` 无 setne
//   截断 ⇒ bool）+ killed bool 槽 + m_stageCur 同型化。
bool CBossTower::handleKillMonster(CUser* user, unsigned short a, unsigned short b,
                                  const MSG_MONSTER_DIE& die, int* d)
{
    // b28d-54 翻正：ORIG 0x08143f03 `mov %al,-0x9; movzbl -0x9,%eax; xor $1,%eax;
    // test %al,%al; je` ⇒ 局部是 bool（P248 档②；char 恒 `cmpb $0,-0x9; jne`）。
    // 仅改局部类型，被调 CBossStage::handleKillMonster 保持 int（历史实测 bool 化
    // 其大 body 致全帧槽漂移回归）。
    bool killed = (bool)CBossStage::handleKillMonster(user, a, b, die, d);
    if (!killed)
        return 0;
    if (m_stageData.m_monsterMap.empty())
    {
        onFinishStage();
        // b28d-54：m_stageCur 已同型化为 unsigned（CBossStage.h）⇒ 去 cast 后
        // 4.4 复现 ORIG 0x08143f38 单段 `mov 0xae0(%eax),%edx`（P211 档③）。
        if (m_stageCur >= m_stageMax)
        {
            _onPrepareFinishTower(true);
        }
    }
    return 1;
}
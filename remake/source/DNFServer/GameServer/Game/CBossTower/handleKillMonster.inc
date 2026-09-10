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

bool CBossStage::moveNextStage()
{
    if (!m_stageData.m_monsterMap.empty())
        return false;
    // ORIG 0x0814d1ee `jb` 跳增块、mov$0 fall-through：条件写作 >= 早退布局。
    // [b28d-62] 删同型冗余 cast：ORIG 0x814d1dd `mov 0xae0(%eax),%edx` 单段直落；
    //   `(unsigned int)` cast 使 4.4 多一条 `mov %eax,%edx` 中转（P41 变体）。
    if (m_stageCur >= m_stageMax)
        return false;
    m_stageCur += 1;
    return true;

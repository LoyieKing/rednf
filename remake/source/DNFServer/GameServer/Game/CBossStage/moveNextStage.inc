// ---- moveNextStage（ORIG 0x0814d1b6）----
// 怪物清空（monsterMap.empty()）且 m_stageCur < m_stageMax（无符号比较）
// 时 m_stageCur+=1 进入下一层。
// [N18a 封顶] 残差仅 0x814d1dd `mov 0xae0(%eax),%edx` 单步直落；4.4 恒折
// `mov %eax; mov %eax,%edx` 两步（N18a：单段 regmove 不可复现），其余 27 条逐条同形。
// [b28d-62] AE：删同型冗余 cast（P41 变体：cast 致 `mov %eax,%edx` 中转）后
//   28=28，p167 CLEAR。ORIG 0x0814d1b6
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
}
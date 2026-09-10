bool LevelupSupportEventManger::CheckNewStep(int level)
{
    // ORIG T 0x08147164：end() 先求值（-0x10）后 find()（-0xc）⇒ 4.4 对
    // `==` 右起装载同序；比较用 operator== 后取反（test/je + mov$0/mov$1
    // 岛），故写 `if (find == end) return false; return true;`（P56/P220）。
    if (m_script.m_stepItems.find(level) == m_script.m_stepItems.end())
        return false;
    return true;

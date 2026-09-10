bool CCreature::isMaxLevel()
{
    // ORIG 0x833742x：je→尾mov0；call 体 fall；setge;test;je→中mov0；fall mov1。
    if (m_script != 0)
    {
        if (m_level >= m_script->GetMaxLevel())
            return true;
    }
    return false;
}

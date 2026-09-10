bool CCreature::vaildItemExp(int itemIdx)
{
    if (m_state == 2)
        return false;
    if (m_isGrow != 0)
        return false;
    // ORIG 83378e9：call; test al; je→mov0 尾块 = if/return true 形。
    if (m_script->validItemExp(itemIdx))
        return true;
    return false;

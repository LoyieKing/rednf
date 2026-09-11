// ORIG vaildItemExp @ 0x83378aa：state==2 拒绝；m_isGrow!=0 拒绝；
// 否则委托 CCreatureScript::validItemExp(m_script, itemIdx)
// （ORIG 不判 m_script 空，忠实转写）。
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
}

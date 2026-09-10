bool CCreatureScript::validItem_AType(int itemIdx)
{
    // [b28d-34] P0 清绑（同 validItemExp，ORIG 0x833f2bc）。
    if (m_field4c != 1)
        return false;
    if (m_field5c == itemIdx)
    {
        return true;
    }
    return false;

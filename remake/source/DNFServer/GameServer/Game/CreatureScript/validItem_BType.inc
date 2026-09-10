bool CCreatureScript::validItem_BType(int itemIdx)
{
    // [b28d-34] P0 清绑（同 validItemExp，ORIG 0x833f2ee）。
    if (m_field4c != 1)
        return false;
    if (m_field60 == itemIdx)
    {
        return true;
    }
    return false;

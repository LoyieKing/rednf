bool CExtraMatrix::MoveUser(CForcedMoveCharacter* fm)   // ORIG 0x82e5c0a
{
    // ORIG 0x82e5c0a：每处使用重新装载 fm->m_slotIdx（无 idx 槽）⇒ 表达式直用。
    if (fm->m_slotIdx < 0 ||
        (unsigned int)fm->m_slotIdx >= m_points.size())
        return false;
    fm->m_x = m_points.at(fm->m_slotIdx).m_x;
    fm->m_y = m_points.at(fm->m_slotIdx).m_y;
    fm->ForcedMove();
    return true;

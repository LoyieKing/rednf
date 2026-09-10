bool DB_AvatarItemMove::MoveAvatarItem(SIG_AVATAR_MOVE* sig)
{
    ChangeSlot(reinterpret_cast<DB_AvatarMoveView*>(sig)->m_uiId0,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_slot0,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_clearAvatarId0);
    ChangeSlot(reinterpret_cast<DB_AvatarMoveView*>(sig)->m_uiId1,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_slot1,
               reinterpret_cast<DB_AvatarMoveView*>(sig)->m_clearAvatarId1);
    return 1;

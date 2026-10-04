WarRoom* CUser::GetWarRoom()
{
    // ORIG 0x86551f2 js 尾岛（0x8655214 mov $0）：正向 early-return（probe12 B 形）
    if (m_warRoomIndex >= 0)
        return G_CGameManager()->GetWarRoom(m_warRoomIndex);
    return 0;
}
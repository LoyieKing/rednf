PvP_Room* CUser::GetPVPRoom()   // [Main·检查点22] 返回类型 void*→PvP_Room*（证据见 CUser.h:2611）
{
    // ORIG 0x86551a2 js 尾岛 + 0x86551d1 jmp merge：正向 return（probe12 B 实证复现 js;body;jmp;mov$0;merge）
    if (m_pvpRoomIdx >= 0)
        return G_CGameManager()->GetPvp(m_pvpRoomIdx, this, 0);
    return 0;

void CVillageMonsterArea::OnMoveArea(CUser* user)
{
    if (m_monsters.empty())              // ORIG 0x86b3aXX test al;jne 尾（空跳，Send fall）
        goto T_END;
    SendVillageMonsterList(user);        // ORIG 0x86b3abe（列表发送）
T_END:
    ;

bool StageControl::leaveUser()
{
    CUser* user =
        reinterpret_cast<CharacData*>(m_characData)->getUser();
    if (user == 0)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    HistoryLog::leaveDungeon(*user, *getProcStage());
    G_GameWorld()->out_from_dungeon(user);
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_itemspace(0);
    user->getCurCharacInvenR()->SendItemLockListInven();
    user->setAdvanceAltarIndex(-1);  // ORIG CUser::setAdvanceAltarIndex(user,-1)
    destroy();                // ORIG 尾部真实调用 destroy(this)（leaveUser.md 实证）
    return 1;                 // ORIG uVar4 = 1

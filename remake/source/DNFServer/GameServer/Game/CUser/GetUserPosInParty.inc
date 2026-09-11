// CUser::GetUserPosInParty @ 0x08688638 —— 返回 own 在队伍中的槽位，不在返回 -1。
int CUser::GetUserPosInParty()
{
    CParty* party = (CParty*)GetParty();
    // ORIG 0x8688650 je 尾岛（0x8688666 mov $-1）：正向 early-return（同 GetPVPRoom 族 probe12 B 形）
    if (party != 0)
        return party->GetUserPosition(this);
    return -1;
}
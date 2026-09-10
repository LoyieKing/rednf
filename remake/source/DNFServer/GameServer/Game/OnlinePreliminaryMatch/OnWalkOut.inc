int COnlinePreliminary::OnWalkOut(PvP_Room* room, int a)
{
    unsigned int team = room->get_team(a);
    CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(a));
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561fc6-0x5d 压参序实证：
     * 0xc(%esp)=team(-0x10 槽)、0x10(%esp)=a(0x10(%ebp)) ⇒ 第 3 参是
     * get_team(a) 的 team 值、第 4 参是座位号 a。原写 (a, team) 使
     * OnLeaveRoom 的 teamGroup 收到座位号 = 真语义 bug；按 ORIG 交换。 */
    OnLeaveRoom(u, room, team, a);
    return 1;

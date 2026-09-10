int COnlinePreliminary::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;

    /* ORIG @08560862/@085608?? movzbl %al,%eax（(unsigned char) 强转形，
     * `& 0xff` 出 and $0xff）+ add %eax,%eax 两段，size 累加走 add %eax,%ebx */
    if (room->get_player_count() >= (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
        return 4;

    /* ORIG @085608xx sum(两 size call)→%ebx 先、再第 2 次 pc call：
     * lhs=sum 正向 `>`（cmp %eax,%ebx; setg） */
    if (m_teams[0].size() + m_teams[1].size() >
        (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
        return 4;

    switch (m_field04)
    {
    case 0:
        /* [b28d-solo-OnlinePreliminaryMatch] P248档①：_CheckSameTeam/
         * _CheckObserver 返回类型 int→bool 后，裸 `if(!call) return 4;`
         * 直产 ORIG @08560866/@0856088d 的 `call; xor $1,%eax; test %al,%al;
         * je`（旧 uchar 栈化三段 mov%al,-N;xorb$1,-N;cmpb 与帧 +0x10 一并消）。 */
        if (!_CheckSameTeam(room, user))
            return 4;
        if (!_CheckObserver(room, user))
            return 4;
        return 0;
    case 3:                     /* 叶岛序（P46：叶排放=case 书写序，跳表
                                 * @8ca230c：3→0 岛、1/4/5→共享 4 岛、
                                 * 2/default→共享 4 岛） */
        return 0;
    case 1:                     /* 1/4/5 标签叠放：4.1 同体 case 合并共享岛
                                 * （跳表 1/4/5→同一块），4.4 需显式叠放 */
    case 4:
    case 5:
        return 4;
    default:
        return 4;
    }

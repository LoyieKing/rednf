void COnlinePreliminary::OnEndFight(PvP_Room* room)
{
    unsigned int managerTeam = room->get_manager_team();  /* T 0x85d9242 */
    m_field04 = 0;
    _ReturnTeam(room, managerTeam, (CUser*)0);
    /* ORIG @08562067 十对字 block-move（%eax=LHS %edx=RHS 双 this 装载、
     * 末对复用 %edx）= 结构体整体赋值【GetUserList 同法】 */
    if (managerTeam != 1)
        m_teams[0] = m_teams[1];
    m_teams[0].refresh();                                 /* T 0x855fcf6 */
    m_teams[1].reset();
}
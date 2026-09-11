int COnlinePreliminary::OnCreateRoom(CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 0x13;                              /* ORIG mov $0x13,%eax（0x8560afc） */

    m_authValue = ++nextAuthValue_;               /* 全局自增令牌 0x943e144 */
    m_field04 = 0;
    m_teams[1].reset();                           /* T 0x855fdc0 */
    m_teams[0].reset();
    m_teams[0].add(user);                         /* T 0x855fdfe */
    m_teams[0].m_teamId = user->GetCurCharacOnlinePreliminaryTeamId();
    m_teams[0].m_rating = _GetRating(user);
    return 0;                                     /* ORIG mov $0x0,%eax（0x8560b83） */
}
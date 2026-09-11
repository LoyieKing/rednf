bool CUser::CheckFatigue()
{
    if (m_accId == 0)
        return true;
    return getCurCharacTotalFatigue() < getCurCharacTotalMaxFatigue();
}
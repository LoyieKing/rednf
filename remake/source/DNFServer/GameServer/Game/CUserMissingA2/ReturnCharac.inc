bool UserMercenaryInfoMgr::ReturnCharac(CUser* user, unsigned int characNo,
                                        competition_reward_result& reward)
{
    if (user == 0)
        return 0;
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    return m_characs[index].Reward(user, reward);

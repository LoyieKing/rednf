void CAchievement::setTrigger(unsigned int id, unsigned short t0,
                              unsigned short t1, unsigned short t2)
{
    Quest* quest = G_CDataManager()->find_quest((int)id);
    if (quest == 0)
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if ((quest->m_type != 7) ||
        (m_user->get_charac_level() < quest->m_levelMin))
    {
        _sendSetTrigger(id, 1);
        return;
    }
    if (isClearedAchievement(id))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    // ORIG 0x828b781：_verifyTrigger 返回值 xor 1 后 je +0x12c，
    // 即 verify==1 才走 _setTrigger 路径；verify==0 → _sendSetTrigger(id,2)。
    if (!_verifyTrigger(id, t0, t1, t2))
    {
        _sendSetTrigger(id, 2);
        return;
    }
    _setTrigger(id, t0, t1, t2);
    _sendSetTrigger(id, 0);
    _checkComplete(id);
}
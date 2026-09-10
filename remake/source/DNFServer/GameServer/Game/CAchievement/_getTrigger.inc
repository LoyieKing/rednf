stAchievement CAchievement::_getTrigger(unsigned int id)
{
    stAchievement out;
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828bb72：found 分支 fall-through 复制 it->second 到 sret，
    // not-found 分支复制构造默认 out（双返回，不合并单出口）
    if (it != m_achievements.end())
    {
        return it->second;
    }
    return out;

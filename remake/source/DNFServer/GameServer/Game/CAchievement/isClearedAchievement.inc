bool CAchievement::isClearedAchievement(unsigned int id)
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828cc37：found 分支为 fall-through（je 跳到 return 0）
    if (it != m_achievements.end())
    {
        return it->second.isComplete();
    }
    return 0;

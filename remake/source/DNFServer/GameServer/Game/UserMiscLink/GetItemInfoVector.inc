LevelupSupportEventManger::GetItemInfoVector(int level)
{
    std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
        m_script.m_stepItems.find(level);
    // 同 GetStepIndex：ORIG T 0x08147724 为 op!= + je 落 mov $0 岛（P220）。
    if (it != m_script.m_stepItems.end())
        return it->second;
    return 0;
}
void* CDataManager::GetExpertJobScript(int job)
{
    // ORIG 0x0822b5f2 W：m_regeneration.m_expertJobList（CDM+0x5090）直取后
    // 调 CExpertJobList::get(job)，无中间暂存。
    return m_regeneration.m_expertJobList->get(job);
}
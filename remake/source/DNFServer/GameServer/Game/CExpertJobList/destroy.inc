void CExpertJobList::destroy()
{
    // b28d-solo [P13：ORIG 0x0896dada 前一次性求值 end()（循环条件只比较不重调）]。
    for (std::map<int, STExpertJobScript*>::iterator it = m_jobMap.begin(),
             e = m_jobMap.end(); it != e; ++it)
    {
        STExpertJobScript* script = it->second;
        delete script;
    }

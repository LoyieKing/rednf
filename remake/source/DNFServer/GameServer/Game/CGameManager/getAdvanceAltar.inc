advancealtar::StageControl* CGameManager::getAdvanceAltar()
{
    advancealtar::StageControl* control = m_stagePool.Acquire();
    if (control == 0)
        return 0;
    control->setIndex(m_stagePool.GetIndex(control));
    int ctrlIdx = control->getIndex();
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(ctrlIdx);
    if (it == m_stageMap.end())
    {
        m_stageMap[control->getIndex()] = control;
        control->reset();
        return control;
    }
    else
    {
        return 0;
    }
}
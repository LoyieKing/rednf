void PerfMgr::clear()
{
    for (int i = TASK_IDX_BEGIN; i < TASK_IDX_END; ++i)
    {
        m_count[i] = 0;
    }
}
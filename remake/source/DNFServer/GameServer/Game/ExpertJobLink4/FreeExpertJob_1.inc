void CExpertJobFactory::FreeExpertJob(CExpertJob* job)
{
    // b28：正极性包 if（P214①族/探针 /tmp/l3/p4.cpp fs2）——负极性 return
    // 早退使 4.4 尾块前多插 jmp+nop 中继；正包形末 case fall-through leave。
    if (job != 0)
    {
        switch (job->GetType())
        {
        case 1:
            m_enchanterPool.Free((CEnchanter*)job);
            break;
        case 3:
            m_disjointerPool.Free((CDisjointer*)job);
            break;
        }
    }

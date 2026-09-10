CExpertJob* CExpertJobFactory::AcquireExpertJob(ENUM_EXPERT_JOB_TYPE type)
{
    CExpertJob* acquired = 0;
    switch (type)
    {
    case (ENUM_EXPERT_JOB_TYPE)1:
        acquired = m_enchanterPool.Acquire();
        break;
    case (ENUM_EXPERT_JOB_TYPE)3:
        acquired = m_disjointerPool.Acquire();
        break;
    }

    if (acquired != 0)
    {
        acquired->Reset();
        acquired->SetType(type);
    }
    return acquired;

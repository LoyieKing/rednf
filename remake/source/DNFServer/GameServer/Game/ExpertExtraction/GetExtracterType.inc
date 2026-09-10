int GetExtracterType(ENUM_EXPERT_JOB_TYPE job)
{
    switch (job)
    {
    case (ENUM_EXPERT_JOB_TYPE)2:
        return 0;
    case (ENUM_EXPERT_JOB_TYPE)4:
        return 2;
    case (ENUM_EXPERT_JOB_TYPE)1:
        return 1;
    default:
        return 3;
    }

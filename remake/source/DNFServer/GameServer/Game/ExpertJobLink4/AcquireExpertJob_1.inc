// ---- CExpertJobFactory::AcquireExpertJob（ORIG W 0x0849f5d2）：
//      type==1 → Enchanter 池（this+0x00）；type==3 → Disjointer 池（this+0x30）；
//      取到后先虚调用 Reset()（vtable+0xc 槽）再非虚 SetType(type)。 ----
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
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
}
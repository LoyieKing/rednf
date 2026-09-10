EquipmentAniInfoScript* STEquipmentScript::getAniInfo(ENUM_CHARACTERJOB job)  // 0x0898ef1a
{
    std::map<ENUM_CHARACTERJOB, EquipmentAniInfoScript>::iterator it =
        m_aniInfo640.find(job);
    if (it != m_aniInfo640.end())
    {
        return &(*it).second;   // ORIG 用 operator*（de）
    }
    if (job == 0xb)
    {
        return 0;                  // ORIG 返回 NULL
    }
    return getAniInfo((ENUM_CHARACTERJOB)0xb);

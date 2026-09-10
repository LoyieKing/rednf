bool CUser::ChangeGrowType_GM(int a, int b)
{
    if (getCurCharacR() == 0)
        return 0;
    if (a < 0 || a > 5 || b < 0 || b > 1)
    {
        std::cerr << "Invalid grow type , first :" << a << "second :" << b << std::endl;
        return 0;
    }
    int level = get_charac_level();
    // ORIG 0x0867b5f4：GetPremiumInfo() 转发后 getOverSkillLevel，不直读 m_premium
    unsigned int overLevel = GetPremiumInfo()->getOverSkillLevel();
    SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
    slotW->debugCheckGrowTypeSkill(level + (int)(overLevel & 0xffff),
                                   get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
    return 1;

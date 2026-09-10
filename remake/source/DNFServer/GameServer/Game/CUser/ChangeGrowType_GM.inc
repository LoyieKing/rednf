bool CUser::ChangeGrowType_GM(int a, int b)
{
    if (getCurCharacR() == 0)
        return 0;
    if (a < 0 || a > 5 || b < 0 || b > 1)
    {
        std::cerr << "Invalid grow type , first :" << a << "second :" << b << std::endl;
        return 0;
    }
    // 2026-09-10 gA（A 组）P0/P183/P200：ORIG 0x0867b0e7 `mov %eax,%ebx`
    //   （get_charac_level() 结果无槽、跨 GetPremiumInfo/getOverSkillLevel 调用保活 %ebx）、
    //   0x0867b101 `movzwl %ax,%eax; lea (%ebx,%eax,1),%eax; mov %eax,-0xc(%ebp)`
    //   （只有 level+over 的结果落 -0xc 槽）、0x0867b115 `mov %eax,%ebx`
    //   （get_charac_job() 无槽）、receiver getCurCharacSkillW 最后求值
    //   ⇒ 删 level/overLevel/slotW/job 四个具名量（我方旧形 3 槽 + 帧 0x38 vs 0x34）。
    // ORIG 0x0867b5f4：GetPremiumInfo() 转发后 getOverSkillLevel，不直读 m_premium
    ((SkillSlot*)getCurCharacSkillW())->debugCheckGrowTypeSkill(
        get_charac_level() + (int)(GetPremiumInfo()->getOverSkillLevel() & 0xffff),
        get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
    return 1;

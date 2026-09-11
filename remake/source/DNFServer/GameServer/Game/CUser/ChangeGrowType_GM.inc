// ORIG：GM 改成长类型。无角色/参数越界(0..5, 0..1) → 0 并 cerr 报错；
// 否则 debugCheckGrowTypeSkill 校验 → set_grow_type(reason=2) →
// _postCheckForceChangeGrowType 后处理，返回 1。
// 返回 int：ORIG 0x0867b048 尾 `mov eax,0x1; ret`、早退 `mov eax,0x0`
// （Model22 桥清零实证；声明 CUser.h:3330 由 Main 代落）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b048 | _ZN5CUser17ChangeGrowType_GMEii
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
    // 2026-09-11 CuA A组 P0：ORIG 0x0867b107 `mov %eax,-0xc(%ebp)`（level+over 结果**落槽**，
    //   装载段 0x0867b134 `mov -0xc(%ebp),%edx` 回读）⇒ 原码有具名局部；且 ORIG 求值序为
    //   「先 sum 落槽 → 再 get_charac_job（跨 receiver call 保活 %ebx）→ receiver」，
    //   与单一表达式右→左求值（我方旧形：job 先、sum 留 %esi）相反。
    int checkLevel = get_charac_level() + (int)(GetPremiumInfo()->getOverSkillLevel() & 0xffff);
    ((SkillSlot*)getCurCharacSkillW())->debugCheckGrowTypeSkill(
        checkLevel, get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
    return 1;
}
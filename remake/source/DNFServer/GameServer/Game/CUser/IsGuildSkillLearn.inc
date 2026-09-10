unsigned char CUser::IsGuildSkillLearn(int skillIdx)
{
    // P212：全头部声明时 4.4 与 4.1 同为「先声明深」⇒ 声明序按 ORIG 槽深序
    //   gdb(-0x14)→count(-0x10)→i(-0xc)。
    STGuildDBInfoOnly* gdb;
    int count;
    int i;
    gdb = (STGuildDBInfoOnly*)GetGuildDBInfo();
    // b28d-solo-CUser 试形（P0/P171/P183 三旋钮）：
    //   ① ORIG 0x864fac9 call 后直接 `mov %eax,-0x14; mov -0x14,%eax; movzbl 0x44(%eax)`
    //      ——**无判空**（原码遗留，照抄不写防御）；
    //   ② count 为 int（ORIG `movzbl %al,%eax; mov %eax,-0x10` 零扩展入槽）；
    //   ③ 无 skills 指针缓存（ORIG 每访问重算 `gdb+i*5+0x40+0x5`，P183 去缓存）。
    count = gdb->m_skillLearnCnt;   // ORIG movzbl 0x44 + movzbl %al,%eax
    for (i = 0; i < count; ++i)
    {
        // ORIG 0x0864fb01 `mov 0x5(...)`（idx int）/ 0x0864fb1c `movzbl`
        // （level 零扩展）；ORIG 直返 level 字节（非布尔规范化——见回报，
        // CUser.h 返回类型待属主核对 P9）。
        if (((const STGuildSkill*)gdb->m_skills)[i].m_field0 == skillIdx)
            return (unsigned char)(((const STGuildSkill*)gdb->m_skills)[i]
                                       .m_field4);
    }
    return 0;

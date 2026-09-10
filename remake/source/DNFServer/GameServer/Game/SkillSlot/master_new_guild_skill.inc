int SkillSlot::master_new_guild_skill(int skillIdx, int job, int type,
                                      int secondGrow,
                                      const STGuildDBInfoOnly& guildInfo,
                                      int money, int& needGold,
                                      int& learnCnt, short& needGSP,
                                      int availableGSP)
{
    // [b28d-solo-SkillSlot] 判词（N7/P75 封顶，gap=+2，p167 CLEAR 14=14）：
    // 已复现：帧 0x40、槽集(skill -0x1c/count -0x18/i -0x10/maxLevel -0xc)、
    // skill 预清(P198①)、m_skills 组件视图(P161：lea base+i*5; add $0x40;
    // mov 0x5 / add $0x49)、needLevel 具名比较(jge 直跳)、SkillAdd 全内联
    // 实参(esi/ebx/ecx 直传 + lea receiver)。残余 3 处纯寄存器/装载序：
    // ① logFormat 段 learnCnt 落 %eax vs ORIG %edx（guildInfo 指针重载时机）；
    // ② m_skills 两处 base(%ecx) 先于 i(%edx) 装载（ORIG 反之，P23/P75）；
    // ③ needGSP 存储 4.4 多 `mov %eax,%edx` 中转（ORIG 直接 `mov %ax,(%edx)`）。
    // 三者语义等价，属 4.1/4.4 寄存器分配族，无源码旋钮可择。
    // 真语义 bug 修正：ORIG 0x086039ac 的 GSP 检查用
    // 0x30(%ebp)（第 10 形参）；0x20(%ebp)（第 6 形参 money）全函数未被使用
    // ⇒ 原码参数序为 (…, guildInfo, money, needGold, learnCnt, needGSP,
    // availableGSP)。旧实现把第 6 参当 availableGSP 比较，是**取错参数**。
    (void)money;
    if ((unsigned char)guildInfo.m_skillLearnCnt >= 0x10)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0x102, "BUY GUILD SKILL, GName(%s) , Learn Cnt(%d)", &guildInfo,
            (unsigned char)guildInfo.m_skillLearnCnt);
        return -2;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        return -1;
    }
    learnCnt = 1;
    int count = (unsigned char)guildInfo.m_skillLearnCnt;
    // ORIG 0x0860387e..0x86038a6：i*5 步长（STGuildSkill packed 5B）；
    // 禁缓存 m_skills 指针（P183）：ORIG 每次重读 guildInfo 基址并折
    // `lea base+i*5; add $0x40; mov 0x5(...)` / `add $0x49`。
    for (int i = 0; i < count; ++i)
    {
        if (((const STGuildDBTailView&)guildInfo)
                .m_section.m_entries[i].m_skillIdx == skillIdx)
        {
            learnCnt = (char)((const STGuildDBTailView&)guildInfo)
                            .m_section.m_entries[i].m_level + 1;
            break;
        }
    }
    if (secondGrow < 1)
    {
        if (skill->get_type_max_level(type) < learnCnt)
        {
            return -3;
        }
    }
    else
    {
        int maxLevel =
            skill->get_second_growtype_max_level(type, secondGrow);
        if (maxLevel == 0)
        {
            maxLevel = skill->get_type_max_level(type);
        }
        if (maxLevel < learnCnt)
        {
            return -3;
        }
    }
    needGold = skill->get_guildSkillNeedGold(learnCnt);
    if (needGold < 0 ||
        guildInfo.m_fundMoney < (unsigned int)needGold)
    {
        return -5;
    }
    // [round1-L19 2026-09-09] P248 档①（callee 返回类型侧）：ORIG 该 store 段为
    // `mov 0x2c(%ebp),%edx; mov %ax,(%edx)`（LHS-first 直存 16 位）⇒ 原码 RHS 与
    // short 目标**同型**；我方 `CSkill::get_guildSkillNeedGSP` 声明 int
    //（CSkill.h:102）⇒ 4.4 产 `mov %eax,%edx; mov 0x2c(%ebp),%eax; mov %dx,(%eax)`
    // 中转。调用点 `(short)` 收窄后该段逐条归位（ndiff 15 → 15，内容置换：残余
    // 转为其后两处实参装载序 + `movzbl %al,%edx` 寄存器选择）。根治需 CSkill.h:102
    // int→short 并同步 CSkill.cpp 定义（跨 TU，未落）。
    // [round5-A17 2026-09-10] 上述「根治需 CSkill.h int→short」**已证伪**：
    // ORIG 本体 `_ZNK6CSkill21get_guildSkillNeedGSPEi` @0x083500f2 尾部为
    // `mov (%eax),%eax; leave; ret`（32 位单段读、无 movzwl 截断）⇒ callee 确为 int，
    // 改 short 会使本体反落 DIFF。本段 `mov 0x2c(%ebp),%edx; mov %ax,(%edx)` 与
    // 4.4 `mov %eax,%edx; mov 0x2c(%ebp),%eax; mov %dx,(%eax)` 之差属 P211
    // 异型 store（short& 目标 + int RHS）跨版本寄存器族 ⇒ N 封顶，勿再改 CSkill。
    needGSP = (short)skill->get_guildSkillNeedGSP(learnCnt);
    if (needGSP < 0 || availableGSP < needGSP)
    {
        return -2;
    }
    int needLevel = skill->get_needGuildLevel(learnCnt);
    if ((int)(unsigned char)guildInfo.m_fundLevel < needLevel)
    {
        return -6;
    }
    if (m_parent != 0)
    {
        (&m_parent->m_historyLog)
            ->SkillAdd(
                ((CUserCharacInfo*)m_parent)->get_charac_job(),
                ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex(),
                skillIdx, learnCnt, (eSkillAddReason)0);
        m_parent->makeGuildSkillMessage(skill->getSkillName(), learnCnt);
    }
    return 0;

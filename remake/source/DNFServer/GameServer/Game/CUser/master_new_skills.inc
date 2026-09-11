// ---- CUser::master_new_skills @ 0x0866b53e ----
// 批量习得技能：按 _Mastered_skill 数组计算 SP/SFP 用量并扣减对应技能树余量，
// 将技能槽写入（set_skill_slot_at_index），同时记录 SP/SFP 消耗日志。
// PvP 树频道直接返回 true；普通树按 secondGrow/firstGrow 走成长型
// get_give_skill 展开（ORIG 866b844-866bcd5 逐段转写）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866b53e | _ZN5CUser17master_new_skillsEP15_Mastered_skill20ENUM_SKILL_TREE_KIND
bool CUser::master_new_skills(_Mastered_skill* skills, ENUM_SKILL_TREE_KIND kind)
{
    // 2026-09-10 gA（A 组）负结论 N（ORIG 506 vs 我方 491）：ORIG 0x0866b53e 与我方
    //   的差**全部**来自跨版本寄存器/槽分配（ORIG `push %edi` + 帧 0xac + 差值临时经
    //   %esi；我方无 %edi + 帧 0x90 + 差值临时经 %ebx），逐块比对无字段/调用缺口
    //   （calcUsedSPE/get_remain_sp_at_index/set_remain_sp_at_index 调用序一致）
    //   ⇒ N78/N82 封顶，不为单条 mov 破坏表达式。
    int usedSp = 0;
    int usedSfp = 0;
    // [P-L11d 2026-09-11 Main-K4C] 下面 secondGrow/firstGrow **两个互斥分支**各自声明过
    //   一批同名标量/指针局部（chars/got/i/skillId/level/group/slotNo）⇒ 提升为函数级
    //   共用，促 4.4 复用同一批槽（ORIG 4.1 复用：ORIG 506 vs 我方 495/491 的槽族差）。
    //   安全性：两支互斥且每个使用点前必赋值（skill/slotNo 的 0 初始化仍在循环内保留）。
    //   不提升 `std::vector`/`std::pair`/`CSkill* skill`：前二者非平凡（P331 禁提前声明
    //   非 POD，会提前发射 ctor/dtor 改帧序）；skill 在循环内重置，保持原形。
    CCharacList* chars = 0;
    bool got = false;
    int i = 0;
    int skillId = 0;
    int level = 0;
    int group = 0;
    int slotNo = 0;
    ((SkillSlot*)getCurCharacSkillR())->calcUsedSP(skills, usedSp, usedSfp);

    // ORIG 0x0866b586..0x0866b6a9：SP/SFP 段逐处重调 getter（无 remainSp/sfpKind 缓存，
    //   P183）；SFP 按 kind 分成**两个完整分支**（SKILL_TREE_NONE/SKILL_TREE_1 → 树 2，
    //   否则树 3），各自「判余量 → set(0) / 重调减 used → set(diff)」+ 扣减后重调 SFPSub。
    if (((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind) < usedSp)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(0, kind);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind) - usedSp,
            kind);
    }

    if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
    {
        if (((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_1) <
            usedSfp)
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                0, SKILL_TREE_SFP_1);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sfp_at_index(SKILL_TREE_SFP_1) - usedSfp,
                SKILL_TREE_SFP_1);
        }
        // ORIG 0x866b6eb：SFPSub(扣减后余量, 树号, 消耗量, reason)
        m_historyLog.SFPSub(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_1),
            (int)SKILL_TREE_SFP_1, usedSfp, (eSPSubReason)1);
    }
    else
    {
        if (((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_2) <
            usedSfp)
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                0, SKILL_TREE_SFP_2);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sfp_at_index(SKILL_TREE_SFP_2) - usedSfp,
                SKILL_TREE_SFP_2);
        }
        m_historyLog.SFPSub(
            ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(SKILL_TREE_SFP_2),
            (int)SKILL_TREE_SFP_2, usedSfp, (eSPSubReason)1);
    }
    m_historyLog.SPSub(
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind),
        (int)kind, usedSp, (eSPSubReason)1);

    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot_at_index((char*)skills, 0x198, kind);

    if (G_GameWorld()->IsPvPSkilTreeChannel())
        return true;

    // ORIG 866b852：secondGrow 非零 → get_give_skill(flag=0) 展开。
    if (getCurCharSecondGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：giveVec 后构造独立 pair 局部（C1），循环内逐字段拷入
        std::pair<int, int> pr;
        chars = G_CDataManager()->m_characList;
        // 配对（D06 定义侧已 bool 化）：ORIG 消费形 `call; mov %al,-N` 1 字节直存 ⇒
        //   消费端局部同 bool（净账必配对律）。
        got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 0);
        if (got == 0)
            return false;
        for (i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            skillId = pr.first;
            level = pr.second;
            CSkill* skill = 0;
            skill = G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    __PRETTY_FUNCTION__,
                    0x52bd, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            group = skill->get_group();
            slotNo = 0;
            slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }

    // ORIG 866ba98：firstGrow 非零 → get_give_skill(flag=1) 展开。
    if (getCurCharFirstGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：同第二块，giveVec 后 pair 局部（C1）+ 循环内拷入
        std::pair<int, int> pr;
        chars = G_CDataManager()->m_characList;
        got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 1);
        if (got == 0)
            return false;
        for (i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            skillId = pr.first;
            level = pr.second;
            CSkill* skill = 0;
            skill = G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    __PRETTY_FUNCTION__,
                    0x52e0, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            group = skill->get_group();
            slotNo = 0;
            slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }
    return true;
}
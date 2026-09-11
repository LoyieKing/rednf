// 0x0867a95c（返回 1 成功 / 0 角色为空；DisPatcher_DebugCommand 以 !=1 判失败）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867a95c | _ZN5CUser14SetCharacLevelEi
bool CUser::SetCharacLevel(int level)
{
    // 2026-09-10 gA（A 组）负结论 N（458 vs 461）：ORIG 0x0867a9cc..0x0867a9d6 条件为
    //   `cmp; setl %al; test %al,%al; je <else>`（4.1 布尔物化），4.4 对同一比较恒产
    //   `cmp; jge <else>` 直跳（P197④ 镜像族，setcc/直跳不可互换）；另我方帧 0x90 vs
    //   ORIG 0x9c 属槽分配差 ⇒ 判 N，保留现三目源形。
    // 0x0867a95c 主分支（SetCharacLevel.md）：等级钳制[1,200]；按方向取
    // GetSpAtLevelUp 计算 SP 增量加到树 0/1 并 clamp；exp=get_level_exp；
    // SFP：<50 清 idx2/3，旧级<0x33 则=level-0x31 否则各+差值；逐级加/减
    // 成长属性、写日志、premium 金币奖励；专家/公会/监控/副本通知收尾。
    if (getCurCharacR() == 0)
        return 0;
    // ORIG 0x0867a986..0x0867a9ba：level = max(1, min(200, level))——两个
    // std::min/std::max<int> 各发 out-of-line call（常量实参在栈上建临时槽）。
    level = std::max(1, std::min(200, level));

    // P183：ORIG 逐处重调 get_charac_level()（+0x6b/+0xb8/+0x35e/+0x39e/+0x3c6/
    //   +0x3f3/+0x466/+0x48e/+0x4ce/+0x615/+0x6a6 共 11 次），不落 oldLevel 缓存。
    int spGain = (get_charac_level() < level)
                     ? G_CDataManager()->GetSpAtLevelUp(level)
                     : G_CDataManager()->GetSpAtLevelUp(level + 1);
    int spDiff = (level - get_charac_level()) * spGain;

    setCurCharacExp(G_CDataManager()->get_level_exp(level));

    // ORIG 0x0867aa54..0x0867ab36：树 0 / 树 1 两段显式展开（非循环），
    //   各以 std::max(0, std::min(0x3fffffff, v)) 钳制（min/max 各发 call）。
    {
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
                    (ENUM_SKILL_TREE_KIND)0) + spDiff;
        v = std::max(0, std::min(0x3fffffff, v));
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            v, (ENUM_SKILL_TREE_KIND)0);
    }
    {
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
                    (ENUM_SKILL_TREE_KIND)1) + spDiff;
        v = std::max(0, std::min(0x3fffffff, v));
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(
            v, (ENUM_SKILL_TREE_KIND)1);
    }

    if (level < 0x32)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)3);
    }
    else
    {
        int sfp2, sfp3;
        if (get_charac_level() < 0x33)
        {
            sfp2 = sfp3 = level - 0x31;
        }
        else
        {
            sfp2 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)2) + (level - get_charac_level());
            sfp3 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)3) + (level - get_charac_level());
        }
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp2, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp3, (ENUM_SKILL_TREE_KIND)3);
    }

    // 成长类型属性表：G_CDataManager()->m_characList（ORIG set_grow_type 0x08678c36
    // `mov 0x14(%eax),%edi` 指针装载 + imul 0x7dc 实证；CCharacList::m_characs 表）
    CCharacList* growTable = G_CDataManager()->m_characList;
    const _Additioal_info* growStat = (const _Additioal_info*)growTable->m_characs[get_charac_job()]
        .get_growtype_stat(getCurCharFirstGrowType(), getCurCharSecondGrowType());

    while (get_charac_level() != level)
    {
        if (get_charac_level() < level)
        {
            // ORIG 0x867acca：先加成长属性再升级（升/降分支对称）
            ((_Additioal_info*)getCurCharacAddInfoRefW())->add(*growStat);
            incCurCharacLevel();
            m_historyLog.LevelUp(get_charac_level(), (eLevelUpReason)1);
            // ORIG 0x0867ad32/+0x0867ad5f：GetGoldBonus 调两次（判零一次、传参一次）
            if (m_premium.GetGoldBonus(get_charac_level()) != 0)
            {
                getCurCharacInvenW()->gain_money(
                    m_premium.GetGoldBonus(get_charac_level()),
                    (eMoneyAddReason)0xd, true, 0);
                SendUpdateItemList((eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            }
        }
        else
        {
            ((_Additioal_info*)getCurCharacAddInfoRefW())->subtract(*growStat);
            decCurCharacLevel();
            m_historyLog.LevelDown(get_charac_level(), (eLevelDownReason)0);
        }
        m_premium.RecalcAdditionalInfo(this);
    }
    setCurCharacLevel(level);

    // ORIG 0x867ae4c：专家职业——类型非 0 且等级命中限定等级 → 增加专家职业经验
    STExpertJobEtcScript* ejScript =
        (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    if (GetCurCharacExpertJobType() != 0 && ejScript->isExpertJobLimitLevel(level))
    {
        expert_job::CExpertJob::IncreaseExpertJobExp(this, GetCurCharacExpertJobExp());
    }

    makeGuildLevelUpMessage(level);   // ORIG T 0x8679754

    if (level == 0x46 &&
        GlobalData::s_pGMAccounts_->isGM(get_acc_id()) != 1)
    {
        // ORIG 0x0867aef9：真实 Packet_Monitor_Max_Level_BroadCast 对象（ctor
        //   W 0x08694560 = PacketHeader(0x177e,0x28) + memset(name@+0xa,0,0x1e)）
        Packet_Monitor_Max_Level_BroadCast pkt;
        strncpy(pkt.m_name, getCurCharacName(), 0x1e);
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket((char*)&pkt, 0x28);
    }

    // ORIG 0x867af66：等级==副本 0x2aff 最低要求等级时，按副本开放状态广播进出场
    if (get_charac_level() == G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff))
    {
        // ORIG 0x0867af66..0x0867aff7：isDungeonOpen 假/真各有独立调用点
        //   （无 bool 局部），且两个分支各自重取 G_GameWorld()。
        if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) == 0)
            G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff, false);
        else
            G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff, true);
    }

    if (get_charac_level() > 0x3b)
        DimensionInoutUpdate(true, true);

    m_characterView.enableSaveCharacView();
    return 1;
}
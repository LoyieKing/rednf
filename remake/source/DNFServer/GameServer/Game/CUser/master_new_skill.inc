void CUser::master_new_skill(stBuySkillInfo& info, bool flag)
{
    (void)flag;
    int treeIdx = GetCurCharacSkillTreeIndex();
    unsigned char skillIdx = (unsigned char)info.m_skillIdx;
    int ret;
    int learnCnt = 0;
    int needGold = 0;
    short needGSP = 0;
    bool isGuildSkill = (skillIdx >= 0xc8 && skillIdx <= 0xd0);

    if (isGuildSkill)
    {
        if (!IsGuildMaster() && !IsSubGuildMaster())
            return;
        // ORIG 0x0866b..：经 GetGuildDBInfo() 访问器取 guildDB(+0x42=m_guildSkillPoint)。
        unsigned short guildLevel =
            ((STGuildDBInfoOnly*)GetGuildDBInfo())->m_guildSkillPoint;
        ret = ((SkillSlot*)getCurCharacSkillR())->master_new_guild_skill(
            (int)skillIdx, get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            *reinterpret_cast<STGuildDBInfoOnly*>(GetGuildDBInfo()), (int)getCurCharacMoney(),
            needGold, learnCnt, needGSP, (int)guildLevel);
        if (ret >= 0)
        {
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            if (proxy != 0)
                proxy->SendBuyGuildSkill((unsigned int)get_charac_no(-1),
                                         (unsigned int)get_charac_guildkey(),
                                         (int)skillIdx, learnCnt, needGSP,
                                         (unsigned int)needGold);
        }
    }
    else
    {
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (info.m_field0 == 1)
                return;
        }
        else if (info.m_field0 != 1)
        {
            return;
        }
        ret = ((SkillSlot*)getCurCharacSkillW())->master_new_skill(
            (int)skillIdx, get_charac_level(), get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            (int)info.m_count);
    }

    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(1, 0x1f);
    if (ret < 0)
    {
        pkt->put_byte(0);
        // ORIG 0x0866b0xx 跳表(0x8cef5b0)：每 ret 值一独立 put_byte 常量站点（6 case），
        // 非数组索引。展开以匹配 put_byte 调用多重集。
        switch (ret)
        {
            case -6: pkt->put_byte(0x5c); break;
            case -5: pkt->put_byte(0x5b); break;
            case -4: pkt->put_byte(0x01); break;
            case -3: pkt->put_byte(0x12); break;
            case -2: pkt->put_byte(0x16); break;
            case -1: pkt->put_byte(0x15); break;
            default: break;
        }
    }
    else
    {
        pkt->put_byte(1);
        // ORIG 0x0866b3b2..：put_byte/sp/sfpKind 判定各重调 GetCurCharacSkillTreeIndex()
        //（P44），sfp 按 (tree==-1||tree==0) 分 kind2/kind3 两独立 get_remain_sfp 站点。
        pkt->put_byte(GetCurCharacSkillTreeIndex());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex()));
        int treeForSfp = GetCurCharacSkillTreeIndex();
        if (treeForSfp == -1 || GetCurCharacSkillTreeIndex() == 0)
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)2));
        else
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)3));
        if (!isGuildSkill)
        {
            pkt->put_byte(ret);
            pkt->put_byte(skillIdx);
            int level = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
            pkt->put_byte(level);
        }
        else
        {
            pkt->put_byte(0xff);
            pkt->put_byte(skillIdx);
            pkt->put_byte(learnCnt);
        }
        // ORIG 0x0866b4b4：if/else 之后两分支共用一次 getCurCharacSkillR()+get_skill_level()
        // 再 WriteLearnSkill（原实现误置于 !isGuildSkill 内，公会技能分支漏记日志）。
        int learnLevel = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
        HistoryLog::WriteLearnSkill((FILE*)m_field796f8, (int)skillIdx, learnLevel);
    }
    pkt->finalize(true);
    Send(packet);

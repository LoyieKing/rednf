void DisPatcher_BuySkill::master_new_skill_process(CUser* user, char treeKind,
                                                   buySkillInfo& info)
{
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // ---- 公会技能分支（ORIG +0x02b..+0x1ba；正区间判定使公会块内联、
        //      ja→非公会，与 ORIG 块排布一致）----
        if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
        {
            // 权限不足：ORIG 直落 +0x2af→尾部（不写 m_4，沿用调用前遗留值）
        }
        else
        {
            // STGuildDBInfoOnly::m_guildSkillPoint（CUser.h:1297 @+0x42 ushort，
            // 与 ORIG 0x081bf9ba `movzwl 0x42(%eax)` 同宽；ORIG 两次独立
            // GetGuildDBInfo 调用（0x081bf9b5 与 guildDB），故不合并成一次调用）
            unsigned short guildLevel = static_cast<STGuildDBInfoOnly*>(
                user->GetGuildDBInfo())->m_guildSkillPoint;                    // ebx
            int money = user->getCurCharacMoney();                      // -0x54
            STGuildDBInfoOnly* guildDB =
                static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo());// -0x50
            int secondGrow =
                static_cast<int>(user->getCurCharSecondGrowType());           // -0x4c
            int firstGrow =
                static_cast<int>(user->getCurCharFirstGrowType());            // -0x48
            int job = user->get_charac_job();       // ORIG edi
            int needGold;   // ORIG -0x20：调用前未初始化，由出参引用写出
            short needGSP;  // ORIG -0x22：同上（&别名传参）
            SkillSlot* skillRG = (SkillSlot*)(user->getCurCharacSkillR());
            info.m_4 = skillRG->master_new_guild_skill(
                static_cast<unsigned char>(info.m_0), job, firstGrow, secondGrow,
                *guildDB, money, needGold,
                /*learnCnt=*/info.m_8,  // ORIG &info+8 别名 int& 出参
                needGSP, static_cast<int>(guildLevel));
            if (info.m_4 >= 0)
            {   // ORIG +0x132..+0x1ba：读回出参并发公会服务器包
                CGuildServerProxy* proxy =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy(
                        user->GetServerGroup());
                proxy->SendBuyGuildSkill(
                    static_cast<unsigned int>(user->get_charac_no(-1)),
                    static_cast<unsigned int>(user->get_charac_guildkey()),
                    static_cast<unsigned char>(info.m_0),
                    /*level(=出参 learnCnt 回读)=*/info.m_8,
                    /*needGSP=*/needGSP, static_cast<unsigned int>(needGold));
            }
        }
    }
    else
    {   // ---- 非公会技能分支（ORIG +0x1bf..+0x2ad）----
        int treeIdx = user->GetCurCharacSkillTreeIndex();               // -0x1c
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (treeKind == 1)
            {
                return;
            }
        }
        else if (treeKind != 1)
        {
            return;
        }
        int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));       // -0x38
        int secondGrow =
            static_cast<int>(user->getCurCharSecondGrowType());                       // -0x34
        int firstGrow =
            static_cast<int>(user->getCurCharFirstGrowType());                        // -0x30
        int job = user->get_charac_job();             // ORIG esi
        int level = user->get_charac_level();         // ORIG ebx
        SkillSlot* skillW = (SkillSlot*)(user->getCurCharacSkillW());
        info.m_4 = skillW->master_new_skill(
            static_cast<unsigned char>(info.m_0), level, job, firstGrow,
            secondGrow, addLevel);
        if (info.m_4 >= 0)
        {
            SkillSlot* skillR = (SkillSlot*)(user->getCurCharacSkillR());
            info.m_8 = skillR->get_skill_level(info.m_4);
        }
    }
    if (info.m_4 >= 0)
    {   // ---- 尾部（ORIG +0x2b3）----
        SkillSlot* skillRT = (SkillSlot*)(user->getCurCharacSkillR());
        int curLevel = skillRT->get_skill_level(info.m_4);
        HistoryLog::WriteLearnSkill(reinterpret_cast<FILE*>(user->m_field796f8),
                                    static_cast<unsigned char>(info.m_0),
                                    curLevel);
    }

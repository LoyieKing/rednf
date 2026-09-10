void DB_LoadEtc::LoadComboSkillInfo(SIG_LOAD_ETC* sig)
{
    // ORIG: 直读 sig+0x14ab0（mov 0x14ab0(%eax)，直接折叠反偏移），无意引局部槽（P0）。
    // 局部视图使 4.4 折叠单 disp（P82/P95：数组退化 cast 会拆 add+mov）。
    struct ComboJobView
    {
        char m_pad[0x14ab0];
        unsigned int m_job;   // = m_comboSkill + 6（SIG_COMBO_SKILL::m_job，4B 装载）
    };
    // ORIG 0x0844a3c1：cmp $9/jne 直达 leave;ret（无尾岛）——试 P257 正向包裹消
    // 4.4 的 [jmp+nop] epilogue-threading 中继（旧三形均留 1 岛）。
    if (((ComboJobView*)sig)->m_job == 9)
    {
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)0))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xbab4, "DB_LOADETC ERROR : LoadComboSkill Fail");
        sig->m_field8 = 1;
    }
    // [b28c-lane17] 判词：残余 1 条尾 `jmp+nop` 中继岛差（4.4 jump-threading，
    // P223 反例向）。实测三形：`if(!call())` 43/44（最优）、
    // `if(call()==0)` 45、`if(positive){}else{}` 45 —— 均无法消该岛，
    // 保留最短残差形。
    if (!LoadComboSkill(sig, (ENUM_SKILL_TREE_KIND)1))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xbaba, "DB_LOADETC ERROR : LoadComboSkill Second Fail");
        sig->m_field8 = 1;
    }
    }

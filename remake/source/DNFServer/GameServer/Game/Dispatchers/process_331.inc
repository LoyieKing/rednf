int Dispatcher_Skill_Command_Customizing::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* m = reinterpret_cast<char*>(&msg);
    stSkillCommandParam& res = static_cast<stSkillCommandParam&>(param);

    const int commandCount = (int)(char)m[0xd];
    if (commandCount > 0x31)
    {
        // ORIG:count>49 → err=0xfc 后直接返回(不经错误日志/default 分支)
        res.m_4 = 0xfc;
        return 0;
    }

    for (int i = 0; i < commandCount; ++i)
    {
        // STSkillCommandData ctor ORIG W 0x0822db66(CUser.h 内嵌定义已一致)
        STSkillCommandData data;
        data.m_command = *reinterpret_cast<unsigned short*>(m + i * 8 + 0xe);
        data.m_count   = (unsigned char)m[i * 8 + 0x10];

        const int job = user->get_charac_job();   // CUserCharacInfo::get_charac_job
        // ORIG 走 const 重载 _ZNK12CDataManager10find_skillEii @0x0835fda2,
        // 实参=(job, 命令ID);经 const 指针调用以匹配 _ZNK 符号。
        const CDataManager* dm = G_CDataManager();
        CSkill* skill = dm->find_skill(job, (int)(short)data.m_command);
        if (skill == 0)
        {
            res.m_4 = 0xfc;
            break;
        }
        if (skill->isCommandCustomizing() != 1)   // CSkill::isCommandCustomizing W 0x0822b5d2
        {
            res.m_4 = 0xf9;
            break;
        }

        const char keyCount = (char)data.m_count;
        if (keyCount > 5 || keyCount < 1)
        {
            res.m_4 = 0xfb;
            break;
        }

        unsigned char keys[8];   // ORIG 帧 -0x29 起的命令键缓存(keyCount<=5 已证)
        bool keyRangeError = false;
        for (int j = 0; j < (int)keyCount; ++j)
        {
            keys[j] = (unsigned char)m[i * 8 + j + 0x11];
            if ((char)keys[j] < 0 || (char)keys[j] > 8)
            {
                res.m_4 = 0xfa;
                keyRangeError = true;
                break;   // ORIG:跳出内层后仍无条件执行末位键检查序列
            }
        }

        // 末位键检查:keys[keyCount-1] ∉ {0,1,2,3,7} 才放行(ORIG 0x081d2c92..0x081d2cfd)
        const unsigned char lastKey = keys[(int)keyCount - 1];
        if (lastKey == 0 || lastKey == 1 || lastKey == 2 ||
            lastKey == 3 || lastKey == 7)
        {
            res.m_4 = 0xfa;
            break;
        }

        // ORIG: cVar1 = customizingSkillCommand(user, &data);
        //       if (cVar1 != 1) { res.m_4 = 0xfc; break; }
        if (user->customizingSkillCommand(data) != 1)
        {
            res.m_4 = 0xfc;
            break;
        }
    }

    if (res.m_4 != 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            0x3051,
            "SKILL_COMMAND_LOG : CUSTOMIZING ERROR!! ALL COMMAND DEFAULT!! m_id (%s)",
            NumberToString(user->get_acc_id(), 0));
        user->defaultSkillCommand();
    }
    return 0;

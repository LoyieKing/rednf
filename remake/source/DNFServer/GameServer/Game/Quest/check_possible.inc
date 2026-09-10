bool Quest::check_possible(const stSelectQuestParam& param) const
{
    int job = param.m_job;
    int firstGrow = param.m_firstGrow;
    int secondGrow = param.m_secondGrow;  // ORIG 局部（checkMatchedGrowType 用 -1）
    int level = param.m_level;
    int questCount = param.m_pvpGrade;  // ORIG param+0x10
    int expertJobType = param.m_expertJobType;     // param+0x1c
    int expertJobLevel = param.m_expertJobLevel;   // param+0x20

    if (!checkPossibleJob((ENUM_CHARACTERJOB)job))
    {
        return 0;
    }
    if (_isNeededCheckGrowType((ENUM_CHARACTERJOB)job))
    {
        if (m_jobChangeQuest == 1)
        {
            // ORIG 0x8352e13：日志后返回 0
            LogManager::logFormat(
                1, "data_manager.cpp",
                __PRETTY_FUNCTION__,
                0x1193, "Quest Script Error Break !, [grow type] != -1 and [job change quest] = 1");
            return 0;
        }
        if (m_jobChangeQuest == 2)
        {
            if (!checkMatchedGrowType((ENUM_CHARACTERJOB)job, firstGrow, -1))
            {
                return 0;
            }
        }
        else if ((m_jobChangeQuest != 10) && (m_jobChangeQuest != 0x14))
        {
            if (!checkMatchedGrowType((ENUM_CHARACTERJOB)job, firstGrow, -1))
            {
                return 0;
            }
        }
    }

    // ORIG `mov 0x20;cmp;jle <pass>`：levelMin <= level 跳下一检查。
    // 返回 0 条件 = levelMin > level ⇒ level < levelMin。与 Ghidra
    // `if (iVar2 < this+0x20)` 一致。
    if (level < m_levelMin)
    {
        return 0;
    }
    // ORIG `mov 0x24;cmp;jge <pass>`：levelMax >= level 跳下一检查。
    // 返回 0 条件 = levelMax < level ⇒ level > levelMax（严格大于，不含等号）。
    // Ghidra `else if (this+0x24 < iVar2)` 一致。
    if (level > m_levelMax)
    {
        return 0;
    }
    // ORIG `mov 0x28;test;je <inner>; cmp;jle <inner>`：countLimit==0 或
    // countLimit<=questCount 进内部检查；返回 0 条件 = countLimit>0 且
    // countLimit > questCount。Ghidra `else if (this+0x28==0 || this+0x28<=iVar3)`
    // 一致（原写法 `<=` 与 ORIG 反向，语义差一位，已反）。
    if ((m_questCountLimit != 0) && (m_questCountLimit > questCount))
    {
        return 0;
    }

    if (m_type == 1)
    {
        // b28d-solo-Quest [P0/N96 清绑 + P168a 档①]：ORIG 0x08352f3b `call
        // isThereDailyTrainingQuestList; xor $0x1,%eax; test %al,%al; je`
        // ⇒ callee 返回类型 bool 化（S37 已同步 CDataManager.h:3468/.cpp:690）
        // + 裸形 `!call()` 精确复现。此前 `register asm("eax")` 绑定已删除
        // （P0 全域禁绑）；int callee 的 `&1`/`(uchar)^1`/`!call()` 三形探针
        // 分别多 and / 产 cmp+setne / 产 testl，均非 ORIG 形。
        if (!G_CDataManager()->isThereDailyTrainingQuestList(level, m_index))
        {
            return 0;
        }
    }
    // ORIG tail：`mov 13c;test;je <ret1>; mov 140;cmp -1;je <ret1>;`
    // 两个 == 判定共享同一个 return 1 尾块（0x223）。前置 `if(13c!=0 && 140!=-1)`
    // 逆置结构让 4.4 对两条件同发 je→共享块（probe p4 形态）。
    if ((m_field13c != 0) && (m_field140 != -1))
    {
        if ((m_field13c == -1) || (m_field13c == expertJobType))
        {
            if (expertJobLevel < m_field140)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
        // b28d-solo-Quest [N93附 复核]：ORIG 0x8352dxx 尾块序 ret0,ret0,ret1,ret1
        // ⇒ 两 ret0 合并到相邻发射（内层早返 + else 块），ret1 归 fall-through。
        return 1;
    }
    return 1;

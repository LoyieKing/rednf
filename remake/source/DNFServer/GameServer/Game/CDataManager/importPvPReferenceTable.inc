int importPvPReferenceTable(const char* path, stPvpRefTable_t& table)
{
    // ORIG 0x0890857b：ok(-0x25) 为函数级共享旗（入口 `movb $0,-0x25` 一次，
    // 各 ScanInt(&ok) 复用不重置）；line(-0x2c) 先于 load 构造；各块
    // 「先落 table 字段、后查 ok」（0x08908720/0x08908746/0x08908787）。
    bool ok = false;
    std::string line;
    if (!loadRDARScriptFile("", path))
    {
        return 0x1816;
    }
    // [b28d-solo-CDataManager] ORIG 0x089085bd 条件块在循环头（+0x42 直落）、
    // 各块失败路径 `jmp +0x42` 回边；4.4 对 while 恒旋转成入口 jmp（P164）。
    // 改 goto 头条件形复现（LoadScript 同法 AE 实证）。
pvp_cond:
    if (!ScanType(line, true))
        goto pvp_done;
    {
        if (line == "[pvp experience max grade]")
        {
            // [b28d-solo-CDataManager] ORIG 0x0890861c 无 cnt 槽往返
            // （`call ScanInt; add $0x1,%eax` 表达式直用，P0）——旧具名 cnt
            // 多 store+load 两条。
            table.m_count = 1 + ScanInt(&ok);
            if (!ok)
                return 0x181f;
            table.m_grades.reserve(table.m_count);
            table.m_grades.resize(table.m_count, stPvPGradeExp_t());
        }
        else if (line == "[pvp experience]")
        {
            // ORIG 0x0890867f：while(逗号) 形——grade 先落 -0x10、再查 ok；
            // 内层失败 ebx=0x182d；exp 落 -0xc；operator[] LHS 地址先行
            // （0x089086c1），stPvPGradeExp_t(grade,exp) 临时(-0x1c) 后构，
            // 末两 dword 写入。
            int grade;
        pvp_exp_scan:
            grade = ScanInt(&ok);
            if (!ok)
                goto pvp_cond;
            {
                int exp = ScanInt(&ok);
                if (!ok)
                    return 0x182d;
                table.m_grades[grade] = stPvPGradeExp_t(grade, exp);
            }
            goto pvp_exp_scan;
        }
        else if (line == "[pvp experience loss]")
        {
            table.m_pvpExpLoss1 = ScanInt(&ok);
            if (!ok)
            {
                return 0x1835;
            }
            table.m_pvpExpLoss2 = ScanInt(&ok);
            if (!ok)
            {
                return 0x1838;
            }
        }
        else if (line == "[pvp rank rate grade]")
        {
            table.m_rankRateGrade = ScanInt(&ok);
            if (!ok)
                return 0x183e;
        }
        else if (line == "[pvp rank rate]")
        {
            // [b28d-solo-CDataManager] ORIG 0x089087bd：v 无入口清零、
            // push_back 传左值槽（`push_backERKi`，ORIG call 8111126）；
            // 旧形 `int v = 0` + `static_cast<int&&>(v)` 多入口 store 与
            // 右值临时槽（-0x18）两条并选错 EOi 重载。
            int v;
        pvp_rate_scan:
            v = ScanInt(&ok);
            if (!ok)
                goto pvp_cond;
            table.m_rankRate.push_back(v);
            goto pvp_rate_scan;
        }
        else if (line == "[pvp normal battle time]")
        {
            PVP_NORMAL_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time]")
        {
            PVP_DM_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time]")
        {
            PVP_RELAY_BATTLE_TIME[0] = ScanInt((bool*)0);
        }
        else if (line == "[pvp normal battle time for tournament]")
        {
            PVP_NORMAL_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp deathmatch battle time for tournament]")
        {
            PVP_DM_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp relay battle time for tournament]")
        {
            PVP_RELAY_BATTLE_TIME[1] = ScanInt((bool*)0);
        }
        else if (line == "[pvp record order]")
        {
            table.m_recordOrder.clear();
            std::string job;
            // [b28d-SweepDataMgrInit] ORIG 0x08908931..0x08908974 实测：
            //   call ScanStr; xor $1,%eax; test %al,%al; jne 退出; body;
            //   jmp 回 ScanStr —— 循环头即 ScanStr 调用、失败取反物化退出。
            // 4.4 对 `while(ScanStr(&job))` 恒旋转成入口 jmp + 尾 test/jne
            // （缺 xor $1，入口多 jmp），与 ORIG 逐条不符；改 goto 头条件形
            // 复现（P164/P223；本函数外层与两个内层逗号 while 已同法）。
        pvp_record_scan:
            if (!ScanStr(&job))
                goto pvp_record_done;
            // [b28d-solo-CDataManager] ORIG 0x08908961 选
            // `push_backEOi`（右值）⇒ 直接传函数返回值，无具名 idx
            // （具名左值会选 ERKi，p167 MISSING EOi/EXTRA ERKi）。
            table.m_recordOrder.push_back(getCharacterJobToEnumValue(job));
            goto pvp_record_scan;
        pvp_record_done:
            ;
        }
        else if (line == "[pvp team hp correction]")
        {
            ScanFloat((bool*)0);  // 读取但丢弃
        }
    }
    goto pvp_cond;
pvp_done:
    return 0;  // EOF：解析成功

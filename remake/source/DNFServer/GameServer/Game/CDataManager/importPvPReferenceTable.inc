// ============================================================================
// importPvPReferenceTable —— ORIG 0x0890857b（_Z23importPvPReferenceTablePKcR15stPvpRefTable_t）
// 解析 Etc/pvp_ref.etc 填充 stPvpRefTable_t。返回 0 成功（token 流耗尽 EOF）；
// 非 0 为各键解析失败码，均与 ORIG 反汇编逐操作数一致。
// 反汇编证据（objdump -d df_game_r，0x890857b）：
//   loadRDARScriptFile("", path) 失败 → return 0x1816
//   Quote 循环：ScanType(line,1) 返回 0（EOF）→ return 0
//   "[pvp experience max grade]"：m_count=ScanInt+1；!ok→0x181f；reserve+resize
//   "[pvp experience]"：循环 {grade=ScanInt(!ok则break), exp=ScanInt(!ok→0x182d);
//                            m_grades[grade]={grade,exp}}
//   "[pvp experience loss]"：m_pvpExpLoss1=ScanInt(!ok→0x1835)；
//                             m_pvpExpLoss2=ScanInt(!ok→0x1838)
//   "[pvp rank rate grade]"：m_rankRateGrade=ScanInt(!ok→0x183e)
//   "[pvp rank rate]"：循环 push_back(ScanInt)，!ok break
//   6 个战斗时间键 → PVP_*_BATTLE_TIME[i] 对应地址（0x93f4ee0..0x93f4ef4）
//   "[pvp record order]"：clear；while(ScanStr) m_recordOrder.push_back(getCharacterJobToEnumValue)
//   "[pvp team hp correction]"：ScanFloat 读取但丢弃（fstp st0）
// 未知键忽略（继续 ScanType 循环）。
// ============================================================================
// 残余 DIFF 归因（o=318 n=309；ok 共享旗/落字段序/逗号 while/临时链已按
// ORIG 恢复）：① resize 值参签名 `vector<GradeExp>::resizeEjS1_`(0x0891f268)
// 为 4.1 libstdc++ 值传递形，4.4.7 头仅有 `resizeEjRKS1_` const& 形——跨
// libstdc++ 版本结构性不可翻（范式库「libstdc++ 内部结构差」行）；② 内层
// 逗号 while ORIG 头检测形 vs 4.4 循环旋转（P75/EvtC9 技巧①对该形未试全）；
// ③ 外 guard `xor$1;test;je` 与 `cnt add$1;mov %eax,%edx` 两段式 = N18a/N1；
// ④ 入口三组 `jmp +0x42; nop` 中转岛 = 跨版本空 else nop;jmp 负例。
// [b28d-solo-CDataManager DIFF-N] 本轮复核（o 318 / n 305→317，差 1 条）：
// p167 46/46 CLEAR。翻正项：① 外层 while 与两个内层逗号 while 改 goto 头条件
// 形（P223/P283；ORIG 0x089085bd 条件在循环头、失败路径 `jmp +0x42`），消
// 入口 jmp 旋转并复现 ORIG 的 ScanInt→查 ok→continue 直落序；② m_count 字段
// int→unsigned（P211 异型化，RefPvpGrade.h 已 R2 广播）复现 ORIG
// `add $0x1,%eax; mov %eax,%edx` 两段 store；③ grade 去入口清零、`1+ScanInt`
// 书写序。残差 1 条 = 4.4 为 goto 回边生成 4 个 `nop;jmp` 中继岛 vs ORIG 2 个
// （jump-threading 跨版本差），另 resize 值参 `EjS1_` vs const& `EjRKS1_` 属
// 跨 libstdc++ 结构性差异。语义完成、byte 层 N 封顶。
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
}
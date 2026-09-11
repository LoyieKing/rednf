// ORIG 0x08a3b0ad（0x1515 字节）：带 map 形参重载
// _Z17InitMonsterScriptPKcS0_PSt3mapIiiSt4lessIiESaISt4pairIKiiEEES0_。
// 与 3 参数版（0x08a11311）逐条 objdump 比对（本轮）：差异仅
//  ①loadRDARScriptListFile 第 4 参装载 0x10(%ebp)（map）而非 movl $0；
//  ②logFormat 行号本版 0x2b22/0x2b38/0x2b54/0x2b6f（3 参版 0x7e6/0x7fc/0x818/0x833）；
//  ③limit slowdown 段 map value 本版 0x2c6e/0x2c71/0x2c85/0x2c99
//    （3 参版 0x932/0x935/0x949/0x95d）；④__FUNCTION__/static 变量/字符串地址不同；
// 其余指令序列、跳转结构逐条相同（两版函数级 static __lookup/__init_lock/__inited 各自独立：
// ORIG 0x94fff80/0x94fff98/0x94fff9c vs 0x94ff8a8/0x94ff8b8/0x94ff8d4）。
// [本轮 2026-09-09] gap 8→0（raw 1240 vs 1232 → 1240 vs 1240；p167 168/168 CLEAR，
//   余 C1/C2、D1/D2 同址别名 P93 豁免）：真缺口 = 错常量实参——原源码 4 处 map value
//   与 4 处 logFormat 行号误抄 3 参版常量，已按本版 ORIG 修正；另同 3 参版
//   P22/P133 负极性改写（if (!inited)…else…）与 first(-0x78) 预清零（P198①/P0）。
// 注：本函数体内行内注释的 0x8a11xxxx/0x8a12xxxx 地址系上轮自 3 参版复制的同构位置
// 注释；本版真实地址以 objdump 0x08a3b0ad 起为准（仅注释，机器码不受影响）。
// [本轮 2026-09-10 结构归位 · byte 层封顶] 与 3 参版同批：①first/ok/param 上提至
//   tableIdx 体内（ORIG 0x8a3b205 first=0 → ok=false → param C1 → row=0）；②m_grid
//   双视角写；③三段难度表循环改 `while(true){…;++r; if(!ok||r==13) break;}`；
//   ④deadIdx@-0x84；⑤resize 分支 def 改匿名临时；⑥row==size 比较改 unsigned 形。
//   ⇒ 指令数 1227→1221 与 ORIG 全等，flat ndiff → 36；p167 168/168 CLEAR。
//   残余同 3 参版 = N93/N98 槽分配策略差。
bool InitMonsterScript(const char* dir, const char* list, std::map<int, int>* map,
                       const char* apcBonus)
{
    if (list != 0)                          // ORIG cmpl $0x0,0xc(%ebp)
    {
        strncpy(g_szMonsterScriptBaseDirectory, dir, 0x100);
        // ORIG 0x8a11342..0x8a113a0：本 TU 仅此两处**内联**分隔符判定
        // （strlen×2 短路 + strcat 内建展开 strlen+memcpy 2B），
        // 其余 *Script 段为真实 AppendDirSeparator 调用——勿用 helper。
        if (g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '\\' &&
            g_szMonsterScriptBaseDirectory[strlen(g_szMonsterScriptBaseDirectory) - 1] != '/')
        {
            __builtin_strcat(g_szMonsterScriptBaseDirectory, "\\");
        }

        if (!loadRDARScriptListFile(&g_stMonsterScriptFileList,
                                    g_szMonsterScriptBaseDirectory, list, map))
            return false;

        // ORIG 0x8a113d9: 临时空向量 → resize(5, 临时)（0x94ff278）。
        // 传具名 empty 会多发 vector C1ERKS + 2×D1Ev（ORIG 无）。
        g_monsterBaseParameters.resize(5, std::vector<MonsterBaseParameter>());

        for (int tableIdx = 0; tableIdx <= 4; ++tableIdx)
        {
            // ORIG 0x93f54c4[tableIdx] 文件名，经全局基目录打开。
            if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory,
                                    MONSTER_BASIS_PARAMETER_TABLE_NAMES[tableIdx]))
                return false;

            // [槽布局对齐] ORIG 0x8a3b205(first=0) → 0x8a3b20c(ok=false) →
            // param C1 → row=0：三者声明在 row 循环**外**（C1 每 tableIdx 仅一次，
            // row 循环内不再构造）⇒ 与 3 参版同构。
            int first = 0;                          // ORIG -0x78 具名中转
            bool ok = false;
            MonsterBaseParameter param;             // ORIG -0x26c(%ebp)
            for (int row = 0;; ++row)       // ORIG -0x74(%ebp)
            {
                first = ScanInt(&ok);
                if (!ok)                                // ORIG 0x8a3b245 jne
                    break;
                param.m_value[0] = first;
                // ORIG 列序（0x8a114b5..0x8a116a0）：3,4,5,6,1,2,7..12 + 两循环 + 21..23
                param.m_value[3] = ScanInt((bool*)0);
                param.m_value[4] = ScanInt((bool*)0);
                param.m_value[5] = ScanInt((bool*)0);
                param.m_value[6] = ScanInt((bool*)0);
                param.m_value[1] = ScanInt((bool*)0);
                param.m_value[2] = ScanInt((bool*)0);
                param.m_value[7] = ScanInt((bool*)0);
                param.m_value[8] = ScanInt((bool*)0);
                param.m_value[9] = ScanInt((bool*)0);
                param.m_value[10] = ScanInt((bool*)0);
                param.m_value[11] = ScanInt((bool*)0);
                param.m_value[12] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x70 循环
                    param.m_grid[3][k] = ScanInt((bool*)0);
                for (int k = 0; k <= 3; ++k)            // ORIG -0x6c 循环
                    param.m_grid[4][k] = ScanInt((bool*)0);
                param.m_value[21] = ScanInt((bool*)0);         // +0x54
                param.m_value[22] = ScanInt((bool*)0);         // +0x58
                param.m_value[23] = ScanInt((bool*)0);         // +0x5c

                // ORIG P157：operator[] 每用点独立取（size×2/push_back/resize/
                // 赋值共 5 次），勿建 vec 引用缓存。
                if (static_cast<unsigned int>(row) ==
                    g_monsterBaseParameters[tableIdx].size())
                    g_monsterBaseParameters[tableIdx].push_back(param);   // ORIG 0x8a11670
                // ORIG 0x8a1169a seta（size_t 无符号比较），勿 cast 成 int（会生 setl）。
                else if (static_cast<unsigned int>(row) >
                         g_monsterBaseParameters[tableIdx].size())
                {
                    // ORIG 0x8a116a1: 默认元素 resize(row+1) 后再赋值。
                    g_monsterBaseParameters[tableIdx].resize(row + 1, MonsterBaseParameter());
                }
                g_monsterBaseParameters[tableIdx][row] = param;          // ORIG rep movsl 0x18
            }
        }
    }

    // 注意：apcBonus 用原始 dir 形参打开（ORIG 0x8a11757 直接压 0x8(%ebp)）。
    if (!loadRDARScriptFile(dir, apcBonus))
    {
        LogManager::logFormat(0, "../RDARScriptObject.cpp", __FUNCTION__,
                              0x2b22, "error : MonsterApcDifficultyBonus.tbl is missing!!");
        return false;
    }

    // 同 3 参版：[ORIG 初始化序] movl $0,-0x84（原码遗留死局部）→ movl $0,-0x80(r)
    // → movb $0,-0x151(ok)。声明序照抄以复现槽位。
    int deadIdx = 0;                            // ORIG -0x84 [推断：原码遗留未使用局部]
    int r = 0;
    bool ok = false;
    // 难度加成主表：13 行 × 5 列（缺项报错行号 0x7fc，末行/末列豁免）。
    while (true)
    {
        for (int c = 0; c <= 4; ++c)
        {
            DIFFICULTY_OF_DUNGEON_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 4 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b38,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 死塔加成：13 行 × 4 列（行号 0x818）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            DEATH_TOWER_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b54,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }
    // 血条系统加成：13 行 × 4 列写入 5 列数组前 4 列（行号 0x833）。
    r = 0;
    while (true)
    {
        for (int c = 0; c <= 3; ++c)
        {
            BLOOD_SYSTEM_BONUS[r][c] = ScanFloat(&ok);
            if (!ok && c != 3 && r != 12)
            {
                LogManager::logFormat(
                    0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2b6f,
                    "error : MonsterApcDifficultyBonus.tbl is incorrect!! index %d i %d",
                    r, c);
                break;
            }
        }
        ++r;
        if (!ok || r == 13)
            break;
    }

    for (int k = 0; k <= 4; ++k)            // ORIG 0x8a119fd clear 循环
        MONSTER_DIFFICULTY_BONUS[k].clear();

    // ---- 标签化段（[easy]..[hero]/[dungeon party balance]/... ）----
    std::string token;
    for (;;)                                // ORIG 0x8a11a39 主循环头
    {
        if (!ScanType(token, true))         // ORIG 0x8a127e6 → 成功返回
            return true;

        // ORIG 429-462：5 连短路比较 + bool 物化，块内再逐项定 difficulty。
        if (token == "[easy]" || token == "[medium]" || token == "[hard]" ||
            token == "[ultimate]" || token == "[hero]")
        {
            int difficulty = 0;                 // ORIG -0x58
            if (token == "[easy]")
                difficulty = 0;                 // ORIG 0x8e1eceb
            else if (token == "[medium]")
                difficulty = 1;                 // 0x8e1ecf2
            else if (token == "[hard]")
                difficulty = 2;                 // 0x8e1ecfb
            else if (token == "[ultimate]")
                difficulty = 3;                 // 0x8e1ed02
            else if (token == "[hero]")
                difficulty = 4;                 // 0x8e1ed0d

            // ---- 难度组读取器（ORIG 0x8a11ba7）：每 consume 1 个弃置 float +
            //      最多 13 个 float 为一组；组不满或读失败则该难度段结束。
            for (;;)
            {
                ScanFloat(&ok);                 // 组头 float（fstp %st(0) 弃置）
                if (!ok)
                    break;                      // → 下一主标签
                std::vector<float> values;      // ORIG -0x168 局部向量
                values.clear();                 // ORIG 0x8383ad8
                values.reserve(13);             // ORIG reserve 0xd
                for (int i = 0; i <= 12; ++i)
                {
                    float f = ScanFloat(&ok);
                    if (!ok)
                        break;
                    values.push_back(f);
                }
                if (!ok)                        // 不完整组整体丢弃（ebx=0 路径）
                    break;
                MONSTER_DIFFICULTY_BONUS[difficulty].push_back(values);
            }
            continue;
        }
        else if (token == "[dungeon party balance]")    // 0x8e1ed14
        {
            DUNGEON_PARTY_BALANCE_TABLE.clear();
            for (;;)                        // ORIG 0x8a11cf2
            {
                DungeonPartyBalance entry;  // ORIG 栈复用 -0x26c 缓冲
                entry.m_a = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                entry.m_b = static_cast<float>(ScanInt(&ok));
                if (!ok)
                    break;
                for (int r = 0; r <= 12; ++r)
                {
                    for (int c = 0; c <= 3; ++c)
                    {
                        entry.m_grid[r][c] = ScanFloat(&ok);
                        if (!ok)
                            break;
                    }
                    if (!ok)
                        break;
                }
                if (!ok)                    // 不完整行丢弃 → 下一个标签
                    break;
                DUNGEON_PARTY_BALANCE_TABLE.push_back(entry);
            }
            continue;
        }
        else if (token == "[mesmerization damage balance]")  // 0x8e1ed2c
        {
            // ORIG 顺序：sub C1(661) → h C1(665) → clear×3(667-671) → reserve×3(674-680)。
            std::string sub;
            int a;                          // ORIG -0x4c（具名中转，P0 槽往返）
            int b = 0;                      // ORIG -0x48（P198① 预清必写）
            DungeonHoldBalance h;           // ORIG 0x8a3deee @0x8a3bbd5：段首一次，三支复用
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.clear();       // ORIG 0x8a11e46
            NAMED_MONSTER_HOLD_BALANCE_TABLE.clear();
            BOSS_MONSTER_HOLD_BALANCE_TABLE.clear();
            NORMAL_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8); // 各 reserve 200
            NAMED_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);
            BOSS_MONSTER_HOLD_BALANCE_TABLE.reserve(0xc8);

            while (ScanType(sub, true))     // ORIG 0x8a12046
            {
                if (sub == "[/mesmerization damage balance]")   // 0x8e1ed4c
                    break;
                if (sub == "[normal]")      // 0x8e1ed6c
                {
                    for (int i = 0; i <= 199; ++i)          // cmpl $0xc7,setle
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NORMAL_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[boss]")   // 0x8e1ed75
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        BOSS_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
                else if (sub == "[named]")  // 0x8e1ed7c
                {
                    for (int i = 0; i <= 199; ++i)
                    {
                        a = ScanInt(&ok);
                        b = ScanInt(&ok);
                        h.m_a = a;
                        h.m_b = b;
                        NAMED_MONSTER_HOLD_BALANCE_TABLE.push_back(h);
                    }
                }
            }
            continue;
        }
        else if (token == "[limit slowdown]")               // 0x8e1ed84
        {
            // ORIG 函数级静态：__lookup map<const char*,int,less_TCHAR>
            // （0x94ff8a8，guard 0x94ff8b0）+ __init_lock SpinLock
            // （0x94ff8b8，guard 0x94ff8b0 复用）+ __inited（0x94ff8d4）。
            // ORIG 0x8a3be4e..0x8a3c413：P333「首次建表、之后查表」状态机形。
            // 段首 sub C1 → a=0(-0x38) → b=0(-0x34) → bStop=false(-0x2d) → e C1；
            // 每轮按 id switch 派发：未建表时各 case 体先查 __inited 再逐条 insert
            // 并 fallthrough（首轮一次插满 4 条），尾部 leave+置位+goto retry 重查；
            // 已建表时执行真实解析。id==0/0x932（[/limit slowdown]）置 bStop 后 break。
            std::string sub;
            int a = 0;                  // ORIG -0x38（三支复用，P198① 预清必写）
            int b = 0;                  // ORIG -0x34
            bool bStop = false;         // ORIG -0x2d
            stLimitSlowDown e;          // ORIG 0x8a3df1a：段首一次，三支复用
            int id;
            for (;;)
            {
                if (!ScanType(sub, true))   // ORIG 0x8a3c3f5
                    break;                  // 段结束 → 下一主标签

                // P177：ORIG 的 static 三件套声明在循环体内（guard 每轮执行）。
                static std::map<const char*, int, less_TCHAR> __lookup;
                static SpinLock __init_lock;
                static bool __inited = false;

            retry:
                if (__inited)
                {
                    std::map<const char*, int, less_TCHAR>::const_iterator it =
                        __lookup.find(sub.c_str());
                    id = (it != __lookup.end()) ? it->second : -1;
                }
                else
                {
                    __init_lock.enter();
                    if (__inited)
                    {
                        __init_lock.leave();
                        goto retry;                     // 回跳重查
                    }
                    id = 0;                             // 建表哨兵
                }

                switch (id)
                {
                case 0:
                case 0x2c6e:                            // [/limit slowdown] 终止标记
                    // P22/P133：ORIG 0x8a3c0ad movzbl __inited; xor $1; test; je 8a3c10a
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/limit slowdown]", 0x2c6e));
                        // fall through
                    }
                    else
                    {
                        bStop = true;
                        break;
                    }
                case 0x2c71:                            // [normal]
                    // ORIG 0x8a3c113 movzbl __inited; xor $1; test; je 8a3c173
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal]", 0x2c71));
                        // fall through
                    }
                    else
                    {
                        NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);               // 首列索引（弃）
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NORMAL_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x2c85:                            // [boss]
                    // ORIG 0x8a3c1f7 movzbl __inited; xor $1; test; je 8a3c257
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss]", 0x2c85));
                        // fall through
                    }
                    else
                    {
                        BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            BOSS_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                case 0x2c99:                            // [named]
                    // ORIG 0x8a3c2db movzbl __inited; xor $1; test; je 8a3c338
                    if (!__inited)
                    {
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[named]", 0x2c99));
                        break;
                    }
                    else
                    {
                        NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.clear();
                        for (int i = 0; i <= 199; ++i)
                        {
                            ScanInt(&ok);
                            a = ScanInt(&ok);
                            b = ScanInt(&ok);
                            e.m_a = a;
                            e.m_b = b;
                            NAMED_MONSER_LIMIT_SLOWDOWN_TABLE.push_back(e);
                        }
                        break;
                    }
                default:                                // id==-1（未知子标签）
                    break;
                }

                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    goto retry;                         // 回跳重查
                }
                if (bStop)
                    break;
            }
            continue;
        }
        else if (token == "[adjust hp gauge]")              // 0x8e1eda7
        {
            // ORIG 段首 float v = 1.0f（-0x1c，三支复用）；
            // ORIG 段首构造 DungeonHoldBalance 局部对象且从未使用
            // （纯编译产物）——P204 照抄：ORIG 无条件发射该 C1 调用。
            std::string sub;
            float v = 1.0f;                 // ORIG -0x1c
            DungeonHoldBalance h;
            (void)h;
            while (ScanType(sub, true))     // ORIG 0x8a1279a
            {
                if (sub == "[/adjust hp gauge]")                // 0x8e1edb9
                    break;
                if (sub == "[normal]")
                {
                    v = ScanFloat(&ok);
                    NORMAL_MONSTER_GAUGE_ADJUST = v;            // 0x93f4d10
                }
                else if (sub == "[boss]")
                {
                    v = ScanFloat(&ok);
                    BOSS_MONSTER_GAUGE_ADJUST = v;              // 0x93f4d18
                }
                else if (sub == "[named]")
                {
                    v = ScanFloat(&ok);
                    NAMED_MONSTER_GAUGE_ADJUST = v;             // 0x93f4d14
                }
            }
            continue;
        }
        else
        {
            continue;                       // 未识别标签：ORIG 0x8a126b0 je
        }
    }
}
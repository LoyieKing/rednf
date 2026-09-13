bool ImportDungeonScript(STDungeonScript* pScript, const char* path)
{
    // b28d-solo-STDungeonScript 判词（9343 vs 9332 insn，gap 11；§6.2 三件套）：
    // ①p167 CLEAR 1079=1079（MISSING 0 / EXTRA 0）。此前 EXTRA 16（8 类型 D1Ev 各 +2）
    //   根因为用户空析构致 4.4 判 non-trivial（P317/P12：ORIG 8 个 D1/D2 全 W size=5）；
    //   该跨泳道修复（CDungeon.h/DungeonClearCondition.h/CDungeon.cpp/W9_StructMisc.cpp）
    //   落地后本件 -142 insn，gap 131→11、p167 归零。
    // ②语义对位：printf 3/puts 2/memset 2/ScanInt 200/静态五组 guard+atexit 10/
    //   map insert 172/string ctor-dtor 全对位。
    // ③残余 DIFF = N82 全局槽号平移封顶：帧 0x137c vs ORIG 0x134c、最深槽 -0x1360 vs
    //   -0x1330（平移 0x30，outgoing 同为 0x1c）；ORIG 另有具名局部 `mov %eax,-0x128`
    //   （script 缓存），4.1 复用死槽 / 4.4 不复用（P112/N98/P182：声明序不可翻、
    //   pad/volatile 反劣）。语义逐块核对完成。
    //
    // BigDungeon 2026-09-09 复核（同一 .o 重测 AE 口径 ndiff 5914→5398；
    // 槽归一辅助口径 964→888。台账 all_tu_ledger.tsv 记的 11131/N89 是 b28c 前
    // 旧 our=9404 的数，已过期。p167 调用点差集两侧 1079=1079 全等（MISSING 0 /
    // EXTRA 0），即 4.1 out-of-line STL 实例 call 在我方逐一对位存在
    // ⇒ N89「内联决策差」对本件不成立，残差全部为栈槽号漂移 + 少量形态差）：
    //   a) 形参缓存：ORIG 在 `Clear()` 之后把 script 溢出到 -0x128，其后 235 处
    //      字段访问全部 `mov -0x128(%ebp),%eax` 重载（0x8(%ebp) 仅余 2 处）。
    //      4.4 直读形参 ⇒ 基址寄存器用 %edx、且每处少一条 mov。改为
    //      `STDungeonScript* volatile script = pScript;` 后形态与基址寄存器全对位
    //      （AE 口径 -520）。槽号仍差（4.1 把该溢出放进高带，4.4 放进深带）。
    //   b) 真缺失分支两处（已补，行为向 ORIG 收敛）：
    //      · [event monster2] 循环内后 4 个 ScanInt 失败 = import 失败
    //        （088ee68b/6b7/6e3/70f `mov $0x0,%ebx`），仅首个（088ee665）为跳出；
    //      · [tournament clear reward gold rate] ScanFloat 失败 = import 失败
    //        （088effa7 → `movzbl -0x109d,%ebx` 收尾桩），我方原先完全不判。
    //   c) 逐 tag 审计 Scan* 失败动作（273 个调用点，ORIG 侧从反汇编判定
    //      FAIL/break，比对我方源码）：除 b) 两项外全部一致；其余"不一致"经
    //      逐点查汇编核实为工具误判（子表 tag 归属、`ScanInt((bool*)0)`、
    //      `FindIndexByFullScan` 名字含 Scan 被误计）。
    //   d) 字段写入宽度/偏移比对（94 个脚本字段偏移）：仅 0x2ac/0x44c 两处
    //      两侧基址对象不同（子结构元素指针），非布局建模错。
    //   e) 残余 888（槽归一口径）逐块看均为 4.1/4.4 形态差：`cmpl $0x5,-0x88(%ebp)`
    //      vs `mov+cmp`、bool 存 %esi vs 直接在 %eax、循环尾置等 ⇒ 命中 N 类。
    // ORIG 0x88e5fbd：loadRDARScriptFile(g_szDungeonScriptBaseDirectory, path)
    if (!loadRDARScriptFile(g_szDungeonScriptBaseDirectory, path))
    {
        return false;
    }
    pScript->Clear();
    // ORIG 0x88e6017/0x88e601c：`mov 0x8(%ebp),%eax` + `mov %eax,-0x128(%ebp)` ⇒
    // 4.1 把 script 参数溢出到一个具名栈槽，其后 235 处字段访问一律
    // `mov -0x128(%ebp),%eax` 重载。用 volatile 局部指针复现该重载形态
    // （语义等价：指针值全程不变，仅访问方式由形参改为局部量）。
    // [E06 2026-09-10 实验] 去 volatile：volatile 形使 4.4 把它落深带 -0x1094；
    // 普通具名指针期望落回标量区（ORIG -0x128）。
    // [E06 2026-09-10 复测·保留 volatile] 去 volatile 变体：script 落 -0x130
    //（更近 ORIG -0x128）、指令数 9255→9270（gap −8→+7），但归一化 diff 内容
    // 175−/160+ = 335 行 vs volatile 形 121−/131+ = 252 行 ⇒ volatile 更优，保留。
    // 残余为 N82 非恒定槽平移（line/token/iv0/ok/mazeLocal/mazeInited 各段常数
    // 不同：−8/0/+4/+4/+0x60/+8；帧 ORIG 0x134c vs 本方 0x13ac）⇒ 按 P331
    // 「多段不同常数 ⇒ 封 N」封顶。
    STDungeonScript* volatile script = pScript;
    script->m_vec384.clear();
    // ORIG 0x88e6027：getDirectoryPath(&script->m_str17c, "%s%s", 基目录, path)
    // → m_str17c 为脚本完整路径（目录 + 文件名）。
    getDirectoryPath(script->m_str17c, "%s%s", g_szDungeonScriptBaseDirectory, path);
    std::string line;
    std::string token;
    int iv0 = 0;      // ORIG -0x109c：子表 ScanInt(int*) 共用实参槽（Ghidra local_10a0）
    bool ok = false;  // ORIG -0x109d：ScanInt(bool*) 实参槽（Ghidra local_10a1）
    // ORIG 函数级局部（-0x1324 MazeScript / -0x121 flag）
    MazeScript mazeLocal;
    bool mazeInited = false;
    int v1;
    int v2;

    // ORIG 惰性初始化：五组静态（主表 + 4 张嵌套子表），每组带独立
    // __cxa_guard 初始化的 map/SpinLock 与 bool __inited_N。ORIG 的声明位于
    // 各自的使用循环内（LAB_088e608f / LAB_088e9bee / LAB_088ea0d8 /
    // LAB_088ebe77 / LAB_088f0202 处每轮 __cxa_guard 测试），因此下方把声明
    // 放进对应 while 循环体首；首过以 typeId=0 进入该表 case 链逐例 insert，
    // 链尾 leave + 置位 + goto 循环头重试当前 tag。
    typedef std::map<const char*, int, less_TCHAR> DungeonTagMap;
    int tagId;

    // 标签分派：主循环每轮 ScanType → __lookup 表查找 → switch。
    while (true)
    {
        // ORIG 0x88e606e：ScanType 线性置于循环头（4.4 对 while(cond) 会把
        // 检查挪到循环尾并加前置 jmp；改写成 if-break 恢复线性形态）。
        if (!ScanType(line, true))
            break;
    retry_dispatch:
        static DungeonTagMap __lookup;
        static bool __inited = false;
        static SpinLock __init_lock;
    tag_dispatch:
        if (__inited)
        {
            DungeonTagMap::const_iterator it =
                __lookup.find(line.c_str());
            tagId = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry_dispatch;
            }
            tagId = 0;   // ORIG：首过以 typeId=0 进入主表填充链（case 0）
        }
        switch (tagId)
        {
case 0:
case 0x75:  // "[name]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[name]", 0x75));
    }
    else
    {
        ScanStr(&script->m_str1c);  // +0x1c
        break;
    }

case 0x79:  // "[explain]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[explain]", 0x79));
    }
    else
    {
        ScanStr(&script->m_str20);  // +0x20
        break;
    }

case 0x7d:  // "[entering title]"（ORIG LAB_088e6d1f/88e6dcd：ByDifficulty→m_34=1
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[entering title]", 0x7d));
    }
    else
    {
        // 且循环收集到 m_vec38；否则首串直接赋 m_str24）
        {
            std::string title;
            if (!ScanStr(&title))
                break;
            if (title == "ByDifficulty") {  // 0x8e0d55a
                script->m_34 = 1;  // +0x34
                // ORIG 0x88e6dcd 循环头为 `call; xor $1; test; je <body>`（否定形，
                // P292 bool callee + `if(!f())`）⇒ 写成 while(true)+否定 break。
                while (true)             // @88e6dcd 循环 push 到 +0x38
                {
                    if (!ScanStr(&title))
                        break;
                    script->m_vec38.push_back(title);  // @88e6dfb push_back(+0x38)
                }
            }
            else
            {
                script->m_str24 = title;  // @88e6e18 SsaSERKSs（非 ByDifficulty 时单标题串）
            }
        }
        break;
    }

case 0x98:  // "[entering title next]"（ORIG LAB_088e6ead/88e6efb：同构 [entering title]，
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[entering title next]", 0x98));
    }
    else
    {
        // 目标 +0x44/+0x48 vector/+0x54）
        {
            std::string title;
            if (!ScanStr(&title))
                break;
            if (title == "ByDifficulty") {  // 0x8e0d55a
                script->m_44 = 1;  // +0x44
                // 同 [entering title]：ORIG 0x88e6efb 为否定形循环头。
                while (true)             // @88e6efb 循环 push 到 +0x48
                {
                    if (!ScanStr(&title))
                        break;
                    script->m_vec48.push_back(title);  // @88e6f29 push_back(+0x48)
                }
            }
            else
            {
                script->m_str54 = title;  // @88e6f46 SsaSERKSs（+0x54）
            }
        }
        break;
    }

case 0xb3:  // "[cutscene image]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[cutscene image]", 0xb3));
    }
    else
    {
        ScanStr(&script->m_str28);  // +0x28
        script->m_2c = ScanInt((bool*)0);  // +0x2c
        break;
    }

case 0xb9:  // "[dungeon loading image path]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon loading image path]", 0xb9));
    }
    else
    {
        ScanStr(&script->m_str58);  // +0x58
        break;
    }

case 0xbe:  // "[minimap image]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[minimap image]", 0xbe));
    }
    else
    {
        ScanStr(&script->m_str30);  // +0x30
        break;
    }

case 0xc3:  // "[worldmap info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[worldmap info]", 0xc3));
    }
    else
    {
        // ORIG LAB_088e70e9（Ghidra C 实证）：2x2 循环 4 个 int → m_5c[4]
        // （偏移 (2i+j+0x14)*4+0xc = +0x5c..+0x68），随后 ScanStr → m_str74(+0x74)。
        for (int rr = 0; rr < 2; ++rr)
        {
            for (int cc = 0; cc < 2; ++cc)
            {
                script->m_5c[rr * 2 + cc] = ScanInt((bool*)0);
            }
        }
        ScanStr(&script->m_str74);
        break;
    }

case 0xca:  // "[herosmode required quest]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[herosmode required quest]", 0xca));
    }
    else
    {
        script->m_14 = ScanInt((bool*)0);  // +0x14
        break;
    }

case 0xcf:  // "[background pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[background pos]", 0xcf));
    }
    else
    {
        script->m_488 = ScanInt((bool*)0);  // +0x488
        break;
    }

case 0xd7:  // "[herosmode enable]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[herosmode enable]", 0xd7));
    }
    else
    {
        script->m_c = ScanInt((bool*)0) != 0;  // +0xc
        break;
    }

case 0xdd:  // "[worldmap pattern info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[worldmap pattern info]", 0xdd));
    }
    else
    {
    {
        // ORIG LAB_088e7338（Ghidra C + STWorldmapPattern.h）：ScanInt x/y →
        // GetWorldMapCoordFromTable(x,y,pat) 取表格图案；把 pat.m_coord（+0x08 6 int）平铺拷贝到
        // m_5c[0..5]；ScanStr → m_str74；ScanInt → m_0；m_4 = x-1；m_8 = y-1。
        int wx = ScanInt((bool*)0);
        int wy = ScanInt((bool*)0);
        STWorldmapPattern pat;
        GetWorldMapCoordFromTable(wx, wy, pat);
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                script->m_5c[i * 2 + j] = pat.m_coord.m_coord[i][j];
            }
        }
        ScanStr(&script->m_str74);
        script->m_0 = ScanInt((bool*)0);
        script->m_4 = wx - 1;
        script->m_8 = wy - 1;
        break;
    }
    }

case 0xff:  // "[monsterapc diff table]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monsterapc diff table]", 0xff));
    }
    else
    {
        // ORIG 0x88e7538：块级临时 string ctor→ScanStr→operator= m_str10→dtor；
        // +0x17c 已由函数入口保存脚本完整路径，不能在此覆盖。
        {
            std::string diffName;
            ScanStr(&diffName);
            script->m_str10 = diffName;
        }
        break;
    }

case 0x109:  // "[minimum required level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[minimum required level]", 0x109));
    }
    else
    {
        script->m_78 = ScanInt((bool*)0);  // +0x78
        break;
    }

case 0x10d:  // "[required item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[required item]", 0x10d));
    }
    else
    {
        script->m_requiredItem7c.m_itemIdx = ScanInt((bool*)0);  // +0x7c
        script->m_requiredItem7c.m_itemCount = ScanInt((bool*)0);  // +0x80
        // ORIG 0x088e7657（0x16fd）：`movl $0x0,(%esp)` ⇒ 实参 (bool*)0（非 &ok）。
        script->m_requiredItem7c.m_enableFlag = ScanInt((bool*)0) != 0;  // +0x84
        break;
    }

case 0x114:  // "[event dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event dungeon]", 0x114));
    }
    else
    {
        script->m_88 = ScanInt((bool*)0) != 0;  // +0x88
        break;
    }

case 0x118:  // "[event required item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event required item]", 0x118));
    }
    else
    {
        script->m_requiredItem8c.m_itemIdx = ScanInt((bool*)0);  // +0x8c
        script->m_requiredItem8c.m_itemCount = ScanInt((bool*)0);  // +0x90
        // ORIG 0x088e7785（0x182b）：同 (bool*)0 形。
        script->m_requiredItem8c.m_enableFlag = ScanInt((bool*)0) != 0;  // +0x94
        break;
    }

case 0x11e:  // "[clear reward item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[clear reward item]", 0x11e));
    }
    else
    {
        script->m_vec98.clear();  // vector<pair<int,int>> @+0x98
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        int v2 = ScanInt(&ok);
        if (!ok) break;
        script->m_vec98.push_back(std::pair<int, int>(v1, v2));
        }
        break;
    }

case 0x18c:  // "[recommended level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[recommended level]", 0x18c));
    }
    else
    {
        script->m_450 = ScanInt((bool*)0);  // +0x450
        script->m_454 = ScanInt((bool*)0);  // +0x454
        break;
    }

case 0x191:  // "[coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[coin limit]", 0x191));
    }
    else
    {
        script->m_a4 = ScanInt((bool*)0);  // +0xa4
        break;
    }

case 0x195:  // "[character coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[character coin limit]", 0x195));
    }
    else
    {
        script->m_a8 = ScanInt((bool*)0);  // +0xa8
        break;
    }

case 0x199:  // "[basis level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[basis level]", 0x199));
    }
    else
    {
        script->m_ac = ScanInt((bool*)0);  // +0xac
        break;
    }

case 0x19d:  // "[experience increasing point]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[experience increasing point]", 0x19d));
    }
    else
    {
        // ORIG 0x088e6b09（0x1baf）：`movl $0x0,(%esp)` ⇒ (bool*)0。
        script->m_b0 = ScanFloat((bool*)0);   // +0xb0（float；export 对称 %.3f）
        break;
    }

case 0x1a1:  // "[champion]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[champion]", 0x1a1));
    }
    else
    {
        // ORIG LAB_088e7b6f（Ghidra C）：循环 5×ScanInt → m_b4[i]（偏移 (i+0x2c)*4+4=+0xb4）。
        for (int ci = 0; ci < 5; ++ci)
        {
            script->m_b4[ci] = ScanInt((bool*)0);
        }
        break;
    }

case 0x1a6:  // "[item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[item]", 0x1a6));
    }
    else
    {
        script->m_vecc8.clear();  // vector<pair<int,int>> @+0xc8
        while (true) {
        // ORIG pair<int,int>::pair(int,int)（080dd93a@088e7cdb）：v1 具名 +
        // 第二值 ScanInt(NULL)（忽略 ok），4.4.7 形参 const _U& 推导值类型。
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vecc8.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1b3:  // "[realdungeon checkup]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[realdungeon checkup]", 0x1b3));
    }
    else
    {
    {
        // ORIG LAB_088e7d00（Ghidra C）：3 个 vector reserve(100)；循环 ScanStr 子标签，
        // [monster]→m_vec458、[passive]→m_vec464、[apc]→m_vec470：先 ScanInt 读数量，
        // 再循环 ScanInt push（单项失败退出子循环）；遇到 "[/realdungeon checkup]" 或 EOF 结束。
        script->m_vec458.reserve(100);
        script->m_vec464.reserve(100);
        script->m_vec470.reserve(100);
        std::string subtok;
        while (true)
        {
            if (!ScanStr(&subtok)) break;
            if (subtok == "[/realdungeon checkup]") break;
            if (subtok == "[monster]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    // ORIG 0x088e7e3e 起三个子循环失败各出独立 nop 中继岛
                    // （088e7fb8/7fbb/7fbe → 088e7fbf）；4.4 实测 goto 尾标签形与
                    // while 形产物同 break 形（`je body; nop; jmp exit`，无岛）
                    // ⇒ P223 负例，保留 for+break 自然形（gap 差 2/处，N 封顶）。
                    if (!ok) break;
                    script->m_vec458.push_back(v);
                }
            }
            else if (subtok == "[passive]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    if (!ok) break;
                    script->m_vec464.push_back(v);
                }
            }
            else if (subtok == "[apc]")
            {
                int n = ScanInt((bool*)0);
                for (int i = 0; i < n; ++i)
                {
                    int v = ScanInt(&ok);
                    if (!ok) break;
                    script->m_vec470.push_back(v);
                }
            }
        }
        break;
    }
    }


case 0x1e9:  // "[dungeon drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon drop]", 0x1e9));
    }
    else
    {
        script->m_vecd4.clear();  // vector<pair<int,int>> @+0xd4
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vecd4.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1f4:  // "[dungeon object drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon object drop]", 0x1f4));
    }
    else
    {
        script->m_vece0.clear();  // vector<pair<int,int>> @+0xe0
        while (true) {
        int v1 = ScanInt(&ok);
        if (!ok) break;
        script->m_vece0.push_back(std::pair<int, int>(v1, ScanInt((bool*)0)));
        }
        break;
    }

case 0x1ff:  // "[special passive object item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[special passive object item]", 0x1ff));
    }
    else
    {
    {
        // ORIG LAB_088e81e4：清 m_vecec，循环读（itemIdx 消费 / assignItemIdx / pairCount + pair 列表）
        script->m_vecec.clear();
        while (true) {
            int v0 = ScanInt(&ok);       // itemIdx（消费）
            if (!ok) break;
            int assignItemIdx = ScanInt(&ok);
            if (!ok) return false;
            int pairCount = ScanInt(&ok);
            if (!ok) return false;
            stDungeonAssignItem_t item;
            item.m_assignItemIdx = assignItemIdx;
            for (int i = 0; i < pairCount; ++i) {
                int a = ScanInt(&ok);
                if (!ok) return false;
                int b = ScanInt(&ok);
                if (!ok) return false;
                item.m_list.push_back(std::pair<int, int>(a, b));
            }
            script->m_vecec.push_back(item);
        }
        break;
    }
    }

case 0x21d:  // "[pathgate object]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[pathgate object]", 0x21d));
    }
    else
    {
    {
        // ORIG LAB_088e8418：4 组 ScanInt 写入 +0xf8 起 POD 区共 10 个 int
        // （3+3+2+2：偏移 (0x3c+i)*4+8=+0xf8、+0x104、+0x110、+0x11c；与 export 对称）。
        for (int i = 0; i < 3; ++i) script->m_pathgate.m_cells[0][i] = ScanInt((bool*)0);
        for (int i = 0; i < 3; ++i) script->m_pathgate.m_cells[1][i] = ScanInt((bool*)0);
        for (int i = 0; i < 2; ++i) script->m_pathgate.m_cells[2][i] = ScanInt((bool*)0);
        for (int i = 0; i < 2; ++i) script->m_pathgate.m_cells[3][i] = ScanInt((bool*)0);
        break;
    }
    }

case 0x228:  // "[maze info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[maze info]", 0x228));
    }
    else
    {
        // ORIG LAB_088e8587：首轮置 -0x121 标志；后续轮先取 m_mazeList170.size()
        // （结果仅落槽不使用，ORIG 088e85fb 实证）再 push_back + mazeLocal.clear()。
        if (mazeInited) {
            script->m_mazeList170.size();
            script->m_mazeList170.push_back(mazeLocal);
            mazeLocal.clear();
        } else {
            mazeInited = true;
        }
        break;
    }

case 0x27d:  // "[quest connection]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[quest connection]", 0x27d));
    }
    else
    {
    {
        // ORIG LAB_088e8643：questType ∈ {-1,0,1}；type==0 时再读 1 int（quest index）。
        // 越界 → logFormat("error! maze quest type(%d)") 后 import 失败。
        int questType = ScanInt((bool*)0);
        ScanInt((bool*)0);   // 第 2 值（未用）
        // ORIG 0x088e86cd/088e86d8：先 `cmp $0x1,%eax; jg <err>` 再
        // `cmp $0xffffffff,%eax; jge <ok>` ⇒ 源码条件序为上界在前。
        if (questType <= 1 && questType >= -1)
        {
            if (questType == 0)
            {
                ScanInt((bool*)0);
            }
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                  __FUNCTION__, 0x282,
                                  "error! maze quest type(%d)", questType);
            return false;
        }
        break;
    }
    }

case 0x28a:  // "[clear condition]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[clear condition]", 0x28a));
    }
    else
    {
    {
        // ORIG LAB_088e8742：ScanTypeOrStr 读取条件标签；未知标签、左/右变量
        // 缺失均记录 RDARScriptDungeon.cpp 日志并使整个导入失败。
        DungeonClearCondition cond;
        bool isType = false;
        while (true) {
            std::string sub;
            if (!ScanTypeOrStr(isType, sub, true)) break;
            if (sub == "[/clear condition]") break;
            int type = -1;
            if (sub == "[destroy object]") type = 0;
            else if (sub == "[clear map]") type = 1;
            else if (sub == "[hunt monster]") type = 2;
            else if (sub == "[hunt apc]") type = 3;
            else if (sub == "[hunt boss]") type = 4;
            else
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2a1,
                                      "error! dungeon clear condition(%s)",
                                      sub.c_str());
                return false;
            }
            cond.m_0 = type;
            cond.m_4 = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2a8,
                                      "error! dungeon clear condition left variable");
                return false;
            }
            cond.m_8 = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptDungeon.cpp",
                                      __FUNCTION__, 0x2ae,
                                      "error! dungeon clear condition right variable");
                return false;
            }
            if (type == 1) cond.m_8 = 1;   // ORIG：clear map → 右变量恒 1
            mazeLocal.m_clearCondition.push_back(cond);
        }
        break;
    }
    }

case 0x2be:  // "[party num check]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[party num check]", 0x2be));
    }
    else
    {
        ScanInt((bool*)0);  // 局部用途 @88e8aac
        ScanInt((bool*)0);  // 局部用途 @88e8abe
        break;
    }

case 0x2c4:  // "[escape hell]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[escape hell]", 0x2c4));
    }
    else
    {
        ScanInt((bool*)0);  // 局部用途 @88e8b32
        break;
    }

case 0x2c8:  // "[size]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[size]", 0x2c8));
    }
    else
    {
        // [party num check]：maze 尺寸 → grids.resize
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
        mazeLocal.m_width = v1;
        mazeLocal.m_height = v2;
        mazeLocal.m_grids.resize(mazeLocal.m_width * mazeLocal.m_height, GridScript());
        break;
    }

case 0x2d0:  // "[greed]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[greed]", 0x2d0));
    }
    else
    {
    {
        // ORIG LAB_088e8c37：每轮 substr(pos,2) 临时串 + 多次 .c_str() 重读比较
        // 分隔符/符号表（O0 无 CSE，P48）；未命中符号 pos+=1 并 bad++；命中直接
        // 写 m_grids[gridIdx]（ORIG 无越界检查）。bad 非零时导入失败。
        // ORIG ScanStr 复用函数级路径串槽（-0x1098，无独立 ctor/dtor）→ 用 token。
        ScanStr(&token);
        int len = (int)token.size();
        int pos = 0;
        int gridIdx = 0;
        int bad = 0;
        while (pos < len - 1)
        {
            std::string two = token.substr(pos, 2);
            if (two.c_str()[0] == '\r' || two.c_str()[0] == '\n' ||
                two.c_str()[0] == ' ')
            {
                pos += 1;
            }
            else
            {
                int cc = 0;
                for (; cc < 0x10; ++cc)
                {
                    if (two.c_str()[0] == g_gridSymbols[cc][0] &&
                        two.c_str()[1] == g_gridSymbols[cc][1])
                        break;
                }
                if (cc == 0x10)
                {
                    pos += 1;
                    bad += 1;
                }
                else
                {
                    mazeLocal.m_grids[gridIdx].m_gridValue = cc;
                    gridIdx += 1;
                    pos += 2;
                }
            }
        }
        if (bad != 0)
            return false;
        break;
    }
    }

case 0x30e:  // "[map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[map specification]", 0x30e));
    }
    else
    {
        // [map specification]：填充 maze grid
        v1 = ScanInt((bool*)0);  // x
        v2 = ScanInt((bool*)0);  // y
        {
            GridScript* g = mazeLocal.getGrid(v1, v2);
            g->m_mapCandidates.clear();
            while (true) {
                v1 = ScanInt(&ok);
                if (!ok) break;
                g->m_mapCandidates.push_back(v1);
            }
        }
        break;
    }

case 0x31e:  // "[boss map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss map specification]", 0x31e));
    }
    else
    {
        // [boss map specification]：同 [map specification]
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
        {
            GridScript* g = mazeLocal.getGrid(v1, v2);
            g->m_secondCandidates.clear();
            while (true) {
                v1 = ScanInt(&ok);
                if (!ok) break;
                g->m_secondCandidates.push_back(v1);
            }
        }
        break;
    }

case 0x32e:  // "[layered map specification]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[layered map specification]", 0x32e));
    }
    else
    {
        // [layered map specification]：同 [map specification]
        v1 = ScanInt((bool*)0);
        v2 = ScanInt((bool*)0);
        {
            GridScript* g = mazeLocal.getGrid(v1, v2);
            g->m_layeredMapIndexes.clear();
            while (true) {
                v1 = ScanInt(&ok);
                if (!ok) break;
                g->m_layeredMapIndexes.push_back(v1);
            }
        }
        break;
    }

case 0x33e:  // "[start map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[start map]", 0x33e));
    }
    else
    {
        // [start map]：读 (x,y) 坐标对入 mazeLocal.m_vec14（ORIG LAB_088e919e）
        mazeLocal.m_vec14.clear();
        while (true) {
            int mx = ScanInt(&ok);
            if (!ok) break;
            mazeLocal.m_vec14.push_back(std::pair<int, int>(mx, ScanInt((bool*)0)));
        }
        break;
    }

case 0x34a:  // "[boss map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss map]", 0x34a));
    }
    else
    {
        // [boss map]：读 (x,y) 坐标对入 mazeLocal.m_vec20（ORIG LAB_088e9288）
        mazeLocal.m_vec20.clear();
        while (true) {
            int mx = ScanInt(&ok);
            if (!ok) break;
            mazeLocal.m_vec20.push_back(std::pair<int, int>(mx, ScanInt((bool*)0)));
        }
        break;
    }

case 0x355:  // "[hell dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hell dungeon]", 0x355));
    }
    else
    {
        script->m_140 = ScanInt((bool*)0) != 0;  // +0x140
        break;
    }

case 0x35b:  // "[prohibit practice]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[prohibit practice]", 0x35b));
    }
    else
    {
        script->m_141 = ScanInt((bool*)0) != 0;  // +0x141
        break;
    }

case 0x362:  // "[disable skill]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[disable skill]", 0x362));
    }
    else
    {
        // ORIG LAB_088e947c：ScanInt → m_142 = (值 == 0)（disable skill 段标记）
        script->m_142 = ScanInt((bool*)0) == 0;  // +0x142
        break;
    }

case 0x367:  // "[impossible skills]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[impossible skills]", 0x367));
    }
    else
    {
    {
        // ORIG LAB_088e94fb：循环 ScanStr job 名 → getCharacterJobToEnumValue + ScanInt → m_vec144
        while (true) {
            std::string token;
            if (!ScanStr(&token)) break;
            if (token == "[/impossible skills]") break;
            int job = getCharacterJobToEnumValue(token);
            int val = ScanInt((bool*)0);
            script->m_vec144.push_back(std::make_pair(job, val));
        }
        break;
    }
    }

case 0x373:  // "[/impossible skills]"（ORIG 88e9648：__inited==1 时直接落到 default，
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[/impossible skills]", 0x373));
    }
    else
    {
        // 无字段操作——结束 token 由 [impossible skills] 的 while 内部自行消费）
        break;
    }

case 0x377:  // "[use countdown]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[use countdown]", 0x377));
    }
    else
    {
        // ORIG LAB_088e969b：仅当值等于 1 时置位 m_150。
        script->m_150 = ScanInt((bool*)0) == 1;
        break;
    }

case 0x37c:  // "[use zoom rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[use zoom rate]", 0x37c));
    }
    else
    {
        script->m_154 = ScanFloat((bool*)0);   // ORIG 0x088e8f2f（0x37d5）(bool*)0
        break;
    }

case 0x381:  // "[room move keep state]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[room move keep state]", 0x381));
    }
    else
    {
        script->m_158 = ScanInt((bool*)0) == 1;  // cmp $1,sete
        break;
    }

case 0x386:  // "[usable item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[usable item]", 0x386));
    }
    else
    {
        script->m_159 = ScanInt((bool*)0) == 1;  // cmp $1,sete
        break;
    }

case 0x38a:  // "[seal door map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door map index]", 0x38a));
    }
    else
    {
        ScanInt((bool*)0);  // 丢弃 @88e98f9
        break;
    }

case 0x38d:  // "[seal door pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door pos]", 0x38d));
    }
    else
    {
        // [usable item]：两值
        ScanInt((bool*)0);
        ScanInt((bool*)0);
        break;
    }

case 0x392:  // "[hell coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hell coin limit]", 0x392));
    }
    else
    {
        script->m_168 = ScanInt((bool*)0);  // +0x168
        break;
    }

case 0x397:  // "[ai character appear rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[ai character appear rate]", 0x397));
    }
    else
    {
        script->m_338 = ScanInt((bool*)0);  // +0x338
        break;
    }

case 0x39b:  // "[dead tower map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dead tower map index]", 0x39b));
    }
    else
    {
        // [hell coin limit]：push_back 到 m_deadTower180.m_vec
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_deadTower180.m_vec.push_back(v1);
        }
        break;
    }

case 0x3a5:  // "[randomized object creation]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[randomized object creation]", 0x3a5));
    }
    else
    {
        std::string rsub;
        RidableObject ridable;
        int t0 = 0;
        while (true)
        {
            // ORIG 088e9bb5：ScanType 失败 → import 返回 false（非静默块结束）。
            if (!ScanType(rsub, true))
                return false;
            if (rsub == "[/randomized object creation]")
                break;
        retry_0_head:
            static DungeonTagMap __lookup_0;
            static bool __inited_0 = false;
            static SpinLock __init_lock_0;
        retry_0:
            if (__inited_0)
            {
                DungeonTagMap::const_iterator it0 = __lookup_0.find(rsub.c_str());
                t0 = (it0 != __lookup_0.end()) ? it0->second : -1;
            }
            else
            {
                __init_lock_0.enter();
                if (__inited_0)
                {
                    __init_lock_0.leave();
                    goto retry_0_head;
                }
                t0 = 0;   // ORIG：首过 subId=0 进子表 0 填充链
            }
            switch (t0)
            {
            case 0:
            case 0x3b3:  // "[select]" → mazeLocal.m_ridable.m_0
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[select]", 0x3b3));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    mazeLocal.m_ridable.m_0 = iv0;
                    break;
                }
            case 0x3b8:  // "[regenerate]" → mazeLocal.m_ridable.m_4
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[regenerate]", 0x3b8));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    mazeLocal.m_ridable.m_4 = (iv0 != 0);
                    break;
                }
            case 0x3bd:  // "[minimap icon]" → script+0x500
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[minimap icon]", 0x3bd));
                }
                else
                {
                    if (!ScanInt(&iv0)) return false;
                    script->m_500 = iv0;
                    break;
                }
            case 0x3c2:  // "[object]" → 内层 __lookup_1 表分派收集 RidableObject
                if (!__inited_0)
                {
                    __lookup_0.insert(DungeonTagMap::value_type("[object]", 0x3c2));
                }
                else
                {
                    std::string ss;
                    int t1 = 0;
                    ridable.clear();   // ORIG 088ea072：每轮 [object] 前 clear
                    while (true)
                    {
                        // ORIG 088ea083：ScanType 失败 → import false。
                        if (!ScanType(ss, true))
                            return false;
                        if (ss == "[/object]")
                        {
                            mazeLocal.m_ridable.m_objects.push_back(ridable);
                            break;
                        }
                    retry_1_head:
                        static DungeonTagMap __lookup_1;
                        static bool __inited_1 = false;
                        static SpinLock __init_lock_1;
                    retry_1:
                        if (__inited_1)
                        {
                            DungeonTagMap::const_iterator it1 = __lookup_1.find(ss.c_str());
                            t1 = (it1 != __lookup_1.end()) ? it1->second : -1;
                        }
                        else
                        {
                            __init_lock_1.enter();
                            if (__inited_1)
                            {
                                __init_lock_1.leave();
                                goto retry_1_head;
                            }
                            t1 = 0;
                        }
                        switch (t1)
                        {
                        case 0:
                        case 0x3d5:  // "[map]" → m_x/m_y
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[map]", 0x3d5));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_x)) return false;
                                if (!ScanInt(&ridable.m_y)) return false;
                                break;
                            }
                        case 0x3de:  // "[index]" → m_field0c
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[index]", 0x3de));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_field0c)) return false;
                                break;
                            }
                        case 0x3e3:  // "[team]" → m_field18；未识别串静默越过
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[team]", 0x3e3));
                            }
                            else
                            {
                                std::string team;
                                if (!ScanType(team, true)) return false;
                                if (team == "[neutral]") ridable.m_field18 = 200;
                                else if (team == "[monster]") ridable.m_field18 = 100;
                                else if (team == "[character]") ridable.m_field18 = 0;
                                break;
                            }
                        case 0x3ee:  // "[pos]" → m_field10/m_field14
                            if (!__inited_1)
                            {
                                __lookup_1.insert(DungeonTagMap::value_type("[pos]", 0x3ee));
                            }
                            else
                            {
                                if (!ScanInt(&ridable.m_field10)) return false;
                                if (!ScanInt(&ridable.m_field14)) return false;
                                break;
                            }
                        default:
                            // ORIG LAB_088ea62c（0x088ea62c）：`movzbl __inited_1;
                            // test %al,%al; je <尾>` + 仅已初始化时 return false ⇒
                            // 建表轮（__inited_1==false）未知子 tag 静默跳过，
                            // 非首轮才判失败。我方原无条件 return false = 真语义 bug。
                            if (__inited_1)
                                return false;
                            break;
                        }
                        if (!__inited_1)
                        {
                            __init_lock_1.leave();
                            __inited_1 = true;
                            goto retry_1_head;   // ORIG LAB_088ea665
                        }
                    }
                    break;
                }
            default:
                // ORIG LAB_088ea6e0（0x088ea6e0）：同 __inited_1 形——`movzbl
                // __inited_0; test %al; je <尾>` ⇒ 建表轮静默跳过，非首轮才失败。
                if (__inited_0)
                    return false;
                break;
            }
            if (!__inited_0)
            {
                __init_lock_0.leave();
                __inited_0 = true;
                goto retry_0_head;   // ORIG LAB_088ea711
            }
        }
        // [b53FIX · case 0x3a5 缺 break ⇒ 穿透进 case 0x421 吞掉 `[size]` 标签]
        // ORIG 088e9be8（`rsub == "[/randomized object creation]"` 成立）→
        //   `jne 0x88ea723` = `nop; mov $0x1,%esi; jmp 0x88ea746`（~string 后
        //   `test %esi,%esi; je 0x88f0a27; jmp 0x88f0988`）
        // ⇒ 收尾即 switch 公共 break 棱形（esi 为「块正常结束」标志，false 棱形走
        // return-false）。本方旧形漏写 `break;`，4.4 把 case 末尾排成
        // case 0x421 `[point by type]` 的 `!__inited` 守卫，else 棱形穿透进其
        // else 体（首语句 `ScanInt((bool*)0)`）。
        // 实机 A/B 实证（Frida 17.18，/data/ab_gn_ours.js 对 Act7/gentnorthgate.dgn）：
        //   tok#100 TYPE `[/randomized object creation]` → 本 case 收尾
        //   tok#101 TYPE **`[size]`** 被穿透后的 `ScanInt` 吞掉（无 getType）
        //   tok#102/103（7 / 3）亦被误读 ⇒ `[size]` 处理器（含
        //   `m_grids.resize(m_width*m_height)`，ORIG 0x88e8c02）从未执行
        //   ⇒ mazeLocal.m_grids 空 ⇒ 其后 `[greed]`（ORIG 0x88e8e21 同构无越界检查）
        //   在 `m_grids[0].m_gridValue = cc`（+0x2e9e）上 SIGSEGV。
        // ORIG 同数据：tok#101 由主循环 ScanType 正常分派 → resize(7*3=21) → ret=1。
        break;
    }

case 0x421:  // "[point by type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[point by type]", 0x421));
    }
    else
    {
        // ORIG LAB_088ea761（Ghidra：local_12c+(i+0x60)*4+0xc）：i=0..2 循环，
        // ScanInt((bool*)0) 结果写 script+0xc+(0x60+i)*4 ⇒ stride 4 命中
        // m_warRoom18c 起始 3 个 int（0x18c/0x190/0x194）。ORIG 0x088ea7e2
        // `lea 0x60(%esi),%ecx` + `mov %eax,0xc(%edx,%ecx,4)` 为直接实证；
        // 旧写法 `0x18c + i*0x180` 系误读 Ghidra 表达式（真语义 bug，已修）。
        // §4.3 例外：ORIG 遗留线形算术（跨成员指针加法），无对应具名数组成员。
        for (int i = 0; i <= 2; ++i)
            *(int*)((char*)script + 0xc + (0x60 + i) * 4) = ScanInt((bool*)0);
        break;
    }

case 0x426:  // "[phase fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[phase fatigue]", 0x426));
    }
    else
    {
        script->m_warRoom18c.m_c = ScanInt((bool*)0);  // +0x198
        break;
    }

case 0x42c:  // "[warroom map index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[warroom map index]", 0x42c));
    }
    else
    {
        script->m_warRoom18c.m_10 = ScanInt((bool*)0);  // +0x19c
        break;
    }

case 0x430:  // "[monster exp bonus per user decrease]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster exp bonus per user decrease]", 0x430));
    }
    else
    {
        script->m_warRoom18c.m_bc = ScanFloat((bool*)0);   // ORIG 0x088ea90b（0x49b1）
        break;
    }

case 0x433:  // "[result exp bonus per user decrease]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[result exp bonus per user decrease]", 0x433));
    }
    else
    {
        script->m_warRoom18c.m_c0 = ScanFloat((bool*)0);   // ORIG 0x088ea985（0x4a2b）
        break;
    }

case 0x436:  // "[monster type spawn prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn prob]", 0x436));
    }
    else
    {
    {
        // ORIG LAB_088ea9eb：逐条读 4 int（怪物类型 + 3 概率）push 入 m_warRoom18c.m_spawnProb
        while (true) {
            STMonsterTypeSpawnProb sp;
            sp.m_probability[0] = ScanInt(&ok);
            if (!ok) break;
            sp.m_probability[1] = ScanInt(&ok);
            if (!ok) return false;
            sp.m_probability[2] = ScanInt(&ok);
            if (!ok) return false;
            sp.m_probability[3] = ScanInt(&ok);
            if (!ok) return false;
            script->m_warRoom18c.m_spawnProb.push_back(sp);
        }
        break;
    }
    }

case 0x456:  // "[monster type spawn cost]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn cost]", 0x456));
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            script->m_warRoom18c.m_c4[i] = ScanInt((bool*)0);
        break;
    }

case 0x45b:  // "[monster type spawn interval rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster type spawn interval rate]", 0x45b));
    }
    else
    {
        for (int i = 0; i < 4; ++i)
            script->m_warRoom18c.m_dc[i] = ScanFloat((bool*)0);   // ORIG 0x088eabf3（0x4c99）
        break;
    }

case 0x45f:  // "[monster spawn base interval]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster spawn base interval]", 0x45f));
    }
    else
    {
        script->m_warRoom18c.m_ec = ScanInt((bool*)0);  // +0x278
        break;
    }

case 0x462:  // "[monster spawn random interval]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster spawn random interval]", 0x462));
    }
    else
    {
        script->m_warRoom18c.m_f0 = ScanInt((bool*)0);  // +0x27c
        break;
    }

case 0x465:  // "[fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[fatigue]", 0x465));
    }
    else
    {
        script->m_warRoom18c.m_f4 = ScanInt((bool*)0);  // +0x280
        break;
    }

case 0x468:  // "[fatigue result]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[fatigue result]", 0x468));
    }
    else
    {
        script->m_warRoom18c.m_f8 = ScanInt((bool*)0);  // +0x284
        break;
    }

case 0x46b:  // "[max monster]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[max monster]", 0x46b));
    }
    else
    {
        script->m_warRoom18c.m_d8 = ScanInt((bool*)0);  // +0x264
        break;
    }

case 0x46e:  // "[player kc]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[player kc]", 0x46e));
    }
    else
    {
        script->m_warRoom18c.m_d4 = ScanInt((bool*)0);  // +0x260
        break;
    }

case 0x472:  // "[spawn common monster index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn common monster index]", 0x472));
    }
    else
    {
        // ORIG LAB_088eafab：clear(+0x1a0) 后循环 push（写 STWarRoomData+0x14，
        // 不是 item-drop vector +0x6c）。
        script->m_warRoom18c.m_vec14[0].clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            // ORIG 0x088eafbe：写 STWarRoomData+0x14（不是 item-drop
            // vector +0x6c）。
            script->m_warRoom18c.m_vec14[0].push_back(v1);
        }
        break;
    }

case 0x47f:  // "[spawn common champion index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn common champion index]", 0x47f));
    }
    else
    {
        script->m_warRoom18c.m_vec14[1].clear();   // ORIG clear(+0x1ac)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[1].push_back(v1);
        }
        break;
    }

case 0x48c:  // "[spawn super champion index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn super champion index]", 0x48c));
    }
    else
    {
        script->m_warRoom18c.m_vec14[2].clear();   // ORIG clear(+0x1b8)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[2].push_back(v1);
        }
        break;
    }

case 0x499:  // "[spawn boss index]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn boss index]", 0x499));
    }
    else
    {
        script->m_warRoom18c.m_vec14[3].clear();   // ORIG clear(+0x1c4)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec14[3].push_back(v1);
        }
        break;
    }

case 0x4a6:  // "[spawn step max]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn step max]", 0x4a6));
    }
    else
    {
        script->m_warRoom18c.m_50 = ScanInt((bool*)0);  // +0x1dc
        break;
    }

case 0x4ab:  // "[spawn step resource pool]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[spawn step resource pool]", 0x4ab));
    }
    else
    {
        // ORIG LAB_088eb2a8（Ghidra C 实证）：clear(+0x1d0) 后循环 ScanInt push；
        // +0x1d0 = STWarRoomData+0x44 = m_vec44。
        script->m_warRoom18c.m_vec44.clear();
        while (true) {
            int poolVal = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec44.push_back(poolVal);
        }
        break;
    }

case 0x4b9:  // "[kill count const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[kill count const]", 0x4b9));
    }
    else
    {
        script->m_warRoom18c.m_ac = ScanFloat((bool*)0);   // ORIG 0x088eb374（0x541a）
        break;
    }

case 0x4be:  // "[join cost gold]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[join cost gold]", 0x4be));
    }
    else
    {
        script->m_warRoom18c.m_54 = ScanInt((bool*)0);  // +0x1e0
        break;
    }

case 0x4c3:  // "[gold drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[gold drop prob]", 0x4c3));
    }
    else
    {
        script->m_warRoom18c.m_58 = ScanInt((bool*)0);  // +0x1e4
        break;
    }

case 0x4c8:  // "[common monster item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster item drop prob]", 0x4c8));
    }
    else
    {
        script->m_warRoom18c.m_5c[0] = ScanInt((bool*)0);  // +0x1e8
        break;
    }

case 0x4cd:  // "[common champion item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion item drop prob]", 0x4cd));
    }
    else
    {
        script->m_warRoom18c.m_5c[1] = ScanInt((bool*)0);  // +0x1ec
        break;
    }

case 0x4d2:  // "[super champion item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion item drop prob]", 0x4d2));
    }
    else
    {
        script->m_warRoom18c.m_5c[2] = ScanInt((bool*)0);  // +0x1f0
        break;
    }

case 0x4d7:  // "[boss item drop prob]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss item drop prob]", 0x4d7));
    }
    else
    {
        script->m_warRoom18c.m_5c[3] = ScanInt((bool*)0);  // +0x1f4
        break;
    }

case 0x4dc:  // "[common monster exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster exp const]", 0x4dc));
    }
    else
    {
        script->m_warRoom18c.m_9c[0] = ScanInt((bool*)0);  // +0x228
        break;
    }

case 0x4e4:  // "[common champion exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion exp const]", 0x4e4));
    }
    else
    {
        script->m_warRoom18c.m_9c[1] = ScanInt((bool*)0);  // +0x22c
        break;
    }

case 0x4ec:  // "[super champion exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion exp const]", 0x4ec));
    }
    else
    {
        script->m_warRoom18c.m_9c[2] = ScanInt((bool*)0);  // +0x230
        break;
    }

case 0x4f4:  // "[boss exp const]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss exp const]", 0x4f4));
    }
    else
    {
        script->m_warRoom18c.m_9c[3] = ScanInt((bool*)0);  // +0x234
        break;
    }

case 0x4fc:  // "[common monster item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common monster item drop list]", 0x4fc));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[0].clear();   // ORIG clear(+0x1f8)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[0].push_back(v1);
        }
        break;
    }

case 0x508:  // "[common champion item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[common champion item drop list]", 0x508));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[1].clear();   // ORIG clear(+0x204)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[1].push_back(v1);
        }
        break;
    }

case 0x514:  // "[super champion item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[super champion item drop list]", 0x514));
    }
    else
    {
        // ORIG 088eba6b：clear 的是 +0x204（前一 vector），push 目标 +0x210 —— 忠实保留。
        script->m_warRoom18c.m_vec6c[1].clear();
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[2].push_back(v1);
        }
        break;
    }

case 0x520:  // "[boss item drop list]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss item drop list]", 0x520));
    }
    else
    {
        script->m_warRoom18c.m_vec6c[3].clear();   // ORIG clear(+0x21c)
        while (true) {
            v1 = ScanInt(&ok);
            if (!ok) break;
            script->m_warRoom18c.m_vec6c[3].push_back(v1);
        }
        break;
    }

case 0x52c:  // "[battle spawn time]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[battle spawn time]", 0x52c));
    }
    else
    {
        script->m_warRoom18c.m_fc = ScanInt((bool*)0);  // +0x288
        break;
    }

case 0x534:  // "[ancient dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[ancient dungeon]", 0x534));
    }
    else
    {
        script->m_28c = ScanInt((bool*)0) != 0;  // +0x28c
        break;
    }

case 0x539:  // "[powerwar dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[powerwar dungeon]", 0x539));
    }
    else
    {
        script->m_28d = ScanInt((bool*)0) != 0;  // +0x28d
        break;
    }

case 0x53d:  // "[blood dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[blood dungeon]", 0x53d));
    }
    else
    {
        script->m_28e = (char)ScanInt((bool*)0);  // mov %dl,0x28e
        break;
    }

case 0x549:  // "[limit item stat]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit item stat]", 0x549));
    }
    else
    {
        script->m_28f = ScanInt((bool*)0) != 0;  // +0x28f
        break;
    }

case 0x54f:  // "[absolute change stat]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[absolute change stat]", 0x54f));
    }
    else
    {
        // ORIG LAB_088ebe52：每轮 statKind=5 → ScanStr 名 → __lookup_2 表惰性链
        // （[move speed]/[physical attack add]/[physical defense add]/
        //   [magical attack add]/[magical defense add] = 0x559..0x565 → 0..4，
        //   子表条目在 088ec0bb..088ec2c5 的 per-case !__inited 分支中逐例 insert）；
        // 未知名保持 5。收尾 ScanFloat(float*)（088bc58e）忽略返回值，statKind!=5
        // 时以本地左值 key/val 构造 value_type 写 m_statMap290（重复 key 不覆盖）。
        int statKind = 5;
        int t2 = 0;
        while (true)
        {
            statKind = 5;   // ORIG 088ebe52：每轮初值 5（不写 map）
            // ORIG 088ebe62：复用函数级串槽（-0x1098），无独立 string ctor/dtor。
            if (!ScanStr(&token))
                break;
        retry_2_head:
            static DungeonTagMap __lookup_2;
            static bool __inited_2 = false;
            static SpinLock __init_lock_2;
        retry_2:
            if (__inited_2)
            {
                DungeonTagMap::const_iterator it2 = __lookup_2.find(token.c_str());
                t2 = (it2 != __lookup_2.end()) ? it2->second : -1;
            }
            else
            {
                __init_lock_2.enter();
                if (__inited_2)
                {
                    __init_lock_2.leave();
                    goto retry_2_head;
                }
                t2 = 0;   // ORIG：首过 subId=0 并入 [move speed] case
            }
            switch (t2)
            {
            case 0:
            case 0x559:  // "[move speed]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[move speed]", 0x559));
                }
                else
                {
                    statKind = 0;
                    break;
                }
            case 0x55c:  // "[physical attack add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[physical attack add]", 0x55c));
                }
                else
                {
                    statKind = 1;
                    break;
                }
            case 0x55f:  // "[physical defense add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[physical defense add]", 0x55f));
                }
                else
                {
                    statKind = 2;
                    break;
                }
            case 0x562:  // "[magical attack add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[magical attack add]", 0x562));
                }
                else
                {
                    statKind = 3;
                    break;
                }
            case 0x565:  // "[magical defense add]"
                if (!__inited_2)
                {
                    __lookup_2.insert(DungeonTagMap::value_type("[magical defense add]", 0x565));
                }
                else
                {
                    statKind = 4;
                    break;
                }
            default:
                break;
            }
            if (!__inited_2)
            {
                __init_lock_2.leave();
                __inited_2 = true;
                goto retry_2_head;   // ORIG LAB_088ec2e9
            }
            float v = 0.0f;
            ScanFloat(&v);   // ORIG 088ec301：调用后忽略返回值
            if (statKind == 5)
                continue;    // ORIG 088ec36e：未知名只消费浮点值
            AbsoluteChangeStat::T key = static_cast<AbsoluteChangeStat::T>(statKind);
            script->m_statMap290.insert(
                std::map<AbsoluteChangeStat::T, float>::value_type(key, v));
        }
        // [b53FIX · case 0x54f 缺 break ⇒ 穿透进 case 0x576 的同类缺陷]
        // ORIG 088ebe6f/088ebe72：同一 while(ScanStr) 棱形为
        //   `je 0x88ebe77`（成功续体）+ `nop; jmp 0x88f0988`（switch 公共 break 落点），
        // ⇒ ORIG 本 case 结束于 break。本方旧形漏写，4.4 把 else 失败棱形直排成
        // `jne 0x860aeb7`（= case 0x576 `[limit escape character]` 的守卫），
        // 由 `[absolute change stat]` 列表收尾的 ScanStr 失败处穿透进下一 case。
        // 该缺陷被上一条（case 0x912）掩盖在更早的失败点之后，一并按 ORIG 修正。
        break;
    }


case 0x576:  // "[limit escape character]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit escape character]", 0x576));
    }
    else
    {
        script->m_2a8 = ScanInt((bool*)0) != 0;  // +0x2a8
        break;
    }

case 0x57d:  // "[defense dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[defense dungeon]", 0x57d));
    }
    else
    {
        script->m_2ab = ScanInt((bool*)0) != 0;  // +0x2ab
        break;
    }

case 0x580:  // "[advance altar type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar type]", 0x580));
    }
    else
    {
        // ORIG 088ec4d7：独立 string 槽（-0x1144），失败按 ebx=1 出口返回 true。
    {
        std::string typeName;
        if (!ScanStr(&typeName))
            return true;  // ORIG 0x088ec4ea 失败时直接以 true 结束，不写字段
        // ORIG 0x088ec52d-0x088ec533：`mov -0x128(%ebp),%eax; add $0x2ac,%eax;
        // mov %eax,0x4(%esp)` ⇒ 第二实参直传 &script->m_2ac，无局部枚举中转
        // （P167 调用点形；局部版多 lea/mov/mov 三条 + 深槽）。与
        // ImportAdvanceAltarDungeonTag 的 0x088f2140 同形。
        if (!advancealtar::StageType::getStageType(
                typeName, reinterpret_cast<advancealtar::StageType::T&>(script->m_2ac)))
            return true;
    }
        break;
    }

case 0x58e:  // "[advance altar map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar map]", 0x58e));
    }
    else
    {
    {
        // ORIG LAB_088ec56a：循环 ScanStr 难度名 → getDifficulty（easy/medium/hard），
        // 命中则读 2 int → m_2b0[diff]/m_2bc[diff]；任意失败/不可识别 token 结束。
        while (true)
        {
            std::string dtok;
            if (!ScanStr(&dtok)) break;
            advancealtar::StageDifficulty::T sd;
            if (!advancealtar::StageDifficulty::getDifficulty(dtok, sd)) break;
            int a = ScanInt(&ok);
            script->m_2b0[(int)sd] = a;
            if (!ok) break;
            int b = ScanInt(&ok);
            script->m_2bc[(int)sd] = b;
            if (!ok) break;
        }
        break;
    }
    }

case 0x5a2:  // "[advance altar clear reward]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar clear reward]", 0x5a2));
    }
    else
    {
    {
        // ORIG LAB_088ec6dc（同 ImportAdvanceAltarDungeonTag）：ScanType 子 tag 选
        // [easy]/[medium]/[hard]→vec 槽 0..2，然后逐项 importAltarRewardItem push；
        // "[/advance altar clear reward]" 显式比对后结束（ORIG 088ec803 eq 实证），
        // 其余子 tag 同样结束本 case。
        while (true)
        {
            std::string dsub;
            if (!ScanType(dsub, true)) break;
            int idx = -1;
            if (dsub == "[easy]") idx = 0;
            else if (dsub == "[medium]") idx = 1;
            else if (dsub == "[hard]") idx = 2;
            else if (dsub == "[/advance altar clear reward]") break;
            else break;
            // ORIG 0x088ec838：取址落具名指针槽 -0x84 后 `cmpl $0x0` + jne 判空，
            // 空则 `mov $0x0,%esi; jmp 088ec984`（跳出内层循环）⇒ 指针形 + 判空。
            std::vector<advancealtar::_RewardItem>* vec =
                &script->m_clearReward2d4.m_vec[idx];
            if (vec == 0)
                break;
            while (true)
            {
                advancealtar::_RewardItem item;
                std::string rewardType;
                if (!ScanStr(&rewardType)) break;
                advancealtar::RewardItemType::T rewardItemType;
                // ORIG 0x088ec8a5：getRewardItemType 返回值落具名 bool -0x1161，
                // 同一变量复用于后续两次 ScanInt 的 bool* 实参（088ec8c3/088ec8ec
                // 均 `lea -0x1161`）⇒ 非全局 ok。
                bool parsed = advancealtar::RewardItemType::getRewardItemType(
                    rewardType, rewardItemType);
                if (!parsed)
                    break;
                item.m_type = static_cast<int>(rewardItemType);
                item.m_value1 = ScanInt(&parsed);
                if (!parsed) break;
                item.m_value2 = ScanInt(&parsed);
                if (!parsed) break;
                vec->push_back(item);
            }
        }
        break;
    }
    }

case 0x5d9:  // "[advance altar survival map]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar survival map]", 0x5d9));
    }
    else
    {
    {
        // ORIG LAB_088ec99f：循环 ScanInt → push m_vec2c8 直到失败
        while (true)
        {
            int mapId = ScanInt(&ok);
            if (!ok) break;
            script->m_vec2c8.push_back(mapId);
        }
        break;
    }
    }

case 0x5e5:  // "[advance altar survival clear reward]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[advance altar survival clear reward]", 0x5e5));
    }
    else
    {
    {
        // ORIG（Ghidra C，同 ImportAdvanceAltarDungeonTag 语义）：多个 [type]…[/type] 块 →
        // SurvivalRewardData::m_vec；[round] 两项、[list] 逐项 importAltarRewardItem（type 0/1）。
        while (true)
        {
            std::string token;
            if (!ScanType(token, true)) break;
            if (token != "[type]") break;
            advancealtar::RoundRewardData rrd;
            rrd.m_0 = 0;
            rrd.m_4 = 0;
            while (true)
            {
                std::string sub;
                if (!ScanType(sub, true)) break;
                if (sub == "[/type]")
                {
                    script->m_survival2f8.m_vec.push_back(rrd);
                    break;
                }
                if (sub == "[round]")
                {
                    rrd.m_0 = ScanInt(&ok);
                    if (!ok) break;
                    rrd.m_4 = ScanInt(&ok);
                    if (!ok) break;
                }
                else if (sub == "[list]")
                {
                    while (true)
                    {
                        advancealtar::_RewardItem item;
                        std::string rewardType;
                        if (!ScanStr(&rewardType)) break;
                        advancealtar::RewardItemType::T rewardItemType;
                        // ORIG 0x088ecc82：返回值落具名 bool -0x1195 并复用（同上）。
                        bool itemOk = advancealtar::RewardItemType::getRewardItemType(
                            rewardType, rewardItemType);
                        if (!itemOk)
                            break;
                        item.m_type = static_cast<int>(rewardItemType);
                        item.m_value1 = ScanInt(&itemOk);
                        if (!itemOk) break;
                        item.m_value2 = ScanInt(&itemOk);
                        if (!itemOk) break;
                        // ORIG 088ecd14/088ecd36：type==0 与 ==1 分别为独立 push_back
                        // 调用点（两条实例化调用），非合并条件。
                        if (item.m_type == 0)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                        else if (item.m_type == 1)
                        {
                            rrd.m_vec8.push_back(item);
                        }
                    }
                }
            }
        }
        break;
    }
    }

case 0x622:  // "[dimension dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dimension dungeon]", 0x622));
    }
    else
    {
        // ORIG LAB_088ece04：2×ScanInt → m_304/m_305（byte）
        script->m_304 = (char)ScanInt((bool*)0);
        script->m_305 = (char)ScanInt((bool*)0);
        break;
    }
case 0x62c:  // "[necessary party]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[necessary party]", 0x62c));
    }
    else
    {
        // ORIG LAB_088ece9a：ScanInt → m_306（byte）
        script->m_306 = (char)ScanInt((bool*)0);
        break;
    }
case 0x630:  // "[road necessary party]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[road necessary party]", 0x630));
    }
    else
    {
    {
        // ORIG LAB_088ecf16：读 count，循环读 DimensionPartyCount(3 char)，去重后 push 入 m_dimension308
        int count = ScanInt(&ok);
        for (int i = 0; i < count; ++i) {
            DimensionPartyCount dpc;
            dpc.m_0 = (char)ScanInt(&ok);
            dpc.m_1 = (char)ScanInt(&ok);
            dpc.m_2 = (char)ScanInt(&ok);
            std::vector<DimensionPartyCount>::iterator it =
                std::find(script->m_dimension308.begin(), script->m_dimension308.end(), dpc);
            // ORIG 088ed08e：operator== 后取反判失败（__gnu_cxx::operator== 而非 !=）。
            if (!(it == script->m_dimension308.end()))
            {
                // 重复值不是幂等去重，而是错误并
                // 立即失败（日志参数为 dungeon/type/difficulty/count）。
                // ORIG 0x088ed0e9 的 rodata 实证：该格式串**无**结尾 \n
                // （ORIG 二进制 0xdc60c4 后紧跟 0x00 0x00）⇒ 删除多余换行。
                printf("road necessary party invalid value (dungeon:%d,type:%d,difficulty:%d,count:%d)",
                       script->m_18, (int)dpc.m_0, (int)dpc.m_1, (int)dpc.m_2);
                return false;
            }
            script->m_dimension308.push_back(dpc);
        }
        // ORIG 0x088ed12a：列表结束后无条件消费一个 type token。
        ScanType(token, true);
        break;
    }
    }

case 0x655:  // "[village attack revenge dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[village attack revenge dungeon]", 0x655));
    }
    else
    {
        // ORIG LAB_088ed12f：4×ScanInt → m_316/m_317/m_314/m_315（按 C 顺序 byte）
        script->m_316 = (char)ScanInt((bool*)0);
        script->m_317 = (char)ScanInt((bool*)0);
        script->m_314 = (char)ScanInt((bool*)0);
        script->m_315 = (char)ScanInt((bool*)0);
        break;
    }
case 0x65d:  // "[schedule]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[schedule]", 0x65d));
    }
    else
    {
    {
        // ORIG LAB_088ed1f9：5 个时间串默认构造在循环外（088ed259-088ed297），
        // FindIndexByFullScan 循环外一次（088ed2a6→local_78）；循环体固定槽
        // ds(-0x12b0)：memset 0x24(088ed2ce)→m_type=0 显式写(088ed2d3)→
        // m_idx(088ed2dd)→ScanInt id(088ed2ef)，ok!=1 则 break 出块回主循环
        // (088ed303→088ed681 esi=1)→m_eventId=id→ScanStr×5：首串失败同为
        // break 语义(088ed325→088ed684)，后四串失败 ebx=0 返回 false
        // (088ed340)。Convert 各段：拷贝临时串一次+值传参（088ed3ce），
        // ConvertStringToTime 直接把 long& 写进 +0x0c/+0x10 字段
        // (lea ds+0xc, 088ed3d3)、weekday 经循环内 unsigned int 局部回填
        // +0x14(088ed530)、两组 HHMMSS 直接取 +0x18/+0x19/+0x1a 与
        // +0x1b/+0x1c/+0x1d 字段地址(088ed554-088ed577)，无 h/m/sec 局部。
        std::string strStart, strEnd, strWeekday, strStartTime, strEndTime;
        const int scheduleFileIdx = reinterpret_cast<STScriptFileList*>(
            &g_stDungeonScriptFileList)->FindIndexByFullScan(path);
        while (true) {
            _DailySchedule ds;
            memset(&ds, 0, sizeof(ds));              // 0x24，ORIG 088ed2ce
            ds.m_type = 0;                           // ORIG 088ed2d3 显式零写
            ds.m_idx = scheduleFileIdx;              // ORIG 088ed2dd
            int id = ScanInt(&ok);
            if (!ok) break;
            ds.m_eventId = id;                       // ORIG 088ed30c
            if (!ScanStr(&strStart)) break;          // ORIG 088ed325 首串失败=结束
            if (!ScanStr(&strEnd)) return false;
            if (!ScanStr(&strWeekday)) return false;
            if (!ScanStr(&strStartTime)) return false;
            if (!ScanStr(&strEndTime)) return false;
            if (!ConvertStringToTime(std::string(strStart),
                    *(long*)&ds.m_startSec))
                return false;                        // +0x0c
            if (!ConvertStringToTime(std::string(strEnd),
                    *(long*)&ds.m_endSec))
                return false;                        // +0x10
            unsigned int wday = 0;                   // ORIG -0x11b4 循环内初始化
            if (!ConvertWeekdayStringToInt(std::string(strWeekday), wday))
                return false;
            ds.m_wday = static_cast<int>(wday);      // +0x14
            if (!ConvertHHMMSSStringToInt(std::string(strStartTime),
                    ds.m_startHour, ds.m_startMin, ds.m_startSec2))
                return false;                        // +0x18..0x1a 字段直写
            if (!ConvertHHMMSSStringToInt(std::string(strEndTime),
                    ds.m_endHour, ds.m_endMin, ds.m_endSec2))
                return false;                        // +0x1b..0x1d 字段直写
            script->m_dailySchedule.push_back(ds);
        }
        break;
    }
    }

case 0x697:  // "[limit party count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit party count]", 0x697));
    }
    else
    {
        // ORIG LAB_088ed7a2：写入 +0x324，默认值为 4。
        script->m_324 = (unsigned char)ScanInt((bool*)0);
        break;
    }

case 0x69a:  // "[limit inout count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[limit inout count]", 0x69a));
    }
    else
    {
        script->m_325 = (char)ScanInt((bool*)0);   // +0x325（report LAB_088ed81e 实证）
        break;
    }

case 0x69d:  // "[hit count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[hit count]", 0x69d));
    }
    else
    {
        // ORIG LAB_088ed89a：连续五个整数写入 +0x128 的数组。
        for (int i = 0; i < 5; ++i)
        {
            script->m_128[i] = ScanInt(&ok);
            if (!ok)
                break;
        }
        break;
    }

case 0x6a5:  // "[seal door appear rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[seal door appear rate]", 0x6a5));
    }
    else
    {
        script->m_13c = ScanInt((bool*)0);  // +0x13c
        break;
    }

case 0x6a8:  // "[boss room entrance condition]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[boss room entrance condition]", 0x6a8));
    }
    else
    {
    {
        // ORIG LAB_088ed9c0（Ghidra C 实证）：token∈{[hunt monster]→m_0=1 且每组 3 int /
        // [seeking]→m_0=2 且每组 2 int}，先 reserve(count*n)，再逐组循环 ScanInt push 到
        // m_vec4(+0x32c)；任一 ScanStr/ScanInt 失败 → import 返回 false。
        if (!ScanStr(&token))
        {
            return false;
        }
        int condCount = ScanInt(&ok);
        if (!ok)
        {
            return false;
        }
        if (token == "[hunt monster]")
        {
            script->m_bossRoom328.m_0 = 1;
            script->m_bossRoom328.m_vec4.reserve(condCount * 3);
            for (int i = 0; i < condCount; ++i)
            {
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
            }
        }
        else if (token == "[seeking]")
        {
            script->m_bossRoom328.m_0 = 2;
            script->m_bossRoom328.m_vec4.reserve(condCount * 2);
            for (int i = 0; i < condCount; ++i)
            {
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
                script->m_bossRoom328.m_vec4.push_back(ScanInt(&ok));
                if (!ok) return false;
            }
        }
        else
        {
            return false;
        }
        break;
    }
    }

case 0x6ca:  // "[dummy appear count]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dummy appear count]", 0x6ca));
    }
    else
    {
        script->m_16c = ScanInt((bool*)0);  // +0x16c
        break;
    }

case 0x6cd:  // "[dungeon type]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[dungeon type]", 0x6cd));
    }
    else
    {
    {
        // ORIG LAB_088edd44（Ghidra C 实证）：ScanStr 读 dungeon 类型名 → m_358(+0x358)；
        // "tower of death"/"tower of boss" 额外把 maze 置 2x2 并
        // resize grids(m_width*m_height)（ORIG 0x88ede26/0x88eded4 为 mov+mov+imul
        // 两成员相乘，非常量 4）；
        // "advance altar" 额外置 m_2ab=1；未知名 → m_358=0。ScanStr 失败 → import false。
        if (!ScanStr(&token))
        {
            return false;
        }
        if (token == "tower of death")
        {
            script->m_358 = 1;
            mazeLocal.m_width = 2;
            mazeLocal.m_height = 2;
            mazeLocal.m_grids.resize(mazeLocal.m_width * mazeLocal.m_height, GridScript());
        }
        else if (token == "tower of boss")
        {
            script->m_358 = 3;
            mazeLocal.m_width = 2;
            mazeLocal.m_height = 2;
            mazeLocal.m_grids.resize(mazeLocal.m_width * mazeLocal.m_height, GridScript());
        }
        else if (token == "advance altar")
        {
            script->m_358 = 4;
            script->m_2ab = 1;
        }
        else if (token == "power dungeon")
        {
            script->m_358 = 2;
        }
        else
        {
            script->m_358 = 0;
        }
        break;
    }
    }

case 0x6f5:  // "[tower random map indexes]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower random map indexes]", 0x6f5));
    }
    else
    {
        script->m_33c = ScanInt(&ok);  // +0x33c
        break;
    }

case 0x6f8:  // "[tower stage]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower stage]", 0x6f8));
    }
    else
    {
    {
        // ORIG LAB_088ee05d：vector 构造后显式 clear()（088ee0de）；每项先
        // memset(8B 临时 RandomList) 再落两个 ScanInt 值槽；weight==0（含读失败）
        // 直接导入失败。完成后 printf 并以 make_pair(具名左值) insert（重复 key 不覆盖）。
        int maxStage = ScanInt(&ok);
        if (!ok)
            return false;
        std::vector<RandomList> vec;
        vec.clear();
        while (true) {
            RandomList rl;
            memset(&rl, 0, sizeof(rl));
            const int mapIdx = ScanInt(&ok);
            if (!ok)
                break;
            // ORIG 0x088ee15f 把第二个 ScanInt 存入具名槽 -0x58（非直接落 rl 成员），
            // 088ee177 才 `mov -0x5c; mov %eax,-0x11d0` / `mov -0x58; mov %eax,-0x11cc`
            // 逐成员拷进 RandomList（P0：有槽往返 ⇒ 原码有具名局部）。
            const int weight = ScanInt(&ok);
            if (weight == 0)
                return false;
            rl.m_mapIndex = mapIdx;
            rl.m_weight = weight;
            vec.push_back(rl);
        }
        printf("\n[max stage] : %d, [stage] : %d\n",
               script->m_33c, maxStage);
        for (unsigned int i = 0; i < vec.size(); ++i)
            printf("[map] : %d, [rate] : %d\n",
                   vec[i].m_mapIndex, vec[i].m_weight);
        script->m_map340.insert(std::make_pair(maxStage, vec));
        break;
    }
    }

case 0x71e:  // "[death tower map indexes]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[death tower map indexes]", 0x71e));
    }
    else
    {
    {
        // ORIG LAB_088ee328：m_368=ScanInt；随后循环 (index,index) 对 push m_vec35c
        // （第二值读取失败 → import false）。
        script->m_368 = ScanInt(&ok);
        while (true)
        {
            int a = ScanInt(&ok);
            if (!ok) break;
            int b = ScanInt(&ok);
            if (!ok) return false;
            // ORIG 088ee413 调用 make_pair<int&,int&>（非 pair 直构）
            script->m_vec35c.push_back(std::make_pair(a, b));
        }
        break;
    }
    }

case 0x72c:  // "[named monster map pos]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[named monster map pos]", 0x72c));
    }
    else
    {
    {
        // ORIG LAB_088ee43e：循环 (x,y) → STNamedMonsterMapPos push m_vec3c8，直到 x 读取失败。
        while (true)
        {
            int x = ScanInt(&ok);
            if (!ok) break;
            int y = ScanInt(&ok);
            STNamedMonsterMapPos nm;
            nm.m_x = x;
            nm.m_y = y;
            script->m_vec3c8.push_back(nm);
        }
        break;
    }
    }

case 0x73f:  // "[blood max round]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[blood max round]", 0x73f));
    }
    else
    {
        script->m_3c4 = ScanInt(&ok);  // +0x3c4
        break;
    }

case 0x744:  // "[event monster2]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event monster2]", 0x744));
    }
    else
    {
    {
        // ORIG LAB_088ee594 __inited==1：读 m_3ec/m_3f0 + 循环 5 int 的 stSecondEventMonster_t 入 m_vec3f8。
        // 循环内首个 ScanInt（088ee665，jne→088ee775 跳出）失败=结束该段；
        // 其后 4 个（088ee68b/6b7/6e3/70f，`mov $0x0,%ebx`）失败=整体 import 失败。
        script->m_3ec = ScanInt(&ok);
        if (!ok) return false;
        script->m_3f0 = ScanInt(&ok);
        if (!ok) return false;
        while (true) {
            stSecondEventMonster_t em;
            em.m_eventIdx = ScanInt(&ok); if (!ok) break;
            em.m_monsterIdx = ScanInt(&ok); if (!ok) return false;
            em.m_prob = ScanInt(&ok); if (!ok) return false;
            em.m_field0c = ScanInt(&ok); if (!ok) return false;
            em.m_field10 = ScanInt(&ok); if (!ok) return false;
            script->m_3f4 += em.m_prob;   // ORIG 累加 local_11f0(prob) 到 +0x3f4
            script->m_vec3f8.push_back(em);
        }
        // ORIG：非零 m_3ec 必须伴随至少一个事件条目。
        if (script->m_3ec != 0 && script->m_vec3f8.empty())
            return false;
        break;
    }
    }

case 0x766:  // "[event monster]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[event monster]", 0x766));
    }
    else
    {
    {
        // ORIG LAB_088ee7bb（Ghidra C）：m_3d4/m_3d8=ScanInt；随后循环 stEventMonster_t
        // （eventIdx/monsterIdx/prob/field0c/field10）push m_vec3e0 且 m_3dc 累加 prob；
        // 任一组内 ScanInt 失败 → import false。收尾校验：非零 m_3d4 必须有至少
        // 一个事件条目，否则 import false。
        script->m_3d4 = ScanInt(&ok);
        if (!ok) return false;
        script->m_3d8 = ScanInt(&ok);
        if (!ok) return false;
        while (true)
        {
            stEventMonster_t em;
            em.m_eventIdx = ScanInt(&ok);
            if (!ok) break;
            em.m_monsterIdx = ScanInt(&ok);
            if (!ok) return false;
            em.m_prob = ScanInt(&ok);
            if (!ok) return false;
            em.m_field0c = ScanInt(&ok);
            if (!ok) return false;
            em.m_field10 = ScanInt(&ok);
            if (!ok) return false;
            script->m_3dc += em.m_prob;
            script->m_vec3e0.push_back(em);
        }
        if (script->m_3d4 != 0 && script->m_vec3e0.empty())
            return false;
        break;
    }
    }

case 0x7a9:  // "[tower high skill initial cool time]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower high skill initial cool time]", 0x7a9));
    }
    else
    {
        // ORIG LAB_088ee9e2/088eea93：2×ScanFloat 后直接 fnstcw/fldcw(RC=truncate)
        // +fistpl → m_36c/m_370（int）；ORIG 无任何 flds/faddp（无 +0.5 语义）。
        script->m_36c = (int)ScanFloat(&ok);
        script->m_370 = (int)ScanFloat(&ok);
        break;
    }
case 0x7ad:  // "[tower high skill initial cool time rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower high skill initial cool time rate]", 0x7ad));
    }
    else
    {
        // ORIG LAB_088eead7：2×ScanFloat → +0x374/+0x378（float 位）
        script->m_374 = ScanFloat(&ok);
        script->m_378 = ScanFloat(&ok);
        break;
    }
case 0x7b1:  // "[tower fp cubepiece]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower fp cubepiece]", 0x7b1));
    }
    else
    {
        // ORIG LAB_088eeb6d（0x88eebd3）：ScanFloat → fldcw(RC=truncate)+fistpl
        // → +0x37c（int）；ORIG 无 faddp。
        script->m_37c = (int)ScanFloat(&ok);
        break;
    }
case 0x7b4:  // "[tower limit of stackable item]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower limit of stackable item]", 0x7b4));
    }
    else
    {
        // ORIG LAB_088eec17（0x88eec7d）：ScanFloat → fldcw(RC=truncate)+fistpl
        // → +0x380（int）；ORIG 无 faddp。
        script->m_380 = (int)ScanFloat(&ok);
        break;
    }
case 0x7b7:  // "[tower recovery]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower recovery]", 0x7b7));
    }
    else
    {
        // [tower limit of stackable item]：RecoveryStairInfo（=现有 [tower recovery] 逻辑）
        {
            RecoveryStairInfo rsi;
            // ORIG 0x88eed38/0x88eed7a：ScanFloat 后直接 fldcw(RC=truncate)+fistpl，
            // 即 C 截断语义；ORIG 无 flds/faddp（旧注释"+0.5 四舍五入"系误判，已删）。
            rsi.m_0 = static_cast<int>(ScanFloat(&ok));
            rsi.m_4 = static_cast<int>(ScanFloat(&ok));
            rsi.m_8 = ScanFloat(&ok);
            while (true) {
                RecoveryInfo ri;
                ri.m_data[0] = ScanFloat(&ok);
                if (!ok) break;
                ri.m_data[1] = ScanFloat(&ok);
                ri.m_data[2] = ScanFloat(&ok);
                rsi.m_vec.push_back(ri);
            }
            script->m_vec384.push_back(rsi);
        }
        break;
    }

case 0x7ce:  // "[tower dialog]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower dialog]", 0x7ce));
    }
    else
    {
    {
        // ORIG LAB_088eee7d（Ghidra C）：STTowerDialog{index,name,flag8/9/A}。
        // ScanInt index → ScanStr name → ScanStr 事件串；"on start"/"on complete"/"on die"
        // 置对应 flag 并 push 到 m_towerDialog390 三个 vector（+0x390/+0x39c/+0x3a8）；
        // 未匹配串 logFormat（省略日志）；任一 Scan 失败仅终止本条。
        STTowerDialog dlg;
        dlg.m_index = ScanInt(&ok);
        if (!ok) return false;
        // ORIG 0x088eef2f/0x088eef35：ScanStr 返回值先存 ok 槽（-0x109d）再
        // `movzbl ok` 回读判定 ⇒ 原码是两句式 `ok = ScanStr(...); if(!ok)…`
        // （P0 槽往返律），非内联 `if(!ScanStr(...))`（后者 4.4 寄存器直用少 2 条）。
        ok = ScanStr(&dlg.m_name);
        if (!ok) return false;
        std::string eventStr;
        ok = ScanStr(&eventStr);   // ORIG 0x088eef6e/0x088eef74 同形
        if (!ok) return false;
        if (eventStr == "on start")
        {
            dlg.m_flag8 = 1;
            script->m_towerDialog390.m_vec0.push_back(dlg);
        }
        else if (eventStr == "on complete")
        {
            dlg.m_flag9 = 1;
            script->m_towerDialog390.m_vecc.push_back(dlg);
        }
        else if (eventStr == "on die")
        {
            dlg.m_flagA = 1;
            script->m_towerDialog390.m_vec18.push_back(dlg);
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x7ee,
                "Error on scanning tower dialog. There is no match string %s",
                eventStr.c_str());
        }
        break;
    }
    }

case 0x819:  // "[tower item drop]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower item drop]", 0x819));
    }
    else
    {
        script->m_3b4 = ScanInt(&ok);  // +0x3b4
        if (!ok)
            return false;
        break;
    }

case 0x81e:  // "[tower max clear item num]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower max clear item num]", 0x81e));
    }
    else
    {
        script->m_3b8 = ScanInt(&ok);  // +0x3b8
        if (!ok)
            return false;
        break;
    }

case 0x824:  // "[evil]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil]", 0x824));
    }
    else
    {
        // ORIG LAB_088ef22e：循环 5×ScanInt → m_410[i]（偏移 (i+0x104)*4=+0x410）
        for (int ei = 0; ei < 5; ++ei)
        {
            script->m_410[ei] = ScanInt((bool*)0);
        }
        break;
    }
case 0x829:  // "[evil high level]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil high level]", 0x829));
    }
    else
    {
        // ORIG LAB_088ef2c6：循环 5×ScanInt → m_424[i]（偏移 (i+0x108)*4+4=+0x424）
        for (int ei = 0; ei < 5; ++ei)
        {
            script->m_424[ei] = ScanInt((bool*)0);
        }
        break;
    }
case 0x834:  // "[evil rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[evil rate]", 0x834));
    }
    else
    {
    {
        // ORIG LAB_088ef35f：四个整数百分比写 m_438/m_440/m_43c/m_444。
        script->m_438 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_440 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_43c = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        script->m_444 = static_cast<float>(ScanInt((bool*)0)) / 100.0f;
        break;
    }
    }

case 0x83c:  // "[monster difficulty bonus]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[monster difficulty bonus]", 0x83c));
    }
    else
    {
    {
        // ORIG LAB_088ef474：13 行 × 5 float → m_vec404；每个元素即使
        // ScanFloat 失败也先 push 该值，随后记录日志并终止本段，不提交不完整行。
        for (int i = 0; i < 0xd; ++i) {
            std::vector<float> row;
            row.clear();   // ORIG 088ef4f7：ctor 后显式 clear()
            row.reserve(5);
            bool rowOk = true;
            for (int j = 0; j < 5; ++j) {
                float f = ScanFloat(&ok);
                row.push_back(f);
                if (!ok) {
                    rowOk = false;
                    LogManager::logFormat(
                        0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x848,
                        "[%s] : Error - DungeonScript MonsterDifficultyBouns Number",
                        __FUNCTION__);
                    break;
                }
            }
            if (!rowOk)
                break;
            script->m_vec404.push_back(row);
        }
        if (!ok)
        {
            LogManager::logFormat(
                0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x84e,
                "[%s] : Error - DungeonScript MonsterDifficultyBouns Number",
                __FUNCTION__);
        }
        else
        {
            std::string endTag;
            ScanType(endTag, true);
            if (endTag != "[/monster difficulty bonus]")
            {
                LogManager::logFormat(
                    0, "../RDARScriptDungeon.cpp", __FUNCTION__, 0x858,
                    "[%s] : Error - DungeonScript [/monster difficulty bonus]",
                    __FUNCTION__);
            }
        }
        break;
    }
    }

case 0x85d:  // "[revision table]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[revision table]", 0x85d));
    }
    else
    {
        ScanStr(&script->m_str448);  // +0x448
        break;
    }

case 0x862:  // "[gold card use]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[gold card use]", 0x862));
    }
    else
    {
        // ORIG 0x088ef7a7：无 case 级 ok 重置（全函数仅 088e6052 一处 `movb $0x0,-0x109d`）；
        // ScanInt 结果落具名槽 -0x34 后 `cmpl $0x0` + jne 两臂，
        // 分别发 `movb $0x0/$0x1,0x44c`（if/else 形）；setne 表达式形不匹配。
        int goldCardUse = ScanInt(&ok);
        if (goldCardUse == 0)
            script->m_44c = 0;
        else
            script->m_44c = 1;
        if (!ok)
            return false;
        break;
    }

case 0x86e:  // "[SWAY EFFECT]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[SWAY EFFECT]", 0x86e));
    }
    else
    {
        // ORIG LAB_088ef7e5（Ghidra C 实证）：先置 m_47c=1，再读 0x484/0x480。
        script->m_47c = 1;
        script->m_484 = ScanInt((bool*)0);  // +0x484
        script->m_480 = ScanInt((bool*)0);  // +0x480
        break;
    }

case 0x874:  // "[no fatigue]"（ORIG LAB_088ef884/Ghidra C：置 +0x48c 标志）
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[no fatigue]", 0x874));
    }
    else
    {
        script->m_48c = 1;
        break;
    }

case 0x87a:  // "[tower of despair]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tower of despair]", 0x87a));
    }
    else
    {
        script->m_2a9 = 1;   // ORIG LAB_088ef8f3：local_12c[0x2a9]=1
        break;
    }

case 0x880:  // "[quest npc dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[quest npc dungeon]", 0x880));
    }
    else
    {
        // ORIG LAB_088ef962：ScanInt 非零 → m_2aa(+0x2aa)；失败 → import false
        {
            int value = ScanInt(&ok);
            if (!ok) return false;
            script->m_2aa = value != 0;
        }
        break;
    }
case 0x889:  // "[designate dungeon difficulty]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[designate dungeon difficulty]", 0x889));
    }
    else
    {
        script->m_490 = ScanInt((bool*)0);  // +0x490
        break;
    }

case 0x88e:  // "[tournament dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament dungeon]", 0x88e));
    }
    else
    {
        // ORIG LAB_088efa7a：ScanInt → m_494(+0x494 byte)；失败 → import false
        script->m_494 = (char)ScanInt(&ok);
        if (!ok) return false;
        break;
    }
case 0x893:  // "[tournament round fatigue]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament round fatigue]", 0x893));
    }
    else
    {
        // ORIG LAB_088efb11：ScanInt → m_498(+0x498)；失败 → import false
        script->m_498 = ScanInt(&ok);
        if (!ok) return false;
        break;
    }
case 0x898:  // "[result card]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[result card]", 0x898));
    }
    else
    {
    {
        // ORIG LAB_088efba8：先 ScanType 一次；仅 [reward item rate] 触发
        // 奖励物品表，随后无论该首标签内容都直接读取奖励卡三元组。
        // 原始格式没有 [reward rate] 子标签。
        std::string sub;
        if (!ScanType(sub, true))
            return false;
        if (sub == "[reward item rate]")
        {
            script->m_itemRate4b4.Clear();
            for (;;)
            {
                TournamentRewardItem item;
                item.m_data[0] = ScanInt(&ok);
                if (!ok)
                    break;
                item.m_data[1] = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_itemRate4b4.m_c += item.m_data[1];
                item.m_data[2] = ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_itemRate4b4.m_items.push_back(item);
            }
        }

        for (;;)
        {
            TournamentRewardRate rate;
            int key = ScanInt(&ok);   // ORIG make_pair<int&,…>：非 const 左值
            if (!ok)
                break;                  // ORIG：卡片条目正常结束
            rate.m_0 = ScanInt(&ok);
            if (!ok)
                return false;
            rate.m_4 = ScanInt(&ok);
            if (!ok)
                return false;
            rate.m_8 = ScanInt(&ok);
            if (!ok)
                return false;
            if (rate.m_0 + rate.m_4 + rate.m_8 == 0)
                return false;
            std::pair<std::map<int, TournamentRewardRate>::iterator, bool> res =
                script->m_map49c.insert(std::make_pair(key, rate));
            if (!res.second)
            {
                puts("TournamentRewardRateMap Key is Duplicated!!!");
                return false;
            }
        }
        break;
    }
    }

case 0x8ed:  // "[tournament clear reward gold rate]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament clear reward gold rate]", 0x8ed));
    }
    else
    {
        // ORIG LAB_088eff24：ScanFloat → +0x4c4（float 语义，m_4c4 初值 1.0f）
        // 失败即整体 import 失败（088effa7 `je 088f0984` → `movzbl ok,%ebx`+收尾）。
        script->m_4c4Value = ScanFloat(&ok);
        if (!ok) return false;
        break;
    }
case 0x8f2:  // "[tournament clear reward exp]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[tournament clear reward exp]", 0x8f2));
    }
    else
    {
    {
        // ORIG LAB_088effb9：循环 (key uchar, val uint)；m_map4c8 insert，重复 key →
        // puts("TournamentClearRewardExpMap Key is Duplicated!!!") 后 import 返回 false。
        while (true)
        {
            int key = ScanInt(&ok);
            if (!ok) break;
            int val = ScanInt(&ok);
            if (!ok) return false;
            // ORIG make_pair<unsigned_char&,unsigned_int&>：实参为具名左值槽
            // （-0x1275 uchar / -0x127c uint），非 cast 临时。
            unsigned char kSlot = static_cast<unsigned char>(key);
            unsigned int vSlot = static_cast<unsigned int>(val);
            std::pair<std::map<unsigned char, unsigned int>::iterator, bool> res =
                script->m_map4c8.insert(std::make_pair(kSlot, vSlot));
            if (!res.second)
            {
                puts("TournamentClearRewardExpMap Key is Duplicated!!!");
                return false;
            }
        }
        break;
    }
    }

case 0x90c:  // "[risk dungeon]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[risk dungeon]", 0x90c));
    }
    else
    {
        script->m_4e0 = ScanInt((bool*)0) != 0;  // +0x4e0
        break;
    }


















































case 0x912:  // "[coin info]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[coin info]", 0x912));
    }
    else
    {
        // ORIG LAB_088f01dd：循环 ScanStr 难度子 tag → __lookup_3 表惰性链
        // （[easy]/[medium]/[hard]/[ultimate] = 0x91d/0x922/0x927/0x92c → 0..3，
        //   条目在 088f0432..088f05ce 的 per-case !__inited 分支逐例 insert）；
        // 未识别串（含 -1）同样走收尾：2×ScanInt(bool)（值写槽、不检查失败）→
        // UseCoinDungeonDiff{m_0=v1, m_4=(v2!=0)} → map+0x4e4 以 make_pair
        // 插入（重复 key 不覆盖）。
        int diff = -1;
        int t3 = 0;
        while (true)
        {
            // ORIG 088f01e3：复用函数级串槽，无独立 string ctor/dtor。
            if (!ScanStr(&token))
                break;   // ORIG ScanStr false → 结束本 case
            diff = -1;   // ORIG 088f01f8：每轮先置 -1
        retry_3_head:
            static DungeonTagMap __lookup_3;
            static bool __inited_3 = false;
            static SpinLock __init_lock_3;
        retry_3:
            if (__inited_3)
            {
                DungeonTagMap::const_iterator it3 = __lookup_3.find(token.c_str());
                t3 = (it3 != __lookup_3.end()) ? it3->second : -1;
            }
            else
            {
                __init_lock_3.enter();
                if (__inited_3)
                {
                    __init_lock_3.leave();
                    goto retry_3_head;
                }
                t3 = 0;   // ORIG：首过 subId=0 并入 [easy] case
            }
            switch (t3)
            {
            case 0:
            case 0x91d:  // "[easy]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[easy]", 0x91d));
                }
                else
                {
                    diff = 0;
                    break;
                }
            case 0x922:  // "[medium]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[medium]", 0x922));
                }
                else
                {
                    diff = 1;
                    break;
                }
            case 0x927:  // "[hard]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[hard]", 0x927));
                }
                else
                {
                    diff = 2;
                    break;
                }
            case 0x92c:  // "[ultimate]"
                if (!__inited_3)
                {
                    __lookup_3.insert(DungeonTagMap::value_type("[ultimate]", 0x92c));
                }
                else
                {
                    diff = 3;
                    break;
                }
            default:
                break;
            }
            if (!__inited_3)
            {
                __init_lock_3.leave();
                __inited_3 = true;
                goto retry_3_head;   // ORIG LAB_088f05f4
            }
            UseCoinDungeonDiff d;
            d.m_0 = ScanInt(&ok);
            d.m_4 = (ScanInt(&ok) != 0);
            // ORIG 088f0670：make_pair<int,UseCoinDungeonDiff>（两实参均 prvalue
            // 推导）+ pair<const int,U>(pair&&) 转换 + ~pair×2。
            script->m_map4e4.insert(std::make_pair(static_cast<int>(diff),
                                                   static_cast<UseCoinDungeonDiff>(d)));
        }
        // [b53FIX · case 0x912 缺 break ⇒ 穿透进 case 0x942 导致整份 dgn 导入失败]
        // ORIG 088f01f0/088f01f3：`while(ScanStr(&token))` 失败棱形为
        //   `je 0x88f01f8`（成功续体）+ `nop; jmp 0x88f0988`，
        // 其中 0x88f0988 是**switch 之后**的公共 break 落点（其后即
        // `movzbl __inited …` 的循环尾判据），即 ORIG 本 case 结束于 break。
        // 本方旧形漏写 `break;`，4.4 把「case 末尾」直接排成下一个 case 的守卫
        // （`jne 0x860f1cf` = case 0x942 的 `!__inited` 测试），于是 else 棱形
        // 穿透进 `case 0x942 [party member coin limit]` 的 else 体：
        //   `script->m_4fc = ScanInt(&ok); if (!ok) return false;`
        // 实机 A/B 实证（Frida 17.18 gadget，/data/ab_gt_ours.js）：
        //   tok#183(k5 `[/coin info]`) 由 ScanStr 消费（无 getType）
        //   → tok#184(k5 `[name]`) 由**穿透后的 ScanInt** 消费（无 getType）
        //   → `!ok` ⇒ ImportDungeonScript ret=0。
        // 该 dgn 是 `Impossible/GoblinKingdom.dgn`（CDungeonList::load_list 第 43 份，
        // 也是唯一被 `[coin info]` 收尾的文件）⇒ `Loading Dungeons Fail`。
        // ORIG 同数据同位置：tok#185/186 由本 case 后的 `[name]` 的 ScanStr 消费，
        // 一路读到 EOF ⇒ ret=1。
        break;
    }


case 0x942:  // "[party member coin limit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[party member coin limit]", 0x942));
    }
    else
    {
        script->m_4fc = ScanInt(&ok);  // +0x4fc
        if (!ok)
            return false;
        break;
    }

case 0x94d:  // "[individual map movement]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[individual map movement]", 0x94d));
    }
    else
    {
        // ORIG LAB_088f074e：ScanInt 与 1 比较 → m_504(+0x504 byte)
        script->m_504 = ScanInt(&ok) == 1;
        break;
    }
case 0x954:  // "[open door even enemy]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[open door even enemy]", 0x954));
    }
    else
    {
        // ORIG LAB_088f07d0：ScanInt 与 1 比较 → m_505(+0x505 byte)
        script->m_505 = ScanInt(&ok) == 1;
        break;
    }
case 0x95c:  // "[disable exit]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[disable exit]", 0x95c));
    }
    else
    {
        // ORIG LAB_088f0852：ScanInt → +0x508 dword（char m_508 起 4 字节按 int 写入）
        script->m_508 = ScanInt(&ok);
        break;
    }
case 0x962:  // "[character respawn posetion]"
    if (!__inited)
    {
        __lookup.insert(DungeonTagMap::value_type("[character respawn posetion]", 0x962));
    }
    else
    {
        script->m_50c = ScanInt(&ok) != 0;   // +0x50c（report LAB_088f08d4 实证，char）
        break;

    }

        default:
            break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry_dispatch;   // ORIG LAB_088f09a9
        }
    }
    // ORIG 0x88f09c8..0x88f09ff：主循环结束后无条件提交最后一份
    // MazeScript，并把当前脚本在 g_stDungeonScriptFileList 中的索引写入
    // STDungeonScript+0x18。
    // ORIG LAB_088f09bb：收尾 push_back 前先取一次 size()（结果不使用）。
    script->m_mazeList170.size();
    script->m_mazeList170.push_back(mazeLocal);
    script->m_18 = reinterpret_cast<STScriptFileList*>(
        &g_stDungeonScriptFileList)->FindIndexByFullScan(path);
    return true;
}
bool ImportEquipmentScript(STEquipmentScript* script, const char* path,
                           bool flag, int a, int idx)
{
    // [b28d-solo-STEquipmentScript 判词·N] §6.2 三件套（0x08991aeb T）：
    // ①gap：ORIG 9771 条 vs 我们 9838 条（帧 0x19fc vs 0x1a2c，巨件全帧槽号平移）；
    // ②p167：ORIG calls=1292 / 我们 1300，MISSING 仅 2 个 pair converting ctor 实例
    //   （`pair<char const(&)[10],int>` / `[11]`，模板实例化差非语义缺口），EXTRA
    //   含 EquipmentScriptSpecialMonsterDrop ×2（三 case 各自调用点 vs ORIG 单共享块）
    //   与 STL 析构/拷贝 ctor 计数差；③槽位集/帧差属 N82/N89。语义完成，byte 层封顶。
    // [BigEquip pass-1 修订 2026-09-09] ③中 N89 判错：两侧 call 面实测对齐
    //   （1292 vs 1291，差集=snprintf 名/模板实例名/ABI guard 等无害项；9 个
    //   _ZL Get* helper 两侧均外调）。真实残差=N82 帧槽平移（+0x24/+0x1c/+0x54
    //   非恒定；ORIG 浅区 23 个独立 return-temp 槽，我侧 13=4.4 lifetime 复用）。
    //   本 pass 语义/形差修正：删 26 处 ORIG 无的 okFlag 预清零；[emancipate]/
    //   [war room point bonus]/[war room monster exp bonus] 回写 ScanType/ScanStr
    //   结果到 boolScratch；[hide layer]/[hide growtype avatar]/[change status]
    //   拆独立出参 bool（ORIG -0x10d1/-0x10d9/-0x1145）；[spectrum] v4 补 &0xff
    //   （ORIG movzbl 实证）；[Uncompoundable]/[setback img]/[set item]/[int data]/
    //   [fixed select type]/[avatar type select]/[avatar func filter]/[special
    //   ability]/[chat emoticon index]/[required full set]/[character item check]/
    //   [/routing priority]/[epic routing] 实参统一 boolScratch。台账
    //   ndiff 13237→13134、gap -67→-42。
    //   ⚠ 实测三→一 MonsterDrop 共享块合并（-71 insn、gap +23→-23、calls 1=ORIG）
    //   反使 ndiff 13134→13593（difflib 对齐窗口恶化，§6.2 失真实证）⇒ 已回滚；
    //   若要结构保真优先请 Main 裁定改台账口径后重做。
    // [BigEquipScript pass-2 2026-09-09] 按 §6.2 三件套重做 MonsterDrop 合并
    //   （指令数收敛优先，N127 台账 ndiff 不作成败判据）：
    //   ①三 case [special monster drop]/[difficulty for extra drop]/[dungeon type
    //     for extra drop] 的 else 分支 `goto special_monster_drop;` 汇入 0xcd7
    //     else 内共享块（ORIG 0899a83f 共享块、0899a86e 唯一调用点实证）⇒
    //     calls 1300→1292=ORIG（p167 修正版；余差集仅 C1↔C2/D1↔D2 同址别名镜像
    //     对，P93 豁免）；our_insns 9865→9799、gap −44→+22。
    //   ②[emblem socket default] 0x08999a3e / [avatar type select] 0x08999c00 两处
    //     `5 < size()` 删冗余 (int) cast ⇒ seta（无符号，与 ORIG 同；原 setg）。
    //   ③[spectrum] 08992ba9 位组合 `&0xff`→(unsigned char) cast ⇒ 复现 ORIG 逐项
    //     movzbl（消 3 and、movzbl 差 6→2）。
    //   残余 +22 = N82 帧槽族（+0x24/+0x1c/+0x54 非恒定）+ 分散 movb(+6)/add(+10)
    //   形态差；difflib 对本件失真（N127），逐 case 段对齐不可用（字符串段界
    //   ORIG 225 vs 我方重定位段 520）⇒ 封 N。
    // [SweepSTEquipScript 2026-09-09 复核] gap 22 保持（9821/9799）；p167 差集仅
    //   C2/D2 ↔ C1/D1 同址别名镜像（P93）；三个惰性表符号 __lookup/__lookup_0/
    //   __lookup_1 + __init_lock×3 + __inited×3 与 ORIG 名字/数量逐一对应（P333
    //   状态机结构完整）；帧 0x1a3c vs ORIG 0x19fc（N82 槽复用）。13 个 missing
    //   段抽查（08991f60/0899925d/0899b26e/0899b3ed 等）均为 4.1 switch 决策树与
    //   叶子块布局差，非语义缺口 ⇒ 维持封 N。
    // [R17a00 2026-09-13 独立复核 · 形态保真修复 ×1]
    //   ① 修复（四闸门全过，保留）：ORIG 0899a307/0899a334/0899a361/0899a38e 在四次
    //      `ScanInt(&rateFlag)` 之后各有 `movzbl -0x1145(%ebp),%eax; mov %al,-0x1145(%ebp)`
    //      （4.1 未 fold 的 (bool) 自读写、语义 NOP）。补 `rateFlag = (bool)rateFlag;`
    //      ×4（[change status on change hp] 段）⇒ 新 .o 该处 4 对逐条同形（-0x10bb 槽）、
    //      位置与 ORIG 一致；A/B 四数 710/1440/2/6/464 逐值零劣化；符号集
    //      `nm --defined-only` 3129=3129 逐名零差，仅本函数 size 0xb234→0xb268(+0x34)。
    //   ② 回滚留证（未达同形）：case 0x877 原写 `if (ScanInt(&levelAniIdx) && ScanStr(&scanBuf))`
    //      ⇒ 4.4 产 bool 物化（mov $1/jmp/mov $0/test），ORIG 089958ed/08995906 为两条
    //      独立 `xor $1; test; jne <nop 岛>`（0899c97b/0899c97e）。拆双 goto 后物化消除
    //      且四数不变，但 4.4 恒产 `je <fall>; jmp <岛>`（两跳、两守卫共岛）≠ ORIG 单跳，
    //      §16.3 闸门③不达标 ⇒ 已回滚（保留原 `&&` 写法）。
    // 声明顺序 = ORIG prologue 构造/初始化执行顺序（帧槽位由声明序决定，
    // 注释内为 ORIG %ebp 槽位，报告 0x08991b00-0x08991d8b）：
    std::string tmp;                       // ORIG -0x106c（各 case 第一读取暂存）
    std::string token;                     // ORIG -0x1070（ScanType 目标）
    std::string unusedStr;                 // ORIG -0x1074（ORIG 死变量：仅 ctor/dtor）
    std::string scanBuf;                   // ORIG -0x1078（getDirectoryPath 输出与 motion 共用）
    STEquipmentAniScript normalLayer;      // ORIG -0x1258
    int intScratch = 0;                    // ORIG -0x107c（ScanInt(int*)/值暂存）
    bool boolScratch = false;              // ORIG -0x107d（ScanInt(bool*) 出参）
    int normalLayerId = 0;                 // ORIG -0xb0
    std::string normalPath("");            // ORIG -0x1084（string("") 带 allocator 形参）
    int curJob = 0xb;                      // ORIG -0xac（0xb = 全职业缺省）
    int cnt[4];                            // ORIG -0x1088..-0x1094（参数解析器计数输出）
    cnt[0] = 0;
    cnt[1] = 0;
    cnt[2] = 0;
    cnt[3] = 0;
    bool aniMode = false;                  // ORIG -0xa5（[expand ani]..[/expand ani] 区）
    STEquipmentAniScript jobLayer;         // ORIG -0x12d4
    int jobLayerId = 0;                    // ORIG -0xa4
    std::string jobPath("");               // ORIG -0x1098
    int levelAniIdx = -1;                  // ORIG -0x109c（level section 状态）
    STEquipmentAniScript levelLayer;       // ORIG -0x1350
    GrowthEquipmentInfo growth;            // ORIG -0x10b4
    int levelLayerId = 0;                  // ORIG -0xa0
    std::string levelPath("");             // ORIG -0x10b8
    bool hideFlag774;                      // ORIG -0x10d1（[hide layer] 循环出参，ORIG 无显式初始化）
    bool hideFlag780;                      // ORIG -0x10d9（[hide growtype avatar] 循环出参）
    bool rateFlag;                         // ORIG -0x1145（[change status on change hp] 循环出参）
    // [2026-09-10 顺序修正] stopFlag/curParam/setAbilityBackup 的声明点见下方
    // m_str664 赋值之后（ORIG idx 132..138）；upgMin/upgMax/upgType 初值不在
    // 声明点：ORIG prologue 无 `movl $-1/-1/5`，
    // 初值由 [upgrade] 段每次进入时重置（ORIG idx 7992..8000：`upgMin=-1;
    // upgMax=-1; ScanInt(&upgMin); ScanInt(&upgMax); upgType=5;`，本文件同序已存在）。
    int upgMin;                            // ORIG -0x1160
    int upgMax;                            // ORIG -0x1164
    int upgType;                           // ORIG -0x1168
    char printBuf[0x200];                  // ORIG -0x1550

    if (!loadRDARScriptFile(g_szEquipmentScriptBaseDirectory, path))
    {
        return false;
    }
    script->Clear();
    getDirectoryPath(scanBuf, "%s%s", g_szEquipmentScriptBaseDirectory, path);
    script->m_str664 = scanBuf;

    // [2026-09-10 顺序修正] ORIG 的声明点在此（idx 132..138，for 之前）；初始化
    // 指令序 = curParam → setAbilityBackup(构造) → stopFlag（见上注释）。
    EquipmentParameterInfo* curParam = &script->m_param1c0;
    std::vector<std::pair<int, int> > setAbilityBackup;
    bool stopFlag = false;

    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;                       // EOF → 收尾（ORIG jne 0x0899c9f1）
        }

        // ---------------- 第一段分派（主属性区，ORIG switch(local_98)） ----------
        // ORIG 0x08991daa-0x08994d8b：静态关键字表（std::map<char const*,int,
        // less_TCHAR>）+ SpinLock 双检锁；case 值为整数 switch（GCC 二分降形态），
        // 每个 case 体首查 __inited：首轮（!__inited）idA=0 命中 case 0/0x67d 别名，
        // 沿 case 布局逐例 __lookup.insert 惰性建表（74 项，ORIG 0x08992431 起），
        // 链尾填完置 __inited 并回循环头对本 token 重新 find + 真实分派。
        bool matched = true;                        // ORIG local_99（+0x2b8 置 1）
        static std::map<char const*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        int idA;
        for (;;)
        {
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                idA = 0;
            }
            else
            {
                char const* key = token.c_str();
                std::map<char const*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(key);
                idA = (it != __lookup.end()) ? it->second : -1;
            }
            switch (idA)
            {
        case 0:
        case 0x67d:                              // [name]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name]", 0x67d));
        }
        else
        {
            ScanStr(&script->m_strac);
            if (flag)
            {
                stopFlag = true;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x682:                              // [name2]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[name2]", 0x682));
        }
        else
        {
            ScanStr(&script->m_strb0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x685:                              // [explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain]", 0x685));
        }
        else
        {
            ScanStr(&script->m_strb4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x689:                              // [explain2]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[explain2]", 0x689));
        }
        else
        {
            ScanStr(&script->m_strb4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x68d:                              // [basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[basic explain]", 0x68d));
        }
        else
        {
            ScanStr(&script->m_str6a4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x690:                              // [detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[detail explain]", 0x690));
        }
        else
        {
            ScanStr(&script->m_str6a8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x693:                              // [parameter basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[parameter basic explain]", 0x693));
        }
        else
        {
            ScanStr(&curParam->m_str278);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x696:                              // [parameter detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[parameter detail explain]", 0x696));
        }
        else
        {
            ScanStr(&curParam->m_str27c);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x699:                              // [flavor text]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[flavor text]", 0x699));
        }
        else
        {
            ScanStr(&script->m_strb8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x69c:                              // [attack info]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attack info]", 0x69c));
        }
        else
        {
            ScanStr(&script->m_str6b8);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x69f:                              // [fullset explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset explain]", 0x69f));
        }
        else
        {
            ScanStr(&script->m_str6ac);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a2:                              // [fullset basic explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset basic explain]", 0x6a2));
        }
        else
        {
            ScanStr(&script->m_str6b0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a5:                              // [fullset detail explain]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[fullset detail explain]", 0x6a5));
        }
        else
        {
            ScanStr(&script->m_str6b4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6a8:                              // [set name]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[set name]", 0x6a8));
        }
        else
        {
            ScanStr(&script->m_str668);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ab:                              // [grade]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[grade]", 0x6ab));
        }
        else
        {
            script->m_field4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ae:                              // [spectrum]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[spectrum]", 0x6ae));
        }
        else
        {
            unsigned int v1 = (unsigned int)ScanInt((bool*)0);
            unsigned int v2 = (unsigned int)ScanInt((bool*)0);
            unsigned int v3 = (unsigned int)ScanInt((bool*)0);
            unsigned int v4 = (unsigned int)ScanInt((bool*)0);
            // ORIG 08999c... 逐项 movzbl %al,%eax（零扩展）⇒ 用 (unsigned char)
            // cast 而非 &0xff（后者生 and，助记符不符）。
            script->m_790 = (int)((unsigned char)v1 |
                                  ((unsigned char)v2 << 8) |
                                  ((unsigned char)v3 << 0x10) |
                                  ((unsigned char)v4 << 0x18));
            script->m_794 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6b6:                              // [clear avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[clear avatar]", 0x6b6));
        }
        else
        {
            if (ScanInt((bool*)0) == 1)
            {
                script->m_7ac = 1;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6bb:                              // [attach type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[attach type]", 0x6bb));
        }
        else
        {
            ScanStr(&tmp);
            if (tmp == "[free]")               script->m_field8 = 0;
            else if (tmp == "[trade]")         script->m_field8 = 1;
            else if (tmp == "[trade delete]")  script->m_field8 = 2;
            else if (tmp == "[sealing]")       script->m_field8 = 3;
            else if (tmp == "[sealing trade]") script->m_field8 = 4;
            else if (tmp == "[account]")       script->m_field8 = 5;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6cc:                              // [creation rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creation rate]", 0x6cc));
        }
        else
        {
            script->m_fieldc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6cf:                              // [rarity]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[rarity]", 0x6cf));
        }
        else
        {
            script->m_field10 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6d2:                              // [weight]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[weight]", 0x6d2));
        }
        else
        {
            script->m_field34 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6d5:                              // [icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon]", 0x6d5));
        }
        else
        {
            ScanStr(&script->m_str6c);
            script->m_field70 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6da:                              // [aura hud icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[aura hud icon]", 0x6da));
        }
        else
        {
            script->m_188 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6df:                              // [tarotcard view]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[tarotcard view]", 0x6df));
        }
        else
        {
            ScanStr(&script->m_str7c);
            script->m_field80 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6e5:                              // [extra icon]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[extra icon]", 0x6e5));
        }
        else
        {
            ScanStr(&tmp);
            script->m_vec18c.push_back(tmp);
            script->m_vec198.push_back(ScanInt((bool*)0));
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ea:                              // [field image]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[field image]", 0x6ea));
        }
        else
        {
            ScanStr(&script->m_str74);
            script->m_field78 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6ee:                              // [field animation]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[field animation]", 0x6ee));
        }
        else
        {
            ScanStr(&script->m_str1a4);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x6f1:                              // [custom animation]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[custom animation]", 0x6f1));
        }
        else
        {
            script->m_vec1a8.clear();
            while (ScanStr(&tmp))
            {
                script->m_vec1a8.push_back(tmp);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x71c:                              // [usable job]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable job]", 0x71c));
        }
        else
        {
            while (ScanStr(&tmp))
            {
                int job = getCharacterJob(tmp.c_str());
                if (job == 0xb)
                {
                    for (int i = 0; i < 0xb; ++i)
                    {
                        script->m_usable[i] = 1;
                    }
                    script->m_fielda0 = 1;
                }
                else
                {
                    script->m_usable[job] = 1;
                }
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x72a:                              // [suitable job]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[suitable job]", 0x72a));
        }
        else
        {
            script->m_set44.clear();
            while (ScanStr(&tmp))
            {
                int job = getCharacterJob(tmp.c_str());
                if (job == 0xb)
                {
                    for (int i = 0; i < 0xb; ++i)
                    {
                        script->m_set44.insert(i);
                    }
                }
                else
                {
                    script->m_set44.insert(job);
                }
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x739:                              // [max having count]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[max having count]", 0x739));
        }
        else
        {
            script->m_field60 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x73e:                              // [minimum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum level]", 0x73e));
        }
        else
        {
            script->m_field5c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x741:                              // [maximum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[maximum level]", 0x741));
        }
        else
        {
            script->m_17c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x744:                              // [minimum rank]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[minimum rank]", 0x744));
        }
        else
        {
            script->m_field64 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x747:                              // [control index]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[control index]", 0x747));
        }
        else
        {
            script->m_180 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x74a:                              // [required skill]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[required skill]", 0x74a));
        }
        else
        {
            script->m_184 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x74d:                              // [cool time]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cool time]", 0x74d));
        }
        else
        {
            script->m_field68 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x750:                              // [equipment type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[equipment type]", 0x750));
        }
        else
        {
            ScanStr(&tmp);
            script->m_strbc = tmp;
            script->m_field1b4 = GetEquipmentType(tmp.c_str());
            script->m_fielda4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x75a:                              // [avatar task num]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar task num]", 0x75a));
        }
        else
        {
            ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x762:                              // [weapon full avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[weapon full avatar]", 0x762));
        }
        else
        {
            script->m_72d = 1;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x766:                              // [hide equipment]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide equipment]", 0x766));
        }
        else
        {
            script->m_equipTypes768.clear();
            while (ScanStr(&tmp))
            {
                if (tmp == "[/hide equipment]")
                {
                    break;
                }
                ENUM_EQUIPMENTTYPE eqType = (ENUM_EQUIPMENTTYPE)0x16;
                if (tmp == "[hat avatar]")          eqType = (ENUM_EQUIPMENTTYPE)0;
                else if (tmp == "[hair avatar]")    eqType = (ENUM_EQUIPMENTTYPE)1;
                else if (tmp == "[face avatar]")    eqType = (ENUM_EQUIPMENTTYPE)2;
                else if (tmp == "[coat avatar]")    eqType = (ENUM_EQUIPMENTTYPE)3;
                else if (tmp == "[pants avatar]")   eqType = (ENUM_EQUIPMENTTYPE)4;
                else if (tmp == "[shoes avatar]")   eqType = (ENUM_EQUIPMENTTYPE)5;
                else if (tmp == "[breast avatar]")  eqType = (ENUM_EQUIPMENTTYPE)6;
                else if (tmp == "[waist avatar]")   eqType = (ENUM_EQUIPMENTTYPE)7;
                else if (tmp == "[skin avatar]")    eqType = (ENUM_EQUIPMENTTYPE)8;
                else if (tmp == "[aurora avatar]")  eqType = (ENUM_EQUIPMENTTYPE)9;
                else if (tmp == "[weapon]")         eqType = (ENUM_EQUIPMENTTYPE)10;
                else if (tmp == "[title name]")     eqType = (ENUM_EQUIPMENTTYPE)0xb;
                if ((int)eqType != 0x16)
                {
                    script->m_equipTypes768.push_back(eqType);
                }
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x792:                              // [hide layer]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide layer]", 0x792));
        }
        else
        {
            script->m_vec774.clear();
            for (;;)
            {
                int v = ScanInt(&hideFlag774);
                if (!hideFlag774)
                {
                    break;
                }
                script->m_vec774.push_back(v);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x79e:                              // [hide growtype avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hide growtype avatar]", 0x79e));
        }
        else
        {
            script->m_vec780.clear();
            for (;;)
            {
                int v = ScanInt(&hideFlag780);
                if (!hideFlag780)
                {
                    break;
                }
                script->m_vec780.push_back(v);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7aa:                              // [usable period]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable period]", 0x7aa));
        }
        else
        {
            script->m_fieldf4 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ad:                              // [expiration date]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[expiration date]", 0x7ad));
        }
        else
        {
            script->m_fieldf8 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7bf:                              // [usable even peace]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable even peace]", 0x7bf));
        }
        else
        {
            script->m_78c = (char)(ScanInt((bool*)0) == 1);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7c3:                              // [sub type]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[sub type]", 0x7c3));
        }
        else
        {
            if (aniMode)
            {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                info.m_3c = ScanInt((bool*)0);
            }
            else if (levelAniIdx != -1)
            {
                ScanInt((bool*)0);
            }
            else
            {
                script->m_1b8 = ScanInt((bool*)0);
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d3:                              // [durability]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[durability]", 0x7d3));
        }
        else
        {
            script->m_1bc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d6:                              // [creature species]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature species]", 0x7d6));
        }
        else
        {
            script->m_6bc = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7d9:                              // [creature minimum level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature minimum level]", 0x7d9));
        }
        else
        {
            script->m_artifact738.m_minLevel = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7dc:                              // [creature physical attack]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature physical attack]", 0x7dc));
        }
        else
        {
            script->m_artifact738.m_physAttack = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7df:                              // [creature magical attack]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature magical attack]", 0x7df));
        }
        else
        {
            script->m_artifact738.m_magAttack = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e2:                              // [creature skill consume mp rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill consume mp rate]", 0x7e2));
        }
        else
        {
            script->m_artifact738.m_mpRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e5:                              // [creature skill charge time rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill charge time rate]", 0x7e5));
        }
        else
        {
            script->m_artifact738.m_chargeRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7e8:                              // [creature skill over charge time rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature skill over charge time rate]", 0x7e8));
        }
        else
        {
            script->m_artifact738.m_overChargeRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7eb:                              // [creature experience amount rate]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature experience amount rate]", 0x7eb));
        }
        else
        {
            script->m_artifact738.m_expRate = ScanFloat(2, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ee:                              // [creature physical critical hit]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature physical critical hit]", 0x7ee));
        }
        else
        {
            script->m_artifact738.m_physCrit = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f1:                              // [creature magical critical hit]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature magical critical hit]", 0x7f1));
        }
        else
        {
            script->m_artifact738.m_magCrit = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f4:                              // [creature stuck]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature stuck]", 0x7f4));
        }
        else
        {
            script->m_artifact738.m_stuck = ScanFloat(1, (bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7f7:                              // [creature piercing]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[creature piercing]", 0x7f7));
        }
        else
        {
            script->m_artifact738.m_piercing = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x7ff:                              // [output index]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[output index]", 0x7ff));
        }
        else
        {
            script->m_764 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x802:                              // [price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[price]", 0x802));
        }
        else
        {
            script->m_field14 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x805:                              // [value]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[value]", 0x805));
        }
        else
        {
            script->m_field24 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x808:                              // [repair price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[repair price]", 0x808));
        }
        else
        {
            script->m_178 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x80b:                              // [finish point price]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[finish point price]", 0x80b));
        }
        else
        {
            script->m_field20 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x80e:                              // [cash]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[cash]", 0x80e));
        }
        else
        {
            script->m_field18 = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x811:                              // [medal]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[medal]", 0x811));
        }
        else
        {
            script->m_field1c = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x81a:                              // [usable upgrade chracter level]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable upgrade chracter level]", 0x81a));
        }
        else
        {
            script->m_field134 = (unsigned short)ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x81e:                              // [need material]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[need material]", 0x81e));
        }
        else
        {
            script->m_pair28.first = ScanInt((bool*)0);
            script->m_pair28.second = ScanInt((bool*)0);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x823:                              // [recieve postal message]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[recieve postal message]", 0x823));
        }
        else
        {
            ScanStr(&script->m_str174);
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x828:                              // [Uncompoundable]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[Uncompoundable]", 0x828));
        }
        else
        {
            int v = ScanInt(&boolScratch);
            if (boolScratch && v == 1)
            {
                script->m_82c = 1;
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x83d:                              // [remove avatar]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[remove avatar]", 0x83d));
        }
        else
        {
            // ORIG LAB_08994bb2：块作用域临时 string（ctor/dtor 各 1 次）。
            std::string moduleName;
            while (ScanStr(&moduleName))
            {
                // ORIG 08994c54：实参为 prvalue → push_back(int&&)。
                script->m_vec830.push_back(getModuleType(moduleName.c_str()));
            }
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x845:                              // [auction free]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[auction free]", 0x845));
        }
        else
        {
            script->m_field30 = 1;
            goto stage1_switch_end;
        }
            /*FALLTHRU*/
        case 0x849:                              // [hidden option]
        if (!__inited)
        {
            __lookup.insert(std::map<char const*, int, less_TCHAR>::value_type("[hidden option]", 0x849));
        }
        else
        {
            script->m_field31 = 1;
            goto stage1_switch_end;
        }
        default:
            if (__inited)
            {
                matched = false;
            }
            }
        stage1_switch_end:
            if (__inited)
            {
                break;                 // ORIG LAB_08994d8b → code_r0x08994db2
            }
            __init_lock.leave();       // ORIG 首轮沿 case 链填完表后置位并重新分派
            __inited = true;
        }

        if (!matched)
        {
            // 未命中第一段：交给参数区解析器（ORIG code_r0x08994db2）。
            bool consumed;
            if (curParam == &script->m_param1c0)
            {
                consumed = importEquipmentParameterInfo(curParam, token, path,
                                                        cnt[3], cnt[2]);
            }
            else
            {
                consumed = importEquipmentParameterInfo(curParam, token, path,
                                                        cnt[1], cnt[0]);
            }
            if (!consumed && cnt[3] < 0 && cnt[2] < 0)
            {
                return false;            // ORIG fail-return（不 unload）
            }
            if (!consumed)
            {
                // ---------------- 第二段分派（动画/扩展区，ORIG switch(local_70)） --
                matched = true;
                // ORIG 0x08994e85-0x0899c9bb：第二级静态表 __lookup_0 + __inited_0
                // 双检锁（独立于第一段）；整数 switch 二分降形态，每个 case 体首查
                // __inited_0：首轮 idB=0 命中 case 0/0x867 别名，沿 case 布局逐例
                // 惰性 insert（109 项，ORIG 0x089956c4 起），填完置位重新分派。
                static std::map<char const*, int, less_TCHAR> __lookup_0;
                static SpinLock __init_lock_0;
                static bool __inited_0 = false;
                int idB;
                for (;;)
                {
                    if (!__inited_0)
                    {
                        __init_lock_0.enter();
                        if (__inited_0)
                        {
                            __init_lock_0.leave();
                            continue;
                        }
                        idB = 0;
                    }
                    else
                    {
                        char const* keyB = token.c_str();
                        std::map<char const*, int, less_TCHAR>::const_iterator itB =
                            __lookup_0.find(keyB);
                        idB = (itB != __lookup_0.end()) ? itB->second : -1;
                    }
                    switch (idB)
                    {

                case 0:
                    /*FALLTHRU*/
                case 0x867:                          // [expand ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expand ani]", 0x867));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                aniMode = true;
                info.m_24 = ScanInt((bool*)0);
                info.m_28 = ScanInt((bool*)0);
                info.m_2c = ScanInt((bool*)0);
                info.m_38 = ScanInt((bool*)0);
                info.m_expand14 = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x870:                          // [/expand ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/expand ani]", 0x870));
                }
                else
                {
                aniMode = false;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x873:                          // [expand path]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expand path]", 0x873));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                ScanStr(&info.m_path4c);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x877:                          // [level section ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section ani]", 0x877));
                }
                else
                {
                if (ScanInt(&levelAniIdx) && ScanStr(&scanBuf))
                {
                    curJob = getCharacterJob(scanBuf.c_str());
                    growth.clear();
                    growth.m_level = levelAniIdx;
                    growth.m_type = script->m_field1b4;
                    script->m_69c = 1;
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    info.m_level5c.push_back(std::vector<STEquipmentAniScript>());
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x886:                          // [/level section ani]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/level section ani]", 0x886));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                if (levelLayer.m_field0 != 0)
                {
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.rbegin()->push_back(levelLayer);
                    }
                    levelLayer.clear();
                }
                info.m_growth50.push_back(growth);
                growth.clear();
                levelAniIdx = -1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x897:                          // [level section path]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section path]", 0x897));
                }
                else
                {
                // ORIG LAB_08995ae3：读入暂存串后赋 growth.m_str14（非 levelPath）。
                ScanStr(&scanBuf);
                growth.m_str14 = scanBuf;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x89e:                          // [layer variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[layer variation]", 0x89e));
                }
                else
                {
                // ORIG 08995b6e 区：aniMode 分支在前；层变体走 ScanInt(int*)，
                // 0 是合法层编号（失败标志才置 stopFlag）；先赋 levelLayerId 再 levelPath。
                if (aniMode)
                {
                    jobLayerId = ScanInt((bool*)0);
                    ScanStr(&jobPath);
                }
                else if (levelAniIdx != -1)
                {
                    // [ImpBig1 2026-09-12 真语义修复] ORIG 08995c07..08995c5a 实证：
                    // `call ScanInt(int*); xor $1; test; je 08995c28` ⇒ 返回 1（成功）
                    // 才 ScanStr 解析并赋 levelLayerId/levelPath；返回 0 才置 stopFlag
                    // （08995c1c movb $1,-0x96(%ebp)）。原译文 if/else 体反转 ⇒ 成功
                    // 反而置 stopFlag 中止导入（真语义 bug）。
                    // 变量用函数级 intScratch（ORIG -0x107c），非新建局部。
                    if (ScanInt(&intScratch))
                    {
                        ScanStr(&scanBuf);
                        levelLayerId = intScratch;
                        levelPath = scanBuf;
                    }
                    else
                    {
                        stopFlag = true;
                    }
                }
                else
                {
                    normalLayerId = ScanInt((bool*)0);
                    ScanStr(&normalPath);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x8bb:                          // [equipment ani script]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[equipment ani script]", 0x8bb));
                }
                else
                {
                ScanStr(&scanBuf);
                std::map<std::string, STEquipmentAniScript>* table =
                    getMapEquipmentAniScript();
                if (aniMode)
                {
                    setEquipmentAniScriptLay(table, &jobLayer, &scanBuf, jobLayerId,
                                             &jobPath, script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             (equipmentAniType::T)1);
                }
                else if (levelAniIdx != -1)
                {
                    setEquipmentAniScriptLay(table, &levelLayer, &scanBuf,
                                             levelLayerId, &levelPath, script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             (equipmentAniType::T)2);
                }
                else
                {
                    setEquipmentAniScriptLay(table, &normalLayer, &scanBuf,
                                             normalLayerId, &normalPath,
                                             script,
                                             (ENUM_CHARACTERJOB)curJob,
                                             equipmentAniType::T_normal);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x8da:                          // [LAYER]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[LAYER]", 0x8da));
                }
                else
                {
                if (aniMode)
                {
                    if (jobLayer.m_field0 != 0)
                    {
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                            .m_ani18.push_back(jobLayer);
                    }
                    jobLayer.clear();
                    if (jobLayerId == 0)
                    {
                        jobLayer.m_field0 = ScanInt((bool*)0);
                        jobLayer.m_str60 = jobPath;
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        jobLayer.m_field0 = jobLayerId;
                        jobLayer.m_str60 = jobPath;
                    }
                }
                else if (levelAniIdx != -1)
                {
                    if (levelLayer.m_field0 != 0)
                    {
                        EquipmentAniInfoScript& info =
                            script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                        if (!info.m_level5c.empty())
                        {
                            info.m_level5c.rbegin()->push_back(levelLayer);
                        }
                    }
                    levelLayer.clear();
                    // ORIG 08995fb2/08995fc0 实证：ScanInt(&intScratch)（忽略返回值），
                    // 随后 `cmpl $0,-0xa0(%ebp); je` ⇒ 非 0 分支在前（用 levelLayerId），
                    // 为 0 才取 intScratch。
                    ScanInt(&intScratch);
                    if (levelLayerId != 0)
                    {
                        levelLayer.m_field0 = levelLayerId;
                        levelLayer.m_str60 = levelPath;
                    }
                    else
                    {
                        levelLayer.m_field0 = intScratch;
                        levelLayer.m_str60 = levelPath;
                    }
                }
                else
                {
                    if (normalLayer.m_field0 != 0)
                    {
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                            .m_ani0.push_back(normalLayer);
                    }
                    normalLayer.clear();
                    // ORIG LAB_08995e01：两分支各自求值一次 m_str60 = path。
                    if (normalLayerId == 0)
                    {
                        normalLayer.m_field0 = ScanInt((bool*)0);
                        normalLayer.m_str60 = normalPath;
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        normalLayer.m_field0 = normalLayerId;
                        normalLayer.m_str60 = normalPath;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x925:                          // [waiting motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[waiting motion]", 0x925));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str04 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str04 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str04 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x939:                          // [move motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[move motion]", 0x939));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str08 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str08 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str08 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x94d:                          // [sit motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[sit motion]", 0x94d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str0c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str0c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str0c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x961:                          // [damage motion 1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[damage motion 1]", 0x961));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr10[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr10[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr10[0] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x975:                          // [damage motion 2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[damage motion 2]", 0x975));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr10[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr10[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr10[1] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x989:                          // [down motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[down motion]", 0x989));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str18 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str18 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str18 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x99d:                          // [overturn motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[overturn motion]", 0x99d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str1c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str1c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str1c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9b1:                          // [jump motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[jump motion]", 0x9b1));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str20 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str20 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str20 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9c5:                          // [jumpattack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[jumpattack motion]", 0x9c5));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str24 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str24 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str24 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9d9:                          // [rest motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[rest motion]", 0x9d9));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str28 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str28 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str28 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0x9ed:                          // [throw motion 1-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 1-1]", 0x9ed));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr2c[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[0] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa01:                          // [throw motion 1-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 1-2]", 0xa01));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr3c[0] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[0] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[0] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa15:                          // [throw motion 2-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 2-1]", 0xa15));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr2c[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[1] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa29:                          // [throw motion 2-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 2-2]", 0xa29));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr3c[1] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[1] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[1] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa3d:                          // [throw motion 3-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 3-1]", 0xa3d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr2c[2] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[2] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[2] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa51:                          // [throw motion 3-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 3-2]", 0xa51));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr3c[2] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[2] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[2] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa65:                          // [throw motion 4-1]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 4-1]", 0xa65));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr2c[3] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr2c[3] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr2c[3] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa79:                          // [throw motion 4-2]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[throw motion 4-2]", 0xa79));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_arr3c[3] = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_arr3c[3] = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_arr3c[3] = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xa8d:                          // [dash motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dash motion]", 0xa8d));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str4c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str4c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str4c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xaa1:                          // [dashattack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dashattack motion]", 0xaa1));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str50 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str50 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str50 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xab5:                          // [getitem motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[getitem motion]", 0xab5));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str54 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str54 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str54 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xac9:                          // [buff motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[buff motion]", 0xac9));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str58 = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str58 = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str58 = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xadd:                          // [back motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[back motion]", 0xadd));
                }
                else
                {
                    ScanStr(&scanBuf);
                    if (aniMode)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                        jobLayer.m_str5c = printBuf;
                    }
                    else if (levelAniIdx != -1)
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                        levelLayer.m_str5c = printBuf;
                    }
                    else
                    {
                        snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                        normalLayer.m_str5c = printBuf;
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xaf1:                          // [attack motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[attack motion]", 0xaf1));
                }
                else
                {
                    if (aniMode)
                    {
                        jobLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                            tmp = printBuf;
                            jobLayer.m_vec64.push_back(tmp);
                        }
                    }
                    else if (levelAniIdx != -1)
                    {
                        levelLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                            tmp = printBuf;
                            levelLayer.m_vec64.push_back(tmp);
                        }
                    }
                    else
                    {
                        normalLayer.m_vec64.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                            tmp = printBuf;
                            normalLayer.m_vec64.push_back(tmp);
                        }
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb16:                          // [etc motion]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[etc motion]", 0xb16));
                }
                else
                {
                    if (aniMode)
                    {
                        jobLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), jobPath.c_str());
                            tmp = printBuf;
                            jobLayer.m_vec70.push_back(tmp);
                        }
                    }
                    else if (levelAniIdx != -1)
                    {
                        levelLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), levelPath.c_str());
                            tmp = printBuf;
                            levelLayer.m_vec70.push_back(tmp);
                        }
                    }
                    else
                    {
                        normalLayer.m_vec70.clear();
                        while (ScanStr(&scanBuf))
                        {
                            snprintf(printBuf, sizeof(printBuf), scanBuf.c_str(), normalPath.c_str());
                            tmp = printBuf;
                            normalLayer.m_vec70.push_back(tmp);
                        }
                    }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb3d:                          // [move wav]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[move wav]", 0xb3d));
                }
                else
                {
                ScanStr(&script->m_str84);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb40:                          // [custom wav]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[custom wav]", 0xb40));
                }
                else
                {
                if (aniMode)
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    info.m_wav40.clear();
                    while (ScanStr(&tmp))
                    {
                        info.m_wav40.push_back(tmp);
                    }
                }
                else
                {
                    script->m_vec658.clear();
                    while (ScanStr(&tmp))
                    {
                        script->m_vec658.push_back(tmp);
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb51:                          // [animation job]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[animation job]", 0xb51));
                }
                else
                {
                ScanStr(&scanBuf);
                // ORIG LAB_08998b69：提交后仅复位 m_field0（不调 clear()）。
                if (normalLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani0.push_back(normalLayer);
                    normalLayer.m_field0 = 0;
                }
                if (jobLayer.m_field0 != 0)
                {
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob)
                        .m_ani18.push_back(jobLayer);
                    jobLayer.m_field0 = 0;
                }
                if (levelLayer.m_field0 != 0)
                {
                    EquipmentAniInfoScript& info =
                        script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                    if (!info.m_level5c.empty())
                    {
                        info.m_level5c.rbegin()->push_back(levelLayer);
                    }
                    levelLayer.m_field0 = 0;
                }
                curJob = getCharacterJob(scanBuf.c_str());
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb6d:                          // [variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[variation]", 0xb6d));
                }
                else
                {
                EquipmentAniInfoScript& info =
                    script->addAniInfo((ENUM_CHARACTERJOB)curJob);
                if (!aniMode)
                {
                    if (levelAniIdx == -1)
                    {
                        info.m_c = ScanInt((bool*)0);
                        info.m_10 = ScanInt((bool*)0);
                    }
                    else
                    {
                        ScanInt((bool*)0);
                        ScanInt((bool*)0);
                    }
                }
                else
                {
                    info.m_30 = ScanInt((bool*)0);
                    info.m_34 = ScanInt((bool*)0);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb82:                          // [setback img]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[setback img]", 0xb82));
                }
                else
                {
                ScanStr(&script->m_str83c);
                for (;;)
                {
                    stSetItemBackImageInfo info;
                    info.m_a = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    info.m_b = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    info.m_c = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    script->m_setItem840.push_back(info);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb98:                          // [/setback img]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/setback img]", 0xb98));
                }
                else
                {
                // 空 case（ORIG 仅建表项，无 body）
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xb9b:                          // [set item]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set item]", 0xb9b));
                }
                else
                {
                script->m_vec66c.clear();
                for (;;)
                {
                    intScratch = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    script->m_vec66c.push_back(intScratch);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xba8:                          // [piece set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[piece set ability]", 0xba8));
                }
                else
                {
                // ORIG LAB_08999014：分档号由 ScanInt 读出（bool* 出参形式）；
                // find==end（该档尚不存在）时仅求值一次 operator[]（建立默认元素），
                // find!=end（该档已存在）时才 [key].clear() 重置旧参数；最后再取
                // &table[key] 作为当前参数区。
                // [R6a00 2026-09-12 ④ 修正] 原实现把两分支体写反（缺失才 clear）：
                // ORIG 0x89990e8 `je 0x8999107`（eq 为假 = 已存在）落入 clear 块
                // （0x899911d ix + 0x8999125 call clear），fall-through（缺失）仅 ix；
                // raw 0x89990e6 = `84 c0 74 1d`。第五轮误判 EQUAL。
                int key = ScanInt(&boolScratch);
                // [R7a00 2026-09-12 P183 修正] 原为引用局部 `table = script->m_paramMap678`
                // ⇒ 4.4 把 &script->m_paramMap678 缓存进槽（mov -0x5c,%r）复用；ORIG
                // 每处重算（0x08999084 `mov 0x8(%ebp),%eax; lea 0x678(%eax),%edx` …
                // 0x08999140 共 5 处/本 case）⇒ 改为成员直用逐处重物化。
                if (script->m_paramMap678.find(key) == script->m_paramMap678.end())
                {
                    script->m_paramMap678[key];
                }
                else
                {
                    script->m_paramMap678[key].clear();
                }
                curParam = &script->m_paramMap678[key];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbb5:                          // [set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set ability]", 0xbb5));
                }
                else
                {
                // ORIG LAB_08999174：进入“分档 0”参数区。find(0)==end()（尚不存在）
                // 时仅求值一次 table[0]（建立默认元素），find(0)!=end()（已存在）时
                // 才 table[0].clear() 重置；最后再取 &table[0] 作为当前参数区并置
                // m_414 = 1、清计数。
                // [R6a00 2026-09-12 ④ 修正] 同上一处：ORIG 0x899923e `je 0x8999267`
                // （eq 假 = 已存在）落入 clear 块（0x8999287 ix + 0x899928f call clear），
                // fall-through（缺失）仅 ix。
                // [R7a00 2026-09-12 P183 修正] 同上：删引用局部，逐处重算
                // （ORIG 0x089991d4/0x089921ff/0x0899924d/0x08999274/0x089992a1 五处 lea 0x678）。
                int probeKey = 0;
                if (script->m_paramMap678.find(probeKey) == script->m_paramMap678.end())
                {
                    int existKey = 0;
                    script->m_paramMap678[existKey];
                }
                else
                {
                    int clearKey = 0;
                    script->m_paramMap678[clearKey].clear();
                }
                int layKey = 0;
                curParam = &script->m_paramMap678[layKey];
                curParam->m_414 = 1;
                cnt[1] = 0;
                cnt[0] = 0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbc1:                          // [/set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/set ability]", 0xbc1));
                }
                else
                {
                // ORIG LAB_089992e8：保存当前分档的 [set ability] 向量，
                // 然后切回主参数区继续解析。
                setAbilityBackup = curParam->m_vec468;
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbc8:                          // [/piece set ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/piece set ability]", 0xbc8));
                }
                else
                {
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbcb:                          // [set item master]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[set item master]", 0xbcb));
                }
                else
                {
                script->m_6a0 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbce:                          // [int data]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[int data]", 0xbce));
                }
                else
                {
                script->m_vec88.clear();
                for (;;)
                {
                    intScratch = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    script->m_vec88.push_back(intScratch);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbd9:                          // [string data]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[string data]", 0xbd9));
                }
                else
                {
                script->m_vec94.clear();
                while (ScanStr(&tmp))
                {
                    script->m_vec94.push_back(tmp);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbe0:                          // [packagable]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[packagable]", 0xbe0));
                }
                else
                {
                script->m_fieldfc = (char)(ScanInt((bool*)0) != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbe3:                          // [icon mark]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[icon mark]", 0xbe3));
                }
                else
                {
                // ORIG 08999691/0899969f/089996ba/089996c8 实证：ScanStr 后立刻
                // ScanInt(&intScratch)（两条 case 体同一序），再依次赋 m_strc0
                // （operator=(const string&)）与 m_fieldc4。
                ScanStr(&scanBuf);
                ScanInt(&intScratch);
                script->m_strc0 = scanBuf;
                script->m_fieldc4 = intScratch;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf0:                          // [part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[part set index]", 0xbf0));
                }
                else
                {
                script->m_798 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf4:                          // [effect part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[effect part set index]", 0xbf4));
                }
                else
                {
                script->m_79c = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbf7:                          // [reference effect part set index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[reference effect part set index]", 0xbf7));
                }
                else
                {
                script->m_vec7a0.push_back(ScanInt((bool*)0));
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xbfb:                          // [image packs]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[image packs]", 0xbfb));
                }
                else
                {
                while (ScanStr(&tmp))
                {
                    script->m_vec7b0.push_back(tmp);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc04:                          // [fixed select type]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[fixed select type]", 0xc04));
                }
                else
                {
                script->m_6e0 = ScanInt(&boolScratch);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc08:                          // [emblem socket default]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[emblem socket default]", 0xc08));
                }
                else
                {
                for (;;)
                {
                    if (!ScanStr(&scanBuf))
                    {
                        break;
                    }
                    int emblemType = getAvatarEmblemType(scanBuf.c_str());
                    if (emblemType == 0)
                    {
                        return false;
                    }
                    script->m_vec6cc.push_back((unsigned short)emblemType);
                }
                if (5 < script->m_vec6cc.size())
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc16:                          // [avatar type select]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar type select]", 0xc16));
                }
                else
                {
                stAvatarTypeSelect_t head;
                head.init();
                head.m_field0 = 7;
                script->m_avatar6c0.push_back(head);
                for (;;)
                {
                    stAvatarTypeSelect_t sel;
                    sel.init();
                    sel.m_field0 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    sel.m_field4 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_field8 = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_fieldc = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                    sel.m_field10 = ScanInt(&boolScratch);
                    if (!boolScratch || 5 < sel.m_field10)
                    {
                        return false;
                    }
                    if (0 < sel.m_field10)
                    {
                        if (5 < script->m_vec6cc.size() + sel.m_field10)
                        {
                            return false;
                        }
                        for (int i = 0; i < sel.m_field10; ++i)
                        {
                            if (!ScanStr(&scanBuf))
                            {
                                return false;
                            }
                            sel.m_socketType[i] =
                                (unsigned short)getAvatarEmblemType(scanBuf.c_str());
                            if (sel.m_socketType[i] == 0)
                            {
                                return false;
                            }
                        }
                    }
                    script->m_avatar6c0.push_back(sel);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc55:                          // [avatar func filter]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[avatar func filter]", 0xc55));
                }
                else
                {
                script->m_6d8 = ScanInt(&boolScratch);
                if (!boolScratch)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc5a:                          // [emancipate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[emancipate]", 0xc5a));
                }
                else
                {
                for (;;)
                {
                    std::string key;
                    boolScratch = ScanType(key, true);
                    script->m_field10e = 1;
                    register bool cont;
                    if (key == "[input]")
                    {
                        for (;;)
                        {
                            // ORIG 0x08999df2/0x08999dfc：两变量先预清零
                            //（`movl $0x0,-0x110c/-0x1110`），再逐次赋值
                            // ⇒ 原码为「int = 0; 后接赋值」两句式（P198①）。
                            int first = 0;
                            int second = 0;
                            first = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            second = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            script->m_vec7d8.push_back(
                                std::pair<int, int>(first, second));
                        }
                        cont = true;
                    }
                    else if (key == "[output]")
                    {
                        for (;;)
                        {
                            // ORIG 0x08999eb2/0x08999ebc：同上（-0x1114/-0x1118）
                            int first = 0;
                            int second = 0;
                            first = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            second = ScanInt(&boolScratch);
                            if (!boolScratch)
                            {
                                break;
                            }
                            script->m_vec7e4.push_back(
                                std::pair<int, int>(first, second));
                        }
                        cont = true;
                    }
                    else if (key == "[emancipate explain]")
                    {
                        ScanStr(&script->m_str7f0);
                        cont = true;
                    }
                    else
                    {
                        cont = false;
                    }
                    if (!cont)
                    {
                        break;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc8f:                          // [hardcoding cooltime]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[hardcoding cooltime]", 0xc8f));
                }
                else
                {
                intScratch = ScanInt(&boolScratch);
                script->m_6e4 = intScratch;
                    goto stage2_switch_end;
                }
            // ORIG 0xc93（汇编 LAB_0899a04f __inited==1 分支）：ScanStr/表查找失败
            // 均直接 return 0——失败路径不调 unloadRDARScriptFile（全函数唯一
            // unloadRDARScriptFile 在成功收尾 0xb084）。
                    /*FALLTHRU*/
                case 0xc93:                          // [item group name]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[item group name]", 0xc93));
                }
                else
                {
                if (!ScanStr(&scanBuf))
                {
                    return false;
                }
                std::map<std::string, int>::const_iterator git =
                    getItemGroupNameTable().find(scanBuf);
                if (git == getItemGroupNameTable().end())
                {
                    return false;
                }
                script->m_field100 = git->second;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xc9d:                          // [npc gift disallowance]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[npc gift disallowance]", 0xc9d));
                }
                else
                {
                int v = ScanInt(&boolScratch);
                script->m_field104 = (char)(v != 1);
                if (!boolScratch)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xca2:                          // [change status on change hp]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[change status on change hp]", 0xca2));
                }
                else
                {
                // ORIG 结构：块作用域 string（ctor/dtor 各 1）+ do-while 标志位；
                // 循环体内还有一个未被消费的 std::vector<float> 临时（ORIG 0899a24b
                // 构造、两条退出路径各析构一次），照其形态保留。
                std::string appendageName;
                register bool cont;
                do
                {
                    if (!ScanStr(&appendageName))
                    {
                        break;
                    }
                    std::vector<float> rateBuf;
                    (void)rateBuf;
                    if (appendageName == "[/change status on change hp]")
                    {
                        cont = false;
                    }
                    else
                    {
                        int appendage = loadAppendageType(appendageName);
                        if (appendage != -1)
                        {
                            STChangeStatusRate rate;
                            rate.m_appendage = appendage;
                            rateFlag = true;
                            rate.m_rate1 = (float)ScanInt(&rateFlag);
                            // ORIG 0899a307 / 0899a334 / 0899a361 / 0899a38e 逐次
                            // `movzbl -0x1145(%ebp),%eax; mov %al,-0x1145(%ebp)`
                            // ——每次 ScanInt 后对 bool 出参做 (bool) 归一化写回
                            // （4.1 不 fold 该自读写；语义为 NOP，照抄形态）。
                            rateFlag = (bool)rateFlag;
                            rate.m_rate3 = (float)ScanInt(&rateFlag);
                            rateFlag = (bool)rateFlag;
                            rate.m_rate2 = (float)ScanInt(&rateFlag);
                            rateFlag = (bool)rateFlag;
                            rate.m_rate4 = (float)ScanInt(&rateFlag);
                            rateFlag = (bool)rateFlag;
                            if (rateFlag)
                            {
                                script->m_rate6e8.push_back(rate);
                            }
                        }
                        cont = true;
                    }
                } while (cont);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc1:                          // [epic routing]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[epic routing]", 0xcc1));
                }
                else
                {
                script->m_7bc = (ScanInt(&boolScratch) == 1);
                if (!boolScratch)
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc6:                          // [special ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[special ability]", 0xcc6));
                }
                else
                {
                script->m_7c0 = ScanInt(&boolScratch);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcc9:                          // [chat emoticon index]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[chat emoticon index]", 0xcc9));
                }
                else
                {
                script->m_7c4 = ScanInt(&boolScratch);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xccc:                          // [required full set for chatting emoticon]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[required full set for chatting emoticon]", 0xccc));
                }
                else
                {
                int v = ScanInt(&boolScratch);
                script->m_7c8 = (char)(v != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xccf:                          // [guild power war point]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[guild power war point]", 0xccf));
                }
                else
                {
                script->m_fieldc8 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcd2:                          // [user power war point]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[user power war point]", 0xcd2));
                }
                else
                {
                script->m_fieldcc = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                case 0xcd5:                          // [special monster drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[special monster drop]", 0xcd5));
                }
                else
                {
                    goto special_monster_drop;
                }
                    /*FALLTHRU*/
                case 0xcd6:                          // [difficulty for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[difficulty for extra drop]", 0xcd6));
                }
                else
                {
                    goto special_monster_drop;
                }
                    /*FALLTHRU*/
                case 0xcd7:                          // [dungeon type for extra drop]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[dungeon type for extra drop]", 0xcd7));
                }
                else
                {
                special_monster_drop:;
                if (!EquipmentScriptSpecialMonsterDrop(script,
                                                       std::string(token), path))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcdd:                          // [character item check]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[character item check]", 0xcdd));
                }
                else
                {
                for (;;)
                {
                    STEnchantSystemData entry;
                    entry.m_a = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    entry.m_b = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    std::string equipTypeName;
                    if (!ScanStr(&equipTypeName))
                    {
                        return false;
                    }
                    entry.m_c = getEquipmentTypeToEnumValue(equipTypeName);
                    script->m_enchant7cc.push_back(entry);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcf4:                          // [hp recovery add rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[hp recovery add rate]", 0xcf4));
                }
                else
                {
                // ORIG 0899aa72..0899aa89：ScanInt(&intScratch) 后 m_730 = intScratch。
                ScanInt(&intScratch);
                script->m_730 = intScratch;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xcfc:                          // [mp recovery add rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[mp recovery add rate]", 0xcfc));
                }
                else
                {
                // ORIG 0899aaf1..0899ab08：ScanInt(&intScratch) 后 m_734 = intScratch。
                ScanInt(&intScratch);
                script->m_734 = intScratch;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd04:                          // [impossible contents]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[impossible contents]", 0xd04));
                }
                else
                {
                // ORIG LAB_0899ab13：块作用域 string（ctor 1 / dtor 2 路径）。
                std::string contentsName;
                while (ScanStr(&contentsName))
                {
                    if (contentsName == "upgrade")
                    {
                        script->m_6f4 = 1;
                    }
                    else if (contentsName == "amplify upgrade")
                    {
                        script->m_6f5 = 1;
                    }
                    else if (contentsName == "disjoint")
                    {
                        script->m_6f6 = 1;
                    }
                    else if (contentsName == "gift")
                    {
                        script->m_fieldd0 = 1;
                    }
                    else if (contentsName == "trade")
                    {
                        script->m_fieldd1 = 1;
                    }
                    else if (contentsName == "separate upgrade")
                    {
                        script->m_fieldd4 = 1;
                    }
                    else if (contentsName == "charac cargo")
                    {
                        script->m_fieldda = 1;
                    }
                    else if (contentsName == "ban redeemitem")
                    {
                        script->m_fieldd8 = 1;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd2d:                          // [possible kiri protect]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[possible kiri protect]", 0xd2d));
                }
                else
                {
                script->m_fieldd9 = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd31:                          // [limit upgradable level]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[limit upgradable level]", 0xd31));
                }
                else
                {
                while (ScanStr(&scanBuf))
                {
                    upgMin = -1;
                    upgMax = -1;
                    ScanInt(&upgMin);
                    ScanInt(&upgMax);
                    upgType = 5;
                     // ORIG 0x0899ae13-0x0899b289：第三级独立静态表 __lookup_1 + __inited_1
                     // 双检锁；整数 switch 五路降形态，case 体首查 __inited_1，首轮 idC=0 命中
                     // case 0/0xd3d 别名进入逐例惰性 insert 链（5 项）。
                     static std::map<char const*, int, less_TCHAR> __lookup_1;
                     static SpinLock __init_lock_1;
                     static bool __inited_1 = false;
                     int idC;
                     for (;;)
                     {
                         if (!__inited_1)
                         {
                             __init_lock_1.enter();
                             if (__inited_1)
                             {
                                 __init_lock_1.leave();
                                 continue;
                             }
                             idC = 0;
                         }
                         else
                         {
                             char const* key = scanBuf.c_str();
                             std::map<char const*, int, less_TCHAR>::const_iterator it =
                                 __lookup_1.find(key);
                             idC = (it != __lookup_1.end()) ? it->second : -1;
                         }
                         switch (idC)
                         {
                         case 0:
                         case 0xd3d:                          // all upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("all upgrade", 0xd3d));
                         }
                         else
                         {
                             upgType = 0;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd40:                          // normal upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("normal upgrade", 0xd40));
                         }
                         else
                         {
                             upgType = 1;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd43:                          // amplify upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("amplify upgrade", 0xd43));
                         }
                         else
                         {
                             upgType = 2;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd46:                          // separate upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("separate upgrade", 0xd46));
                         }
                         else
                         {
                             upgType = 3;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         case 0xd49:                          // 3rd chronicle upgrade
                         if (!__inited_1)
                         {
                             __lookup_1.insert(std::map<char const*, int, less_TCHAR>::value_type("3rd chronicle upgrade", 0xd49));
                         }
                         else
                         {
                             upgType = 4;
                             goto stage3_switch_end;
                         }
                             /*FALLTHRU*/
                         default:
                             break;
                         }
                     stage3_switch_end:
                         if (__inited_1)
                         {
                             break;                     // ORIG LAB_0899b262 → code_r0x0899b289
                         }
                         __init_lock_1.leave();     // ORIG 首轮沿链填表后置位重新分派
                         __inited_1 = true;
                     }
                    if (upgType != 5 && (upgMin != -1 || upgMax != -1))
                    {
                        if (upgMin == -1)
                        {
                            upgMin = 1;
                        }
                        if (upgMax == -1)
                        {
                            upgMax = 0x46;
                        }
                        if (upgType == 0)
                        {
                            for (int t = 1; t < 5; ++t)
                            {
                                if (script->m_upgrade6fc.find(
                                        (ItemUpgradeType::T)t) ==
                                    script->m_upgrade6fc.end())
                                {
                                    std::pair<ItemUpgradeType::T,
                                              std::pair<int, int> > kv;
                                    // [R4a00 SEMANTIC-DIFF 修复] ORIG 0x0899b364
                                    // `mov -0x1168(%ebp),%eax; mov %eax,-0x1178(%ebp)`
                                    // 实证 kv.first 取自 upgType（进入本分支时恒 0），
                                    // 非循环变量 t ⇒ 照抄 ORIG 原码语义（含其怪癖）。
                                    kv.first = (ItemUpgradeType::T)upgType;
                                    kv.second = std::pair<int, int>(upgMin, upgMax);
                                    script->m_upgrade6fc.insert(kv);
                                }
                            }
                        }
                        else if (script->m_upgrade6fc.find(
                                     (ItemUpgradeType::T)upgType) ==
                                 script->m_upgrade6fc.end())
                        {
                        std::pair<ItemUpgradeType::T,
                                  std::pair<int, int> > kv;
                        kv.first = (ItemUpgradeType::T)upgType;
                        kv.second = std::pair<int, int>(upgMin, upgMax);
                        script->m_upgrade6fc.insert(kv);
                        }
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd7d:                          // [particle]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[particle]", 0xd7d));
                }
                else
                {
                ScanStr(&scanBuf);
                if (scanBuf == "same position")
                {
                    ScanStr(&script->m_str7f4);
                    script->m_particle7f8.m_x = ScanInt((bool*)0);
                    script->m_particle7f8.m_y = ScanInt((bool*)0);
                    script->m_particle7f8.m_z = ScanInt((bool*)0);
                    ScanType(scanBuf, true);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd89:                          // [command]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[command]", 0xd89));
                }
                else
                {
                script->m_keyCommand = new STKeyCommand();
                importCommand(script->m_keyCommand);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd8d:                          // [character name y revision]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[character name y revision]", 0xd8d));
                }
                else
                {
                script->m_80e = (unsigned short)ScanInt((bool*)0);
                script->m_80c = (unsigned short)ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xd91:                          // [routing priority]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[routing priority]", 0xd91));
                }
                else
                {
                script->m_field10f = 1;
                std::string jobName;
                for (;;)
                {
                    if (!ScanStr(&jobName))
                    {
                        break;
                    }
                    if (jobName == "[/routing priority]")
                    {
                        break;
                    }
                    int jobId = getCharacterJobToEnumValue(jobName);
                    if (jobId == -1)
                    {
                        break;
                    }
                    int pri = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        break;
                    }
                    if (pri < 0 || 4 < pri)
                    {
                        return false;
                    }
                    script->m_vec110.push_back(std::make_pair(jobId, pri));
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdac:                          // [quest item drop rate bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[quest item drop rate bonus]", 0xdac));
                }
                else
                {
                script->m_810 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdb0:                          // [level section ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[level section ability]", 0xdb0));
                }
                else
                {
                // ORIG 0899ba20 传 int*：合法值 0 也必须创建新段；
                // 不能用 ScanInt(bool*) 的“数值非零”结果代替成功标志。
                if (ScanInt(&intScratch))
                {
                    EquipmentParameterInfo section;
                    section.m_410 = intScratch;          // ORIG 0899ba4f..0899ba55 直证
                    script->m_paramVec690.push_back(section);
                    int sectionIdx = script->m_paramVec690.size() - 1;
                    curParam = &script->m_paramVec690[sectionIdx];
                }
                else
                {
                    stopFlag = true;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdbf:                          // [/level section ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/level section ability]", 0xdbf));
                }
                else
                {
                curParam = &script->m_param1c0;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdc3:                          // [Force Result Item Rule]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[Force Result Item Rule]", 0xdc3));
                }
                else
                {
                script->m_814 = (char)ScanInt((bool*)0);
                script->m_818 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdc7:                          // [effect]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[effect]", 0xdc7));
                }
                else
                {
                effect::STEffect effectData;
                if (importEffect(effectData))
                {
                    script->m_effect84c.push_back(effectData);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdcc:                          // [usable module]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable module]", 0xdcc));
                }
                else
                {
                script->m_set714.clear();
                int module = scanModuleType();
                while (module != 0x11)
                {
                    script->m_set714.insert(module);
                    module = scanModuleType();
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdd5:                          // [usable world]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable world]", 0xdd5));
                }
                else
                {
                script->m_setdc.clear();
                int world = scanWorldType();
                while (world != 2)
                {
                    script->m_setdc.insert(world);
                    world = scanWorldType();
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdde:                          // [no random]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[no random]", 0xdde));
                }
                else
                {
                script->m_72c = 1;
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xde1:                          // [usable period after unsealing]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[usable period after unsealing]", 0xde1));
                }
                else
                {
                script->m_6f8 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdeb:                          // [aurora graphic effects]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[aurora graphic effects]", 0xdeb));
                }
                else
                {
                int total = ScanInt((bool*)0);
                for (int i = 0; i < total; ++i)
                {
                    int enabled = ScanInt((bool*)0);
                    script->m_vec864.push_back(enabled == 1);
                    ScanStr(&scanBuf);
                    script->m_vec858.push_back(scanBuf);
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdf5:                          // [msg balloon on mucu skill]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[msg balloon on mucu skill]", 0xdf5));
                }
                else
                {
                script->m_87c = ScanInt((bool*)0);
                ScanStr(&script->m_str878);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdf9:                          // [random option]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[random option]", 0xdf9));
                }
                else
                {
                script->m_888 = 1;
                script->m_889 = (char)ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xdfe:                          // [enable dye]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[enable dye]", 0xdfe));
                }
                else
                {
                script->m_88a = (char)(ScanInt((bool*)0) != 0);
                script->m_88b = (char)(ScanInt((bool*)0) != 0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe0a:                          // [expertjob only]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[expertjob only]", 0xe0a));
                }
                else
                {
                if (!GetExpertJobTypeList(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe0e:                          // [prof material variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof material variation]", 0xe0e));
                }
                else
                {
                if (!GetExpertJobValueMaterialVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe12:                          // [prof compound rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof compound rate]", 0xe12));
                }
                else
                {
                if (!GetExpertJobValueCompoundRate(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe16:                          // [prof result variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof result variation]", 0xe16));
                }
                else
                {
                if (!GetExpertJobResultVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe1a:                          // [prof disjoint big win rate]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof disjoint big win rate]", 0xe1a));
                }
                else
                {
                if (!GetExpertJobDisjointBigWinRate(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe1e:                          // [prof disjoint result variation]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof disjoint result variation]", 0xe1e));
                }
                else
                {
                if (!GetExpertJobDisjointResultVariation(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe22:                          // [prof additional gain exp]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[prof additional gain exp]", 0xe22));
                }
                else
                {
                if (!GetExpertJobAdditionalGainExp(script))
                {
                    return false;
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe28:                          // [war room point bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[war room point bonus]", 0xe28));
                }
                else
                {
                if (boolScratch = ScanStr(&scanBuf))
                {
                    script->m_wpBonus81c.m_type = getCalculationType(scanBuf.c_str());
                    if (script->m_wpBonus81c.m_type == 2)
                    {
                        return false;
                    }
                    script->m_wpBonus81c.m_value = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe37:                          // [war room monster exp bonus]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[war room monster exp bonus]", 0xe37));
                }
                else
                {
                if (boolScratch = ScanStr(&scanBuf))
                {
                    script->m_expBonus824.m_type =
                        getCalculationType(scanBuf.c_str());
                    if (script->m_expBonus824.m_type == 2)
                    {
                        return false;
                    }
                    script->m_expBonus824.m_value = ScanInt(&boolScratch);
                    if (!boolScratch)
                    {
                        return false;
                    }
                }
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe48:                          // [not amplify]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[not amplify]", 0xe48));
                }
                else
                {
                script->m_890 = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe4d:                          // [change type ultimateSkillCurScene]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[change type ultimateSkillCurScene]", 0xe4d));
                }
                else
                {
                script->m_6dc = ScanInt((bool*)0);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe54:                          // [item category]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[item category]", 0xe54));
                }
                else
                {
                GetItemCategory(script);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe57:                          // [aura ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[aura ability]", 0xe57));
                }
                else
                {
                GetAuraAvatarOption(script);
                    goto stage2_switch_end;
                }
                    /*FALLTHRU*/
                case 0xe5c:                          // [/aura ability]
                if (!__inited_0)
                {
                    __lookup_0.insert(std::map<char const*, int, less_TCHAR>::value_type("[/aura ability]", 0xe5c));
                }
                else
                {
                // 空 case
                    goto stage2_switch_end;
                }
                default:
                if (__inited_0)
                {
                    // 未命中第二段：模块标签处理（ORIG default LAB_0899c902）。
                    int modOut[2];
                    modOut[0] = -1;
                    modOut[1] = -1;
                    bool okMod = processModuleTagSet(modOut[1], modOut[0], &token,
                                                     a, true);
                    if (modOut[0] != -1 && okMod)
                    {
                        script->m_808 = modOut[0];
                    }
                }
                    }
                stage2_switch_end:
                    if (__inited_0)
                    {
                        break;              // ORIG LAB_0899c9bb → 退出分派循环
                    }
                    __init_lock_0.leave();  // ORIG 首轮沿链填表后置位重新分派
                    __inited_0 = true;
                }
            }
        }
        if (stopFlag)
        {
            break;
        }
    }

    // ---------------- 收尾（ORIG LAB_0899c9f2） ----------------
    if (!setAbilityBackup.empty())
        script->m_param1c0.m_vec468 = setAbilityBackup;
    if (normalLayer.m_field0 != 0)
    {
        script->addAniInfo((ENUM_CHARACTERJOB)curJob).m_ani0.push_back(normalLayer);
    }
    if (jobLayer.m_field0 != 0)
    {
        script->addAniInfo((ENUM_CHARACTERJOB)curJob).m_ani18.push_back(jobLayer);
    }
    if (levelLayer.m_field0 != 0)
    {
        EquipmentAniInfoScript& info =
            script->addAniInfo((ENUM_CHARACTERJOB)curJob);
        if (!info.m_level5c.empty())
        {
            info.m_level5c.rbegin()->push_back(levelLayer);
        }
    }
    if (script->m_178 == -1)
    {
        script->m_178 = script->m_field14;
        if (script->m_field24 != -1)
        {
            script->m_178 = script->m_field24;
        }
    }
    if (script->m_field24 == -1)
    {
        script->m_field24 = script->m_field14;
    }
    if (idx == -1)
    {
        script->m_field0 =
            (int)g_stEquipmentScriptFileList.FindIndexByFullScan(path);
    }
    else
    {
        script->m_field0 = idx;
    }
    unloadRDARScriptFile();
    return true;
}
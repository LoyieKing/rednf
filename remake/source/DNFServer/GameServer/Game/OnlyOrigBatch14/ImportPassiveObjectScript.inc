bool ImportPassiveObjectScript(STPassiveObjectScript* script, const char* path,
                               bool bRepair)
{
    // [BigPassiveObj-wave10 2026-09-09 判词] §6.2 大件专项：本波按「call 锚点间隔
    // 逐段」法定位真缺口（p167 对 995/995 全等仍漏检「缺返回值检查」类缺口）。
    // 本波 4 项修复（均 ORIG 逐地址实证，fresh check_tu_routed 四数不回退）：
    //   ① **12 处 ScanStr 返回值检查缺失**（真语义缺口，ORIG 每处为
    //      `call; xor $0x1,%eax; test %al,%al; je; mov $0x0,%ebx; jmp`）：
    //      0x8a2cba3(m_name) / 0x8a2cd88(str→m_i18) / 0x8a2ceb7(m_s17c) /
    //      0x8a2d065(str→[pass type]) / 0x8a2dd57(m_s44) / 0x8a2ddd7(m_s54) /
    //      0x8a2e276(m_s64) / 0x8a2e2f6(m_s68) / 0x8a2e75f(str→m_i94) /
    //      0x8a314bc(m_s154) / 0x8a320dc(vs) / 0x8a3350b(ns)
    //      ⇒ 全改 `if (!ScanStr(&x)) return false;`。修后 ORIG 25 处 ScanStr
    //      调用与重建一一对应（23 带检查 + 2 裸调用），本波前为 12/13。
    //   ② **1 处多余检查删除**：0x8a34e8b（[passive object type] 的 ScanStr）
    //      ORIG call 后直落 `movl $0x2,-0x28` + guard 检查（无 xor/test/je）
    //      ⇒ 原码为裸调用，旧译文多写 5 条。
    //   ③ **m_i270 落盘缓冲形态**：ORIG 用局部 -0x28（初值 2 于 0x8a34e90；
    //      两个子 token 的 else 分支改写 0/1 于 0x8a350fd/0x8a35163；公共尾
    //      0x8a35190 `movl -0x28 → this+0x270`）。旧译文直接 3 次成员写（含
    //      3 次 script 指针重载 + 重复 if-else 比较）⇒ 改具名 `t270` 后 −7 insn。
    //   ④ **P304**：`reinterpret_cast<STScriptFileList*>(&g_stPassiveObjectScriptFileList)`
    //      （0x8a3579b）令 4.4 物化 `mov $imm,%eax; mov %eax,(%esp)` 并挤占 %eax；
    //      extern 声明改权威型 STScriptFileList（NpcScript.h:61，+0 即 map）后
    //      复现 ORIG `movl $imm,(%esp)`，−1 insn。
    // 【本波读数】clean（normdiff/compare_tu 口径）orig 8417 / ours 8370→**8426**
    //   （gap +47→**−9**）；raw objdump 8490 / 8452→**8512**（+38→−22）；
    //   p167 calls **995=995**（MISSING/EXTRA 仅 C1↔C2 / D1↔D2 同址镜像，P93 豁免）；
    //   check_tu_routed 四数 identical=391 ae=782 near=0 diff=20 / missing=189
    //   **与基线逐项相同（不回退）**。
    // 【残余 −9 归因（逐块 classify 后）】均非语义缺口：
    //   a) 我方多 5：`info.m_ints44[i]` 的 4.4 寻址展开 `add $0x10; 槽往返; shl $2;
    //      add base`（ORIG 3 条 scaled `mov v,-0xdc0(%ebp,%edx,4)`）——N125/N117 族；
    //   b) 我方多 24（ORIG 同形 12 条在 5167 处）：内层子表 case 叶块物理序 +
    //      4.4 `cmpl $imm,<S>` 内存比较 vs ORIG `mov <S>,%eax; cmp $imm,%eax`
    //      （P324 叶块序 / P209 无符号比较恒载寄存器）；
    //   c) ORIG 多 4（序言 `movl $0,<S>; mov $0,%eax; mov %eax,<S>; movb $0,<S>`）
    //      = ORIG 源码 3 个死局部（P171 死写入），补回会使 |gap| 反向增大 ⇒ 不补；
    //   d) 其余 ±1~3 为 call 间隔边界的错位对（同一段代码在两侧落于相邻 call 之间）。
    //   ⇒ 剩余全部属 N82/N125/N117/P324 跨版本布局族；语义已逐段核对闭合。
    // [b28d-solo-OnlyOrigBatch14] §6.2 超大件（orig 8490 insn）：本波以
    // game_func_report gap（8490-8452=38）+ 调用点差集 + 逐块比对为验收口径
    // （ndiff 对超大件失真，N82）。18 处 static map 惰性注册链、token 分派树与
    // ORIG 逐段对齐；残余为 4.1/4.4 帧槽复用策略差（N82 族）。
    // [BigGM2-wave9 双判据复核（Main 2026-09-09 口径：gap+call 多重集为主，①不作派工
    // 序）] triage ①「26 段/1537 行」经逐项取证判为**布局假性缺口**，无未翻译语义：
    //   ① 指令缺口 38/8490 = 0.45%（形族：false 岛 `mov $0x0,%eax` 我方 37 vs ORIG 1
    //     + lea+add 拆分寻址）；② call 多重集 **995/995 逐名全等**（含 pair ctor 模板
    //     实参按 key 长度分桶 IRA{5:1,7:8,…,16:13,…,42:1} **129 键直方图逐一相等**，
    //     ScanInt 72/72、ScanStr 25/25、ScanTypeOrStr 9/9、insert 129/129、
    //     guard 三件套 36/36、$0xffffffff(v=-1) 22/22）⇒ 18 张 static map 注册内容与
    //     全部分派/失败动作 1:1 在场。①段实体 = ORIG 建表体连续段 + 异常 dtor island
    //     与我方分散 case 落码的排布差（N82/N126 族）。判 N 封顶，不再照 ① 行数硬补。
    // 【验收标注 §6.2】超大函数（ORIG 8490 insn 切片）。调用点差集归零
    // （995=995，ORIG 侧 libstdc++/PLT 无符号地址已聚类反查对位）；
    // leaf id 125/125（五子表 case-0 修复后跳表 2→0，注册轮 id=0 由
    // ORIG 树根 test/je 实证路由链首，P163）。残余 gap -38 归因：
    // ①0x2152/0x2193/0x21e1/0x2241 嵌套块内 4.1 提前装载 + 叶局部
    // string 临时槽物化（N76/N82 槽号系）；②0x227e 族「子表 switch
    // 注册 + 体外 if(id) 执行」等价重排（语义同、cmp 形每位 ±2）。
    // 语义已逐块核对完成（子表 26 tag 字段解析修复前从未执行）。
    if (!loadRDARScriptFile(g_szPassiveObjectScriptBaseDirectory, path))
    {
        return false;
    }
    script->clear();
    script->m_destroy.clear();
    script->m_addEffect.clear();
    getDirectoryPath(script->m_s90, "%s%s",
                     g_szPassiveObjectScriptBaseDirectory, path);

    std::string token;
    std::string str;
    bool nameSet = false;   // ORIG -0x8d（[name] 已读标志）
    for (;;)
    {
        if (!ScanType(token, true))
        {
            break;
        }
        int v;
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;
        for (;;)
        {
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                v = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                v = 0;
            }
            switch (v)
            {
            case 0:
            case 0x1e0d:   // // [name] → m_name；m_layer = m_name；nameSet = true
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[name]", 0x1e0d));
                }
                else
                {
                    // ORIG 0x8a2cba3：ScanStr 后 `xor $1; test %al; je` ⇒ 原码检查返回值。
                    if (!ScanStr(&script->m_name))
                    {
                        return false;
                    }
                    script->m_layer = script->m_name;
                    nameSet = true;
                        break;
                }
                /* FALL THROUGH */
            case 0x1e16:   // // [width] → m_width、m_height
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[width]", 0x1e16));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_width = v;
                    }
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_height = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e1f:   // // [floating height] → m_i14
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[floating height]", 0x1e1f));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i14 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e24:   // // [layer] → getDrawLayer → m_i18
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[layer]", 0x1e24));
                }
                else
                {
                    // ORIG 0x8a2cd88：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    script->m_i18 = getDrawLayer(str.c_str());
                        break;
                }
                /* FALL THROUGH */
            case 0x1e29:   // // [layer level] → m_i15c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[layer level]", 0x1e29));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i15c = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e2e:   // // [burning death path] → m_s17c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[burning death path]", 0x1e2e));
                }
                else
                {
                    // ORIG 0x8a2ceb7：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s17c))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e32:   // // [is hp by difficulty] → m_b158
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is hp by difficulty]", 0x1e32));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b158 = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e37:   // // [hp gage] → m_b159
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp gage]", 0x1e37));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b159 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e3c:   // // [pass type] → m_i1c（子映射 0..7）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[pass type]", 0x1e3c));
                }
                else
                {
                {
                    // ORIG 0x8a2d065：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    int pt;
                    static LookupMap __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    for (;;)
                    {
                        if (__inited)
                        {
                            LookupMap::const_iterator it = __lookup.find(str.c_str());
                            pt = (it != __lookup.end()) ? it->second : -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            pt = 0;
                        }
                        // ORIG 0x8a2d271 分派链：整数 switch；未登记轮由每个 case 头
                        // 登记自身键值后贯穿下一 case（0x8a2d353/0x8a2d3bf/0x8a2d42b/…），
                        // 链末落 0x8a2d64d（default）→ 0x8a2d662 公共尾解锁置位后重查。
                        switch (pt)
                        {
                        case 0:
                        case 0x1e42:   // [pass all] → m_i1c = 0
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass all]", 0x1e42));
                                goto __t_1e45;   // ORIG 0x8a2d353
                            }
                        {
                            script->m_i1c = 0;
                            break;
                        }
                        __t_1e45:
                        case 0x1e45:   // [pass only float type] → m_i1c = 1
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only float type]", 0x1e45));
                                goto __t_1e48;   // ORIG 0x8a2d3bf
                            }
                        {
                            script->m_i1c = 1;
                            break;
                        }
                        __t_1e48:
                        case 0x1e48:   // [do not pass] → m_i1c = 2
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[do not pass]", 0x1e48));
                                goto __t_1e4b;   // ORIG 0x8a2d42b
                            }
                        {
                            script->m_i1c = 2;
                            break;
                        }
                        __t_1e4b:
                        case 0x1e4b:   // [pass only red team] → m_i1c = 3
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only red team]", 0x1e4b));
                                goto __t_1e4e;   // ORIG 0x8a2d497
                            }
                        {
                            script->m_i1c = 3;
                            break;
                        }
                        __t_1e4e:
                        case 0x1e4e:   // [pass only blue team] → m_i1c = 4
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only blue team]", 0x1e4e));
                                goto __t_1e51;   // ORIG 0x8a2d503
                            }
                        {
                            // ORIG 0x8a2d505：写 4 之后**无 break**，贯穿到 0x8a2d50f
                            // （[pass only character team] 的 case 头），登记轮已完成，
                            // 故直接执行其分支体把 m_i1c 覆盖为 5 —— ORIG 真实行为。
                            script->m_i1c = 4;
                        }
                        __t_1e51:
                        case 0x1e51:   // [pass only character team] → m_i1c = 5
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only character team]", 0x1e51));
                                goto __t_1e54;   // ORIG 0x8a2d56a
                            }
                        {
                            script->m_i1c = 5;
                            break;
                        }
                        __t_1e54:
                        case 0x1e54:   // [pass only monster team] → m_i1c = 6
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only monster team]", 0x1e54));
                                goto __t_1e59;   // ORIG 0x8a2d5d6
                            }
                        {
                            script->m_i1c = 6;
                            break;
                        }
                        __t_1e59:
                        case 0x1e59:   // [pass only character] → m_i1c = 7
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[pass only character]", 0x1e59));
                                break;   // 登记链末 → ORIG 0x8a2d63f 直落公共尾
                            }
                        {
                            script->m_i1c = 7;
                            break;
                        }
                        default:   // ORIG 0x8a2d64d：已登记 → 返回 false；登记轮 → 落尾
                            if (__inited)
                            {
                                return false;
                            }
                            break;
                        }
                        // ORIG 0x8a2d662 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e62:   // // [piercing power] → m_i20
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[piercing power]", 0x1e62));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i20 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e67:   // // [piercing tolerance] → m_i24
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[piercing tolerance]", 0x1e67));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i24 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1e6c:   // // [category] → m_category（getObjectCategory；0x2c 跳过）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[category]", 0x1e6c));
                }
                else
                {
                {
                    script->m_category.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/category]")
                            {
                                return false;
                            }
                            break;
                        }
                        const int cat = getObjectCategory(s);
                        if (cat == 0x2c)
                        {
                            continue;
                        }
                        script->m_category.push_back(cat);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e83:   // // [basic motion] → m_s34（必须 .ani；非法扩展名 → false）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[basic motion]", 0x1e83));
                }
                else
                {
                {
                    std::string s;
                    if (!ScanStr(&s))
                    {
                        return false;
                    }
                    // ORIG 0x8a2da37：esi = 1 ^ isValidExtension；valid==0 → 失败返回 0。
                    if (!isValidExtension(s, ".ani"))
                    {
                        return false;
                    }
                    script->m_s34 = s;
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1e8f:   // // [etc motion] → m_basicActions
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc motion]", 0x1e8f));
                }
                else
                {
                {
                    script->m_basicActions.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc motion]")
                            {
                                return false;
                            }
                            break;
                        }
                        // ORIG 0x8a2dc2a：esi = 1 ^ isValidExtension；valid==0 → 失败返回 0。
                        if (!isValidExtension(s, ".ani"))
                        {
                            return false;
                        }
                        script->m_basicActions.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1ea9:   // // [basic action] → m_s44
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[basic action]", 0x1ea9));
                }
                else
                {
                    // ORIG 0x8a2dd57：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s44))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ead:   // // [last action] → m_s54
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[last action]", 0x1ead));
                }
                else
                {
                    // ORIG 0x8a2ddd7：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s54))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1eb1:   // // [etc action] → m_etcActions
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc action]", 0x1eb1));
                }
                else
                {
                {
                    script->m_etcActions.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc action]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_etcActions.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1ec6:   // // [under gravity] → m_b58
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[under gravity]", 0x1ec6));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b58 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ecc:   // // [vanish on move collision] → m_b61=1、m_b60
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[vanish on move collision]", 0x1ecc));
                }
                else
                {
                    script->m_b61 = 1;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b60 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ed3:   // // [trap] → m_b5a
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[trap]", 0x1ed3));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b5a = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ed9:   // // [allow friend hit] → m_b62
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[allow friend hit]", 0x1ed9));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b62 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1edf:   // // [team] → m_i5c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[team]", 0x1edf));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i5c = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ee5:   // // [destroy particle] → m_s64
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[destroy particle]", 0x1ee5));
                }
                else
                {
                    // ORIG 0x8a2e276：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s64))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1ee9:   // // [attack info] → m_s68
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[attack info]", 0x1ee9));
                }
                else
                {
                    // ORIG 0x8a2e2f6：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s68))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1eed:   // // [etc attack info] → m_attackInfos
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[etc attack info]", 0x1eed));
                }
                else
                {
                {
                    script->m_attackInfos.clear();
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/etc attack info]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_attackInfos.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f01:   // // [int data] → m_intData（ScanTypeOrInt 循环）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[int data]", 0x1f01));
                }
                else
                {
                {
                    std::string s;
                    script->m_intData.clear();
                    for (;;)
                    {
                        bool isType = false;
                        bool isInt = false;
                        const int val = ScanTypeOrInt(isType, s, isInt, 1);
                        if (!isInt)
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/int data]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_intData.push_back(val);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f18:   // // [string data] → m_stringData
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[string data]", 0x1f18));
                }
                else
                {
                {
                    std::string s;
                    script->m_stringData.clear();
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/string data]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_stringData.push_back(s);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f2e:   // // [quest type] → m_i94
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[quest type]", 0x1f2e));
                }
                else
                {
                {
                    // ORIG 0x8a2e75f：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&str))
                    {
                        return false;
                    }
                    if (str == "[none]")
                    {
                        script->m_i94 = 0;
                    }
                    else if (str == "[must be destroyed]")
                    {
                        script->m_i94 = 1;
                    }
                    else if (str == "[chaos object]")
                    {
                        script->m_i94 = 2;
                    }
                    else
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1f3c:   // // [hp max] → m_i98
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp max]", 0x1f3c));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_i98 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1f42:   // // [hp destroy] → m_b59
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[hp destroy]", 0x1f42));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b59 = (v == 1);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x1f48:   // // [object destroy condition] → m_destroy 各子字段
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[object destroy condition]", 0x1f48));
                }
                else
                {
                {
                    // ORIG 0x8a2e926：段内 while(ScanTypeOrStr) 逐词解析（非主循环 token
                    // 喂入）。行内子 token 词以反引号字符串形式出现（isType==false，
                    // 内容形如 "[destroy condition]"），段尾 [/object destroy condition]
                    // 为裸 TYPE 词；未知子 token 忽略继续（ORIG 0x8a2f2f3 出口）。
                    std::string w;
                    std::string w2;   // ORIG -0xc34：构造/析构但从未使用的空串槽
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, w, true))
                        {
                            break;   // EOF → 回主循环（主循环随后 EOF 统一收尾）
                        }
                        if (isType)
                        {
                            if (w != "[/object destroy condition]")
                            {
                                return false;
                            }
                            break;
                        }
                        int dv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(w.c_str());
                                dv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                dv = 0;
                            }
                        switch (dv)
                        {
                        case 0x1f58:   // [destroy condition] → m_type（子 token 0..4 / -1）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[destroy condition]", 0x1f58));
                                goto __reg_1f7a;   // ORIG 0x8a2ec87：登记后贯穿下一 case
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[on end of animation]")
                            {
                                script->m_destroy.m_type = 0;
                            }
                            else if (str == "[z pos equal zero]")
                            {
                                script->m_destroy.m_type = 1;
                            }
                            else if (str == "[time limite]")
                            {
                                script->m_destroy.m_type = 2;
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_i3c = v;
                                }
                            }
                            else if (str == "[not destroy and z pos equal zero]")
                            {
                                script->m_destroy.m_type = 3;
                            }
                            else if (str == "[parent dead]")
                            {
                                script->m_destroy.m_type = 4;
                            }
                            else
                            {
                                script->m_destroy.m_type = -1;
                            }
                            break;
                        }
                        __reg_1f7a:
                        case 0x1f7a:   // [destroy action] → m_flag04
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[destroy action]", 0x1f7a));
                                goto __reg_1f8b;   // ORIG 0x8a2ee2c
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[only destroy]")
                            {
                                script->m_destroy.m_flag04 = 0;
                            }
                            else if (str == "[object create after destroy]")
                            {
                                script->m_destroy.m_flag04 = 1;
                            }
                            else
                            {
                                script->m_destroy.m_flag04 = -1;
                            }
                            break;
                        }
                        __reg_1f8b:
                        case 0x1f8b:   // [on attack] → m_i40
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[on attack]", 0x1f8b));
                                goto __reg_1f9c;   // ORIG 0x8a2ef1a
                            }
                        {
                            if (!ScanStr(&str))
                            {
                                return false;
                            }
                            if (str == "[only destroy]")
                            {
                                script->m_destroy.m_i40 = 0;
                            }
                            else if (str == "[object create after destroy]")
                            {
                                script->m_destroy.m_i40 = 1;
                            }
                            else
                            {
                                script->m_destroy.m_i40 = -1;
                            }
                            break;
                        }
                        __reg_1f9c:
                        case 0x1f9c:   // [add object index] → m_ints08 × n（ORIG push 0xe0，无 reserve）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object index]", 0x1f9c));
                                goto __reg_1fa8;   // ORIG 0x8a2f008
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            for (int i = 0; i < n; ++i)
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_ints08.push_back(v);
                                }
                            }
                            break;
                        }
                        __reg_1fa8:
                        case 0x1fa8:   // [add object particle file] → reserve + m_strs14 × n
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object particle file]", 0x1fa8));
                                goto __reg_1fb7;   // ORIG 0x8a2f0f7
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            script->m_destroy.m_strs14.reserve(n);
                            for (int i = 0; i < n; ++i)
                            {
                                std::string s;
                                if (!ScanStr(&s))
                                {
                                    return false;
                                }
                                script->m_destroy.m_strs14.push_back(s);
                            }
                            break;
                        }
                        __reg_1fb7:
                        case 0x1fb7:   // [add object index on attack] → reserve + m_ints44 × n
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[add object index on attack]", 0x1fb7));
                                break;   // 登记链末：ORIG 0x8a2f249 直落公共尾 0x8a2f2f3
                            }
                        {
                            int n;
                            if (!ScanInt(&n))
                            {
                                return false;
                            }
                            script->m_destroy.m_ints44.reserve(n);
                            for (int i = 0; i < n; ++i)
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    script->m_destroy.m_ints44.push_back(v);
                                }
                            }
                            break;
                        }
                        default:   // 未知子 token → ORIG 忽略继续（0x8a2f2f3 出口）
                            break;
                        }
                        // ORIG 0x8a2f2f3 子表公共尾：登记轮结束后解锁、置位并重查本 token。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x1fc8:   // [sound category] → m_strs20/m_ints2c（5 槽）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sound category]", 0x1fc8));
                }
                else
                {
                {
                    script->m_destroy.m_strs20.clear();
                    script->m_destroy.m_ints2c.clear();
                    script->m_destroy.m_strs20.resize(5, std::string());
                    script->m_destroy.m_ints2c.resize(5, 0);
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/sound category]")
                            {
                                return false;
                            }
                            break;
                        }
                        int slot;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                slot = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                slot = 0;
                            }
                            // ORIG 0x8a2f72c：5 个类别 token 各占一个 case，登记轮由
                            // case 头登记自身后贯穿下一 case（0x8a2f7d2/0x8a2f83b/…），
                            // 真轮只写 slot 局部（-0x70），统一落到 0x8a2f99b 公共尾后
                            // 由后面的共享体读名字与音量。
                            switch (slot)
                            {
                            case 0:
                            case 0x1fe2:   // [after object create] → slot 0
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[after object create]", 0x1fe2));
                                    goto __c_1fe5;   // ORIG 0x8a2f7d2
                                }
                            {
                                slot = 0;
                                break;
                            }
                            __c_1fe5:
                            case 0x1fe5:   // [on frame] → slot 1
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on frame]", 0x1fe5));
                                    goto __c_1fe8;   // ORIG 0x8a2f83b
                                }
                            {
                                slot = 1;
                                break;
                            }
                            __c_1fe8:
                            case 0x1fe8:   // [on attack] → slot 2
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on attack]", 0x1fe8));
                                    goto __c_1feb;   // ORIG 0x8a2f8a4
                                }
                            {
                                slot = 2;
                                break;
                            }
                            __c_1feb:
                            case 0x1feb:   // [on damage] → slot 3
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on damage]", 0x1feb));
                                    goto __c_1fee;   // ORIG 0x8a2f90d
                                }
                            {
                                slot = 3;
                                break;
                            }
                            __c_1fee:
                            case 0x1fee:   // [on destroyobject] → slot 4
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[on destroyobject]", 0x1fee));
                                    break;   // 登记链末 → ORIG 0x8a2f976 直落公共尾
                                }
                            {
                                slot = 4;
                                break;
                            }
                            default:   // ORIG 0x8a2f981：已登记 → false；登记轮 → 落尾
                                if (__inited)
                                {
                                    return false;
                                }
                                break;
                            }
                            // ORIG 0x8a2f99b 子表公共尾。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        if (slot < 0)
                        {
                            return false;
                        }
                        // ORIG 在类别 token 后另读一个字符串作为声音文件名；
                        // 类别本身只用于选择 5 槽索引，不能写入数据槽。
                        if (!ScanStr(&str))
                        {
                            return false;
                        }
                        script->m_destroy.m_strs20[slot] = str;
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_destroy.m_ints2c[slot] = v;
                        }
                        if (slot == 1)   // [on frame] 额外 [on frame] 值 → m_i38
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_destroy.m_i38 = v;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x200a:   // // [add object effect] → m_addEffect
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[add object effect]", 0x200a));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        if (!ScanTypeOrStr(isType, s, 1))
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/add object effect]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_addEffect.m_strs00.push_back(s);
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_addEffect.m_ints0c.push_back(v);
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2022:   // // [add particles] → m_particles
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[add particles]", 0x2022));
                }
                else
                {
                {
                    std::string s1;
                    std::string s2;
                    std::string s3;
                    for (;;)
                    {
                        if (!ScanType(s1, true))
                        {
                            return false;
                        }
                        if (s1 == "[/add particles]")
                        {
                            break;
                        }
                        // ORIG 0x8a2fd0c/0x8a2fd31：eq "[particle]" 命中才进入对象构造，
                        // 其它外层 token 被忽略并继续（ORIG je 回循环头）。
                        if (!(s1 == "[particle]"))
                        {
                            continue;
                        }
                        STPassiveObjectAdditionalParticleInfo* obj =
                            new STPassiveObjectAdditionalParticleInfo();
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                // ORIG 0x8a2fd87：失败直接走出口，不 delete obj。
                                return false;
                            }
                            if (s2 == "[/particle]")
                            {
                                break;
                            }
                            int pv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    pv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    pv = 0;
                                }
                            switch (pv)
                            {
                            case 0:
                            case 0x203e:   // [file] → m_s00
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[file]", 0x203e));
                                    goto __p_2043;   // ORIG 0x8a2d…登记链贯穿
                                }
                                if (!ScanStr(&s3))
                                {
                                    return false;
                                }
                                obj->m_s00 = s3;
                                break;
                            __p_2043:
                            case 0x2043:   // [create delay] → m_i0c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[create delay]", 0x2043));
                                    goto __p_2048;
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj->m_i0c = v;
                                }
                                break;
                            __p_2048:
                            case 0x2048:   // [create sound] → m_s04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[create sound]", 0x2048));
                                    goto __p_204d;
                                }
                                if (!ScanStr(&s3))
                                {
                                    return false;
                                }
                                obj->m_s04 = s3;
                                break;
                            __p_204d:
                            case 0x204d:   // [follow parent] → m_s08 + 循环写 m_i10..m_i18
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[follow parent]", 0x204d));
                                    goto __p_2057;
                                }
                                obj->m_s08 = s2;
                                // ORIG 0x8a3023b..0x8a3028a：单 ScanInt 调用点，
                                // 循环 i=0..2 索引写 obj+0x10+i*4。
                                for (int i = 0; i < 3; ++i)
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    int* base = &obj->m_i10;
                                    base[i] = v;
                                }
                                break;
                            __p_2057:
                            case 0x2057:   // [loop] → m_b1c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[loop]", 0x2057));
                                    break;   // 登记链末 → ORIG 0x8a30328 公共尾
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj->m_b1c = (v != 0);
                                }
                                break;
                            default:
                                // ORIG 0x8a2fd33 前的失败路径同样不 delete。
                                if (__inited)
                                {
                                    return false;
                                }
                                break;
                            }
                            // ORIG 0x8a30328 子表公共尾：登记轮完成后解锁置位并重查本 token。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                        }
                            break;
                        }
                            }
                        script->m_particles.push_back(obj);
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2066:   // // [homing] → homing 字段组
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[homing]", 0x2066));
                }
                else
                {
                {
                    // ORIG 0x8a3043a：段内 while(ScanType) 逐词解析（子 token 与段尾
                    // [/homing] 均为裸 TYPE 词，非主循环喂入）；未知子 token 忽略继续
                    // （ORIG 0x8a31103：inited==1 时无操作回循环头）。
                    std::string hw;
                    for (;;)
                    {
                        if (!ScanType(hw, true))
                        {
                            break;   // EOF → 回主循环统一收尾
                        }
                        if (hw == "[/homing]")
                        {
                            break;
                        }
                        int hv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(hw.c_str());
                                hv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                hv = 0;
                            }
                        switch (hv)
                        {
                    case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                    case 0x2072:   // [homing use] → m_b9c
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing use]", 0x2072));
                            goto __h_2078;   // ORIG 0x8a30843 贯穿
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_b9c = (v != 0);
                        }
                        break;
                    __h_2078:
                    case 0x2078:   // [homing follow] → m_ia0/m_ia4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing follow]", 0x2078));
                            goto __h_2097;   // ORIG 0x8a30a22
                        }
                    {
                        std::string fs;   // ORIG -0xc7c 局部槽
                        if (!ScanStr(&fs))
                        {
                            return false;
                        }
                        if (fs == "[ME]")
                        {
                            script->m_ia0 = 0;
                        }
                        else if (fs == "[ENEMY]")
                        {
                            script->m_ia0 = 1;
                        }
                        else if (fs == "[CHARACTER]")
                        {
                            script->m_ia0 = 3;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_ia4 = v;
                            }
                        }
                        else if (fs == "[MONSTER]")
                        {
                            script->m_ia0 = 2;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_ia4 = v;
                            }
                        }
                        else
                        {
                            return false;
                        }
                        break;
                    }
                    __h_2097:
                    case 0x2097:   // [homing velocity] → m_fa8/m_fac
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing velocity]", 0x2097));
                            goto __h_20a2;   // ORIG 0x8a30aea
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fa8 = v;
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fac = v;
                        }
                        break;
                    __h_20a2:
                    case 0x20a2:   // [homing check gap] → m_ib0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing check gap]", 0x20a2));
                            goto __h_20a8;   // ORIG 0x8a30b7f
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ib0 = v;
                        }
                        break;
                    __h_20a8:
                    case 0x20a8:   // [sync animation rotation] → m_bb4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[sync animation rotation]", 0x20a8));
                            goto __h_20ae;   // ORIG 0x8a30c19
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_bb4 = (v != 0);
                        }
                        break;
                    __h_20ae:
                    case 0x20ae:   // [max rotation] → m_fb8
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[max rotation]", 0x20ae));
                            goto __h_20b4;   // ORIG 0x8a30cae
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fb8 = v;
                        }
                        break;
                    __h_20b4:
                    case 0x20b4:   // [init rotation] → m_fbc
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[init rotation]", 0x20b4));
                            goto __h_20ba;   // ORIG 0x8a30d43
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fbc = v;
                        }
                        break;
                    __h_20ba:
                    case 0x20ba:   // [diff rotation] → m_fc0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[diff rotation]", 0x20ba));
                            goto __h_20c0;   // ORIG 0x8a30dd8
                        }
                        {
                            float v;
                            if (!ScanFloat(&v))
                            {
                                return false;
                            }
                            script->m_fc0 = v;
                        }
                        break;
                    __h_20c0:
                    case 0x20c0:   // [homing time] → m_ic4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing time]", 0x20c0));
                            goto __h_20c6;   // ORIG 0x8a30e6d
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ic4 = v;
                        }
                        break;
                    __h_20c6:
                    case 0x20c6:   // [homing z axis] → m_ic8
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing z axis]", 0x20c6));
                            goto __h_20cc;   // ORIG 0x8a30f02
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_ic8 = v;
                        }
                        break;
                    __h_20cc:
                    case 0x20cc:   // [homing direction] → m_icc
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing direction]", 0x20cc));
                            goto __h_20d2;   // ORIG 0x8a30f97
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_icc = v;
                        }
                        break;
                    __h_20d2:
                    case 0x20d2:   // [homing keep distance] → m_id0
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[homing keep distance]", 0x20d2));
                            goto __h_20d9;   // ORIG 0x8a3102c
                        }
                        {
                            int v;
                            if (!ScanInt(&v))
                            {
                                return false;
                            }
                            script->m_id0 = v;
                        }
                        break;
                    __h_20d9:
                    case 0x20d9:   // [sync target move] → m_bd4
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[sync target move]", 0x20d9));
                            goto __h_20de;   // ORIG 0x8a31097
                        }
                        script->m_bd4 = 1;
                        break;
                    __h_20de:
                    case 0x20de:   // [straight homing] → m_bd5
                        if (!__inited)
                        {
                            __lookup.insert(
                                LookupMap::value_type("[straight homing]", 0x20de));
                            break;   // 登记链末 → ORIG 0x8a31103 公共尾
                        }
                        script->m_bd5 = 1;
                        break;
                        default:   // 未知子 token → ORIG 忽略继续
                            break;
                        }
                        // ORIG 0x8a31103 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x20e7:   // [for tournament] → m_b14c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[for tournament]", 0x20e7));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14c = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20ed:   // // [for school] → m_b14d
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[for school]", 0x20ed));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14d = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20f3:   // // [die when parent die] → m_b14e
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[die when parent die]", 0x20f3));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b14e = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20f8:   // // [transparent on meet player] → m_ub14f
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[transparent on meet player]", 0x20f8));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_ub14f = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x20fd:   // // [pvp room transparent value for observer] → m_us150
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[pvp room transparent value for observer]", 0x20fd));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_us150 = static_cast<unsigned short>(v);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2102:   // // [create sound] → m_s154
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[create sound]", 0x2102));
                }
                else
                {
                    // ORIG 0x8a314bc：ScanStr 返回值检查（xor $1/test/je 实证）。
                    if (!ScanStr(&script->m_s154))
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2106:   // // [is step on]：仅 ScanInt 校验，值丢弃（ORIG 0x8a31533）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is step on]", 0x2106));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x210d:   // // [can beat index] → m_onAttackData
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[can beat index]", 0x210d));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        bool isType = false;
                        bool isInt = false;
                        const int val = ScanTypeOrInt(isType, s, isInt, 1);
                        if (!isInt)
                        {
                            return false;
                        }
                        if (isType)
                        {
                            if (s != "[/can beat index]")
                            {
                                return false;
                            }
                            break;
                        }
                        script->m_onAttackData.push_back(int(val));
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2126:   // // [is blocking obj] → m_b16c
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[is blocking obj]", 0x2126));
                }
                else
                {
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        script->m_b16c = (v != 0);
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x212b:   // // [apply physics] → m_f170
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[apply physics]", 0x212b));
                }
                else
                {
                    {
                        float v;
                        if (!ScanFloat(&v))
                        {
                            return false;
                        }
                        script->m_f170 = v;
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x2131:   // // [following immortal] → m_i174（1；==1 时 2）
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[following immortal]", 0x2131));
                }
                else
                {
                    script->m_i174 = 1;
                    {
                        int v;
                        if (!ScanInt(&v))
                        {
                            return false;
                        }
                        if (v == 1)
                        {
                            script->m_i174 = 2;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            case 0x213f:   // // [follow parent] → m_i178
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[follow parent]", 0x213f));
                }
                else
                {
                {
                    std::string ps;   // ORIG -0xc88 局部槽
                    if (!ScanStr(&ps))
                    {
                        return false;
                    }
                    if (ps == "[path gate]")
                    {
                        script->m_i178 = 1;
                    }
                    else if (ps == "[parent]")
                    {
                        script->m_i178 = 2;
                    }
                    else if (ps == "[from parent]")
                    {
                        script->m_i178 = 3;
                    }
                    else if (ps == "[keep position]")
                    {
                        script->m_i178 = 4;
                    }
                    else
                    {
                        return false;
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2152:   // // [speed control passive objects] → m_speedCtrls
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[speed control passive objects]", 0x2152));
                }
                else
                {
                {
                    std::string s;
                    script->m_speedCtrls.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/speed control passive objects]")
                        {
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                            }
                            // ORIG 0x8a31cb4 分派（test/cmp 0x2161）+ 0x8a31cc6 case 头：
                            // 登记轮在此登记自身，真轮交回下方循环体处理。
                            switch (sv)
                            {
                            case 0:
                            case 0x2161:   // [passive object]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[passive object]", 0x2161));
                                }
                                break;
                            default:   // ORIG 0x8a31cbb else 边 → 0x8a321e2 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a31cbb：非 [passive object] 的词直接 jmp 子表尾
                        // 0x8a321e2 → 0x8a32209 jmp 段扫描头 0x8a31a7a（忽略、继续）。
                        if (sv != 0x2161)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xc94：内层循环独立词槽
                        SpeedCtrlPassiveObjInfo info;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/passive object]")
                            {
                                if (info.m_field0 == -1)
                                {
                                    return false;
                                }
                                script->m_speedCtrls.push_back(info);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            // ORIG 0x8a31f5d 分派 + 0x8a31fd3/0x8a32065 两个 case 头。
                            switch (iv)
                            {
                            case 0:
                            case 0x2177:   // [index]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[index]", 0x2177));
                                    goto __i6_217c;   // ORIG 0x8a32042 贯穿
                                }
                                break;
                            __i6_217c:
                            case 0x217c:   // [speed]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[speed]", 0x217c));
                                }
                                break;
                            default:   // ORIG 0x8a3217e 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                                }
                            if (iv == 0x2177)   // [index] → m_field0
                            {
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    info.m_field0 = v;
                                }
                            }
                            else if (iv == 0x217c)   // [speed] → m_b4（[up]/[down]）
                            {
                                std::string vs;   // ORIG -0xca4 局部槽
                                // ORIG 0x8a320dc：ScanStr 返回值检查（xor $1/test/je 实证）。
                                if (!ScanStr(&vs))
                                {
                                    return false;
                                }
                                if (vs == "[up]")
                                {
                                    info.m_b4 = 1;
                                }
                                else if (vs == "[down]")
                                {
                                    info.m_b4 = 0;
                                }
                                else
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                // ORIG 0x8a3217e 子表尾：非 [index]/[speed] 的词不报错，
                                // 0x8a321a5 jmp 段扫描头 0x8a31d46（忽略并继续）。
                                continue;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2193:   // // [objects] → m_genObjects
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[objects]", 0x2193));
                }
                else
                {
                {
                    std::string s;
                    script->m_genObjects.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/objects]")
                        {
                            break;
                        }
                        int ov;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                ov = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                ov = 0;
                            }
                            // ORIG 0x8a324b9 分派 + 0x8a32516 case 头。
                            switch (ov)
                            {
                            case 0:
                            case 0x21a1:   // [object]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[object]", 0x21a1));
                                }
                                break;
                            default:   // ORIG 0x8a32de5 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a324dd else 边 → 子表尾 0x8a32de5 → 回段扫描头。
                        if (ov != 0x21a1)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xcb0：内层循环独立词槽
                        PassiveObjGeneratorObject obj;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/object]")
                            {
                                script->m_genObjects.push_back(obj);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            switch (iv)
                            {
                            case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                            case 0x21b5:   // [index] → m_i00
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[index]", 0x21b5));
                                    goto __o8_21bc;   // ORIG 0x8a3289c 贯穿
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    obj.m_i00 = v;
                                }
                                break;
                            __o8_21bc:
                            case 0x21bc:   // [probability] → m_f04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[probability]", 0x21bc));
                                    goto __o8_21c3;   // ORIG 0x8a3292e
                                }
                                {
                                    float v;
                                    if (!ScanFloat(&v))
                                    {
                                        return false;
                                    }
                                    obj.m_f04 = v;
                                }
                                break;
                            __o8_21c3:
                            case 0x21c3:   // [warning] → ScanStr → m_s08
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[warning]", 0x21c3));
                                    break;   // 登记链末 → ORIG 0x8a32d50 公共尾
                                }
                            {
                                std::string w;
                                if (!ScanStr(&w))
                                {
                                    return false;
                                }
                                int wv;
                                static LookupMap __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                for (;;)
                                {
                                    if (__inited)
                                    {
                                        LookupMap::const_iterator it = __lookup.find(w.c_str());
                                        wv = (it != __lookup.end()) ? it->second : -1;
                                    }
                                    else
                                    {
                                        __init_lock.enter();
                                        if (__inited)
                                        {
                                            __init_lock.leave();
                                            continue;
                                        }
                                        wv = 0;
                                }
                                // ORIG 0x8a32b9f 分派 + 0x8a32bf4/0x8a32c4f/0x8a32caa 三个
                                // case 头。真轮三者都贯穿到 0x8a32d07 的同一实体
                                // （obj.m_s08 = 词），故下面只做 <0 判定后统一赋值。
                                switch (wv)
                                {
                                case 0:
                                case 0x21cb:   // none
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("none", 0x21cb));
                                        goto __w9_21cc;   // ORIG 0x8a32c4f
                                    }
                                    break;
                                __w9_21cc:
                                case 0x21cc:   // danger
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("danger", 0x21cc));
                                        goto __w9_21cd;   // ORIG 0x8a32caa
                                    }
                                    break;
                                __w9_21cd:
                                case 0x21cd:   // recovery
                                    if (!__inited)
                                    {
                                        __lookup.insert(
                                            LookupMap::value_type("recovery", 0x21cd));
                                        break;   // 登记链末 → ORIG 0x8a32d05 公共尾
                                    }
                                    break;
                                default:   // ORIG 0x8a32d24
                                    break;
                                }
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                                    }
                                if (wv < 0)
                                {
                                    return false;
                                }
                                obj.m_s08 = w;
                                break;
                            }
                            default:
                                return false;
                            }
                            // ORIG 子表公共尾：登记轮完成后解锁置位并重查本 token。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x21e1:   // // [patterns] → m_genPatterns
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[patterns]", 0x21e1));
                }
                else
                {
                {
                    std::string s;
                    script->m_genPatterns.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/patterns]")
                        {
                            break;
                        }
                        int pv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                pv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                pv = 0;
                            }
                            // ORIG 0x8a33115 分派 + 0x8a33182 case 头。
                            switch (pv)
                            {
                            case 0:
                            case 0x21f0:   // [pattern]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[pattern]", 0x21f0));
                                }
                                break;
                            default:   // ORIG 0x8a3384d 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a33146 else 边 → 子表尾 0x8a3384d → 回段扫描头。
                        if (pv != 0x21f0)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xcd4：内层循环独立词槽
                        PassiveObjGeneratorPattern pat;
                        for (;;)
                        {
                            if (!ScanType(s2, true))
                            {
                                return false;
                            }
                            if (s2 == "[/pattern]")
                            {
                                script->m_genPatterns.push_back(pat);
                                break;
                            }
                            int iv;
                            static LookupMap __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            for (;;)
                            {
                                if (__inited)
                                {
                                    LookupMap::const_iterator it = __lookup.find(s2.c_str());
                                    iv = (it != __lookup.end()) ? it->second : -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    iv = 0;
                            }
                            switch (iv)
                            {
                            case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                            case 0x2203:   // [name] → m_s04
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[name]", 0x2203));
                                    goto __o11_220c;   // ORIG 0x8a334f1 贯穿
                                }
                                {
                                    std::string ns;   // ORIG -0xcf8 局部槽
                                    // ORIG 0x8a3350b：ScanStr 返回值检查（xor $1/test/je 实证）。
                                    if (!ScanStr(&ns))
                                    {
                                        return false;
                                    }
                                    pat.m_s04 = ns;
                                }
                                break;
                            __o11_220c:
                            case 0x220c:   // [interval] → m_i0c
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[interval]", 0x220c));
                                    goto __o11_2214;   // ORIG 0x8a335e6
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    pat.m_i0c = v;
                                }
                                break;
                            __o11_2214:
                            case 0x2214:   // [columns] → m_i08
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[columns]", 0x2214));
                                    goto __o11_221c;   // ORIG 0x8a33678
                                }
                                {
                                    int v;
                                    if (!ScanInt(&v))
                                    {
                                        return false;
                                    }
                                    pat.m_i08 = v;
                                }
                                break;
                            __o11_221c:
                            case 0x221c:   // [data] → m_b00=0 + m_v10 byte 流
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[data]", 0x221c));
                                    break;   // 登记链末 → ORIG 0x8a337e9 公共尾
                                }
                            {
                                std::string ds;
                                pat.m_b00 = 0;
                                for (;;)
                                {
                                    bool isType = false;
                                    bool isInt = false;
                                    const int val = ScanTypeOrInt(isType, ds, isInt, 1);
                                    if (!isInt)
                                    {
                                        return false;
                                    }
                                    if (isType)
                                    {
                                        // ORIG 0x8a33786 eq + je fail → 正向等式判定。
                                        if (ds == "[/data]")
                                        {
                                            break;
                                        }
                                        return false;
                                    }
                                    unsigned char byte = static_cast<unsigned char>(val);
                                    pat.m_v10.push_back(byte);
                            }
                                break;
                        }
                            default:
                                return false;
                            }
                            // ORIG 子表公共尾。
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                        break;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2241:   // // [sequences] → m_genSequences
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sequences]", 0x2241));
                }
                else
                {
                {
                    std::string s;
                    script->m_genSequences.clear();
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/sequences]")
                        {
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                            }
                            // ORIG 0x8a345f6 分派 + 0x8a346bf 单 case 头（[pattern]）。
                            switch (sv)
                            {
                            case 0:
                            case 0x2250:   // [pattern]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[pattern]", 0x2250));
                                }
                                break;
                            default:   // ORIG 0x8a34d4e 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        // ORIG 0x8a34626 else 边 → 子表尾 0x8a34d4e → 回段扫描头。
                        if (sv != 0x2250)
                        {
                            continue;
                        }
                        std::string s2;   // ORIG -0xd10：内层循环独立词槽
                        PassiveObjGeneratorSequence seq;
                        for (;;)
                        {
                            bool isType = false;
                            if (!ScanTypeOrStr(isType, s2, 1))
                            {
                                return false;
                            }
                            if (isType)
                            {
                                if (s2 != "[/pattern]")
                                {
                                    return false;
                                }
                                script->m_genSequences.push_back(seq);
                                break;
                            }
                            seq.m_s00 = s2;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                seq.m_i04 = v;
                            }
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x2271:   // // [warning] → m_warnDanger/m_warnRecovery
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[warning]", 0x2271));
                }
                else
                {
                {
                    std::string s;
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/warning]")
                        {
                            break;
                        }
                        int wv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                wv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                wv = 0;
                            }
                            // ORIG 0x8a340ac 分派 + 0x8a340ed/0x8a341eb 两个 case 头。
                            switch (wv)
                            {
                            case 0:
                            case 0x227e:   // [min distance]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[min distance]", 0x227e));
                                    goto __w13_228b;   // ORIG 0x8a3417c 贯穿
                                }
                                break;
                            __w13_228b:
                            case 0x228b:   // [max distance]
                                if (!__inited)
                                {
                                    __lookup.insert(
                                        LookupMap::value_type("[max distance]", 0x228b));
                                }
                                break;
                            default:   // ORIG 0x8a342e3 公共尾
                                break;
                            }
                            if (!__inited)
                            {
                                __init_lock.leave();
                                __inited = true;
                                continue;
                            }
                            break;
                        }
                        if (wv == 0x227e)   // [min distance] → m_warnDanger 3 int
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_a = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_b = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnDanger.m_c = v;
                            }
                        }
                        else if (wv == 0x228b)   // [max distance] → m_warnRecovery 3 int
                        {
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_a = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_b = v;
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                script->m_warnRecovery.m_c = v;
                            }
                        }
                        else
                        {
                            return false;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x22a0:   // // [sniper type] → m_sniper
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[sniper type]", 0x22a0));
                }
                else
                {
                {
                    std::string s;
                    PassiveIObjSniperInfo info;
                    for (;;)
                    {
                        if (!ScanType(s, true))
                        {
                            return false;
                        }
                        if (s == "[/sniper type]")
                        {
                            script->m_sniper = info;
                            break;
                        }
                        int sv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(s.c_str());
                                sv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                sv = 0;
                        }
                        switch (sv)
                        {
                        case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                        case 0x22ae:   // [accelerate] → m_i30
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[accelerate]", 0x22ae));
                                goto __o14_22b3;   // ORIG 0x8a34738 贯穿
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i30 = v;
                            }
                            break;
                        __o14_22b3:
                        case 0x22b3:   // [init velocity] → m_i34
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[init velocity]", 0x22b3));
                                goto __o14_22b8;   // ORIG 0x8a347ca
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i34 = v;
                            }
                            break;
                        __o14_22b8:
                        case 0x22b8:   // [shot area] → m_b40=1 + 6 int → m_ints44[0..5]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shot area]", 0x22b8));
                                goto __o14_22c1;   // ORIG 0x8a34882
                            }
                        {
                            info.m_b40 = 1;
                            // ORIG 0x8a34880：循环写对象 +0x44+i*4（i=0..5）。
                            // 栈对象基址 -0xdc4（08a34d95 传入 operator= 实证），写址
                            // -0xdc0+(i+0x10)*4 = +0x44..+0x58 → m_ints44[6]（与
                            // PassiveIObjSniperInfo::clear 同区清零循环一致）。
                            for (int i = 0; i < 6; ++i)
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_ints44[i] = v;
                            }
                            break;
                        }
                        __o14_22c1:
                        case 0x22c1:   // [shot distance] → m_b38=1、m_i3c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shot distance]", 0x22c1));
                                goto __o14_22c8;   // ORIG 0x8a3491b
                            }
                            info.m_b38 = 1;
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i3c = v;
                            }
                            break;
                        __o14_22c8:
                        case 0x22c8:   // [move check time] → m_i5c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[move check time]", 0x22c8));
                                goto __o14_22cd;   // ORIG 0x8a349ad
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i5c = v;
                            }
                            break;
                        __o14_22cd:
                        case 0x22cd:   // [invoke check time] → m_i60
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke check time]", 0x22cd));
                                goto __o14_22d2;   // ORIG 0x8a34a3f
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i60 = v;
                            }
                            break;
                        __o14_22d2:
                        case 0x22d2:   // [invoke delay time] → m_i64
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke delay time]", 0x22d2));
                                goto __o14_22d7;   // ORIG 0x8a34ad1
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i64 = v;
                            }
                            break;
                        __o14_22d7:
                        case 0x22d7:   // [wait time] → m_i68
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[wait time]", 0x22d7));
                                goto __o14_22dc;   // ORIG 0x8a34b63
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i68 = v;
                            }
                            break;
                        __o14_22dc:
                        case 0x22dc:   // [shake area] → 3 int → m_ints74[0..2]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[shake area]", 0x22dc));
                                goto __o14_22e4;   // ORIG 0x8a34c14
                            }
                        {
                            // ORIG 0x8a34c12：循环写对象 +0x74+i*4（i=0..2）。
                            // 写址 -0xdc0+(i+0x1c)*4 = +0x74..+0x7c → m_ints74[3]
                            //（与 PassiveIObjSniperInfo::clear 同区清零循环一致）。
                            for (int i = 0; i < 3; ++i)
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_ints74[i] = v;
                            }
                            break;
                        }
                        __o14_22e4:
                        case 0x22e4:   // [invoke time] → m_i6c
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke time]", 0x22e4));
                                goto __o14_22e9;   // ORIG 0x8a34ca6
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i6c = v;
                            }
                            break;
                        __o14_22e9:
                        case 0x22e9:   // [invoke count] → m_i70
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[invoke count]", 0x22e9));
                                break;   // 登记链末 → ORIG 0x8a34d4e 公共尾
                            }
                            {
                                int v;
                                if (!ScanInt(&v))
                                {
                                    return false;
                                }
                                info.m_i70 = v;
                            }
                            break;
                        default:
                            return false;
                        }
                        // ORIG 0x8a34dcc 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x22f6:   // // [passive object type] → m_i270
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[passive object type]", 0x22f6));
                }
                else
                {
                {
                    // ORIG 0x8a34e8b：此 ScanStr 返回值**不检查**（call 后直落
                    // `movl $0x2,-0x28` 与 guard 检查，无 xor/test/je）⇒ 原码为裸调用。
                    ScanStr(&str);
                    // ORIG -0x28 落盘缓冲：初值 2（0x8a34e90），两个子 token 的
                    // else 分支分别改写为 0/1（0x8a350fd / 0x8a35163），未知 token
                    // 不改写；公共尾 0x8a35190 统一 `movl -0x28 → this+0x270`。
                    int t270 = 2;
                    int tv;
                    static LookupMap __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    for (;;)
                    {
                        if (__inited)
                        {
                            LookupMap::const_iterator it = __lookup.find(str.c_str());
                            tv = (it != __lookup.end()) ? it->second : -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            tv = 0;
                        }
                        // ORIG 0x8a35063 分派 + 0x8a350a0/0x8a35106 两个 case 头。
                        switch (tv)
                        {
                        case 0:
                        case 0x22fd:   // [missile fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[missile fairy]", 0x22fd));
                                goto __t15_2301;   // ORIG 0x8a350d3 贯穿
                            }
                            t270 = 0;   // ORIG 0x8a350fd
                            break;
                        __t15_2301:
                        case 0x2301:   // [random item fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[random item fairy]", 0x2301));
                            }
                            else
                            {
                                t270 = 1;   // ORIG 0x8a35163
                            }
                            break;
                        default:   // ORIG 0x8a3516a 公共尾
                            break;
                        }
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                    }
                    // ORIG 0x8a35190 公共尾：`movl -0x28 → this+0x270`。
                    script->m_i270 = t270;
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x231a:   // // [random item fairy] → m_itemFairies
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[random item fairy]", 0x231a));
                }
                else
                {
                {
                    bool active = true;
                    RandomItemFairy fairy;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                        {
                            break;
                        }
                        int fv;
                        static LookupMap __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        for (;;)
                        {
                            if (__inited)
                            {
                                LookupMap::const_iterator it = __lookup.find(token.c_str());
                                fv = (it != __lookup.end()) ? it->second : -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                fv = 0;
                        }
                        switch (fv)
                        {
                        case 0:   // ORIG 4.1 树含未列 case 0 路由链首（P163）；注册轮 id=0 走贯穿
                        case 0x2325:   // [/random item fairy]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[/random item fairy]", 0x2325));
                                goto __o16_232a;   // ORIG 0x8a354c2 贯穿
                            }
                            active = false;
                            break;
                        __o16_232a:
                        case 0x232a:   // [entry]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[entry]", 0x232a));
                                goto __o16_232f;   // ORIG 0x8a35532
                            }
                            fairy.clear();
                            break;
                        __o16_232f:
                        case 0x232f:   // [/entry]
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[/entry]", 0x232f));
                                goto __o16_2334;   // ORIG 0x8a355af
                            }
                            script->m_itemFairies.push_back(fairy);
                            break;
                        __o16_2334:
                        case 0x2334:   // [passiveobject] → 4 int 直写 fairy（ORIG 无检查）
                            if (!__inited)
                            {
                                __lookup.insert(
                                    LookupMap::value_type("[passiveobject]", 0x2334));
                                break;   // 登记链末 → ORIG 0x8a3565d 公共尾
                            }
                            // ORIG 0x8a3561c..0x8a35658：四个 ScanInt 直写 fairy 字段，
                            // 返回值不检查（失败仅留下未初始化字段）。
                            ScanInt(&fairy.m_i00);
                            ScanInt(&fairy.m_ints04[0]);
                            ScanInt(&fairy.m_ints04[1]);
                            ScanInt(&fairy.m_ints04[2]);
                            break;
                        default:   // 未知子 token → ORIG 忽略继续（0x8a3565d：inited 后无操作回循环头）
                            break;
                        }
                        // ORIG 0x8a3565d 子表公共尾。
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                        if (!active)
                        {
                            break;
                        }
                    }
                        break;
                }
                /* FALL THROUGH */
            }
            case 0x234c:   // // [passive object icon] → m_b280/m_s284 + 尾部两 int
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[passive object icon]", 0x234c));
                }
                else
                {
                    script->m_b280 = 1;
                    ScanStr(&script->m_s284);
                    // ORIG 0x8a35715/0x8a35725：m_s284 之后连续两次 ScanInt 写
                    // this+0x288（m_i288）与 this+0x28c（m_i28c，主类末 int）。
                    // ORIG 0x8a35715/0x8a35725：ScanInt 直写两字段，返回值不检查。
                    ScanInt(&script->m_i288);
                    ScanInt(&script->m_i28c);
                        break;
                }
                /* FALL THROUGH */
            default:
                if (__inited)
                {
                    return false;
                }
                break;
                return false;
            }
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                continue;
            }
            break;
        }

        // 每 case 处理后的公共出口（ORIG 0x8a3577e）：bRepair 且 [name] 已读
        // 时提前结束；否则回到主 token 循环。
        if (bRepair && nameSet)
        {
            break;
        }
    }

    // ORIG 0x8a3579b：`movl $&g_stPassiveObjectScriptFileList,(%esp);
    // mov 0xc(%ebp),%eax; mov %eax,0x4(%esp); call FindIndexByFullScan`
    // ⇒ 权威型成员直调（P304），勿再 cast。
    script->m_idx = g_stPassiveObjectScriptFileList.FindIndexByFullScan(path);
    return true;

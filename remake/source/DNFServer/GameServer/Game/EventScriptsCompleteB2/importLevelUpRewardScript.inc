// ---- importLevelUpRewardScript T 0x08980261（4824B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importLevelUpRewardScript.md
// （ORIG 汇编 + Ghidra 反编译 C 两段已完整阅读）。标签主循环 4 个：
// [recv check db table]/[level up]/[send mail]/[level up reward end]。
// ORIG 反汇编（报告 /tmp/LevelUpReward.md，已完整读 ## 汇编 + ## 反编译 C）：
// [level up] 节内是 11 个职业标签的线性 operator== 比较链，GCC 对每个分支
// 复制一整块解析代码（ScanType 子标签 + ScanInt×4 + ScanStr×2 + push_back +
// pair(char&,cond&) 构造 + map::insert + pair 析构），故 ORIG 函数体达 4824B、
// 调用 164 个。本文件按同构形态逐分支展开转写。
// 控制流要点（全部经汇编实证）：
//   - 节顶 ScanType 失败（EOF）→ 直接回主循环头（不建局部对象）；
//   - 职业分支内子标签 ScanType 失败 → ebx=0 → 销毁局部、退出本节回主循环
//     （不是返回假！此前重建误写 return false，已修正）；
//   - [/level up] → ebx=0 退出本节；未知 token → ebx=1 继续节内；
//   - 职业命中后子 token 不比较、直接丢弃；cond.m_filter0=立即数职业号，
//     filter[1]=(char)ScanInt(&ok)；reward 循环三 ScanInt 短路检查（任一失败
//     即跳出到 insert），随后两 ScanStr + push_back；
//   - insert 键经 pair<char&,Condition_LevelUp&> 模板推导（ORIG 符号
//     _ZNSt4pairIKi17Condition_LevelUpEC2IRcRS1_EEOT_OT0_），故键实参必须是
//     char 左值（本地 char 变量持有 filter[0] 值）；
//   - ok(=result) 为跨全部分支共享的局部 bool，ScanInt 全写它，且
//     [level up reward end] 置真后作为返回值。
// 残差归因（N 类）：ORIG(4.1) 把 token(-0x140) 与 11 个职业分支各自的
// cond/ri/临时槽在深区按活跃性交错复用（帧 0x170），4.4 分配器为每变量
// 固定独立槽（帧 0x1a0、token -0x68），几乎每条含槽指令文本错位——跨版本
// 栈槽分配器差异（P112/N76 族），无法由合法 C++ 复现；语义与指令序列已
// 逐分支对照 ORIG 汇编一致。
bool importLevelUpRewardScript(LevelUpRewardScript* script, char const* path)
{
    // b28d-solo-EventScriptsCompleteB2 复扫（2026-09-09）：原判 DIFF/N82+N93。
    // 本轮实测：11 处 `if(!ScanType) { next=false; goto levelup_section_end; }`
    // 可对齐 xor（我方 36→47=ORIG 47、ORIG-only xor 归零），但每分支多 1 条
    // next 槽写 + goto 岛 ⇒ gap -5→-15（register next 亦 -15），按 §6.2 gap
    // 优先回退正向形。槽序镜像（ORIG token -0x140 先声明深、帧 0x170；4.4
    // token -0x68 先声明浅、帧 0x1a0）、11 分支 cond/item/entry 块内对象 4.4
    // 独立分槽而 4.1 交错复用、next 在 ORIG 为 %ebx 寄存器（4.4 -O0 恒槽化）
    // ⇒ N82/N93 封顶；语义逐分支与 ORIG 汇编一致。
    // 2026-09-10 复验（判据① jcc/jmp 目标下标逐位比对）：残留 34 处分支目标
    // 偏移**全部同向（+1 平移）且方向一致**，无回边/退出错位；报告 ## 汇编
    // +0x151 `mov $0x0,%ebx` + 节尾 +0x11ec `test %ebx,%ebx` 证实 ORIG 的
    // next 走寄存器，4.4 -O0 恒把具名 bool 槽化（P0/N96 边界）⇒ 不可由合法
    // C++ 复现。
    if (!loadRDARScriptFile("Event/", path))
        return false;

    std::string token;
    bool result = false;
    script->m_dbTableName = "";
    script->m_flag08 = 0;

    // for(;;)+头侧 break 形态：令首 ScanType 调用停在循环体前（ORIG 主循环
    // test 位于 +0x67 循环头；while(cond) 形式会被 GCC 4.4 旋转至环尾）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[recv check db table]")
        {
            ScanStr(&script->m_dbTableName);
            script->m_flag08 = 1;
        }
        else if (token == "[level up]")
        {
            bool next = true;
            while (next)
            {
                if (!ScanType(token, true))
                    break;                    // ORIG 节顶 EOF：回主循环头

                Condition_LevelUp cond;
                Condition_LevelUp::Reward_Item item;

                if (token == "[swordman]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 0;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;         // ORIG ebx=0：退出本节回主循环
                }
                else if (token == "[fighter]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 1;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[gunner]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 2;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 3;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[priest]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 4;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at gunner]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 5;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[thief]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 6;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at fighter]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 7;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[at mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 8;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[demonic swordman]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 9;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[creator mage]")
                {
                    if (ScanType(token, true))
                    {
                    cond.m_filter0 = 0xa;
                    cond.m_filter1 = static_cast<unsigned char>(ScanInt(&result));
                    for (;;)
                    {
                        item.m_level = ScanInt(&result);
                        if (!result) break;
                        item.m_itemIndex = ScanInt(&result);
                        if (!result) break;
                        item.m_itemCount = ScanInt(&result);
                        if (!result) break;
                        ScanStr(&item.m_title);
                        ScanStr(&item.m_content);
                        cond.m_items.push_back(item);
                    }
                    char key = static_cast<char>(cond.m_filter0);
                    std::map<int, Condition_LevelUp>::value_type entry(key, cond);
                    script->m_conditions.insert(entry);
                    }
                    else
                    next = false;
                }
                else if (token == "[/level up]")
                {
                    next = false;
                }
                // 其余未知 token：next 保持真 → 销毁局部并继续本节（ORIG 同）
            }
        }
        else if (token == "[send mail]")
        {
            importActionMailScript(&script->m_mail);
        }
        else if (token == "[level up reward end]")
        {
            result = true;
            break;
        }
    }
    return result;
}
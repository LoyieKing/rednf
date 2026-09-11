// ImportAdvanceAltarDungeonTag ORIG 0x088f20a4（536 insn）：
// 按 str 给出的 tag 分派解析；脚本流已由调用方 loadRDARScriptFile 打开
// （path 参数在 ORIG 中未使用）。返回 false=解析失败，true=成功/结束。
bool ImportAdvanceAltarDungeonTag(STDungeonScript* script, std::string str,
                                  const char* path)
{
    // b28d-solo-STDungeonScript 判词（536=536 insn，p167 CLEAR 70=70）：
    // 真语义修复 3 处（p167 MISSING 各 1 → 0）：①[type] 分支 getStageType 第二实参
    // 直传 &script->m_2ac（ORIG 0x088f2140 `add $0x2ac,%eax` 压栈），非局部枚举再赋字段；
    // ②[clear reward] 补第 4 次 operator==（"[/advance altar clear reward]"，ORIG
    // 0x088f2355，两 break 各落一岛）；③[list] 的 `m_type==0||==1` 拆两条独立
    // push_back（ORIG 0x088f2713/0x088f2722 两次调用点）。
    // 形还原：clear reward 具名指针+判空（ORIG 0x088f2370）、rewardType 先/item 后、
    // out=&item.m_type、[map] do-while 标志形（失败各点 cont=false，P225 同族）、
    // [survival clear reward] 复用函数级 ok（局部版多一条 movb $0x0 预清）。
    // 残余 DIFF = N121/N122 块序封顶：[map] 循环尾 4.4 恒
    // `test %bl,%bl; jne <循环头>; jmp <出口>` vs ORIG `test %ebx,%ebx; je <出口>;
    // jmp <循环头>`（do-while 条件块物理序跨版本固有差；int cont 形 539、while+break 形
    // 543 均反劣，已回退）。槽位集/帧/调用点全等。
    (void)path;  // ORIG 未使用该参数
    bool ok = false;   // ORIG 0x088f20ac：函数序言即初始化（-0x15）
    if (str == "[defense dungeon]")
    {
        // ORIG 0x088f20c7：ScanInt(NULL)（movl $0x0,(%esp)）
        script->m_2ab = (ScanInt((bool*)0) != 0);
    }
    else if (str == "[advance altar type]")
    {
        std::string token;
        if (!ScanStr(&token))
        {
            return true;   // ORIG 0x088f211e：解析失败仍返回 1（结束解析）
        }
        // ORIG 0x088f2140：getStageType 第二实参 = &script->m_2ac（add $0x2ac,%eax
        // 后直接压栈），非"局部取值再赋字段"——直传字段引用（P167 调用点形）。
        if (!advancealtar::StageType::getStageType(
                token, reinterpret_cast<advancealtar::StageType::T&>(script->m_2ac)))
        {
            return true;   // ORIG 0x088f2148：同上
        }
    }
    else if (str == "[advance altar map]")
    {
        // ORIG 0x088f21a9 起为 do-while 形：循环继续标志在 %ebx（失败各点
        // `mov $0x0,%ebx`、成功 `mov $0x1,%ebx`，析构 token 后
        // `test %ebx,%ebx; je <函数尾>`），非 while(true)+return（P225 同族）。
        // 每失败点显式 cont=false 才复现 4 处 `mov $0x0,%ebx`；ok 判定为负极性。
        // cont 用 bool + 纯 register（P231 合规存储类提示）：4.4 实测
        // `test %bl,%bl; jne <循环头>; jmp <出口>`（int 形 539 反劣，bool 形 537 最优）。
        // [E06 2026-09-10 形穷举] 唯一残差 = do-while 条件块跨版本布局：
        // ORIG `test %ebx,%ebx; je <出口>; jmp <循环头>`（4.1 32 位测试+旋转反向）；
        // 4.4 族实测：①bool+register+do-while = 本条基线（533=533，仅 test %bl/je→jne
        // 两行差）；②int+register+do-while 或 while(cont!=0) = 535（多 setne %al;test %al
        // 四段物化）；③int+for(;;)+`if(!cont) goto 尾标签` = 538（goto 被物化）；
        // ④bool+for(;;)+`if(!cont) break;` = 540。⇒ N121/P283 族：4.4 对 do-while 回边
        // 恒发 `jne 循环头; jmp 出口`，4.1 的 `je 出口; jmp 循环头` 无源码旋钮。
        register bool cont;
        do
        {
            std::string token;
            if (!ScanStr(&token))
            {
                cont = false;
            }
            else
            {
                // ORIG 0x088f21e0：声明即置 -1（movl $0xffffffff,-0x24）。
                advancealtar::StageDifficulty::T diff =
                    static_cast<advancealtar::StageDifficulty::T>(-1);
                if (!advancealtar::StageDifficulty::getDifficulty(token, diff))
                {
                    cont = false;
                }
                else
                {
                    script->m_2b0[diff] = ScanInt(&ok);
                    if (!ok)
                    {
                        cont = false;
                    }
                    else
                    {
                        script->m_2bc[diff] = ScanInt(&ok);
                        if (!ok)
                        {
                            cont = false;
                        }
                        else
                        {
                            cont = true;
                        }
                    }
                }
            }
        } while (cont);
    }
    else if (str == "[advance altar clear reward]")
    {
        // 循环：[easy]/[medium]/[hard] 选择 ClearRewardData 的三个 vector，
        // 逐项导入 _RewardItem 直到 [/advance altar clear reward] 或解析失败
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            int idx = -1;
            if (token == "[easy]")
            {
                idx = 0;
            }
            else if (token == "[medium]")
            {
                idx = 1;
            }
            else if (token == "[hard]")
            {
                idx = 2;
            }
            else if (token == "[/advance altar clear reward]")
            {
                // ORIG 0x088f2355 第 4 次 operator==：命中与未命中各落一个
                // `mov $0x0,%ebx; jmp`（4.1 -O0 不合并相同块）——须显式两分支。
                break;
            }
            else
            {
                // 其它 tag → 本节结束
                break;
            }
            // ORIG 0x088f2370：具名指针局部 + 显式判空
            // （`cmpl $0x0,-0x10(%ebp); jne`，4.1 -O0 不折掉对 &arr[idx] 的空检查）。
            std::vector<advancealtar::_RewardItem>* vec =
                &script->m_clearReward2d4.m_vec[idx];
            if (vec == 0)
            {
                break;
            }
            while (true)
            {
                // ORIG 0x088f239e：rewardType 先构造→ScanStr→item 后构造
                // （槽序 -0x2c / -0x38，P28/P212）。
                std::string rewardType;
                if (!ScanStr(&rewardType))
                    break;
                advancealtar::_RewardItem item;
                // ORIG 088f23d9/088f23ee：getRewardItemType 的 bool 结果先落槽
                // （mov %al,-0x39）并作为**同一变量**传入后续两个 ScanInt 的
                // bool* 实参（lea -0x39 复用）——必须保留单变量形（P84/P158）。
                // out 实参 = &item.m_type（ORIG `lea -0x38`），非局部枚举再赋值。
                bool parsed = advancealtar::RewardItemType::getRewardItemType(
                    rewardType,
                    reinterpret_cast<advancealtar::RewardItemType::T&>(item.m_type));
                if (!parsed)
                    break;
                item.m_value1 = ScanInt(&parsed);
                if (!parsed)
                    break;
                item.m_value2 = ScanInt(&parsed);
                if (!parsed)
                    break;
                vec->push_back(item);
            }
        }
    }
    else if (str == "[advance altar survival map]")
    {
        while (true)
        {
            // ORIG 088f24c0：int 局部先置 0（dead init，P171 照抄），
            // ScanInt 用**函数级 ok(-0x15)**，返回值落 int 槽。
            int mapId = 0;
            mapId = ScanInt(&ok);
            if (!ok)
            {
                break;
            }
            script->m_vec2c8.push_back(mapId);
        }
    }
    else if (str == "[advance altar survival clear reward]")
    {
        // 多个 [type]…[/type] 块 → SurvivalRewardData::m_vec
        while (true)
        {
            std::string token;
            if (!ScanType(token, true))
            {
                break;
            }
            if (token != "[type]")
            {
                break;
            }
            advancealtar::RoundRewardData rrd;
            while (true)
            {
                std::string sub;
                if (!ScanType(sub, true))
                {
                    break;
                }
                if (sub == "[/type]")
                {
                    script->m_survival2f8.m_vec.push_back(rrd);
                    break;
                }
                // ORIG 0x088f260e 段：ScanInt 的 bool* 实参是**函数级** ok
                // （lea -0x15），非局部新变量——局部版多一条 movb $0x0 预清。
                if (sub == "[round]")
                {
                    rrd.m_0 = ScanInt(&ok);   // 轮次
                    if (!ok)
                    {
                        break;
                    }
                    rrd.m_4 = ScanInt(&ok);   // 奖励参数
                    if (!ok)
                    {
                        break;
                    }
                }
                else if (sub == "[list]")
                {
                    // 逐项 _RewardItem；ORIG 仅接受 type 0/1（getRewardItemType
                    // 只会产生这两个值），失败即结束本 list
                    // ORIG 0x088f2658：具名指针局部（lea rrd; add $0x8; mov -0xc）。
                    std::vector<advancealtar::_RewardItem>* vec = &rrd.m_vec8;
                    while (true)
                    {
                        std::string rewardType;
                        if (!ScanStr(&rewardType))
                            break;
                        advancealtar::_RewardItem item;
                        // ORIG 088f26d9 区（Ghidra local_71 复用）：结果 bool 与
                        // 后续两个 ScanInt 的 bool* 实参为**同一变量**。
                        // out 实参 = &item.m_type（ORIG `lea -0x6c`）。
                        bool parsed = advancealtar::RewardItemType::getRewardItemType(
                            rewardType,
                            reinterpret_cast<advancealtar::RewardItemType::T&>(item.m_type));
                        if (!parsed)
                            break;
                        item.m_value1 = ScanInt(&parsed);
                        if (!parsed)
                            break;
                        item.m_value2 = ScanInt(&parsed);
                        if (!parsed)
                            break;
                        // ORIG 0x088f2713/0x088f2722：两个条件各自内联一次
                        // push_back（两次调用点，p167 实证）——禁合并成 || 单调用。
                        if (item.m_type == 0)
                        {
                            vec->push_back(item);
                        }
                        else if (item.m_type == 1)
                        {
                            vec->push_back(item);
                        }
                    }
                }
                // 其它子 tag：忽略并继续（推断）
            }
        }
    }
    else
    {
        // ORIG 0x088f27ec：未识别 tag 才返回 0；上述识别分支即使
        // 在内部遇到结束/解析失败，也统一返回 1。
        return false;
    }
    return true;
}
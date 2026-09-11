// ============================================================================
// importUnlimitChallengeScript —— ORIG 0x0890bc63（报告
// _lt_global_gt_/importUnlimitChallengeScript.md 汇编+Ghidra C 双段转写）。
// 无 tag 静态表：直接字符串比较 if/else 链；七职业段为 7 个独立 else-if 块
// （ORIG 每块 info/key/item 临时各占独立栈槽，subIdx 立即数 0..6，
//  如 0x0890be87 swordman info @-0x118、0x0890bfee fighter @-0x124 …），
// 忠实按 ORIG 展开。reward_server 行 = [等级, int, int] → m_vec1c；
// reward_client 行 = 同三列 → multimap（make_pair(key&, item&) 左值模板，
// ORIG 0x0891fb9b _ZSt9make_pairIR22stUnlimitChallengeKeyR... 实证）。
// 各段 while 头四 int 链式置零（0x0890be2e `movl $0,-0x20;mov -0x20,%eax;
// mov %eax,-0x24;…` 链形 = 源码 `int a=0,b=a,c=b,d=c;`）。
// onoff：ScanInt 失败仍 return true（ORIG 0x0890bd1a jne → return 1）；
// 成功则 movb 0/1 写 m_field0（bool）。server_id/level 首列失败 = 段结束回
// 主循环；reward 段第二/三列失败 = return false。message 段 do-while：
// 每轮构造 msg1/msg2（0x0890cc53/-0x1bc），id=ScanInt 失败结束段。
// ============================================================================
// 残余 DIFF 归因（N 类豁免，语义已全等、指令数 1085/1093）：
//  ① 4.1 while/if 块排布镜像：ORIG `call ScanType; xor$1; test; jne
//     exit-island(nop;jmp)`（0x0890bcd3..bcd9）+ body fallthrough；4.4.7 O0
//     恒生成反向 `je body` + exit fallthrough（范式库负例“空 else nop;jmp 岛
//     vs jump-threading”，无合法源码形态可翻转 BB 排布）；
//  ② message 段 ebx 汇聚 flag（0x0890cd42 `mov $0x1,%ebx` / 0x0890cd6f
//     `test %ebx,%ebx`）= 4.1 统一 dtor epilogue；4.4 每路径独立 dtor
//     （负例“EH 双份 dtor epilogue vs merge”）；
//  ③ 其余为 .o 重定位地址显示差（链接后 AE 口径消除）。
// [b28d-solo-InitScriptProducers_8 N] ORIG 0890bc63：p167 CLEAR（121/121 调用点全等）；
// fresh .o 1093/1092（gap=-1）。本轮修：①全部 `if(ok!=true)` → `if(!ok)`（45 处；
// ORIG 0x890bd0e 等为 `movzbl; xor $1; test` 负极性物化）；②[reward_server]/
// [reward_client] 的 `while(ScanType(sub,true))` → `for(;;){ if(!ScanType(sub,true))
// break; ... }`（ORIG 0x890be0c 循环头 `call; xor $1; test; je body`）；③[message]
// 段改 `if(!ok) break;` 正向后置（ORIG 0x890cc8c je 继续）。残余仅 ScanType 失败块
// 极性（ORIG jne 尾块 vs 4.4 je+mov $1,%ebx 就地物化，0x0890bcd3/0x0890cdb4）与
// 循环入口旋转岛——N104/N121 封顶，语义全等。
// [P1T2 复验 2026-09-11] 残余 4 块（clean 1070/1070；原始 1094/1092 之差为
// nop 填充）：ORIG 索引 461/938 处中继 jmp（同 4.1 尾岛族）vs 4.4 `je` 就地
// 物化（索引 103/468 我方多 `jmp`）。A/B 三档（-B cc1plus446bin、
// -B …/4.4.4/、-fno-thread-jumps）读数逐项相同 ⇒ 源码不可翻。
bool importUnlimitChallengeScript(const char* path, stUnlimitChallengeInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;     // ORIG -0xfc（先构造）
    std::string sub;       // ORIG -0x100
    int zeroInit = 0;      // ORIG -0x34：movl $0x0 后不再读（帧槽对齐保留）
    bool ok = false;       // ORIG -0x101
    (void)zeroInit;

    for (;;)
    {
        if (!ScanType(token, true))
            break;                               // EOF → 成功

        if (token == "[onoff]")
        {
            int value = ScanInt(&ok);                  // -0x30
            if (!ok)
                break;                                 // P164 配套：并入共享成功出口（0x0890bd1a）
            if (value == 0)
                out->m_field0 = false;                 // movb $0,(%eax)
            else
                out->m_field0 = true;                  // movb $1,(%eax)
        }
        else if (token == "[server_id]")
        {
            int value;                                 // -0x108
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec4.push_back(value);
            }
        }
        else if (token == "[level]")
        {
            int value;                                 // -0x10c
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                out->m_vec10.push_back(value);
            }
        }
        else if (token == "[reward_server]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x2c（首列=等级）
                int link;                              // -0x28（链上死槽）
                int col2;                              // -0x24（第二列）
                int col3;                              // -0x20（第三列）
                // ORIG 0x0890be2e：movl $0,-0x20 → -0x24 → -0x28 → -0x2c 链式
                // 置零（4.1.2 先声明近 ebp；4.4.7 相反，故声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;                     // 段结束 → ScanType 下一子标签
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x118
                        info.m_jobLevel = col1;
                        info.m_subIdx = 0;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x124
                        info.m_jobLevel = col1;
                        info.m_subIdx = 1;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x130
                        info.m_jobLevel = col1;
                        info.m_subIdx = 2;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x13c
                        info.m_jobLevel = col1;
                        info.m_subIdx = 3;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x148
                        info.m_jobLevel = col1;
                        info.m_subIdx = 4;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x154
                        info.m_jobLevel = col1;
                        info.m_subIdx = 5;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeRewardInfo info;   // -0x160
                        info.m_jobLevel = col1;
                        info.m_subIdx = 6;
                        info.m_a = col2;
                        info.m_b = col3;
                        out->m_vec1c.push_back(info);
                    }
                }
            }
        }
        else if (token == "[reward_client]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                    break;
                int col1;                              // -0x1c（首列=等级）
                int link;                              // -0x18（链上死槽）
                int col2;                              // -0x14（第二列）
                int col3;                              // -0x10（第三列）
                // ORIG 0x0890c45d 链式置零（同 reward_server，声明/赋值拆分）。
                col3 = 0;
                col2 = col3;
                link = col2;
                col1 = link;
                if (sub == "[swordman]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x164
                        stUnlimitChallengeItem item;   // -0x16c
                        key.m_a = col1;
                        key.m_b = 0;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[fighter]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x170
                        stUnlimitChallengeItem item;   // -0x178
                        key.m_a = col1;
                        key.m_b = 1;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[gunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x17c
                        stUnlimitChallengeItem item;   // -0x184
                        key.m_a = col1;
                        key.m_b = 2;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[mage]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x188
                        stUnlimitChallengeItem item;   // -0x190
                        key.m_a = col1;
                        key.m_b = 3;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[priest]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x194
                        stUnlimitChallengeItem item;   // -0x19c
                        key.m_a = col1;
                        key.m_b = 4;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[atgunner]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1a0
                        stUnlimitChallengeItem item;   // -0x1a8
                        key.m_a = col1;
                        key.m_b = 5;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
                else if (sub == "[thief]")
                {
                    for (;;)
                    {
                        col1 = ScanInt(&ok);
                        if (!ok)
                            break;
                        col2 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        col3 = ScanInt(&ok);
                        if (!ok)
                            return false;
                        stUnlimitChallengeKey key;     // -0x1ac
                        stUnlimitChallengeItem item;   // -0x1b4
                        key.m_a = col1;
                        key.m_b = 6;
                        item.m_a = col2;
                        item.m_b = col3;
                        out->m_multiMap34.insert(std::make_pair(key, item));
                    }
                }
            }
        }
        else if (token == "[message]")
        {
            for (;;)
            {
                int id = 0;                            // -0xc
                std::string msg1;                      // -0x1b8
                std::string msg2;                      // -0x1bc
                id = ScanInt(&ok);
                if (!ok)
                    break;                             // 0x0890cd6f test ebx → 退出段
                ScanStr(&msg1);
                ScanStr(&msg2);
                stUnlimitChallengeMessageInfo info;   // -0x1c8（C1 0x891c84a）
                info.m_id = static_cast<short>(id);
                info.m_msg1 = msg1;
                info.m_msg2 = msg2;
                out->m_vec28.push_back(info);
            }
        }
    }
    return true;

}
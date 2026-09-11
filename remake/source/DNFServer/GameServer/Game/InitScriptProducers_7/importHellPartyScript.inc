// ============================================================================
// importHellPartyScript — ORIG 0x08909857（init.md +0x0d30，
// "Etc/HellParty.etc" → *Get_instance_stHellParty_t()）。
// 返回 0=成功；错误码段：0x1a24 装载 / 0x1a2e 首.ScanType /
// 0x1a37..0x1aa3 基础值各 Scan / 0x1aba 组下标 / 0x1acd..0x1aed 组名 /
// 0x1aed 未知组名 / 0x1b08 组下标重复 / 0x1b1a..0x1b30 difficulty 段。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N121/N82] ORIG 0x08909857
//  本波逐 jcc/字段偏移核对，修真语义 bug（判据⑮ 多余门禁 / 写入位置 / P0 / P171 /
//  P293）：
//   ①[group] 怪物循环原写 `for (i < slotCount)`，slotCount=out.m_field72[tag] ——
//     ORIG 0x08909f9e..0x08909fee 是**无上界** for(;;)，仅 ScanInt 后 ok 失败
//     break（越界照抄）；我方多读 out.m_field72[tag] 并提前退出 ⇒ 删门禁；
//   ②[group index] 原先判 ok 再写 out.m_groupIndex —— ORIG 0x08909dc2 先
//     `mov %dx,0x50(%eax)` 写字段、0x08909dc6 才判 ok；
//   ③groupName/rowName 的 ScanStr 改 `ok = ScanStr(...); if (!ok)`，复现 ORIG
//     0x08909e2d/0x0890a197 的 `mov %al,-0xd9; movzbl -0xd9` ok 槽往返（P0）；
//   ④补 tag 分支死写入 out.m_field6c[tag]/out.m_field72[tag] → 两个死局部
//     （ORIG 0x08909e6a..0x08909f86，P171 死写入照抄）+ tag 改 uchar（movb）；
//   ⑤m_groupMap.find 改具名 iterator ⇒ 复现 ORIG 先 find(0x0890a011) 后
//     end(0x0890a02c) 的求值序（P293）；
//   ⑥[hellparty script end] 与 [/hellparty basic values]/[/hellparty monster
//     group] 收尾比较移至链末（ORIG 0x0890a2ae / 0x08909d21 / 0x0890a0f5）；
//     difficulty 循环改 `i < rows+1` 复现 ORIG `add $1; cmp; setg`。
//  残余（N 封顶）：4.4 把 11 个 else-if 回边 jmp 集中并各带 1 字节 nop 对齐
//  （N112/N121 块重排，源码不可控）+ 全帧槽号平移（N82/N126）。
//  p167 别名口径 CLEAR（MISSING 全 C2/D2 与 EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；ratio 0.9357、gap=-10（全部为上述 nop/块序）。
int importHellPartyScript(const char* path, stHellParty_t& out)
{
    bool ok = false;                  // ORIG -0xd9：prologue 后即 movb $0
    std::string token;
    std::string sub;

    if (!loadRDARScriptFile("", path))
    {
        return 0x1a24;
    }

    for (;;)
    {
        // ---- 顶层分派（ORIG 外层头：每次取一 token，流结束 → 0x1a2e）----
        if (!ScanType(token, true))
        {
            return 0x1a2e;
        }

        // ================= [hellparty basic values] =================
        if (token == "[hellparty basic values]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1a37;
                }

                if (sub == "[max base value]")
                {
                    out.m_maxBaseValue = ScanInt(&ok);
                    if (out.m_maxBaseValue == 0) { return 0x1a41; }
                    if (!ok) { return 0x1a43; }
                }
                else if (sub == "[min base value]")
                {
                    out.m_minBaseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a4c; }
                }
                else if (sub == "[base value]")
                {
                    out.m_baseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a53; }
                }
                else if (sub == "[sub base rates]")
                {
                    out.m_subBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a5a; }
                }
                else if (sub == "[pass sub base rates]")
                {
                    out.m_passSubBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a61; }
                }
                else if (sub == "[base rising value]")
                {
                    out.m_baseRisingValue = ScanInt(&ok);
                    if (!ok) { return 0x1a68; }
                }
                else if (sub == "[level difference rates]")
                {
                    // 三连读，仅以最后一次 ScanInt 结果判定（ORIG 同栈槽）。
                    out.m_levelDiffRates[1] = ScanFloat(&ok);
                    out.m_levelDiffRates[2] = ScanFloat(&ok);
                    out.m_levelDiffRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a72; }
                }
                else if (sub == "[difficulty rates]")
                {
                    out.m_difficultyRates[0] = ScanFloat(&ok);
                    out.m_difficultyRates[1] = ScanFloat(&ok);
                    out.m_difficultyRates[2] = ScanFloat(&ok);
                    out.m_difficultyRates[3] = ScanFloat(&ok);
                    out.m_difficultyRates[4] = ScanFloat(&ok);
                    if (!ok) { return 0x1a88; }
                }
                else if (sub == "[party rates]")
                {
                    // 首值经 ScanFloat(3,&ok) 以刻度 3 装载（ORIG _Z9ScanFloatiPb）。
                    out.m_partyRates[0] = ScanFloat(3, &ok);
                    out.m_partyRates[1] = ScanFloat(&ok);
                    out.m_partyRates[2] = ScanFloat(&ok);
                    out.m_partyRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a93; }
                }
                else if (sub == "[ancient dungeon add rate]")
                {
                    out.m_ancientDungeonAddRate = ScanFloat(&ok);
                    if (!ok) { return 0x1a9b; }
                }
                else if (sub == "[guide line value]")
                {
                    out.m_guideLineValue = ScanInt(&ok);
                    if (!ok) { return 0x1aa3; }
                }
                else if (sub == "[/hellparty basic values]")
                {
                    break;                          // 回顶层（ORIG 0x08909d21：
                                                     // 收尾比较位于链末）
                }
                // 其余子 token 忽略
            }
            continue;
        }

        // ================ [hellparty monster group] ================
        if (token == "[hellparty monster group]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1ab2;
                }

                if (sub == "[group index]")
                {
                    // ORIG 0x08909db8：ScanInt 结果先存字段（mov %dx,0x50(%eax)），
                    // 之后才检查 ok（无具名 idx 局部，P0）。
                    out.m_groupIndex = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0x1aba;
                    }
                }
                else if (sub == "[group]")
                {
                    // ORIG 0x08909dfc：i=0 的 store 先于 list C1（for 初始化
                    // 前置为块头声明）。
                    int i = 0;
                    stHellPartyMonsterGroupList list;
                    std::string groupName;
                    // ORIG 0x08909e28：返回值先写 ok 槽再回读（P0：原码有具名
                    // ok 局部往返），不是直判调用结果。
                    ok = ScanStr(&groupName);
                    if (!ok)
                    {
                        return 0x1ac9;
                    }

                    // 'A'..'E' 选择难度槽 tag 1..5。ORIG 每分支先读
                    // out.m_field6c[tag] / out.m_field72[tag] 存入紧随 list 之后的
                    // 两个死局部（P171 死写入照抄；读字段无副作用、值不再被读），
                    // 最后写 tag 字节（ORIG movb ⇒ uchar）。
                    unsigned char deadField6c;
                    unsigned short deadField72;
                    unsigned char tag;
                    if (groupName == "A")
                    { deadField6c = out.m_field6c[1]; deadField72 = out.m_field72[1]; tag = 1; }
                    else if (groupName == "B")
                    { deadField6c = out.m_field6c[2]; deadField72 = out.m_field72[2]; tag = 2; }
                    else if (groupName == "C")
                    { deadField6c = out.m_field6c[3]; deadField72 = out.m_field72[3]; tag = 3; }
                    else if (groupName == "D")
                    { deadField6c = out.m_field6c[4]; deadField72 = out.m_field72[4]; tag = 4; }
                    else if (groupName == "E")
                    { deadField6c = out.m_field6c[5]; deadField72 = out.m_field72[5]; tag = 5; }
                    else
                    {
                        return 0x1aed;
                    }
                    (void)deadField6c;
                    (void)deadField72;
                    (void)tag;
                    // ORIG 0x08909f9e..0x08909fee：无上界门禁的 for(;;)，仅以 ok
                    // 失败 break 退出（越界行为照抄，P171）；ScanInt 结果直存数组
                    // （P0：ORIG 无 monsterIdx/rate 中转局部，store 紧跟 call）。
                    for (; ; ++i)
                    {
                        list.m_monsterIdx[i] = ScanInt(&ok);
                        if (!ok) { break; }
                        list.m_rate[i] = static_cast<unsigned char>(ScanInt(&ok));
                        if (!ok) { break; }
                    }

                    // ORIG 0x0890a011 先 call find、0x0890a02c 后 call end
                    //（P293：具名 it 形复现 find 先/end 后）。
                    std::map<unsigned short, stHellPartyMonsterGroupList>::iterator it =
                        out.m_groupMap.find(out.m_groupIndex);
                    if (it != out.m_groupMap.end())
                    {
                        return 0x1b08;              // 组下标重复
                    }
                    // ORIG make_pair<unsigned short&, stHellPartyMonsterGroupList&>
                    // → value_type 转换构造（p167：MISSING make_pair/pair C1 OS_ 变体）
                    out.m_groupMap.insert(std::make_pair(out.m_groupIndex, list));
                }
                else
                {
                    // ORIG 0x0890a0f5：收尾比较位于 [group] 之后。
                    if (sub == "[/hellparty monster group]")
                    {
                        break;                      // 回顶层
                    }
                    return 0x1aed;                  // 未知子 token
                }
            }
            continue;
        }

        // ======================= [difficulty] =======================
        if (token == "[difficulty]")
        {
            const int rows = ScanInt(&ok);
            if (rows > 6)
            {
                return 0x1b1a;
            }
            if (!ok)
            {
                return 0x1b1d;
            }
            // ORIG 0x0890a295：`mov rows; add $1; cmp i; setg` ⇒ 原码 `i < rows+1`。
            for (int i = 1; i < rows + 1; ++i)
            {
                std::string rowName;
                ok = ScanStr(&rowName);
                if (!ok)
                {
                    return 0x1b25;
                }
                // 行名后五值：tag 字节 / 计数 ushort / 三字节列。
                out.m_field6c[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field72[i] = static_cast<unsigned short>(ScanInt(&ok));
                out.m_field7e[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field84[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field8a[i] = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x1b30;
                }
            }
            continue;
        }

        // ORIG 0x0890a2ae：[hellparty script end] 比较位于全部分派之后
        // （basic → monster group → difficulty → script end）。
        if (token == "[hellparty script end]")
        {
            return 0;
        }

        // 其他顶层 token：忽略并继续外层扫描
    }
}
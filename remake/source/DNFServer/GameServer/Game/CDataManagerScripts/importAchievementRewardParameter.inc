// ============================================================================
// advancealtar::RewardParameter::importAchievementRewardParameter
// （ORIG 0x0889afb0 T）[reward data]…[/reward data] 块解析进
// AchievementRewardData 后按 m_type 插入 m_rewardMap。错误码：
// type 扫描 0x362 / type<=0 0x364 / 难度串 0x36a / 难度枚举 0x36d /
// 清空次数 0x373 / 图标 0x37a / 奖励计数 0x391 / 标题 0x399 /
// 邮政标题 0x39e / 正文 0x3a3。[reward item] 三元组仅 {type,_,itemId}
// 进入向量（第三值写入越界临时不入拷贝，见报告注记）。
// ============================================================================
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（48/48）。历轮修正：
// ①`if (token == "[reward data]") { … }` 正向包裹（ORIG 调 operator==）；
// ②P232 `int unkC` ⇒ -0xc 与 ORIG 对齐（帧 0xc0=ORIG）。
// ③[本轮] [stage difficulty] 去具名 `StageDifficulty::T diff`，出参直传
//   `(StageDifficulty::T&)data.m_difficulty`（ORIG 0x0889b1d5 `lea -0xb4; add $0x4`
//   实证）⇒ 消除 2 条 store/load 并使后续局部槽整体回浅 4。
// ④[本轮] [reward item] 段用独立 bool `itemOk`（ORIG 0x0889b339 `mov %al,-0x89`、
//   0x0889b368/0x0889b38e `movzbl -0x89`）⇒ item -0x84 / typeName -0x88 /
//   itemOk -0x89 / value1 -0x80 / value2 -0x7c 与 ORIG 逐槽对齐。
// 残余 gap 12 = ORIG 8 个 continue 中继岛（0x0889b046..b05e 共 8×(nop;jmp)，落点
// 均为内层循环头 0x0889b05f）+ 少量装载序；4.4 对 continue 恒 jump-thread 直跳
// （我方仅 5 岛），属 P164/P223/N82 控制流布局族，源形不可择；ORIG 0x0889afb0。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 386 / ours 374；p167 48/48
// （差集全为 C1/C2·D1/D2 同址别名 P93 伪差）；jcc 30/30、逐位错位 10（岛/块序所致，
// 两侧同序号 jcc 语义指令均存在，无极性反写）⇒ 维持 N。
// [本波 fresh 复核 · 硬证据] orig 373 / our 367（我方少 6 条），ORIG-only
// {xor:2, jmp:4}。ORIG 报告 `## 汇编` 0x0889b044..0x0889b05c 有 **9 条连续 jmp
// 全部指向同一目标 0x0889b05f**（每 3 字节一条，+0x94/+0x97/…/+0xac）＝ 4.1 为
// 每个 `continue` 各发一条独立岛；4.4 恒 jump-thread 合并（P223 边界：同落点
// 多岛不可造，仅「不同落点连续尾标签」可造）。同族 2 条 xor 为 `while(ScanType)`
// 条件形差（P292 表反例：改 for+if(!) 经 regen 件实测劣化 raw 300→301，已回滚）。
// ⇒ 判 N（4.1 多岛 vs 4.4 merge，ORIG 0x0889b044..0x0889b05c 实证）。
// [GW20Run 2026-09-11 复核] fresh 386/374；ORIG-only {xor:2, jmp:4} 与
// 0x0889b046..0x0889b05e 共 8 条 `nop; jmp 0889b05f` 岛逐条核对一致（我方 5 岛）；
// `while(ScanType)` 取反形（xor）差异已由前序泳道 for+if(!) 实测劣化回滚；
// p167 48=48 CLEAR ⇒ 维持 N（P164/P223）。
int advancealtar::RewardParameter::importAchievementRewardParameter(
    const char* path)
{
    std::string token;
    int unkC = 0;       // ORIG 0x0889afc6 `movl $0x0,-0xc(%ebp)`：token 之后的 dead int（P232/P31）
    (void)unkC;
    bool ok = false;
    reset();
    if (!loadRDARScriptFile("", path))
        return 0;

    // [G23-P168a 落地] `while (ScanType(...))` ⇒ 4.4 无 xor（直折）；改
    // `for (;;) { if (!ScanType(...)) break; ... }` 复现 ORIG `call; xor $0x1;
    // test %al,%al; jne` 的取反形（P168a 修法；两处同改）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        // p167 MISSING operator==×1 / EXTRA operator!=×1 ⇒ ORIG 为正向 `==` 包裹。
        if (token == "[reward data]")
        {
            AchievementRewardData data;
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/reward data]")
                {
                    m_rewardMap.insert(std::make_pair(data.m_type, data));
                    break;
                }
                if (token == "[type]")
                {
                    data.m_type = ScanInt(&ok);
                    if (!ok)
                        return 0x362;
                    if (!(data.m_type > 0))
                        return 0x364;
                    continue;
                }
                if (token == "[stage difficulty]")
                {
                    std::string diffName;
                    if (!ScanStr(&diffName))
                        return 0x36a;
                    // ORIG 0x0889b1d5：`lea -0xb4(%ebp),%eax; add $0x4,%eax`
                    // ⇒ 出参直接是 &data.m_difficulty（data+4），无具名 T 局部。
                    if (!StageDifficulty::getDifficulty(
                            diffName,
                            reinterpret_cast<StageDifficulty::T&>(
                                data.m_difficulty)))
                        return 0x36d;
                    continue;
                }
                if (token == "[stage clear count]")
                {
                    data.m_stageClearCount = ScanInt(&ok);
                    if (!ok)
                        return 0x373;
                    continue;
                }
                if (token == "[icon image]")
                {
                    data.m_iconImage = ScanInt(&ok);
                    if (!ok)
                        return 0x37a;
                    continue;
                }
                if (token == "[reward item]")
                {
                    for (;;)
                    {
                        _RewardItem item;
                        std::string typeName;
                        if (!ScanStr(&typeName))
                            break;                       // 静默退出本段
                        // ORIG 0x0889b327：`lea -0x84(%ebp),%eax` 直接传 &item（+0）⇒
                        // 出参直写 m_type，无具名 itemType 中转。
                        // ORIG 0x0889b339 `mov %al,-0x89(%ebp)` + 0x0889b368/0x0889b38e
                        // `movzbl -0x89(%ebp)`：本段用**独立 bool**（-0x89）接收返回并
                        // 复用为两次 ScanInt 的 ok 出参，非外层 ok(-0x71)。
                        bool itemOk = RewardItemType::getRewardItemType(
                            typeName,
                            reinterpret_cast<RewardItemType::T&>(item.m_type));
                        if (!itemOk)
                            break;
                        // [b28d-solo-CDataManagerScripts-2] 真 bug 修正：ORIG 0x0889b360
                        // 第一次 ScanInt 写 item+4（m_value1），0x0889b386 第二次写
                        // item+8（m_value2）——旧实现把第一次写进 m_value2、第二次丢弃，
                        // 导致 m_value1 恒 0 且 m_value2 取错值。
                        item.m_value1 = ScanInt(&itemOk);    // +4
                        if (!itemOk)
                            break;
                        item.m_value2 = ScanInt(&itemOk);    // +8
                        if (!itemOk)
                            return 0x391;
                        data.m_rewardItems.push_back(item);
                    }
                    continue;
                }
                if (token == "[reward title]")
                {
                    if (!ScanStr(&data.m_rewardTitle))
                        return 0x399;
                    continue;
                }
                if (token == "[postal title]")
                {
                    if (!ScanStr(&data.m_postalTitle))
                        return 0x39e;
                    continue;
                }
                if (token == "[postal letter]")
                {
                    if (!ScanStr(&data.m_postalLetter))
                        return 0x3a3;
                    continue;
                }
            }
        }
    }
    return 0;
}
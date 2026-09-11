// GiveTrainingSeal ORIG T 0x85652c8 报告 GiveTrainingSeal.md
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 226→3，274→276 指令；全部槽位
// 与 ORIG 对齐）。已收：删具名 bool ok 改双短路（ORIG 0x0856564e..0x08565675
// `jle`/`jbe` + mov$1/mov$0 汇聚）；bonusRemain bool→char（S1 一次性旋钮，
// P321-T3 消费端 `cmpb $0x0,-0x11; je` 无 setcc ⇒ bool/char 直证不足，char 复现
// 4.1 内存直比；bool 恒 xor 物化 N122）；P222 正向包裹翻块序（ORIG bonusRemain==0
// 块在尾部、win 链 fall-through）；P0 内联 `getCurCharacInvenW()`（ORIG 无 inven
// 槽，结果直留 %eax 跨实参装载）；vector 声明序 updatedSlots 前置（ORIG 构造序
// -0x30→-0x3c、push_back this=-0x3c）；P183 去 sps/score 缓存、P3 `cmp $1,%al`、
// dailyLimit 表达式内联、P135 push_back(O&&)。残 3 = N121 epilogue 尾岛
// （我方多 `jmp+nop`）。p167 49=49 CLEAR。
// [GW20Run 2026-09-11 复核] fresh 274/276；OURS-only 2 条 = EH cleanup 尾
// `jmp <出口>; nop`（ORIG 0x085656f5 `jmp $+2` 直落 + 0x085656ff nop）⇒ N91/N104；
// p167 49=49 全为 C1/C2·D1/D2 别名、enc_kr_scan 0 ⇒ 维持。
void CFairMatch::GiveTrainingSeal(CUser* user, bool win)
{
    if (user->getCurCharacR() == 0)
        return;

    // S1 一次性旋钮（P321-T3 直证不足：ORIG 消费端 `cmpb $0x0,-0x11; je`
    // 无 setcc，bool/char 均可）——试 char 复现 4.1 内存直比（bool 恒 xor 物化）。
    char bonusRemain = 1;
    // b28d-solo-CFairMatchRooms：P183 去缓存铁律。ORIG（p167 实证）每处 SPS 成员
    // 访问都重发 G_CDataManager()（13 次）、每处 score 访问都重发
    // GetCharacExpandData(7)（10 次）；我方缓存 sps 引用/score 指针只发 1/1 次。
    // 且 win 三目必须展开为两条独立路径（ORIG 各路径各一条 call：
    // SetLastTrainingSealBonusCount ORIG 6 条 vs 我方 4 条）。
    // b28d-solo-CFairMatchRooms：ORIG `movzwl 0x49e0(%eax),%eax; movzwl %ax,%ebx`
    // 值留 callee-saved %ebx 跨 GetCharacExpandData/GetDailyPlayCount 保活、无栈槽
    // ⇒ dailyLimit 以表达式内联进 remain（P0：无槽往返即无具名局部）。
    const int remain = static_cast<int>(FM_SPS().m_schoolMatchDailyPlayLimit -
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->GetDailyPlayCount());
    if (remain <= 0) {
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(0);
        bonusRemain = 0;
    } else {
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(remain);
    }

    int sealTotal = 0;
    // b28d-solo-CFairMatchRooms：P3 —— ORIG `cmp $0x1,%al`（单字节比较）⇒
    // 调用点收窄。
    if (static_cast<unsigned char>(GetPlayCountPerOneTeam()) == 1) {
        if (win)
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[0]);
        else
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[1]);
        // ORIG 块序：`cmpb $0,-0x11; je <尾部>`（bonusRemain==0 块在尾）、win
        // 链 fall-through ⇒ P222 正向包裹。
        if (bonusRemain) {
            if (win) {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[2]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[2]));
            } else {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[3]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[3]));
            }
        } else {
            reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                ->SetLastTrainingSealBonusCount(0);
        }
    } else {
        if (win)
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[4]);
        else
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[5]);
        if (bonusRemain) {
            if (win) {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[6]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[6]));
            } else {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[7]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[7]));
            }
        } else {
            reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                ->SetLastTrainingSealBonusCount(0);
        }
    }
    reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
        static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
        ->SetLastTrainingSealCount(sealTotal);
    if (sealTotal <= 0)
        return;

    // ORIG 声明序实证：updatedSlots 先构造（lea -0x30 @0x085655bd）、giveList
    // 后（lea -0x3c @0x085655c8），且 push_back this=-0x3c ⇒ giveList 后声明。
    std::vector<int> updatedSlots;
    std::vector<std::pair<int,int> > giveList;
    // b28d-solo-CFairMatchRooms：P135——ORIG 选 push_back(O&&) 右值重载，
    // 具名左值 entry 会选 push_back(RKS1_)（p167 EXTRA 实证）。
    giveList.push_back(std::pair<int,int>(static_cast<int>(0xcf0), sealTotal));
    // P0：ORIG 无 inven 具名槽（`call getCurCharacInvenW` 结果直留 %eax 跨
    // 实参装载至 insert_event_items）⇒ 表达式内联。
    const int insertResult = user->getCurCharacInvenW()->insert_event_items(
        giveList, updatedSlots,
        static_cast<eMoneyAddReason>(3), static_cast<eItemAddReason>(8),
        true, "game_server_msg_171", "game_server_msg_172");
    // ORIG 0x0856564e..0x08565675：`jle`/`jbe` 双短路 + `mov $1/mov $0` 汇聚
    // （无 bool 具名局部物化）⇒ 直接短路 if。
    // [b28d-PoolMisc2 复验（属主 PoolSmall，读数以交接）] 本件尾岛两形实测均劣：
    // ①尾部加显式 `return;` ⇒ ndiff 1→1 零效（2d3d/2d40 双 jmp 到同目标不变）；
    // ②`A && B` 拆嵌套正向 `if(A){if(B){}}` ⇒ ndiff 1→**6**（0x08565667..0x0856566e
    // 的 `jbe/mov $1/jmp/mov $0` 汇聚岛被折成单条 `seta %al`，反坏前轮短路形）。
    // ⇒ 维持短路 `&&` 原形；残 1 条 epilogue 双岛 jmp 按 P223 定案③ + N121 封顶。
    if (insertResult >= 1 && updatedSlots.size() >= 3u) {
        user->SendUpdateItem(CUser::eSendTarget_1,
                             static_cast<ENUM_ITEMSPACE>(0),
                             updatedSlots[0]);
    }
}
void write_log_gain_money(eMoneyAddReason reason, int amount, CUser* user)
{
    // ORIG 0x84febd9：if (amount < 1) { if (amount < 0) logFormat } else { AddGold + switch }。
    // switch 体布局序 = ORIG 块序（jump table 实证）：case 4,0,3,0xf,0xa,0x1a。
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xd71,
                "write_log_gain_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, user->get_charac_level());
        }
        // b28d-37：ORIG 0x84febea `jns 0x84fed94(nop 岛)` + log 分支 `jmp 0x84fed95`
        // ⇒ 内层 if 失败与 log 分支共享函数尾出口（P223 空岛形），改共享 return。
        return;
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->AddGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)2, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 3:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)3, user, (unsigned int)amount);
            user->AddMoneyPlus(amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)6, user, (unsigned int)amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)7, user, (unsigned int)amount);
            break;
        case 0x1a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0xe, user, (unsigned int)amount);
            // [b28c-l23 N 判词·4.1 break 跳板 vs 4.4 jump threading]
            // ORIG @0x84fed94 最后一个 case 的 call 之后仍发 `jmp <统一出口>`
            // （4.1 即便出口即下一条指令也保留 break 跳板），4.4 恒做 jump threading
            // 省掉该 jmp ⇒ our 111 vs orig 112，其后果连锁 8 处 jmp 目标偏移差。
            // 实测复形失败：把 `default: break;` 改为显式 `goto L_SWITCH_EXIT;` +
            // 块外空标签后 4.4 仍折叠（ndiff 9→9，零改善），已回退为自然 break 形。
            // 语义完全等价（末 case 落函数尾）。
            // [b28d-37 复核] 函数头 `if(amount<1){if(amount<0)log;}return;` 改共享
            // return 后已复现 ORIG 0x84fed94 nop 岛（our 111→113=ORIG 113）；仅剩
            // 末 case break 跳板 1 条 jmp 差（4.1 保留跳板 / 4.4 jump-threading）⇒
            // goto/空标签形仍不可翻，N 封顶（P223 边界：我方多出岛不可消，缺岛可补）。
        default:
            break;
        }
    }
}
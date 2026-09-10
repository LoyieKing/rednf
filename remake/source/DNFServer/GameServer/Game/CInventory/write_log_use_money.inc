void write_log_use_money(eMoneySubReason reason, int amount, CUser* user)
{
    // ORIG 0x84fed9b：先无条件取 charac_level 存局部，再 if(amount<1){if(<0)log}else{...}。
    // switch 体布局序 = ORIG 块序（jump table 实证）。
    // b28d-37 [N 封顶]：共享 return 后复现 ORIG nop 岛（our 302→304=ORIG 304）；
    // 仅剩末 case break 跳板 1 条 jmp（4.1 保留 / 4.4 jump-threading，goto+空标签形
    // 实测不可翻，同 write_log_gain_money）。
    int level = user->get_charac_level();
    if (amount < 1)
    {
        if (amount < 0)
        {
            LogManager::logFormat(1, "inventory.cpp", __PRETTY_FUNCTION__, 0xde9,
                "write_log_use_money() reason(%d), gold(%d), gold(%d)",
                reason, amount, level);
        }
        // b28d-37：同 write_log_gain_money——ORIG 0x84fee5? `jns <nop 岛>` 与 log
        // 分支共享函数尾出口（P223），改共享 return 复现 nop 岛。
        return;
    }
    else
    {
        if (user != 0)
        {
            user->GetGoldControl()->SubGold((unsigned int)amount, reason);
        }
        switch ((int)reason)
        {
        case 0:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x12, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0xa:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x13, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 1:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x14, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x14:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x16, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 6:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x18, user, (unsigned int)amount);
            break;
        case 0xd:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x19, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 4:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1a, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x10:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1b, user, (unsigned int)amount);
            break;
        case 0xb:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1e, user, (unsigned int)amount);
            break;
        case 0x13:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x1f, user, (unsigned int)amount);
            break;
        case 0x16:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x20, user, (unsigned int)amount);
            break;
        case 0xc:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x21, user, (unsigned int)amount);
            break;
        case 0xf:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x23, user, (unsigned int)amount);
            break;
        case 0x15:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x24, user, (unsigned int)amount);
            break;
        case 0x22:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x25, user, (unsigned int)amount);
            break;
        case 0x21:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x27, user, (unsigned int)amount);
            break;
        case 0x24:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x28, user, (unsigned int)amount);
            break;
        case 0x2a:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x29, user, (unsigned int)amount);
            break;
        case 0x2b:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2b, user, (unsigned int)amount);
            GetInstanceValueStatistic()->AddValueStatistic(
                (VALUE_STATISTIC_FIELD)0x1d, user, (unsigned int)amount);
            break;
        case 0x2c:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2c, user, (unsigned int)amount);
            break;
        case 0x2d:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2d, user, (unsigned int)amount);
            break;
        case 0x2e:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2e, user, (unsigned int)amount);
            user->AddMoneyMinus(amount);
            break;
        case 0x2f:
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x2f, user, (unsigned int)amount);
            // ORIG switch 末尾带 default: break（产生 jmp+nop 尾部形态）
        default:
            break;
        }
    }

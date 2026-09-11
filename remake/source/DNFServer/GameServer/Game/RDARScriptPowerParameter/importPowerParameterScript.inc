// ============================================================================
// importPowerParameterScript —— ORIG 0x08a592bc
// 报告 _lt_global_gt_/importPowerParameterScript.md；调用点 init.md +0x1dc8：
//   importPowerParameterScript(this+0xa628, "Etc/Power.etc")。
// 返回 bool：仅当读到 [power parameter end] 为 true；加载失败 / ScanType 失败 /
// [power war channel] 第二值缺失 返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 638/629、P167 CLEAR(101/101)；残差 =
// 4.1/4.4 break 块序（N122/P168a）+ nop 岛（详见文件头判词表）。
// [2026-09-10 D06 · N] 残差 = 我方多 1 条**不可达中继** `jmp <while_head>`（位于
//   `jmp <return done>` 之后的死区，ORIG 该处为 `nop`）⇒ 4.4 的循环出口中继
//   （A28 边界）；实测 `while(true)`↔`for(;;)` 同值（631 vs 630），`break`↔`goto
//   scan_end` 同值 ⇒ 源码层不可控，封顶。
bool importPowerParameterScript(PowerParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08a592bc：`mov 0x8(%ebp),%eax; mov %eax,-0x14(%ebp)` —— 形参缓存
    // 进具名局部 p（P0），全部访问走 p->。
    PowerParameterScript* p = script;
    p->clear();

    std::string token;
    int dead10 = 0;                // ORIG -0x10 `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[power join cost]")                      { p->m_powerJoinCost = ScanInt((bool*)0); continue; }
        if (token == "[power join guild level]")               { p->m_powerJoinGuildLevel = ScanInt((bool*)0); continue; }
        if (token == "[power join quest]")                     { p->m_powerJoinQuest = ScanInt((bool*)0); continue; }
        if (token == "[kill point to end power war]")          { p->m_killPointToEndPowerWar = ScanInt((bool*)0); continue; }
        if (token == "[exp reward]")                           { p->m_expReward = ScanInt((bool*)0); continue; }
        if (token == "[easy assault range]")                   { p->m_easyAssaultRange = ScanInt((bool*)0); continue; }
        if (token == "[ghost revival base time]")              { p->m_ghostTime = ScanInt((bool*)0); continue; }
        if (token == "[respon penalty time]")                  { p->m_penaltyThreshold = ScanInt((bool*)0); continue; }
        if (token == "[powerwar point multiple number]")       { p->m_powerWarPointMultiple = ScanInt((bool*)0); continue; }
        if (token == "[refresh power war process info]")       { p->m_refreshProcessInfo = ScanInt((bool*)0); continue; }
        if (token == "[abuser ban time]")                      { p->m_abuserBanTime = ScanInt((bool*)0); continue; }
        if (token == "[power up ratio]")                       { p->m_powerUpRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up assault count]")               { p->m_powerUpAssaultCount = ScanInt((bool*)0); continue; }
        if (token == "[power up assault victory ratio]")       { p->m_powerUpAssaultVictoryRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up generate period]")             { p->m_powerUpGeneratePeriod = ScanInt((bool*)0); continue; }
        // ORIG 0x08a595db：`movl $0,-0x28` 预清 + cond 前置
        // （`call ScanInt; movzbl done; xor $1; je break`）+ body 后置。
        if (token == "[power up noti time]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vec20.push_back(value);
            }
            continue;
        }
        if (token == "[straight victories]")                   { p->m_straightVictories = ScanInt((bool*)0); continue; }
        if (token == "[item initial cooltime]")                { p->m_itemInitialCooltime = ScanInt((bool*)0); continue; }
        // asm +0x3e6..+0x434：首值带 ok 校验，次值不校验。
        if (token == "[kill count effect range]")
        {
            while (true)
            {
                std::pair<int, int> range;
                range.first = ScanInt(&done);
                if (!done)
                    break;
                range.second = ScanInt((bool*)0);
                p->m_vec70.push_back(range);
            }
            continue;
        }
        if (token == "[basic powerwar point]")                 { p->m_basicPowerWarPoint = ScanInt((bool*)0); continue; }
        if (token == "[first grade bonus powerwar point]")     { p->m_firstGradeBonusPoint = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus grade]")                  { p->m_rankingBonusGrade = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus decrease point]")         { p->m_rankingBonusDecreasePoint = ScanInt((bool*)0); continue; }
        if (token == "[decrease powerwar point per day]")      { p->m_decreasePointPerDay = ScanInt((bool*)0); continue; }
        if (token == "[required user powerwar point 4 dungeon]") { p->m_requiredUserPoint4Dungeon = ScanInt((bool*)0); continue; }
        if (token == "[power war dungeon info]")
        {
            p->m_powerWarDungeonInfo.first = ScanInt((bool*)0);   // ORIG +0x94
            p->m_powerWarDungeonInfo.second = ScanInt((bool*)0); // ORIG +0x98
            continue;
        }
        if (token == "[power up bonus]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vec38.push_back(value);
            }
            continue;
        }
        if (token == "[pc room bonus]")
        {
            // asm +0x60a..+0x628：`movl $0,-0xc` 预清 + 具名局部中转，
            // ok 标志被忽略。
            int value = 0;
            value = ScanInt(&done);
            p->m_pcRoomBonus = value;
            continue;
        }
        if (token == "[time assault super power state on join powerwar]")
        {
            p->m_timeAssaultSuperPowerStateOnJoin = ScanInt((bool*)0);
            continue;
        }
        if (token == "[time assault super power state after powerwar]")
        {
            p->m_timeAssaultSuperPowerStateAfter = ScanInt((bool*)0);
            continue;
        }
        if (token == "[powerwar enter level limit]")           { p->m_minEnterLevel = ScanInt((bool*)0); continue; }
        if (token == "[powerwar enter age limit]")             { p->m_minEnterAge = ScanInt((bool*)0); continue; }
        if (token == "[power war channel]")
        {
            while (true)
            {
                int channel;
                if (!(channel = ScanInt(&done), done))
                    break;
                int serverGroup = ScanInt(&done);
                if (!done)
                    return false;                    // asm +0x739：第二值缺失即整体失败
                p->m_powerWarServerList.push_back(
                    std::make_pair(channel, serverGroup));
            }
            continue;
        }
        if (token == "[power up point mod]")                   { p->m_powerUpPointMod = ScanInt((bool*)0); continue; }
        if (token == "[power join cost modify]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecAC.push_back(value);
            }
            continue;
        }
        if (token == "[victorious reward time]")               { p->m_victoriousRewardTime = ScanInt((bool*)0); continue; }
        if (token == "[straight victory penalty]")
        {
            stVictoryPenalty penalty;
            while (true)
            {
                penalty.m_field0 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_field4 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_field8 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_fieldC = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecBC.push_back(penalty);
            }
            continue;
        }
        if (token == "[straight victory break bonus]")
        {
            int value;                             // ORIG 无预清（直接 store）
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecC8.push_back(value);
            }
            continue;
        }
        if (token == "[power rejoin panalty]")                 { p->m_powerWarCooldownMinutes = ScanInt((bool*)0); continue; }
        if (token == "[power parameter end]")
        {
            done = true;
            // [2026-09-10 D06] ORIG 该处只有**一条** `jmp <return done>`；`break`
            // 跳出 while 会多一条（4.4 的 while 出口 + epilogue 跳板），改单 goto。
            goto scan_end;
        }
    }

scan_end:
    return done;
}
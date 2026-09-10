int CLeagueMatch::addBadge(PlayerInfo* info, ENUM_BADGE_TYPE badgeType,
                           unsigned char* msgFlag, bool extra)   // ORIG T 0x0855e7ee
{
    // [b23S3C] 直返形（ORIG 各 guard return-0 body fall-through）；flag 判定
    // 为 `== 1`（ORIG xor$1：flag∉{0,1} 值域下 ==1 才过，`!flag` 语义差）；
    // 红蓝合计去 sum 具名局部（ORIG 无槽：movswl/cwtl/lea 直比 cmp $9,%eax）；
    // count 双句式 `int count=0; if(extra)++count`（ORIG `movl $0; cmpb; addl$1`
    // 分支岛非三目物化）；三分支各复制完整 findString+snprintf call 序列
    // （ORIG 三独立岛，非选串指针汇聚）；值表达式 (int)*msgFlag+count 右→左
    // 求值先于 findString（ORIG ebx 先算）。
    // N96 清绑：字段表达式直读（P0）；xor$1 单段形复测。
    if ((info->m_flag ^ 1) != 0)
        return 0;
    if (info->m_user == 0)
        return 0;
    if (info->m_user->getCurCharacR() == 0)
        return 0;

    if (badgeType == 0) {
        // ORIG `mov 0x10(%ebp),%edx` 传形参变量本身（非 $0 立即数）。
        if (info->m_user->GetDailyBadge(badgeType) > 9)
            return 0;
    } else {
        if (info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(1)) +
            info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(2)) > 9)
            return 0;
    }

    info->m_user->AddDailyBadge(badgeType);
    *msgFlag = 1;
    // ORIG：count 初 0，仅 extra 加 1（不读 *msgFlag）。
    int count = 0;
    if (extra)
        ++count;

    char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
    // [b28d-solo-CLeagueMatch] 真语义缺口修正：ENUM_BADGE_TYPE 仅定义值 0，
    // `switch(badgeType)` 在 4.4 被 -fstrict-enums 判值域 {0} 而**删除 case 2**
    // （.o 仅 2×findString/snprintf，ORIG 3×；P167 差集 MISSING 各 1）。
    // 改 `switch((int)badgeType)`（P305 的 S18 规避形）：值域未知 ⇒ 三分支
    // 全保留，且 4.4 产 ORIG 0x0855e90f/914/919 的「形参载入 %eax + 寄存器
    // 比较链」`mov 0x10(%ebp),%eax; cmp $1;je / cmp $2;je / test;je / jmp`
    // （P251/P286；if 链恒折 `cmpl $K,mem` 每段重读内存，探针 /tmp/b28d_cmatch/
    // probe4.cpp sw3 vs if3 实证）。
    switch (static_cast<int>(badgeType)) {
    case 1:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_red_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 2:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_blue_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 0:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_green_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    }

    PacketGuard pkt;
    pkt.put_header(0, 0xc);
    pkt.put_byte(0);
    pkt.put_short(0);
    pkt.put_byte(0);
    pkt.put_int((int)strlen(textBuf));
    pkt.put_str(textBuf, (int)strlen(textBuf));
    pkt.finalize(true);
    info->m_user->Send(pkt);
    return 1;

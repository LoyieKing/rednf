bool DB_InsertAutoPunishSecondUser::dispatch(int /*userIdx*/, int /*slot*/,
                                             Stream* stream)
{
    MySQL* db6 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                   E_SERVER_GROUP_NONE);
    // 声明序 db6→sig 与 ORIG 一致（ORIG 先 GetDBHandle(6) 后 GetOutBuffer，
    // db6=-0x24/sig=-0x20）；db1 只在 punish_flag 分支内获取（ORIG 0x0842e155）。
    DB_InsertAutoPunishSecondUserView* sig =
        reinterpret_cast<DB_InsertAutoPunishSecondUserView*>(
            stream->GetOutBuffer<SIG_INSERT_AUTO_PUNISH_SECOND_USER>());
    // ORIG 实参序（汇编 0x8..0x1c）：sig+8, sig+0xc, sig+8, sig+0xc,
    // punish_flag, m_id —— 重复传递是 ORIG 原样（多余 vararg 被忽略）。
    db6->set_query(
        "upDate auto_punish_second_user set occ_time=now(),total_trade_cnt=total_trade_cnt+1,trade_cnt=trade_cnt+1,total_trade_gold=total_trade_gold+%d,trade_gold=trade_gold+%d,punish_flag=%d where m_id=%s",
        sig->m_totalTradeGold, sig->m_tradeGold, sig->m_totalTradeGold,
        sig->m_tradeGold, sig->m_punishFlag ? 1 : 0,
        NumberToString(sig->m_memberId, 0));
    // P0：ORIG 无 needInsert 槽（`xor $1; test %al,%al; jne` 直判），具名
    // bool 局部使 4.4 产 `mov %al,-0x1b; cmpb $0x0,-0x1b` 槽往返。
    if (!(db6->exec(true) == 1 && db6->getAffectedRowCount() != 0))
    {
        db6->set_query(
            "inSert into auto_punish_second_user(m_id,occ_time,total_trade_cnt,trade_cnt,total_trade_gold,trade_gold,punish_flag) values(%s,now(),1,1,%d,%d,%d)",
            NumberToString(sig->m_memberId, 0), sig->m_totalTradeGold,
            sig->m_tradeGold, sig->m_totalTradeGold, sig->m_tradeGold,
            sig->m_punishFlag ? 1 : 0);
        db6->exec(true);
    }
    db6->set_query(
        "upDate auto_punish_second_log set occ_time=now(),trade_cnt=trade_cnt+1,trade_gold=trade_gold+%d where m_id=%s and hack_m_id=%s",
        sig->m_totalTradeGold, sig->m_tradeGold,
        NumberToString(sig->m_memberId, 0),
        NumberToString(sig->m_hackMemberId, 1));
    if (!(db6->exec(true) == 1 && db6->getAffectedRowCount() != 0))
    {
        db6->set_query(
            "inSert into auto_punish_second_log(m_id,hack_m_id,occ_time,trade_cnt,trade_gold) values(%s,%s,now(),1,%d)",
            NumberToString(sig->m_memberId, 0),
            NumberToString(sig->m_hackMemberId, 1), sig->m_totalTradeGold,
            sig->m_tradeGold);
        db6->exec(true);
    }
    if (sig->m_punishFlag)
    {
        MySQL* db1 = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                       E_SERVER_GROUP_NONE);
        db1->set_query(
            "upDate member_punish_info set occ_time=now(), start_time=now(), end_time='9999-12-31 23:59:59', apply_flag=%d where m_id=%d and punish_type=%d",
            sig->m_punishFlag ? 2 : 0, sig->m_memberId, 4);
        if (!(db1->exec(true) == 1 && db1->getAffectedRowCount() != 0))
        {
            db1->set_query(
                "inSert into member_punish_info(m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag) values(%s, %d, now(), now(), '9999-12-31 23:59:59', 0, %d)",
                NumberToString(sig->m_memberId, 0), 4,
                sig->m_punishFlag ? 2 : 0);
            if (db1->exec(true) != 1)
                return 0;
        }
        // ORIG 0x0842e250 mov $0xff,%ebx + 起始槽 -0x123 非 4 对齐 ⇒ 0xff 字节
        // 数组（P312/P253；旧 0x100 使 4.4 折简单 rep stos，缺 21 条慢路）
        char reason[0xff] = { '\0' };
        sprintf(reason, "{Auto 2nd Information-Mail}|*|ARS 2nd Punish (Trade) / %s / %d",
                NumberToString(sig->m_hackMemberId, 0),
                sig->m_totalTradeGold, sig->m_tradeGold);
        db1->set_query(
            "inSert into member_punish_info_history(m_id, punish_type, occ_time, punish_value, start_time, end_time, admin_id, apply_flag, reason) values(%s, 4, now(), 6, now(), '9999-12-31 23:59:59', 'ARS', 2, '%s')",
            NumberToString(sig->m_memberId, 0), reason);
        if (db1->exec(true) != 1)
            return 0;
    }
    return 1;

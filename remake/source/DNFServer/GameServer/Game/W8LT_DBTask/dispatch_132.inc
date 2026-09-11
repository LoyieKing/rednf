// [b28d-4] DIFF：N82 槽号错位/多局部残差（4.1 不做 lifetime 槽复用）；ORIG 084335dc
bool DB_LoadAutoMarketConditionsControl::dispatch(int userIdx, int slot,
                                                  Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    bool flag = false;
    *stream >> flag;
    // [H4 2026-09-11 B 组取证] ORIG 0x0843361d `movl $0x0,-0x14(%ebp)`（nRows=0）
    // 位于 0x08433624 `memset(&sig,0,0x6c)` **之前** ⇒ nRows 声明须前置到 memset 前。
    int nRows = 0;
    DB_AutoMarketConditionView sig;
    memset(&sig, 0, 0x6c);
    db->set_query(
        "seLect optimum_gold_supply, over_gold from auto_market_condition_ctrl limit 1");
    if (db->exec(true) != 1)
        return 0;
    nRows = db->get_n_rows();
    if (nRows != 0)
    {
        if (db->fetch() != 1)
            return 0;
        if (db->get_ulonglong(0, sig.m_optimumGold) != 1)
            return 0;
        if (db->get_ulonglong(1, sig.m_overGold) != 1)
            return 0;
    }
    db->set_query(
        "seLect total_gold, auction_gold, optimum_gold_supply, over_gold, gold_phase, item_phase, durability_phase from auto_market_condition_ctrl_daily where occ_time >= DATE_SUB(CURDATE(), INTERVAL 1 DAY) order by occ_time limit 2");
    if (db->exec(true) == 1)
    {
        nRows = db->get_n_rows();
        if (nRows == 2 || nRows == 1)
        {
            for (int i = 0; i < nRows && i < 2; ++i)
            {
                if (db->fetch() != 1)
                    return 0;
                if (db->get_ulonglong(0, sig.m_rows[i].m_totalGold) != 1)
                    return 0;
                if (db->get_ulonglong(1, sig.m_rows[i].m_auctionGold) != 1)
                    return 0;
                if (db->get_ulonglong(2, sig.m_rows[i].m_optimumGold) != 1)
                    return 0;
                if (db->get_ulonglong(3, sig.m_rows[i].m_overGold) != 1)
                    return 0;
                if (db->get_int(4, sig.m_rows[i].m_goldPhase) != 1)
                    return 0;
                if (db->get_int(5, sig.m_rows[i].m_itemPhase) != 1)
                    return 0;
                if (db->get_int(6, sig.m_rows[i].m_durabilityPhase) != 1)
                    return 0;
            }
            if (nRows == 1)
                sig.m_rows[1] = sig.m_rows[0];
            Stream* out = GlobalData::s_stream_pool->Acquire("DBThread.cpp",
                                                             0x7c30);
            CStreamGuard guard(out, true);
            guard.operator*()->operator<<((int)0x15a);
            guard.operator*()->operator<<((int)-1);
            SIG_AUTO_MARKET_CONDITIONS_CONTROL* outSig =
                guard.operator->()->GetInBuffer<SIG_AUTO_MARKET_CONDITIONS_CONTROL>();
            memcpy(outSig, &sig, 0x6c);
            // [Lane31W8LTDBTaskView] 权威头具名视角直写（去 cast；ORIG 写 +0x68）。
            // [H4 2026-09-11 B 组取证] ORIG 0x08433a7a `movzbl -0x29(%ebp),%edx;
            // mov -0x10(%ebp),%eax; mov %dl,0x68(%eax)` ⇒ flag **直存**（无三元归一）。
            outSig->m_view.m_flag = flag;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return 1;
        }
        else
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x7c0c, 0);
            trace("DB_LoadAutoMarketConditionsControl error %d", nRows);
        }
    }
    return 0;
}
bool CCharacBloodDungeon::_saveData(CUser* pUser)  // ORIG 0x832a330
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CharacBloodDungeon.cpp", 0x1e),
                       true);
    guard.operator*()->operator<<(0x197);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<((int)pUser->GetUID());

    SIG_BLOOD_DUNGEON* sig = guard.operator->()->GetInBuffer<SIG_BLOOD_DUNGEON>();
    memset(sig, 0, sizeof(SIG_BLOOD_DUNGEON));   // ORIG memset 0x28
    // [P211 档③ 试形记录（已回滚）] 删 `(int)` cast 试同型化（ORIG 0x0832a3e7 为
    //   LHS-first 直存）实测 DIFF 数不变（3 件）⇒ 判 N18b/装载序族，保持直证形。
    sig->m_characNo = (int)pUser->getCurCharacNo();
    sig->m_point = m_point;                      // this+0x08 → sig+0x04
    sig->m_weeklyEnterCount = m_weeklyEnterCount;
    sig->m_useGold = m_useGold;
    sig->m_dailyInitTime = getDailyInitializeTime(6, 0, 0);  // 覆写 +0x10
    // ORIG 跳过 this+0x14：sig+0x14←this+0x18、sig+0x18←this+0x1c（与 loadData
    // 镜像一致；原写 this+0x14/0x18 系真错位 bug）。
    sig->m_field14 = m_enterCount;
    sig->m_enterCount = m_field1c;
    sig->m_flag1c = m_flag20;                    // byte 镜像
    sig->m_field20 = m_field24;
    sig->m_field24 = m_field28;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_flag20 = 0;                                // ORIG 发送后清标志
    return true;                                 // 返回 1

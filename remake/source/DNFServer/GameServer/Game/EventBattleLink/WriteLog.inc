// ---------------------------------------------------------------------------
// §6 CInGameAdvertisementManager::WriteLog（0x080fda56）：
//   state==5 → party+0xb24 战场副本 idx；stream 0x32c + UID；
//   GetInBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG> 填 0x0..0x1f；LOGDB_Q(4) 入队。
// ---------------------------------------------------------------------------
void CInGameAdvertisementManager::WriteLog(CUser* user, char type, short val,
                                           char flag, unsigned int extra)
{
    if (user == 0)
    {
        return;
    }

    // 局部声明序照 ORIG 栈槽（+0x025..+0x0c5）：3 个恒 -1 槽居低地址，
    // sig/party/dungeon 居中，state==3 的 vill/area 居顶（从不写 SIG）。
    int dungeonIdx = -1;                       // ORIG -0x28（SIG +0x14）
    int field18 = -1;                          // ORIG -0x24（SIG +0x18 恒 0xffffffff）
    int field1c = -1;                          // ORIG -0x20（SIG +0x1c 恒 0xffffffff）
    SIG_IN_GAME_ADVERTISEMENT_LOG* sig;        // ORIG -0x1c
    CParty* party;                             // ORIG -0x18
    CDungeon* dungeon;                         // ORIG -0x14（GetDungeon 结果单槽）
    int village;                               // ORIG -0x10（state==3 计算，不写 SIG）
    int area;                                  // ORIG -0xc（state==3 计算，不写 SIG）

    if (user->get_state() == 5)
    {
        party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            dungeon = party->m_battleField.GetDungeon();
            if (dungeon != 0)
            {
                dungeonIdx = dungeon->get_index();
            }
        }
    }
    else if (user->get_state() == 3)
    {
        // ORIG +0x0a1..+0xc5：getCurCharacVill()/get_area(false) 计算结果仅存
        // 局部，从不写入 SIG（+0x18/+0x1c 恒 0xffffffff，忠实保留）。
        village = user->getCurCharacVill();
        area = user->get_area(false);
    }

    // Acquire 结果直接作 guard 实参（ORIG +0x0e0..+0x0f7 无中间槽）；
    // operator<< 走 operator*() 链（P132：ORIG call 0x080c8c46）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InGameAdvertisementManager.cpp", 0x17a), true);
    (*guard)->operator<<(0x32c);
    (*guard)->operator<<(static_cast<int>(user->GetUID()));

    sig = guard->GetInBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>();
    sig->m_accId = user->get_acc_id();                       // +0x00
    sig->m_characNo = user->get_charac_no(-1);               // +0x04
    sig->m_pad1c = field1c;                                  // +0x1c（写序照 ORIG）
    sig->m_adType = val;                                     // +0x0a
    sig->m_type = type;                                      // +0x08（char→uchar 隐式转换）
    sig->m_flag = flag;                                      // +0x0c
    sig->m_extra = extra;                                    // +0x10
    sig->m_dungeonIdx = dungeonIdx;                          // +0x14
    sig->m_village = field18;                                // +0x18（恒 -1，ORIG 写 local_28）

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}
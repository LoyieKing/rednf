void CUser::SendAradEventItemLog(unsigned long itemIndex, int count)
{
    ARAD::Arad_DataManager* mgr = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
    ARAD::Arad_InterfaceGameScript* script =
        mgr->findGameScript("Arad_StatisticsEventItemList");
    // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x08692bc5 `mov 0xc(%ebp),%edx;
    //   mov %edx,0x4(%esp); mov %eax,(%esp); call needStatistics` —— %eax（findGameScript
    //   结果）**直接**当 this，无 `test %eax,%eax / je` 判空；随后 0x08692bd6
    //   `mov %eax,%ebx; xor $0x1,%ebx`（!needStatistics 物化）+ 0x08692c21
    //   `test %bl,%bl; jne 0x08692d22`（跳尾声返回）⇒ 删我方 `script == 0` 防御。
    if (!static_cast<ARAD::Arad_StatisticsEventItemList*>(script)->needStatistics(itemIndex))
    {
        return;
    }

    arad::SigAradEventItemLog sig;
    sig.m_itemId = itemIndex;
    sig.m_itemCount = count;
    sig.m_id = get_acc_id();
    sig.m_characNo = get_charac_no(-1);

    // b28d-lane5：ORIG 0x0866xxx 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb9c7), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, ARAD::K_ARAD_EVENT_ITEM_LOG, (int)GetUID());
    guard.operator->()->put_binary(&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

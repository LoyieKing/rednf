// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08692b84 | _ZN5CUser20SendAradEventItemLogEmi
void CUser::SendAradEventItemLog(unsigned long itemIndex, int count)
{
    // 2026-09-11（H3CUser，P183/P0）：ORIG 0x08692b84 无 mgr/script 具名槽——
    //   `call Singleton<Arad_DataManager>::Get` 结果 %eax 直接作 this 调
    //   findGameScript，其返回 %eax 又直接作 this 调 needStatistics（0x08692bc5
    //   `mov 0xc(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp)`）⇒ 两具名量
    //   内联（我方旧形 -0x10/-0xc 两对 store+load 共 +4 条）。
    // 2026-09-10（G3CUser-B，P171 删发明防御）：ORIG 0x08692bc5 `mov 0xc(%ebp),%edx;
    //   mov %edx,0x4(%esp); mov %eax,(%esp); call needStatistics` —— %eax（findGameScript
    //   结果）**直接**当 this，无 `test %eax,%eax / je` 判空；随后 0x08692bd6
    //   `mov %eax,%ebx; xor $0x1,%ebx`（!needStatistics 物化）+ 0x08692c21
    //   `test %bl,%bl; jne 0x08692d22`（跳尾声返回）⇒ 删我方 `script == 0` 防御。
    if (!static_cast<ARAD::Arad_StatisticsEventItemList*>(
             ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                 ->findGameScript("Arad_StatisticsEventItemList"))
             ->needStatistics(itemIndex))
    {
        return;
    }

    arad::SigAradEventItemLog sig;
    // 2026-09-11 CuB（P329/P0，装载序对接 ORIG 写入序）：ORIG 0x08692c34..0x08692c61
    //   的成员写入序 = +0x8(m_id: get_acc_id) → +0xc(m_characNo: get_charac_no(-1))
    //   → +0x0(m_itemId=itemIndex) → +0x4(m_itemCount=count)（两处 call 在前、
    //   两形参直写在后）；我方旧序（+0/+4 先）致整段对不上。语句重排，语义等价。
    sig.m_id = get_acc_id();
    sig.m_characNo = get_charac_no(-1);
    sig.m_itemId = itemIndex;
    sig.m_itemCount = count;

    // b28d-lane5：ORIG 0x0866xxx 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb9c7), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, ARAD::K_ARAD_EVENT_ITEM_LOG, (int)GetUID());
    guard.operator->()->put_binary(&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}
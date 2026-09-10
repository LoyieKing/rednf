void CEventStayTime::RewardGoGoFighter::operator()(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    int stayTime = m_4 - (int)user->GetLoginTime();
    if (m_0 > stayTime)
    {
        return;
    }
    // P197② register 缓存 accId 见下；数据代理经 GlobalData 真符号（不裸地址）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("GameWorld.cpp", 0x1e),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(5), user->GetUID());
    Taiwan::SigStayTimeEvent* evt =
        guard.operator->()->GetInBuffer<Taiwan::SigStayTimeEvent>();
    if (evt != 0)
    {
        // P197②：ORIG +0x0d3 `mov %eax,%ebx` 把 get_acc_id 结果 callee-saved
        // (%ebx) 缓存，跨 cMyTrace ctor 存活供 trace("%d",accId) 用；evt->set 再
        // fresh 调 get_acc_id（不复用）。
        // b28d-lane11：历史 asm("ebx") 绑定按 P0/N96 清除；改 P227 全表达式临时链
        // （P221 同族）——实参在 ctor 前求值并以 %ebx 跨 ctor 保活，且 trace 不占
        // 具名槽，翻正 guard(-0x28)/trace(-0x20) 槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x24, 0)("%d", (unsigned int)user->get_acc_id());
        evt->init();
        evt->set(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }

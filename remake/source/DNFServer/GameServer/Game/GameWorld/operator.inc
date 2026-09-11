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
    // [H8B 2026-09-11 G3 真语义差修复] ORIG 0x0863c05f `movl $0x8ce6c24,0x4(%esp)` + 0x0863c06a
    // `call StreamPool::Acquire`；0x8ce6c24 = "localtaiwan/Event/EventStayTimeCharacter.h"。
    // StreamPool 以 name 为池键 ⇒ 旧 "GameWorld.cpp" 会与他人共用池、破坏 ORIG 缓冲隔离（行为差）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("localtaiwan/Event/EventStayTimeCharacter.h", 0x1e),
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
        // [H8B 2026-09-11 G4 真语义差修复] ORIG 0x0863c0fc `movl $0x8ce6c50,0x4(%esp)`；
        // 0x8ce6c50 = "[Taiwan, Event] GoGoFighter try reward mid. (%u)"（实参 %ebx=get_acc_id
        // 直压 0x8(%esp)，与本侧 (unsigned int) cast 一致，仅换格式串）。
        cMyTrace(__PRETTY_FUNCTION__, 0x24, 0)("[Taiwan, Event] GoGoFighter try reward mid. (%u)", (unsigned int)user->get_acc_id());
        evt->init();
        evt->set(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }
}
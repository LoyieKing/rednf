// ORIG _ZN5CUser20UpdateEffectItemInfoEj @ 0x08658d90 (T)
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658d90 | _ZN5CUser20UpdateEffectItemInfoEj
// [b28d-GrpD 判词·等价 N] ORIG 0x08658d90：唯一实差异是 size 门槛的方向码
//   —— ORIG `cmp $0x32,%eax; seta %al; test; je <循环>` vs 我方 `setbe; je <退出>`
//   （判据⑦：cmp 同、落点体互换 = 等价）；其余为未解析重定位（"user.cpp"/memcpy@plt）
//   与槽号。字段偏移（-0x1b8/-0x1b4 = entries[i].item/expire）、begin 先于 size、
//   erase/end() 重调序全一致 ⇒ 无语义缺口。
void CUser::UpdateEffectItemInfo(unsigned int expireTime)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2ceb), true);
    guard.operator*()->operator<<((int)0x137);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658d90 实形，同 UpdateCoolTimeItemInfo）：
    //   无 memset(sig)；CONTINUOUS_ITEM_INFO 局部 + 迭代器循环 + 末尾 memcpy。
    // ORIG：同 UpdateCoolTimeItemInfo（count 清零先于 getCurCharacNo）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = effect->begin();
    // CuC P223 变体（ORIG 0x08658d90 直证，同 UpdateCoolTimeItemInfo 0x08658b2a）：
    //   ORIG `cmp $0x32,%eax; seta %al; test %al,%al; je <循环头>; jmp <尾>` = 裸早返双跳形。
    //   4.4 对 `if (size() > 0x32) return;` 复现（探针 /tmp/CuC/probe/p2.cpp f2）；
    //   旧 `if (size() < 0x33) {…}` 恒产 setbe 单 je。语义等价（return 仅析构 guard）。
    if (effect->size() > 0x32)   // 超过 0x32 条不发送
        return;
    for (; it != effect->end(); )
    {
        if (it->m_expireTime < expireTime
            || (unsigned int)it->m_itemIdx > 0x7ffffffe
            || it->m_expireTime > 2000000000u)
        {
            it = effect->erase(it);
        }
        else
        {
            local.m_entries[count].m_itemIdx = it->m_itemIdx;
            local.m_entries[count].m_expireTime = it->m_expireTime;
            ++count;
            it++;
        }
    }
    local.m_count = (unsigned char)count;
    memcpy(&sig->m_info, &local, 0x194);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}
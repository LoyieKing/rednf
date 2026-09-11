// ORIG _ZN5CUser22UpdateCoolTimeItemInfoEj @ 0x08658b2a (T)
// 组 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（0x100 占位：characNo@+0x0 +
// count@+0x4 + [0x194]条目簇）投递队列 2。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658b2a | _ZN5CUser22UpdateCoolTimeItemInfoEj
void CUser::UpdateCoolTimeItemInfo(unsigned int expireTime)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2cb8), true);
    guard.operator*()->operator<<((int)0x132);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658b2a 实形）：无 memset(sig)；用
    //   CONTINUOUS_ITEM_INFO 局部（C1 清零）+ 迭代器循环 + 末尾 memcpy(&sig->m_info)
    //   （MISSING：operator-> ×5 / CONTINUOUS_ITEM_INFO::C1 / operator++(int) / end() /
    //   operator!= / memcpy；EXTRA：operator[] ×4 / memset / size / operator+）。
    // ORIG：count 清零（-0x10）先于 getCurCharacNo 调用（语句序）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = cool->begin();
    // CuC P223 变体（ORIG 0x08658b2a 直证）：ORIG `cmp $0x32,%eax; seta %al; test %al,%al;
    //   je <循环头>; jmp <尾>` 是**裸早返双跳**形，不是 `if(...){…}` 单 je 形。
    //   4.4 对 `if (size() > 0x32) return;` 逐条复现该形（探针 /tmp/CuC/probe/p2.cpp f2：
    //   `cmpl $50; setg %al; testb %al,%al; je <body>; jmp <尾>`；`size()` 为无符号 ⇒ seta）。
    //   旧形 `if (size() < 0x33) {…}` 恒产 setbe 单 je（P197④ 镜像），已证不可翻。
    //   语义等价：size>0x32 时 return 只析构 guard，与旧形跳过整块后落尾一致。
    if (cool->size() > 0x32)   // 超过 0x32 条不发送
        return;
    for (; it != cool->end(); )
    {
        if (it->m_expireTime < expireTime
            || (unsigned int)it->m_itemIdx > 0x7ffffffe
            || it->m_expireTime > 2000000000u)
        {
            it = cool->erase(it);
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
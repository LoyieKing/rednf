// ----------------------------------------------------------------------------
// updateDataIntoDB（ORIG 0x0860f6de）
// ----------------------------------------------------------------------------
// [b28d-30] 两处已修：① 删具名 Stream* 局部（P0/P183，ORIG 0x0860f6fe 直传 %eax）；
// ② for-init 双声明缓存 itEnd（ORIG 0x0860f7ce end 在循环外）。
// 验收（05:08 复编）：DIFF 131/129 → **AE 129=129 gap=0**（删 stream 槽后帧布局自动
// 落回 ORIG，槽号无需再调）。原判词「帧/槽位跨版本差（N82/N93 族）」已被本波推翻。
// ORIG 见
// class_func_reports/CVendingMachineStatistic/updateDataIntoDB.md。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
void CVendingMachineStatistic::updateDataIntoDB()
{
    // ORIG：Acquire Stream("Statistics.cpp",0x624) → CStreamGuard(stream,true) →
    // 写 SIG=0x163, -1, m_useCount, m_output.size(), 每键值对 →
    // MsgQueueMgr::put(LOGDB_Q) → _init()。
    // P183/P0：ORIG 0x0860f6fe 的 Acquire 返回值直接进 ctor 实参
    // （+0x025 `movl $0x1,0x8(%esp); mov %eax,0x4(%esp); lea -0x18(%ebp),%eax`），
    // 无具名 Stream* 槽 ⇒ 不许物化具名 stream 局部。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x624),
                       true);

    **guard << 0x163;                    // SIG（DB 侧据此路由）
    **guard << (int)-1;                  // 固定 -1
    **guard << m_useCount;               // 使用次数
    **guard << (unsigned int)m_output.size();

    // P-END：ORIG 0x0860f7ce（map::end）+0x0f8 iterator ctor@-0x20 位于 **循环外**，
    // 循环尾 +0x160 仅 `call operator!=; test %al,%al; jne +0x10c`
    // ⇒ 源码须在 for-init 双声明缓存 itEnd（4.4 若写 `it != m_output.end()`
    //   会把 end() 落到循环尾，顺序错位）。
    for (std::map<unsigned long, unsigned int>::const_iterator it = m_output.begin(),
             itEnd = m_output.end();
         it != itEnd; ++it)
    {
        **guard << it->first;            // 物品索引（unsigned long）
        **guard << it->second;           // 数量（unsigned int）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    _init();
}
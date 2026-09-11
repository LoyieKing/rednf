// ----------------------------------------------------------------------------
// ORIG T 0x08156ed2（updateDatabase.md 全文）：guard 内 Acquire 流（msg 0x33b，
// 广播 -1）；put entrances.size() + 逐条 GetInBuffer<ST> memcpy 0x10 并 trace；
// clear；同序 layers：size + memcpy 0x10（pair+8 起，超读 2 字节 pad 为 ORIG
// 原行为）/trace/clear；尾 MsgQueueMgr put 队列 4。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（258 vs 266，语义闭合）。
void CAdvanceAltarLog::updateDatabase()
{
    Guard<Mutex> guard(&m_mutex);

    // [b28d-E03 2026-09-10] P259/P0：ORIG 0x08156f08..0x08156f1f 无 raw 具名槽
    //   （Acquire 结果 %eax 直传 CStreamGuard ctor 实参槽），删具名局部后帧/槽归位。
    CStreamGuard stream(GlobalData::s_stream_pool->Acquire(
                            "localglobal/global_Statistics.cpp", 0xe5),
                        true);

    // [b28d-E03 2026-09-10] P132：ORIG 0x08156f24..0x08156f37 每次先
    //   `call CStreamGuard::operator*`（0x080c8c46）再 `call CStreamGuard::operator<<`
    //   （0x080c8c56）⇒ 源形是 `(*stream) << v;`（operator* 链不可省略）。
    (*stream)->operator<<(0x33b);                 // ORIG operator*(0x33b)
    (*stream)->operator<<(-1);                    // 广播

    // [b28d-E03 2026-09-10] P0：ORIG 0x08156f68 `mov %eax,-0x28(%ebp)` ⇒ size 结果
    //   有具名 int 局部（我方原用 %ebx 右值缓存 + 无槽）。
    int nEntrance = (int)m_entrances.size();
    (*stream)->operator<<(nEntrance);
    // [b28d-E03 2026-09-10] P96/P270：ORIG begin 0x08157a12 为非 const
    //   `map::begin()`、比较用 `_ZNKSt17_Rb_tree_iterator…ne`（iterator 的 const 方法）
    //   ⇒ 原码是可变 `iterator`（const_iterator 会多一次转换构造 + 匿名临时槽）。
    // [b28d-E03 2026-09-10] P13：ORIG 0x08156f95..0x08156fb2 在初始化段连续
    //   `begin()`+`end()` 各一次并落相邻槽（-0x58/-0x5c）⇒ 原码手动缓存 end。
    std::map<unsigned int, STAdvanceAltarEntranceData>::iterator it =
        m_entrances.begin();
    std::map<unsigned int, STAdvanceAltarEntranceData>::iterator itEnd =
        m_entrances.end();
    for (; it != itEnd; ++it)
    {
        STAdvanceAltarEntranceData* slot =
            stream->GetInBuffer<STAdvanceAltarEntranceData>();
        std::memcpy(slot, &it->second, 0x10);
        // [b28d-E03 2026-09-10] P328⑩/P221：ORIG 0x08157000..0x08157074 先把 4 个
        //   vararg 求值保活（%edi/%esi/%ebx + -0x74 槽），**之后**才 cMyTrace C1 ⇒
        //   源形是「构造即调用」单语句临时链；两语句具名形把 C1 提前 1 段。
        cMyTrace(__PRETTY_FUNCTION__, 0xf4, 0)(
            "[AdvanceAltar] stageIndex:%d, enterCount:%d, successCount:%d, obtainStar:%d",
            it->second.m_stageIndex, it->second.m_enterCount,
            it->second.m_successCount, it->second.m_obtainStar);
    }
    m_entrances.clear();

    int nLayer = (int)m_layers.size();
    (*stream)->operator<<(nLayer);
    std::map<long long, STAdvanceAltarLayerStatData>::iterator layIt =
        m_layers.begin();
    std::map<long long, STAdvanceAltarLayerStatData>::iterator layEnd =
        m_layers.end();
    for (; layIt != layEnd; ++layIt)
    {
        STAdvanceAltarLayerStatData* slot =
            stream->GetInBuffer<STAdvanceAltarLayerStatData>();
        // pair+8 起复制 0x10（超过 ST 0xe 的 2 字节为 ORIG 原样超读，勿改 0x10）。
        std::memcpy(slot, &layIt->second, 0x10);
        // ORIG 0x08157147..0x081571ee 同形（5 个 vararg 先求值：%ebx/%esi/%edi +
        //   -0x6c/-0x70 槽 → C1(-0x38) → operator()）。
        cMyTrace(__PRETTY_FUNCTION__, 0x106, 0)(
            "[AdvanceAltar] itemType:%u, itemIndex:%u, itemLevel:%u, useCount:%u, useStar:%u",
            (unsigned int)layIt->second.m_itemType, layIt->second.m_itemIndex,
            (unsigned int)layIt->second.m_itemLevel, (unsigned int)layIt->second.m_useCount,
            (unsigned int)layIt->second.m_useStar);
    }
    m_layers.clear();

    // [b28d-E03 2026-09-10] P259/P0：ORIG 0x08157226..0x0815723d 无 msgq 具名槽
    //   （`mov &s_msgq_mgr,%eax` 后直接压栈作 receiver）⇒ receiver 内联。
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, stream);
}
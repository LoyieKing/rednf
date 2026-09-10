// ============================================================================
// CAdvanceAltarLog —— advancealtar 统计日志实现（Wave5 AltarProc 续批）。
// 证据：docs/class_func_reports/CAdvanceAltarLog/ 全文（10 函数）。
// 全部字段化；不使用 GameStubs 桥。
// ============================================================================
#include "CAdvanceAltarLog.h"

#include <cstring>

#include "CStreamGuard.h"
#include "GlobalData.h"      // GlobalData::s_stream_pool / s_msgq_mgr
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ORIG makeKey @0x08157298：低 32 = itemIndex，高 32 = (itemType<<16)|keyC
// （ORIG 汇编实证：组合 (itemType<<16)|keyC 后 sar $0x1f 符号扩展为 64 位；
// 高 32 位即该组合的有符号扩展——与低 32 位 itemIndex 拼合）。
long long CAdvanceAltarLog::makeKey(unsigned short itemType,
                                    unsigned int itemIndex,
                                    unsigned short keyC)
{
    return (static_cast<long long>(
                static_cast<int>((itemType << 16) | keyC))
            << 32) |
           static_cast<long long>(itemIndex);
}

// ORIG getType @0x081572ea：返回 key 高 32 位的高 16 位（= key>>48，符号扩展位型扩展）。
long long CAdvanceAltarLog::getType(long long key)
{
    return key >> 0x30;
}

// ORIG getIndex @0x0815730e：返回 (long long)(int)key（低 32 位符号扩展；
// mov $0,%eax 后 mov %edx,%eax 残留，sar $0x1f 符号扩展为 64 位）。
// [b28d-lane6] DIFF·N18a：ORIG 0x0815730e 的 (int)key→long long 截断-扩链残留 4 条死序（mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax），4.4 恒折（b28c-27 复核）。
long long CAdvanceAltarLog::getIndex(long long key)
{
    // ORIG 0x08157314..+0x26：sub $0x8 + mov eax,edx / mov $0,eax 死序 + sar 0x1f
    // ——4.1 对 (int)key→(long long) 截断-扩链的死序残留。探针三形（无局部/
    // 单 int 局部/单 ll 局部）：4.4 恒 sub $0x18 参数槽拷贝或丢 sar 尾 → N 封顶。
    // [b28c-27] 复核：N18a（4.1 冗余 regmove 不可造）；本形 16/12 条残差 4 条全为
    // 死序（`mov -0x4,%edx; mov %eax,%edx; mov $0,%eax; mov %edx,%eax`）。
    return static_cast<int>(key);
}

// ORIG getLevel @0x08157336：shld+shl 组合 (key<<16) 后取高 32 位 sar 16
// —— 即 (int)((key<<16)>>48) = 高 32 位（(itemType<<16)|keyC）的低 16 位（keyC）。
long long CAdvanceAltarLog::getLevel(long long key)
{
    return (key << 0x10) >> 0x30;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156aaa（ctor）：Mutex() → map(+0x18)/map(+0x30) 构造 → initStatistics。
// ----------------------------------------------------------------------------
CAdvanceAltarLog::CAdvanceAltarLog()
    : m_mutex(), m_entrances(), m_layers()
{
    initStatistics();
}

// ORIG T 0x08156b38（dtor）：ORIG 汇编开头即调 initStatistics（Guard 清双 map），
// 然后成员自动逆序析构。
CAdvanceAltarLog::~CAdvanceAltarLog()
{
    initStatistics();
}

// ORIG T 0x08156bca（initStatistics.md）：Guard 内双 map clear。
void CAdvanceAltarLog::initStatistics()
{
    Guard<Mutex> guard(&m_mutex);
    m_entrances.clear();
    m_layers.clear();
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156c30（incrementEntranceData.md）：guard 内 find(stageIndex)；
// 命中累加 enter/success/star 三计数；未命中经栈上 16B 构造后 operator[] 写入
// {stageIndex, deltas}（data[0] 冗余存键，与 ORIG 二进制布局一致）。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（99=99 长度同，逐条语义等价；N82 帧族）。
void CAdvanceAltarLog::incrementEntranceData(unsigned int stageIndex,
                                             unsigned int enterDelta,
                                             unsigned int successDelta,
                                             unsigned int starDelta)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<unsigned int, STAdvanceAltarEntranceData>::iterator it =
        m_entrances.find(stageIndex);
    // [b28d-E03 2026-09-10] 块序对齐（ORIG 0x08156c95 `je +0xc1`：未命中装配块
    //   fall-through 在前、命中累加块 jump 在后）⇒ 条件按 `it == end` 正极性书写，
    //   装配块为 then、累加块为 else（P59/P222 双向布局）。
    if (it == m_entrances.end())
    {
        STAdvanceAltarEntranceData data;                // 栈装配（ORIG local_28..）
        data.m_stageIndex = stageIndex;
        data.m_enterCount += enterDelta;
        data.m_successCount += successDelta;
        data.m_obtainStar += starDelta;
        // ORIG operator[] 逐 uint 写回（data[0]=键重复，等价整结构赋值）。
        m_entrances[stageIndex] = data;
    }
    else
    {
        it->second.m_enterCount += enterDelta;
        it->second.m_successCount += successDelta;
        it->second.m_obtainStar += starDelta;
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156d60（incrementLayerStatData.md）：guard 内 makeKey 后 find；
// 命中累加 useCount/useStar；未命中 14B 结构装配后 memcpy 运算式写入新槽
// （新建分支计数亦含增量，与 hit 分支累计一致）。
// ----------------------------------------------------------------------------
// [b28d-lane6] DIFF·N93/N82：槽号/寄存器分配（111 vs 123，语义闭合；N82 帧族）。
void CAdvanceAltarLog::incrementLayerStatData(unsigned short itemType,
                                              unsigned int itemIndex,
                                              unsigned short a3,
                                              unsigned short useCountDelta,
                                              unsigned short useStarDelta)
{
    Guard<Mutex> guard(&m_mutex);

    const long long key = makeKey(itemType, itemIndex, a3);
    std::map<long long, STAdvanceAltarLayerStatData>::iterator it =
        m_layers.find(key);
    // ORIG 0x08156e7d/0x08156e94 每语句**只发 1 次** `_Rb_tree_iterator::operator->`
    // （`movzwl 0x12(%eax),%edx; add -0x44(%ebp),%dx; mov %dx,0x12(%eax)`，写回同一
    // %eax）；展开式 `it->second.f = (ushort)(it->second.f + d)` 使 `it->second` 出现
    // 两次 ⇒ 4.4 每语句 2 次 op->（共 4 call）⇒ 必须用复合赋值（本波 -8 条）。
    if (it != m_layers.end())
    {
        it->second.m_useCount += static_cast<unsigned short>(useCountDelta);
        it->second.m_useStar += static_cast<unsigned short>(useStarDelta);
        return;
    }

    STAdvanceAltarLayerStatData data;                   // 栈装配（ORIG local_30 区）
    // ORIG 0x08156d60：装配只经 STAdvanceAltarLayerStatDataC1 清零
    // （movw@0/movl@4/movw@8/movw@a/movw@c，+0x2 空洞与 +0xe pad 不写），
    // **无 memset 调用**（0x08156d60..0x08156e51 区间仅 C1Ev + 逐字段装配）。
    // [b28d-E03 2026-09-10] 字段真值修正：ORIG 0x08156e1d 把 a3 写 second+0x8
    //   （= m_itemLevel，pair<long long,ST> second@+8）；原写 m_field2(+0x2) 是
    //   真语义 bug（+0x2 洞 ORIG 全程不写，实证 0x8156e0f..0x8156e39 段）。
    data.m_itemType = itemType;
    data.m_itemIndex = itemIndex;
    data.m_itemLevel = a3;
    data.m_useCount = static_cast<unsigned short>(data.m_useCount + useCountDelta);
    data.m_useStar = static_cast<unsigned short>(data.m_useStar + useStarDelta);
    // ORIG 0x08156e52 段：operator[] 结果 %eax 直比 &data（无具名槽），
    //   `cmp %edx,%eax; je <出口>`（call 结果直用、无 store/load）⇒ 试 P231
    //   纯 register 存储类提示（非 asm 绑定，P0 合规）让值留寄存器。
    register STAdvanceAltarLayerStatData* slot = &m_layers[key];
    if (slot != &data)
        std::memcpy(slot, &data, 0xe);
}

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

    MsgQueueMgr& msgq = *GlobalData::s_msgq_mgr;
    msgq.put(MsgQueueMgr::LOGDB_Q, stream);   // ORIG put(s_msgq_mgr,4,guard)
}

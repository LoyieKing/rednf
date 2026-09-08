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
void CAdvanceAltarLog::incrementEntranceData(unsigned int stageIndex,
                                             unsigned int enterDelta,
                                             unsigned int successDelta,
                                             unsigned int starDelta)
{
    Guard<Mutex> guard(&m_mutex);
    std::map<unsigned int, STAdvanceAltarEntranceData>::iterator it =
        m_entrances.find(stageIndex);
    if (it != m_entrances.end())
    {
        it->second.m_enterCount += enterDelta;
        it->second.m_successCount += successDelta;
        it->second.m_obtainStar += starDelta;
        return;
    }

    STAdvanceAltarEntranceData data;                    // 栈装配（ORIG local_28..）
    data.m_stageIndex = stageIndex;
    data.m_enterCount += enterDelta;
    data.m_successCount += successDelta;
    data.m_obtainStar += starDelta;
    // ORIG operator[] 逐 uint 写回（data[0]=键重复，等价整结构赋值）。
    m_entrances[stageIndex] = data;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156d60（incrementLayerStatData.md）：guard 内 makeKey 后 find；
// 命中累加 useCount/useStar；未命中 14B 结构装配后 memcpy 运算式写入新槽
// （新建分支计数亦含增量，与 hit 分支累计一致）。
// ----------------------------------------------------------------------------
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
    if (it != m_layers.end())
    {
        it->second.m_useCount = static_cast<unsigned short>(
            it->second.m_useCount + useCountDelta);
        it->second.m_useStar = static_cast<unsigned short>(
            it->second.m_useStar + useStarDelta);
        return;
    }

    STAdvanceAltarLayerStatData data;                   // 栈装配（ORIG local_30 区）
    // ORIG 0x08156d60：装配前 STAdvanceAltarLayerStatDataC1 仅清 14 字节
    // （movw@0/movl@4/movw@8/movw@a/movw@c，+0xe pad 不写）；新槽写入用
    // memcpy 0xe，pad 保持垃圾。
    std::memset(&data, 0, 0xe);
    data.m_itemType = itemType;
    data.m_field2 = a3;
    data.m_itemIndex = itemIndex;
    data.m_useCount = static_cast<unsigned short>(data.m_useCount + useCountDelta);
    data.m_useStar = static_cast<unsigned short>(data.m_useStar + useStarDelta);
    STAdvanceAltarLayerStatData* slot = &m_layers[key];
    std::memcpy(slot, &data, 0xe);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08156ed2（updateDatabase.md 全文）：guard 内 Acquire 流（msg 0x33b，
// 广播 -1）；put entrances.size() + 逐条 GetInBuffer<ST> memcpy 0x10 并 trace；
// clear；同序 layers：size + memcpy 0x10（pair+8 起，超读 2 字节 pad 为 ORIG
// 原行为）/trace/clear；尾 MsgQueueMgr put 队列 4。
// ----------------------------------------------------------------------------
void CAdvanceAltarLog::updateDatabase()
{
    Guard<Mutex> guard(&m_mutex);

    Stream* raw = GlobalData::s_stream_pool->Acquire(
        "localglobal/global_Statistics.cpp", 0xe5);
    CStreamGuard stream(raw, true);

    stream.operator<<(static_cast<int>(0x33b));   // ORIG operator<<(0x33b)
    stream.operator<<(-1);                        // 广播

    stream.operator<<(static_cast<int>(m_entrances.size()));
    for (std::map<unsigned int, STAdvanceAltarEntranceData>::const_iterator it =
             m_entrances.begin(); it != m_entrances.end(); ++it)
    {
        STAdvanceAltarEntranceData* slot =
            stream->GetInBuffer<STAdvanceAltarEntranceData>();
        std::memcpy(slot, &it->second, 0x10);
        cMyTrace trace("void CAdvanceAltarLog::updateDatabase()", 0xf4, 0);
        trace("[AdvanceAltar] stageIndex:%d, enterCount:%d, successCount:%d, obtainStar:%d",
              it->second.m_stageIndex, it->second.m_enterCount,
              it->second.m_successCount, it->second.m_obtainStar);
    }
    m_entrances.clear();

    stream.operator<<(static_cast<int>(m_layers.size()));
    for (std::map<long long, STAdvanceAltarLayerStatData>::const_iterator it =
             m_layers.begin(); it != m_layers.end(); ++it)
    {
        STAdvanceAltarLayerStatData* slot =
            stream->GetInBuffer<STAdvanceAltarLayerStatData>();
        // pair+8 起复制 0x10（超过 ST 0xe 的 2 字节为 ORIG 原样超读，勿改 0x10）。
        std::memcpy(slot, &it->second, 0x10);
        cMyTrace trace("void CAdvanceAltarLog::updateDatabase()", 0x106, 0);
        trace("[AdvanceAltar] itemType:%u, itemIndex:%u, itemLevel:%u, useCount:%u, useStar:%u",
              (unsigned int)it->second.m_itemType, it->second.m_itemIndex,
              (unsigned int)it->second.m_itemLevel, (unsigned int)it->second.m_useCount,
              (unsigned int)it->second.m_useStar);
    }
    m_layers.clear();

    MsgQueueMgr& msgq = *GlobalData::s_msgq_mgr;
    msgq.put(MsgQueueMgr::LOGDB_Q, stream);   // ORIG put(s_msgq_mgr,4,guard)
}

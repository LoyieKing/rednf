#ifndef DNF_GAME_CADVANCEALTARLOG_H_
#define DNF_GAME_CADVANCEALTARLOG_H_

// ============================================================================
// CAdvanceAltarLog —— advancealtar 统计日志（ORIG 0x08156aaa..0x081573xx）。
// 报告：docs/class_func_reports/CAdvanceAltarLog/（10 函数全文）。
// 布局证据（ctor 0x08156aaa）：
//   +0x00 Mutex（pthread_mutex_t 0x18）
//   +0x18 std::map<unsigned int, STAdvanceAltarEntranceData>
//   +0x30 std::map<long long, STAdvanceAltarLayerStatData>
// ST 结构为 ORIG 全局命名空间类型（updateDatabase 按值整块 memcpy 入 stream）。
// ============================================================================

#include <map>

#include "GameTypes.h"  // Mutex / Guard<T>
#include "SigTypes.h"   // STAdvanceAltarEntranceData / STAdvanceAltarLayerStatData
                        //  （字段化权威定义在 SigTypes.h，全局作用域）

class CAdvanceAltarLog
{
public:
    CAdvanceAltarLog();              // T 0x08156aaa
    ~CAdvanceAltarLog();             // T 0x08156b38

    void initStatistics();           // T 0x08156bca：guard 内双 map clear
    void incrementEntranceData(unsigned int stageIndex, unsigned int enterDelta,
                               unsigned int successDelta,
                               unsigned int starDelta);                 // T 0x08156c30
    void incrementLayerStatData(unsigned short itemType, unsigned int itemIndex,
                                unsigned short a3, unsigned short useCountDelta,
                                unsigned short useStarDelta);            // T 0x08156d60
    void updateDatabase();           // T 0x08156ed2

private:
    static long long makeKey(unsigned short itemType, unsigned int itemIndex,
                             unsigned short keyC);                     // T 0x08157298
public:
    static long long getType(long long key);                           // T 0x081572ea
    static void getIndex(long long key);                               // T 0x0815730e
    static long long getLevel(long long key);                          // T 0x08157336

    Mutex m_mutex;                                                   // +0x00
    std::map<unsigned int, STAdvanceAltarEntranceData> m_entrances;  // +0x18
    std::map<long long, STAdvanceAltarLayerStatData> m_layers;       // +0x30
};

// ORIG 全局实例 advanceAltarstatistics_（starAdd/buyItem/updateStatistics 引用）。

#endif  // DNF_GAME_CADVANCEALTARLOG_H_

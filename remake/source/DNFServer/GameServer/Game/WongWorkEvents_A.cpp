// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WongWorkEvents_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WongWorkEvents_A.cpp —— WongWork 事件域 ORIG 符号转写（G2 批，独占 TU）
// 依据 docs/class_func_reports/WongWork__Event{Alttle,Sigong,Everland,FCCombo,Common}/
// 与 objdump df_game_r（HackTypeInfo_t/arad_parsing 直反汇编）。
// ============================================================================

#include <string>
#include <vector>

#include "WongWorkEvents_A.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "GlobalData.h"      // GlobalData::s_db_mgr / s_stream_pool / s_msgq_mgr
#include "GameTypes.h"       // cMyTrace
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"        // SIG_LOAD_EVENTITEM
#include "QuestClear.h"      // STClearedQuest（本 TU 产出其 ctor）

// ---- TU 局部外部依赖（全局符号，producer 归属其它 TU/批次）----
char* NumberToString(unsigned int value, int radix);          // _Z14NumberToStringji

namespace WongWork
{

// ---- TU 局部外部依赖（producer 归属其它 TU/批次）----

namespace DBCommon
{
unsigned int GetIdentity(MySQL* db);                          // _ZN8WongWork8DBCommon11GetIdentityEP5MySQL
}

// SIG_LOAD_EVENTITEM 字段视图（SigTypes.h 当前为 char m_pad[0x5c]，布局等价）。
struct SigLoadEventItemView
{
    unsigned int m_eventId;        // +0x00
    unsigned int m_characNo;       // +0x04
    int          m_itemCount;      // +0x08
    struct Entry                    // stride 0x10 @+0x0c
    {
        unsigned int m_dbId;
        unsigned int m_itemId;
        unsigned int m_stackCount;
        unsigned int m_eventCode;
    };
    Entry m_entries[5];
};
#ifdef WONGWORK_EVENT_STATIC_ASSERT_SIZEOF
WONGWORK_EVENT_STATIC_ASSERT_SIZEOF(SigLoadEventItemView, 0x5c);
#endif

// ----------------------------------------------------------------------------
// EventFCCombo::GetDBHandle —— ORIG W 0x0844dc7c：
//   return DBMgr::GetDBHandle(GlobalData::s_db_mgr, 1, 0);
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/GetDBHandle.inc"
}

// ----------------------------------------------------------------------------
// EventAlttle::UpdateEventAlttleInfo —— ORIG T 0x083fb530 (size 0x64)
//   UPDATE item_check=unix_timestamp(now())
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/UpdateEventAlttleInfo.inc"
}

// ----------------------------------------------------------------------------
// EventAlttle::CheckAlreadyGetAlttleItem —— ORIG T 0x083fb472 (size 0xbd)
// exec 成功且 get_int(item_check)!=0 → 已领(1)；否则 0。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/CheckAlreadyGetAlttleItem.inc"
}

// ----------------------------------------------------------------------------
// EventAlttle::ArrangeAlttleEventItem —— ORIG T 0x083fb595 (size 0x2ed)
// [b28d-solo-WongWorkEvents_A] AE（推翻 b21「N 封顶」判词，188=188）：
// ① P227/P139 错误路径改全表达式临时链 + __PRETTY_FUNCTION__（恢复
//    NumberToString→%ebx 跨 ctor 保活 + push %ebx/sub $0xf4）；
// ② P311 反向 struct 行数组（每列两步 `shl $4; lea -0x8(%ebp); lea; sub $K`）；
// ③ P171/P232 补循环体内 dead 局部 `unsigned int unused = 0;`（ORIG
//    `movl $0,-0x10(%ebp)`，把 db/outCount/row/列局部整体下移 4 槽）；
// ④ P225 尾测 while + `if (!fetch()) break;`。
// 取 occ_check!=0 行（limit 10），对 item_check==0 且 server_id 匹配的行收集，
// 之后逐条 Update + InsertEventItem2EventTable(event_code=6)。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/ArrangeAlttleEventItem.inc"
}

// ----------------------------------------------------------------------------
// EventSigong::UpdateEventSigongInfo —— ORIG T 0x083fa7a5 (size 0xe0)
// 按 CWebEvent::GetItemType(3,itemNo)==1/2 选 item1_check/item2_check 栏。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/UpdateEventSigongInfo.inc"
}

// ----------------------------------------------------------------------------
// EventSigong::CheckAlreadyGetSigongItem —— ORIG T 0x083fa69c (size 0x108)
// [b28d-solo-WongWorkEvents_A] AE + 真语义 bug 修正：尾段极性原为
// `(get_int() && check!=0) ? 0 : 1`（旧写反，get_int 失败误返 1）；修法 =
// P251/P279 switch 直传（GetItemType）+ P226 语句表达式（尾段）。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/CheckAlreadyGetSigongItem.inc"
}

// ----------------------------------------------------------------------------
// EventSigong::ArrangeSigongEventItem —— ORIG T 0x083fa855 (size 0x3a9)
// 双栏结构：row[+8]=itemN_no、row[+6]=serverN、row[+2]=checkN；
// itemN 数量为常量：item1=1、item2=200（ORIG movl $0x1,-0x5c/$0xc8,-0x58，
// SQL 未取 cnt 列）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删多余守卫；P225 尾测 while/break；P311 反向
// 「每栏独立 2 元素数组 + 独立 id」复现 disp 折叠（局部 struct 视图无效）。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/ArrangeSigongEventItem.inc"
}

// ----------------------------------------------------------------------------
// EventEverland::UpdateEventEverlandInfo —— ORIG T 0x083face3 (size 0xd4)
// GetItemType(4,itemNo)==1 时更新 coin_check。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/UpdateEventEverlandInfo.inc"
}

// ----------------------------------------------------------------------------
// EventEverland::CheckAlreadyGetEverlandItem —— ORIG T 0x083fac0d (size 0xd5)
// [b28d-solo-WongWorkEvents_A] AE：P286 单 case switch（GetItemType 直比
// `cmp $1,%eax; jne`）+ P226 语句表达式（尾段短路物化岛）。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/CheckAlreadyGetEverlandItem.inc"
}

// ----------------------------------------------------------------------------
// EventEverland::ArrangeEverlandEventItem —— ORIG T 0x083fad60 (size 0x32e)
// SQL 无 cnt 列；item_no 恒为常量 1（ORIG movl $0x1,(%eax)）。event_code=3。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删 outCount<MAX 多余守卫；P311 反向 struct 行
// 数组复现两步寻址；P225 尾测 while/break。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/ArrangeEverlandEventItem.inc"
}

// ----------------------------------------------------------------------------
// EventFCCombo::CheckAlreadyFCComboEventItem —— ORIG T 0x083fb883 (size 0xa5)
// 使用自有 GetDBHandle()（同型 GlobalData::s_db_mgr, 1, 0）。
// [b28d-solo-WongWorkEvents_A] AE：P226 条件向裸语句表达式复现 4.1 短路
// bool 物化岛（`xor $1;test;jne` + 常量岛 + 尾 `test;je`）。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/CheckAlreadyFCComboEventItem.inc"
}

// ----------------------------------------------------------------------------
// EventFCCombo::UpdateFCComboEventInfo —— ORIG T 0x083fb929 (size 0x4d)
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/UpdateFCComboEventInfo.inc"
}

// ----------------------------------------------------------------------------
// EventFCCombo::ArrangeFCComboEventItem —— ORIG T 0x083fb976 (size 0x2ef)
// 6 列查询含真实 item_cnt。event_code=7。
// [b28d-solo-WongWorkEvents_A] AE：P227/P139 错误路径全表达式临时链 +
// __PRETTY_FUNCTION__；P140 删多余守卫；P311 反向 struct 行数组；P225 尾测
// while/break。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/ArrangeFCComboEventItem.inc"
}

// ----------------------------------------------------------------------------
// EventCommon::InsertEventItem2EventTable —— ORIG T 0x083f9b54 (size 0x127)
// db handle idx=3；成功记录 trace 并返回 @@identity，失败返回 0。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/InsertEventItem2EventTable.inc"
}

// ----------------------------------------------------------------------------
// EventCommon::LoadCharacEventItem —— ORIG T 0x083f9c7b (size 0x2b2)
// loadId==0 按角色取 5 条 delete_flag=0；否则按行 id 单取。
// 组包 header 0x4b + size(-1)，SIG_LOAD_EVENTITEM 装载后投递 msgq queue 1。
// [b28d-solo-WongWorkEvents_A] AE：P259 Acquire 内联实参（去 Stream* 具名
// 局部槽）+ P225 尾测 while/break + P221 cMyTrace 全表达式临时链（翻互斥块
// cMyTrace/guard 槽方向）。
// ----------------------------------------------------------------------------
#include "WongWorkEvents_A/LoadCharacEventItem.inc"
}

// ----------------------------------------------------------------------------
// arad_parsing —— ORIG T 0x08483e98 (size 0x115)
// b28d-60：定义已迁至 CDungeonClear.cpp（ORIG TU = DungeonClear.cpp：
//   orig_tu_names.tsv:429 GI 键名归属 + 0x08483e98 结束 0x08483fad 紧接
//   CDungeonClear::AradfromString T 0x08483fae 的地址相邻证据）。
//   声明仍保留在 WongWorkEvents_A.h（消费方无感）。
// ----------------------------------------------------------------------------

}  // namespace WongWork

// ----------------------------------------------------------------------------

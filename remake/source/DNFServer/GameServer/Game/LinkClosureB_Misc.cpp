// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LinkClosureB_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkClosureB_Misc.cpp — /tmp/undef_bizB.txt 杂项域收口（LinkClosureB 批 B）
//
// 逐符号 ORIG nm/objdump/报告核实后忠实转写；唯一 owner = 本 TU：
//   WongWork::DBCommon::GetIdentity(MySQL*)              T 0x083f9ad4
//   WongWork::CWebEvent::GetItemType(int,int)            T 0x086c1aba
//   WongWork::EventATI::{CheckAlreadyGetATIItem(ji)      T 0x083f9f2e
//                        UpdateEventATIInfo(ji)          T 0x083fa0ad
//                        ArrangeATIEventItem(jj)}        T 0x083fa1d3
//   WongWork::EventMuz::{CheckAlreadyGetMuzItem(ji)      T 0x083fb047
//                        UpdateEventMuzInfo(ji)          T 0x083fb105
//                        ArrangeMuzEventItem(jj)}        T 0x083fb16a
//   AvatarCoin::HistoryLog::SubLog(CUser*,unsigned int)  T 0x0817ff54
//   TOD_UserState::getLastEnterLayer() const             T 0x08643942
//   TimerStartMatching::registNextTimer(long,int,int)    T 0x08637e14
//   advanceAltarstatistics_（ORIG B @0x09400300 全局实例）B
//   CAutoMarketConditionsControl::{GetItemDropRate(i)    T 0x082f8ae8
//                                 ChangeItemRarity(PiS0id)} T 0x082f8c8c
//
// GetInvenTypeFromItemSpace 已由 CTitleBook.cpp 提供（核对同跳表）；roundFloat /
// Drop 族 / ARAD server 族 / CompoundAvatar 族等由本批兄弟 TU 承载。
// ============================================================================

#include <stdio.h>
#include <math.h>

#include "MySQL.h"
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameTypes.h"        // cMyTrace
#include "CInventory.h"       // NumberToString
#include "WongWorkEvents_A.h" // EventCommon::InsertEventItem2EventTable
#include "CUser.h"
#include "AvatarCoin.h"
#include "DataManagerInitDepends.h" // TOD_UserState
#include "TimerQueue.h"
#include "TimerDispatcher.h"  // TimerStartMatching
#include "CAdvanceAltarLog.h" // advanceAltarstatistics_
#include "advancealtar.h"    // advancealtar::HistoryLog（advanceAltarstatistics_ 类静态成员声明）

// ---------------------------------------------------------------------------
// WongWork::DBCommon::GetIdentity —— ORIG T 0x083f9ad4（报告 DBCommon/GetIdentity.md）
// seLect @@identity → exec(true)/fetch/get_uint(0)；任一步失败返回 0。
// ---------------------------------------------------------------------------
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

#include "LinkClosureB_Misc/GetIdentity.inc"
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::CWebEvent::GetItemType —— ORIG T 0x086c1aba（报告+objdump 核对）
// kind==2 离散表分派；kind==3 {8→2, 1→1}；其余 0。
// ---------------------------------------------------------------------------
namespace WongWork {

// CWebEvent 权威声明已收敛至 GlobalData.h:274(Main 沉降仲裁,含 GetItemType);
// 本 TU 的局部 class 视图已删除,仅保留 ORIG T 定义。

// [b28d-solo-LinkClosureB_Misc] AE：P46 同值 case 合并为 fallthrough 标签组
//（4.4 不合并同体叶块）+ P286 内层 switch 形 + 照抄原码遗留重复 else-if。
#include "LinkClosureB_Misc/GetItemType.inc"
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventATI —— ORIG 事件族（报告 WongWork__EventATI/*）。
// 类与 DB_Login.cpp 的调用视图一致（全静态成员，无数据成员）。
// ---------------------------------------------------------------------------
namespace WongWork {

struct EventATI
{
    static int CheckAlreadyGetATIItem(unsigned int uid, int itemNo);
    static void UpdateEventATIInfo(unsigned int uid, int itemNo);
    static void ArrangeATIEventItem(unsigned int mid, unsigned int serverId);
};

// CheckAlreadyGetATIItem —— ORIG T 0x083f9f2e (size 0xd9)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 96/96）（ORIG 0x083f9f2e sz 0x0000017f）
#include "LinkClosureB_Misc/CheckAlreadyGetATIItem.inc"
}

// UpdateEventATIInfo —— ORIG T 0x083fa0ad (size 0x126)
// [b28d-D9] 修复：P46 叶块序：case1 写于 case2 前（交换后 .o AE 70/70）（ORIG 0x083fa0ad sz 0x00000126）
#include "LinkClosureB_Misc/UpdateEventATIInfo.inc"
}

// ArrangeATIEventItem —— ORIG T 0x083fa1d3 (size 0x287)
// 行收集：item_check==0 且 item_server_id==serverId 一条；
// avata_server_id==serverId 时 avata_check1==0 / avata_check2==0 各一条。
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点已闭合（p167 24/24 CLEAR）。
// 已修：__PRETTY_FUNCTION__（P139）、dead local limit=10（P232）、P227 全表达式
// 临时链、P164/P283 for 底部条件 + unsigned 尾测 seta、P171 删 count<10 防御、
// 存储序 [0],[2],[3],[1]。残余 = P209（4.4 unsigned 形参比较恒载寄存器 +1/分支）
// + P311/N81 族（4.4 对 rows[count][1..2] 常量列下标折 scaled，4.1 恒 shl+lea+sub 链）。
// [F2e-Singles2 C05② 复核 2026-09-10] 两处 cmp 差（ORIG `mov -0x40/-0x54(%ebp),%eax;
//   cmp 0xc(%ebp),%eax` mem 直比）**比较对象是 unsigned 形参 serverId**（0xc(%ebp)）⇒
//   按 C05② 边界表 ❌ 形参子型，P209/N18b 封顶（改形参会动 mangled，禁）。主 gap 来源
//   仍是 P311 数组链（our 290 vs ORIG 307，3 处 shl$4+lea 链被 4.4 折 scaled）。
#include "LinkClosureB_Misc/ArrangeATIEventItem.inc"
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// WongWork::EventMuz —— ORIG 事件族（报告 WongWork__EventMuz/*）
// ---------------------------------------------------------------------------
namespace WongWork {

struct EventMuz
{
    static int CheckAlreadyGetMuzItem(unsigned int uid, int itemNo);
    static void UpdateEventMuzInfo(unsigned int uid, int itemNo);
    static void ArrangeMuzEventItem(unsigned int mid, unsigned int serverId);
};

// CheckAlreadyGetMuzItem —— ORIG T 0x083fb047 (size 0xba)
#include "LinkClosureB_Misc/CheckAlreadyGetMuzItem.inc"
}

// UpdateEventMuzInfo —— ORIG T 0x083fb105 (size 0x63)
#include "LinkClosureB_Misc/UpdateEventMuzInfo.inc"
}

// ArrangeMuzEventItem —— ORIG T 0x083fb16a (size 0x240)
// [b28d-solo-LinkClosureB_Misc] N 封顶：同 ArrangeATIEventItem（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、dead local limit=10、P227 临时链、for 底部条件、
// 存储序 [0],[2],[3],[1]。残余 = P209 + P311 数组常量列下标 scaled 折叠。
// [F2e-Singles2 C05② 复核 2026-09-10] 同上：`cmp 0xc(%ebp),%eax`（serverId 形参）⇒
//   N18b/P209 封顶；our 184 vs ORIG 194 gap +10 主体为 P311 scaled 折叠。
#include "LinkClosureB_Misc/ArrangeMuzEventItem.inc"
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// AvatarCoin::HistoryLog::SubLog —— ORIG T 0x0817ff54 (size 0x45)
// user!=0 → LogHistory("AvatarCoin-,%d,%d", amount, coin.Get())。
// coin 对象位于背包基址 +0x658（Arad_AvatarRoulette.cpp 同约定）。
// ---------------------------------------------------------------------------
#include "LinkClosureB_Misc/SubLog.inc"
}

// ---------------------------------------------------------------------------
// TOD_UserState::getLastEnterLayer —— ORIG T 0x08643942 (const, sret)
// ORIG 汇编：add $0x14 → TOD_Layer(TOD_Layer const&) 拷贝 ⇒ 读 +0x14
//（m_todayLayer，TOD_Layer 仅 ushort 字段，拷贝构造与 ushort 构造等价）。
// 头内字段已具名（DataManagerInitDepends.h TOD_UserState）。
// ---------------------------------------------------------------------------
#include "LinkClosureB_Misc/getLastEnterLayer.inc"
}

// ---------------------------------------------------------------------------
// advanceAltarstatistics_ —— ORIG B @0x09400300 全局实例（构造/析构随全局
// __cxa 链）。唯一定义点；extern 声明在 CAdvanceAltarLog.h。
// ---------------------------------------------------------------------------
CAdvanceAltarLog advancealtar::HistoryLog::advanceAltarstatistics_;  // ORIG B 0x09400300

// ---------------------------------------------------------------------------
// CAutoMarketConditionsControl::GetItemDropRate —— ORIG T 0x082f8ae8
// （objdump 转写；报告缺失）。
// rate=1.0f；!m_field380 直接返回；levelRange 越界 → trace+返回 1.0f；
// v=m_vec3dc[level]，v!=0 时
// rate = 1.0f + (-1000.0/((m_equipProbBase-1.0)*v))（fldl/fdivrp 常量
// 0x8c1fc70=-1000.0 实证）；计数器 %1000 命中时打 itemDropRate trace。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureB_Misc] NEAR：真语义 bug 修 3 处——(a) 除法方向应为
// `(v*sub)/(-1000.0)`（原 `-1000.0/(v*sub)` 整段反，ORIG fdivp 实证）；
// (b) 漏第二次 operator[]（P183；p167 MISSING×1 → CLEAR）；(c) 日志格式串多
// %s+重复签名实参、cMyTrace 第三参 5→0。另补 __PRETTY_FUNCTION__（P139）、
// logCount 符号名（P187）+ 后置自增序。残余 = N82附/N93/N95（4.4 聚合对象优先
// 低槽 + 标量分区 vs 4.1 生命周期穿插 ⇒ 槽号整体平移）。
#include "LinkClosureB_Misc/GetItemDropRate.inc"
}

// ---------------------------------------------------------------------------
// CAutoMarketConditionsControl::ChangeItemRarity —— ORIG T 0x082f8c8c
// （objdump 逐段转写；报告缺失）。m_field380==0 → false。
// out[2..5]=item[2..5]; i=2..0 向下：
//   delta=item[i+1]-item[i]; acc+=(int)(delta*factor)(round-to-nearest);
//   out[i]=item[i]-acc; if(out[i]<0) out[i]=0; 恒返 true。
// item/out 两表各取前 4 项打 trace（两次独立匿名计数器）。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureB_Misc] N 封顶：语义/调用点闭合（p167 CLEAR）。
// 已修：__PRETTY_FUNCTION__、logCount/logCount2 符号名（P187）+ 后置自增序、
// cMyTrace 第三参 0、格式串删多余 %s、P191 i 双初始化 + 两循环复用同槽、
// P59 正向包裹、P227 全表达式临时链。残余 = N85（4.4 对 double 局部拷贝恒
// fld/fstp 中转，ORIG 恒 2×mov；探针 /tmp/b28d_lcbm/p4.cpp 六形全败）⇒ 帧 +0x10。
#include "LinkClosureB_Misc/ChangeItemRarity.inc"
}

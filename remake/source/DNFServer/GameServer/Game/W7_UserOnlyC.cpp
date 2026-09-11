// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W7_UserOnlyC/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// W7_UserOnlyC.cpp —— W7C_UserOnly 链接收敛独占 TU（2026-08-27）
//
// 承载此前无 owner 的 ORIG 真实符号定义；函数体依 docs/class_func_reports
// 对应报告或 objdump 逐指令忠实转写，偏移/常量来自汇编。符号清单：
//   CPacketTimeChecker::{reset,check} + validCount_E/validSecond_E
//   CPacketVerifier::CPacketVerifier/Init/UpdatePacket/GetSequence + 4 statics
//   _NS_PI_2ND_GetHackType（b28d-solo：本 TU static 副本已删，改用 GameSecurity.cpp
//     权威全局 W 0x08277359；符号名对齐 _Z22…）
//   PIReceiverManager::CheckHashReceived                    T 0x0808bf7e
//   _NS_PI_2ND_RecvHash_CS                                  T 0x08274f73
//   CUser::Parsing(int)                                     T 0x08647890
//   CUser::SetSkillLevel(int,int)                           T 0x0867b51c
//   CUser::getReturnUserLevelKey(int,int)                   T 0x0869230a
//   CUser::GiveItemsReturnUser()                            T 0x08692000
//   CUser::GiveEquipItemsReturnUser()                       T 0x08692140
//   CUser::setCharacLinkMessageFlag(bool)                   W 0x084ece20
//   CUser::SendLimitItemUsageNotiInfo(int,int,int)          T 0x08688f02
//   CUser::SendCeraShopGiftStatisticLog(uint,uint,ulong,uint) T 0x0868a87a
//   LimitItemUsage::{C1,D1,isAnotherDay,isUsable,useIt}     W 0x8695ef0..
//   UserMercenaryInfoMgr::UpdateCharacName(uint,string const&) T 0x081b462e
//   CUserEventUtil::updateCharacterDayEvent(CUser*,int)     T 0x08688230
//   CDungeon::VerifyMaze() const                            T 0x0834c9f0
//   CDungeon::VerifyMaze(MazeScript const&) const           T 0x0834cb7c
//   CStackableItem::GetMonsterCardId()                      W 0x0849f530

#include "CUser.h"
// ============================================================================
// b28d-52 判词表（本 TU 剩余 DIFF）：
//   已修：CPacketVerifier::GetSequence 0x08696e48 —— P159/SIGFIX 返回类型 int
//     （ORIG 尾双段 movzwl；探针 /tmp/b28d_l52/p_ushort.cpp f2 实证）。CUser.h 冻结，
//     Main 已代落 ⇒ 本件转 AE。
//   已修（头部）：CUser::SetSkillLevel 0x0867b51c —— P248② 裸偏移改具名字段
//     `!m_clientSpec.m_gmUserFlag` + P214① 短路汇聚（头部 20 条与 ORIG 逐条对齐）；
//     残差 N82（帧 0x4c vs 0x44 + 后续槽组/push edi/esi）。
//   CPacketVerifier::UpdatePacket 0x08696e58 —— 残差仅 call/跳转目标重定位（N7 寄存器
//     轮转 + P264 口径）
//   PIReceiverManager::CheckHashReceived 0x0808bf7e —— P0/P183（我方 so 指针缓存槽 -0x14
//     + `add $0x4` 地址形；ORIG 表达式直读）
//   CUser::SetSkillLevel 0x0867b51c —— P248② 字段 bool 化候选（ORIG 0x0867b52d xor $1）
//     （见上「已修（头部）」）
//   CUser::GiveItemsReturnUser 0x08692000 —— N82（帧 0x8c vs 0x98）
//   CUser::GiveEquipItemsReturnUser 0x08692140 —— N82（帧 0x9c vs 0xb4）
//   CUser::SendCeraShopGiftStatisticLog 0x0868a87a —— N82 + P264（Acquire 立即数重定位）
//   CUser::Parsing 0x08647890 —— N82（帧 0x63c vs 0x61c）
//   CDungeon::VerifyMaze() 0x0834c9f0 —— N82（帧 0xa0 vs 0x90）
//   CDungeon::VerifyMaze(MazeScript) 0x0834c990 —— N82（帧 0x94 vs 0x84）
//   _NS_PI_2ND_RecvHash_CS 0x08274f73 —— N82（帧 0x64 vs 0x74）
// ============================================================================
#include "CDungeon.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "CSkill.h"
#include "Cipher.h"
#include "Encryption.h"
#include "PacketSecurity.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"
#include "CHackAnalyzer.h"
#include "CEnvironment.h"
#include "CMailBoxHelper.h"
#include "SkillSlot.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <utility>

extern CDataManager* G_CDataManager();
extern char* NumberToString(unsigned int value, int radix);
extern char* GetPacketName(ENUM_PACKETCLASS packetClass, unsigned short packetId);
extern bool CheckDailyScheduleTime(int scheduleTime, long lastPlayTime,
                                   long curTime);
unsigned char MakeHash(unsigned char* data, unsigned int len,
                       unsigned char seed);          // 定义 PIHash.cpp
#include "W7_UserOnlyC/_NS_PI_2ND_GetPacketIndex.inc"
#include "W7_UserOnlyC/_NS_PI_2ND_GetHackType.inc"
                           unsigned short c);

// _header（CUser.h 冻结）m_checksum 为 unsigned int；ORIG UpdatePacket 赋值段
// `mov 0x7(%eax),%edx` 要求与 int 字段 m_lastChecksum 同型（P211）。
// pack(1) 具名视图提供 int 视角，仅本 TU 使用，无成员函数 ⇒ 零符号发射。
namespace {
#pragma pack(push, 1)
struct HeaderChecksumIntView
{
    char m_pad[7];
    int m_checksum;   // +0x07
};
#pragma pack(pop)
}  // namespace

// ============================================================================
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// ============================================================================

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN15CPacketVerifier13m_nVaridCountE`（CU=user.cpp）
//   ⇒ 本 TU 首个强符号须为 CPacketVerifier::m_nVaridCount。两组 int 静态定义互换先后，
//   零语义/零布局改动（皆为静态初始化 POD，不产生构造序）。
// ============================================================================
// ---- CPacketVerifier（ORIG 0x8696fe0 C1 / 0x8696de4 Init / 0x8696e58
//      UpdatePacket / 0x8696e48 GetSequence）----
// ============================================================================

int CPacketVerifier::m_nVaridCount = 5;         // D 0x093ace38
int CPacketVerifier::m_nVaridSecond = 1;        // D 0x093ace3c
int CPacketVerifier::m_nAuctionVaridCount = 6;  // D 0x093ace40
int CPacketVerifier::m_nAuctionVaridSecond = 1; // D 0x093ace44

int CPacketTimeChecker::validCount_ = 100;     // D 0x093ace48
int CPacketTimeChecker::validSecond_ = 1;      // D 0x093ace4c
// reset/check 定义迁至 CUser.cpp（unit-at-a-time outgoing 塌缩修复）。

#include "W7_UserOnlyC/CPacketVerifier.inc"


#include "W7_UserOnlyC/Init.inc"


#include "W7_UserOnlyC/GetSequence.inc"


#include "W7_UserOnlyC/UpdatePacket.inc"


// ============================================================================
// ---- PI_2ND 反作弊 hash 族 ----
// ============================================================================

// b28d-solo-W7_UserOnlyC：本 TU 原 static _NS_PI_2ND_GetHackType{Enum} 副本已删
// （ORIG 权威符号为 GameSecurity.cpp 的全局 W 0x08277359/0x08275af3；副本符号
// `_ZL22…`/`_ZL26…` 与 ORIG `_Z22…`/`_Z26…` 不符且实现不同）。声明见文件头。

#include "W7_UserOnlyC/CheckHashReceived.inc"


#include "W7_UserOnlyC/_NS_PI_2ND_RecvHash_CS.inc"


// ============================================================================
// ---- CUser::Parsing（ORIG T 0x08647890，报告 Parsing.md 转写）----
// b28d-solo-W7_UserOnlyC 判词：本件 p167 调用点差集 46/46 CLEAR（0 missing /
// 0 extra），语义完成；byte 层残差 4 条为 N82/N95/N7 族：帧 0x63c vs 0x62c
// （槽基差 0x10）+ cMyTrace 实参寄存器角色（4.1 esi/edi/ebx vs 4.4 ecx/edx/eax）
// + zero 槽载入（4.1 `mov -0x34,%edx` vs 4.4 `mov $0x0,%edx`）。已还原：
// net 引用去缓存（this 直读）、tick/zero/headerLen 具名槽、m_lastRecvGapPair
// u64 整写、size 32 位读。不再耗轮次。
// ============================================================================

#include "W7_UserOnlyC/Parsing.inc"


// ============================================================================
// ---- CUser::SetSkillLevel（ORIG T 0x0867b51c，报告 SetSkillLevel.md）----
// ============================================================================

#include "W7_UserOnlyC/SetSkillLevel.inc"


// ============================================================================
// ---- CUser 回馈用户物品族 ----
// ============================================================================

#include "W7_UserOnlyC/getReturnUserLevelKey.inc"


#include "W7_UserOnlyC/GiveItemsReturnUser.inc"


#include "W7_UserOnlyC/GiveEquipItemsReturnUser.inc"


#include "W7_UserOnlyC/setCharacLinkMessageFlag.inc"


#include "W7_UserOnlyC/SendLimitItemUsageNotiInfo.inc"


#include "W7_UserOnlyC/SendCeraShopGiftStatisticLog.inc"


// ============================================================================
// ---- LimitItemUsage（ORIG W；字段按汇编偏移访问，头内布局保留 pad 形态）----
// ============================================================================

// 说明：LimitItemUsage 为 packed 布局（+0x15/+0x19 非对齐 int），
// 具名成员直写（gcc packed 自动非对齐访问）即等价 ORIG 行为。

#include "W7_UserOnlyC/LimitItemUsage.inc"


// [Q2P342ScanB 2026-09-11 杠杆A·P342 落地] `~LimitItemUsage()` 用户空析构定义已删
// （trivial 化）。ORIG D1/D2/D0 0x086a6480 为 **W**：由 `new_allocator<LimitItemUsage>::7destroy`
// 的 `p->~T()` odr-use 发射 —— 我方该实例在 `CUserOnlyOrig.cpp.o`，故按 ORIG 的
// odr-use 链在**同一 TU 集**发射（本 TU 不再定义 D1/D2）。详见 CUser.h `LimitItemUsage` 注。

#include "W7_UserOnlyC/isAnotherDay.inc"


#include "W7_UserOnlyC/isUsable.inc"


#include "W7_UserOnlyC/useIt.inc"


// ============================================================================
// ---- UserMercenaryInfoMgr::UpdateCharacName（ORIG T 0x081b462e）----
// ============================================================================

#include "W7_UserOnlyC/UpdateCharacName.inc"


// ============================================================================
// ---- CUserEventUtil::updateCharacterDayEvent（ORIG T 0x08688230）----
// ============================================================================

class CUserEventUtil
{
public:
    static void updateCharacterDayEvent(CUser*, int);
};

#include "W7_UserOnlyC/updateCharacterDayEvent.inc"


// ============================================================================
// ---- CDungeon::VerifyMaze 两重载（报告 VerifyMaze*.md 转写）----
// ============================================================================

#include "W7_UserOnlyC/VerifyMaze.inc"


#include "W7_UserOnlyC/VerifyMaze_1.inc"


// ============================================================================
// ---- CStackableItem::GetMonsterCardId（ORIG W 0x0849f530）----
// ============================================================================

#include "W7_UserOnlyC/GetMonsterCardId.inc"


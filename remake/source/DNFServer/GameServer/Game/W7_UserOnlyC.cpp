// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W7_UserOnlyC/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "W7_UserOnlyC/Init.inc"
}

#include "W7_UserOnlyC/GetSequence.inc"
}

// 返回错误码：0=正常；1=同包洪水；3=timechecker 节流；4=拍卖包洪水。
// b28d-solo-W7_UserOnlyC 判词：P225 while+break 复现 setg 物化 + m_checksum 经
// HeaderChecksumIntView int 视图同型直存（P211）⇒ 120=120，TU 层 AE。
// 本件无 class_func_reports 报告（报告缺失，按 nm 切片 + objdump 兜底）。
#include "W7_UserOnlyC/UpdatePacket.inc"
}

// ============================================================================
// ---- PI_2ND 反作弊 hash 族 ----
// ============================================================================

// b28d-solo-W7_UserOnlyC：本 TU 原 static _NS_PI_2ND_GetHackType{Enum} 副本已删
// （ORIG 权威符号为 GameSecurity.cpp 的全局 W 0x08277359/0x08275af3；副本符号
// `_ZL22…`/`_ZL26…` 与 ORIG `_Z22…`/`_Z26…` 不符且实现不同）。声明见文件头。

// ORIG T 0x0808bf7e PIReceiverManager::CheckHashReceived(uchar*,uint,uint,bool)
// this+0x04 为接收 hash key 字节状态。
// b28d-solo-W7_UserOnlyC 判词：P0/P183 删 pThis 指针缓存（this 表达式直读）+
// P211 cur 同型化（L/S 同型 ⇒ LHS-first）⇒ 72=72，TU 层 AE。
#include "W7_UserOnlyC/CheckHashReceived.inc"
}

// ORIG T 0x08274f73：接收 hash 登记 / 异常路径报 hack。
// b28d-solo-W7_UserOnlyC 判词（含 3 处真 bug 修正）：①CheckHashReceived 的 key
// 实参应为 hashDword>>8（原写 hashDword）；②record = !check && !existing（原写
// !check && existing，极性反）；③GetHackType 第一实参为常量 0（原写 hackPackId）。
// 另：make_pair 单次构造复用、index 改 unsigned short（对齐 ORIG `IRhRtE`）⇒
// p167 14/14 CLEAR，132=132，TU 层 AE。
#include "W7_UserOnlyC/_NS_PI_2ND_RecvHash_CS.inc"
}

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
}

// ============================================================================
// ---- CUser::SetSkillLevel（ORIG T 0x0867b51c，报告 SetSkillLevel.md）----
// ============================================================================

// 返回 int：ORIG 0x0867b51c 尾 `mov eax,0x1; ret`（成功）/早退 `mov eax,0x0`
// （runnable 假 / skill==0）——Model22 桥清零证据；签名与 CUser.h 同步（Main 代落）。
// b28d-solo-W7_UserOnlyC 判词：P0/P36 内联 job（%ebx 跨 G_CDataManager 保活）、
// secondGrow（两次独立调用，setle 物化）、treeIdx（%esi 保活）三处具名槽 ⇒
// 131=131，TU 层 AE。
#include "W7_UserOnlyC/SetSkillLevel.inc"
}

// ============================================================================
// ---- CUser 回馈用户物品族 ----
// ============================================================================

// ORIG T 0x0869230a：等级段映射。
#include "W7_UserOnlyC/getReturnUserLevelKey.inc"
}

// ORIG T 0x08692000，报告 GiveItemsReturnUser.md。
// b28d-solo-W7_UserOnlyC 判词：addInfo 具名槽（去 const 折叠）+ group/msgLen/
// characNo/sender 内联实参（P0/P52）+ P285 空 else 岛 ⇒ 81=81，TU 层 AE。
#include "W7_UserOnlyC/GiveItemsReturnUser.inc"
}

// ORIG T 0x08692140（objdump 直转）：装备回馈走 +0xb550 =
// m_returnUserReward.m_levelUp 表。
// b28d-solo-W7_UserOnlyC 判词：本件已收至 120/121 —— 唯一残差为
// `mov -0x30(%ebp),%eax; mov %eax,%ebx`（4.1 冗余 regmove 跨 G_CDataManager call
// 保活）。4.4 恒折单段 `mov -0x30(%ebp),%ebx`，语句表达式 / unsigned cast 两旋钮
// 实测均无效 ⇒ 按 N18a（反向亦不可造）封顶；调用点差集与语义已对齐。
#include "W7_UserOnlyC/GiveEquipItemsReturnUser.inc"
}

// ORIG W 0x084ece20：单字节写 this+0x703d8（m_breakAway.m_field14 bool 直写）。
#include "W7_UserOnlyC/setCharacLinkMessageFlag.inc"
}

// ORIG T 0x08688f02，报告 SendLimitItemUsageNotiInfo.md。
#include "W7_UserOnlyC/SendLimitItemUsageNotiInfo.inc"
}

// ORIG T 0x0868a87a，报告 SendCeraShopGiftStatisticLog.md。
// b28d-solo-W7_UserOnlyC 判词：删 raw 裸偏移指针（§4.3），改 SigTypes_E.h 字段化
// 具名成员（sizeof/偏移/mangled 不变，已 R2 广播）⇒ 82=82，TU 层 AE。
#include "W7_UserOnlyC/SendCeraShopGiftStatisticLog.inc"
}

// ============================================================================
// ---- LimitItemUsage（ORIG W；字段按汇编偏移访问，头内布局保留 pad 形态）----
// ============================================================================

// 说明：LimitItemUsage 为 packed 布局（+0x15/+0x19 非对齐 int），
// 具名成员直写（gcc packed 自动非对齐访问）即等价 ORIG 行为。

// ORIG C1/C2 0x08695ef0：CommonTime 子对象刷新当前时间，其余字段清零。
#include "W7_UserOnlyC/LimitItemUsage.inc"
}

// ORIG D1/D2/D0 0x086a6480：空析构（ORIG 仅 ret）。
#include "W7_UserOnlyC/dtor_LimitItemUsage.inc"
}

// ORIG W 0x08688fba，报告 LimitItemUsage/isAnotherDay.md。
#include "W7_UserOnlyC/isAnotherDay.inc"
}

// ORIG W 0x08695f68。
#include "W7_UserOnlyC/isUsable.inc"
}

// ORIG W 0x08695f28。
#include "W7_UserOnlyC/useIt.inc"
}

// ============================================================================
// ---- UserMercenaryInfoMgr::UpdateCharacName（ORIG T 0x081b462e）----
// ============================================================================

#include "W7_UserOnlyC/UpdateCharacName.inc"
}

// ============================================================================
// ---- CUserEventUtil::updateCharacterDayEvent（ORIG T 0x08688230）----
// ============================================================================

class CUserEventUtil
{
public:
    static void updateCharacterDayEvent(CUser*, int);
};

#include "W7_UserOnlyC/updateCharacterDayEvent.inc"
}

// ============================================================================
// ---- CDungeon::VerifyMaze 两重载（报告 VerifyMaze*.md 转写）----
// ============================================================================

// ORIG T 0x0834cb7c。
// b28d-solo-W7_UserOnlyC 判词：P197④ size()==0 复现 sete；两循环具名 pos 消双
// operator[]；第三段 find 内联 pair<int&,int&> + P223 三岛 goto 尾标签 ⇒ 345=345，
// TU 层 AE。
#include "W7_UserOnlyC/VerifyMaze.inc"
}

// ORIG T 0x0834c9f0。
// b28d-solo-W7_UserOnlyC 判词：第一循环补具名 MazeScript* 局部（ORIG -0x14 槽，
// P0 槽往返）⇒ 113=113，TU 层 AE。
#include "W7_UserOnlyC/VerifyMaze_1.inc"
}

// ============================================================================
// ---- CStackableItem::GetMonsterCardId（ORIG W 0x0849f530）----
// ============================================================================

#include "W7_UserOnlyC/GetMonsterCardId.inc"
}

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemLock/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ item_lock::CItemLock + CItemLockTimer 实现（W6_TailA 批）。
// 依据 docs/class_func_reports/item_lock__CItemLock/*.md（汇编为准）与
// docs/class_func_reports/item_lock__CItemLockTimer/*.md；worklist
// /tmp/only_orig/item_lock.txt 全部 33 个符号由本 TU 与共享头产生。
//
// 对象布局（ORIG ctor 0x854156a）：
//   charac_expand::CData 基类（vptr +0x00 / m_bIsSave +0x04）
//   +0x08 map<unsigned char, stItemLockInfo>   当前锁表
//   +0x20 stack<unsigned char, deque<uchar> >  空闲 lock 号池（IsAbleItemLock 判空）
//   +0x48 vector<stItemLockRef>                SendItemLockList 待发列表
//   +0x54 CItemLockTimer                       解锁倒计时计数
// 载体为角色扩展数据区（跨 TU reinterpret_cast，见 CUser/CAccountCargo/CGameManager）。
#include "CTitleBook.h"   // item_lock::stItemLockRef / stItemLockInfo /
                          // CItemLock / CItemLockTimer 权威定义

#include <string.h>
#include <iostream>   // ORIG TU 含 std::ios_base::Init 静态对象（P338 判据）
#include <utility>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "LogManager.h"
#include "SigTypes.h"
#include "TimerQueue.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "PacketGuard.h"
// ---- 最小类型镜像（仓库既有模式，见 CExpandEquipslot.cpp:36）----
// CItemLock.cpp 不 include CUser.h/CInventory.h：二者的联合依赖域含先行批次
// 的 GCC4.4 assert/枚举缺口；此处仅镜像本 TU 调用的方法签名（声明），外部
// 链接定义仍由权威 TU 提供，mangled ABI 完全一致。
// [hc N 记 · b20] CUser 具名字段 m_historyLog（+0x79700）在权威 CUser.h 存在，
// 但本 TU 为镜像隔离 TU（不 include CUser.h，否则与本文件镜像 `class CUser`
// 重复定义 + 先行批次联合依赖缺口），故 cUserHistoryLog* 取址仍按 §4.3「跨 TU
// 隔离」例外保留 `(cUserHistoryLog*)((char*)user + 0x79700)` 裸偏移形态，记 N。
// 证据：CUser.h 内 `cUserHistoryLog m_historyLog;  // +0x79700`；ORIG 各取址点
// lea 0x79700(%reg)。
class CInventory
{
public:
    Inven_Item* GetInvenRef(int invenType, int slot);      // ORIG（槽位访问）
};

class CUserCharacInfo
{
public:
    int getCurCharacNo() const;                            // ORIG 0x080cbc4e
    CInventory* getCurCharacInvenW();                      // ORIG（继承转发）
    unsigned char getCurChannelEquipslotSwitch();          // ORIG 0x086863? 语义一致
    bool FindItemLock(unsigned char lock, ENUM_ITEMSPACE& space, int& slot,
                      CUser* user) const;                  // ORIG 0x08680e04
};

class cUserHistoryLog
{
public:
    void ItemLockUsingLog(unsigned char lockType, int a, int b, int c,
                          int d);                          // ORIG 0x08689808
};

class CUser : public CUserCharacInfo
{
public:
    unsigned int GetUID() const;                           // ORIG 0x080c8c96
    int get_charac_no(int chIdx);                          // ORIG 0x0864dfa0
    Inven_Item* GetCurCharacInventoryRef(unsigned short area,
                                         unsigned short slot); // ORIG 0x08680f2e
    void Send(PacketGuard& packet);                        // ORIG 0x086485ba
};

#include "CItemLock/GetInvenTypeFromItemSpace.inc"

// [b28d-solo-CItemLock] DB 区块 8 字节条目的 POD 块拷贝视图（布局镜像
// stItemLockInfo）。stItemLockInfo 带用户 ctor（非 POD）⇒ 4.4 对其赋值恒逐字段
// 「读-写」交替、且对 +4 成员先发 `add $4`；**两 int 成员**的 POD 视图令 4.4 发
// ORIG 的 movdi 展开「先读高字、再读低字、后连写两字」（探针
// /tmp/b28d_solo_CItemLock/probe_copy3.cpp v11/v12 逐条同形）。平凡 ctor/op=/dtor
// ⇒ 零符号发射，sizeof/偏移/mangled 不受影响。
struct stItemLockInfoPod
{
    int m_unlockSec;          // +0x00
    int m_stateWord;          // +0x04（m_state + m_pad5[3] 的整字视图）
};
static_assert(sizeof(stItemLockInfoPod) == sizeof(stItemLockInfo),
              "stItemLockInfoPod ABI drift");

// std::pair<const unsigned char, stItemLockInfo> 的 POD 布局镜像：以直接成员访问
// 让 4.4 把 +4/+8 偏移折进 mov 的 disp（v12），避免 `&it->second` 的 `add $4`
// （v11 多一条、v1 退化为逐字段交替）。
struct stItemLockInfoPairPod
{
    unsigned char m_first;        // +0x00
    char m_pad3[3];               // +0x01
    stItemLockInfoPod m_second;   // +0x04
};
static_assert(sizeof(stItemLockInfoPairPod) ==
                  sizeof(std::pair<const unsigned char, stItemLockInfo>),
              "stItemLockInfoPairPod ABI drift");

// SIG_ITEM_LOCK_UPDATE（0x324）载荷视图：+0x00 角色编号、+0x04 起 100×8 锁条目。
// m_characNo 声明 unsigned（与 get_charac_no 的 int 返回值异型，P211 档③）⇒ 复现
// ORIG 0x854182d `call; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,(%eax)`
// （RHS-first+中转）；m_locks 为成员数组 ⇒ 复现 ORIG 0x85418c0 scaled 直存
// `mov %eax,0x4(%ecx,%ebx,8)`（探针 /tmp/b28d_solo_CItemLock/probe_save.cpp g/h2）。
// 仅本 TU 视图；线格式字节与 sizeof 不变。
struct SIG_ITEM_LOCK_UPDATEView
{
    unsigned int m_characNo;          // +0x00
    stItemLockInfoPod m_locks[0x64];  // +0x04（0x64×8 = 0x320）
};
static_assert(sizeof(SIG_ITEM_LOCK_UPDATEView) == sizeof(SIG_ITEM_LOCK_UPDATE),
              "SIG_ITEM_LOCK_UPDATEView ABI drift");

// ORIG 匿名常量（rodata 内联实例 _ZN9item_lockL20CHECK_ITEM_LOCK_TIMEE
namespace item_lock
{

// = 0x3c、_ZN9item_lockL24ITEM_UNLOCK_WAITING_TIMEE = 0x3f480，08b25970 取值）。
static const int CHECK_ITEM_LOCK_TIME     = 0x3c;      // 计时器周期 60 秒
static const int ITEM_UNLOCK_WAITING_TIME = 0x3f480;   // 解锁等待期 3 天

// ===========================================================================
// CItemLockTimer（ORIG 方法均 W 内联）
// ===========================================================================

#include "CItemLock/CItemLockTimer.inc"


#include "CItemLock/Reset.inc"


#include "CItemLock/GetTimerCount.inc"


#include "CItemLock/IncreaseItemLockTimer.inc"


#include "CItemLock/DecreaseItemLockTimer.inc"


#include "CItemLock/NextItemLockTimer.inc"


#include "CItemLock/CItemLock.inc"


// ===========================================================================
// CItemLock 生命周期
// ===========================================================================

#include "CItemLock/dtor_CItemLock.inc"


#include "CItemLock/GetItemLockRefVec.inc"


#include "CItemLock/GetItemLockInfo.inc"


#include "CItemLock/CheckItemLock.inc"


#include "CItemLock/MakeItemLockPacket.inc"


// ===========================================================================
// DB 同步包（StreamPool → MsgQueueMgr DB_Q）
// ===========================================================================

#include "CItemLock/UpdateAvatarItem.inc"


#include "CItemLock/UpdateCreatereItem.inc"


#include "CItemLock/UpdateItem.inc"


#include "CItemLock/SendItemLockList.inc"


// ===========================================================================
// IsAble* 门禁（返回码即错误表索引；ORIG 精确常量）
// ===========================================================================

#include "CItemLock/IsAbleItemLock.inc"


#include "CItemLock/IsAbleItemUnlock.inc"


#include "CItemLock/IsAbleItemUnlockCancel.inc"


// ===========================================================================
// 锁 / 解锁业务
// ===========================================================================

#include "CItemLock/DoItemLock.inc"


#include "CItemLock/DoItemUnlock.inc"


#include "CItemLock/DoItemUnlockCancel.inc"


#include "CItemLock/OnItemUnlockWaitTimeout.inc"


#include "CItemLock/PushItemLock.inc"


#include "CItemLock/PopItemLock.inc"


#include "CItemLock/_findItem.inc"


#include "CItemLock/VerifyItemLock.inc"


// ===========================================================================
// 客户端应答包
// ===========================================================================

#include "CItemLock/SendItemUnlock.inc"


#include "CItemLock/SendRequestItemLock.inc"


#include "CItemLock/SendRequestItemUnlock.inc"


#include "CItemLock/SendRequestItemUnlockCancel.inc"


// ===========================================================================
// 角色扩展数据生命周期（charac_expand::CData 虚接口）
// ===========================================================================

#include "CItemLock/loadData.inc"


#include "CItemLock/getData.inc"


#include "CItemLock/_saveData.inc"


#include "CItemLock/_reset.inc"


}  // namespace item_lock

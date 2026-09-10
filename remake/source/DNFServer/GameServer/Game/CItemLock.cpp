// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemLock/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG 0x8543044：m_count = 0。
// [P338/㉓ 符号绑定] ORIG 该符号为 W（弱）⇒ 原码头内 inline 定义；显式 inline
// 复现弱绑定（原为 T 强符号，与 ORIG 绑定不符）。
#include "CItemLock/CItemLockTimer.inc"
}

// ORIG 0x8543052：m_count = 0。
#include "CItemLock/Reset.inc"
}

// ORIG 0x854303a：返回 m_count（非 const）。
#include "CItemLock/GetTimerCount.inc"
}

// ORIG 0x854147c：计数为 0 时注册 TIMER_MESSAGE 0x88 定时器（周期 60s、
// 参数 uid / characNo）；无论如何计数 +1。
#include "CItemLock/IncreaseItemLockTimer.inc"
}

// ORIG 0x85414f0：计数 -1。
#include "CItemLock/DecreaseItemLockTimer.inc"
}

// ORIG 0x8541502：计数非 0 时重新注册 TIMER_MESSAGE 0x88 定时器（保证链式触发）。
#include "CItemLock/NextItemLockTimer.inc"
}

// ORIG C1/C2 0x854156a：CData 基类构造后依次默认构造 map/stack/vector 与
// 计时器成员（全部零态）。[P338/㉓ 布局] ORIG 中本定义紧接 NextItemLockTimer
// （0x8541502+0x67=0x854156a）⇒ 保持同一源码顺序，复现 `_GLOBAL__I_` 键符号名
// （IncreaseItemLockTimer）与 .text 发射顺序。
#include "CItemLock/CItemLock.inc"
}

// ===========================================================================
// CItemLock 生命周期
// ===========================================================================

// ORIG D1/D2 0x8541644 / D0 0x85416f6：基类 vptr 回填后容器逆序析构；
// 编译器自动生成等价序列。
#include "CItemLock/dtor_CItemLock.inc"
}

#include "CItemLock/GetItemLockRefVec.inc"
}

// ORIG 0x854261e：map 查找；命中返回 state 并写解锁时刻，未命中返回 0。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：返回类型 uchar→int 复现 ORIG 尾
// 双段零扩展（详见下行）。
// [b28d-41] 返回类型 uchar→int：ORIG 尾 `movzbl 0x8(%eax),%eax; movzbl %al,%eax`
// 双段零扩展由 int 返回 + uchar 字段表达式复现（探针 /tmp/d41_p4.cpp gB；P248 档①），
// 4.4 对 uchar 返回恒折叠单段。返回类型不入 mangled，调用点隐式截断语义不变。
#include "CItemLock/GetItemLockInfo.inc"
}

#include "CItemLock/CheckItemLock.inc"
}

// ORIG 0x85426a6：未锁定 → put_byte(0)；否则 put_byte(state)，state==2 时
// 再 put_int(剩余秒数 = unlockSec + 3天 - now，负值截 0)。
#include "CItemLock/MakeItemLockPacket.inc"
}

// ===========================================================================
// DB 同步包（StreamPool → MsgQueueMgr DB_Q）
// ===========================================================================

// ORIG 0x8541d42：包 0x1c2 = (0x1c2, uid, item.m_addInfo2(+7), GetLock() & 0xff)。
#include "CItemLock/UpdateAvatarItem.inc"
}

// ORIG 0x8541c36：包 0x1c1，同构。
#include "CItemLock/UpdateCreatereItem.inc"
}

// ORIG 0x8541e4e：item 空间字节(+0x01)==8 → UpdateAvatarItem(0x1c2)；
// ==5 → UpdateCreatereItem(0x1c1)。（注意 ORIG 判定次序：==8 先，调用的是 Avatar。）
#include "CItemLock/UpdateItem.inc"
}

// ORIG 0x85427a0：包 0xfb（put_header(0,0xfb)），short 计数占位后逐项写；
// 已解锁项清锁并 UpdateItem；倒计时项按频道 equipslot 开关重映射 space
// (3<->0x12)，写 space/slot/state，state==2 再写剩余秒数；末尾回填计数，
// finalize 后发送并清空 m_lockList。
// [b28d-solo-CItemLock] 本轮修法：①`state==0` 用 while+goto 复现 4.1 槽比零
// setcc 物化（P225/P197④）；②`getCurChannelEquipslotSwitch()==1 && …` 拆嵌套 if
// 复现首条件 sete（P225 推广）；③m_slot 比较与 put_short 实参按 unsigned short
// 复现 ORIG `ja`/双段 movzwl；④outSpace/item 提至循环体头（outSpace 先声明）复现
// ORIG 槽序 -0x11/-0x10。put_short(count) 用变量、state/remainSec 声明于循环外且
// state 为 int、space 实参 (unsigned char) 同前。
// [b28d-solo-CItemLock] VERDICT: AE（compare_tu 口径）—— P303/P225 等范式修法见体注。
#include "CItemLock/SendItemLockList.inc"
}

// ===========================================================================
// IsAble* 门禁（返回码即错误表索引；ORIG 精确常量）
// ===========================================================================

// ORIG 0x8541afe：空闲号池空 → 0x16 可锁；非空 → 0（CTitleBook.h:290 注释一致；
// 参数 lock 未使用）。
#include "CItemLock/IsAbleItemLock.inc"
}

// ORIG 0x8541b2a：map 未命中或 state!=1 → 0x13；state==1 → 0。
#include "CItemLock/IsAbleItemUnlock.inc"
}

// ORIG 0x8541bb0：map 未命中或 state!=2 → 0x13；state==2 → 0。
#include "CItemLock/IsAbleItemUnlockCancel.inc"
}

// ===========================================================================
// 锁 / 解锁业务
// ===========================================================================

// ORIG 0x8542192：物品未带锁号时从空闲池取出一个号，写 {unlockSec=now,
// state=1} 入 map；alter 后把新号写回物品并发请求包；日志 reason=0。
// [b28d-solo-CItemLock] 本轮修法：insert 实参改内联直构（无具名 value；槽序
// make_pair/-0x18 → pair/-0x24 → insert sret/-0x2c 与 ORIG 一致）。既有权宜：
// 删多余 memset、具名 uchar lock、m_state 先于 m_unlockSec 赋值、删 empty()。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 insert 直构实参 + 内联 history receiver。
#include "CItemLock/DoItemLock.inc"
}

// ORIG 0x854231a：物品已上锁且 map 命中时置 state=2 / unlockSec=now，
// 计时器 +1 并回发剩余秒数；日志 reason=1。
// [b28d-solo-CItemLock] 本轮修法：remain 改 P198① 预清两句式（ORIG 0x8542475
// `movl $0x0,-0xc(%ebp)`）+ history receiver 内联。既有权宜：具名 uchar lock
// 局部（find 复用同槽，消除二次 GetLock 与 sete 归一）。
// [b28d-solo-CItemLock] VERDICT: AE —— P198① remain 预清两句式 + 内联 history receiver。
#include "CItemLock/DoItemUnlock.inc"
}

// ORIG 0x85424c2：物品已上锁且 map 命中时置 state=1 / unlockSec=now，
// 计时器 -1；日志 reason=2。
// [b28d-solo-CItemLock] 本轮修法：history receiver 内联（去多余具名槽，帧
// 0x44→0x34 与 ORIG 一致）。既有权宜：具名 uchar lock 局部 + find 复用同槽。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 history receiver（去多余具名槽）。
#include "CItemLock/DoItemUnlockCancel.inc"
}

// ORIG 0x8542a6a：定时器到期入口 → PopItemLock，恒返 1。
#include "CItemLock/OnItemUnlockWaitTimeout.inc"
}

// ORIG 0x8542a8a：
//   新条目 state==0                → 号回收进空闲池；
//   state==2 且已过等待期          → 清 info 后回收 + alter；
//   其余                           → 继续计时再入 map（state==2 时续上计时器）。
// [b28d-solo-CItemLock] VERDICT: AE —— insert 实参内联直构（无具名 value 临时）。
#include "CItemLock/PushItemLock.inc"
}

// ORIG 0x8542b80：遍历 map，将已过 3 天等待期且 state==2 的条目逐个收尾：
// FindItemLock 定位物品槽 → 清物品锁号 + DB 包；equipslot 开关频道空间重映射
// (3<->0x12)；SendItemUnlock 后号回收进空闲池、erase 条目、计时器 -1、alter、
// 日志 reason=3；FindItemLock 失败直接返回。结束后 NextItemLockTimer 链式续期。
// [b28d-solo-CItemLock] 本轮修法：①`if (A && B)` 正极性 + 体尾 continue 复现
// ORIG 0x8542bfa 的 mov$1/mov$0 物化与「体尾 jmp 条件、++it 置体后」布局；
// ②`getCurChannelEquipslotSwitch()==1 && …` 拆嵌套 if 复现首条件 sete；③history
// receiver 内联。既有权宜：删 space/slotVal 预清（直接传址）、erase(it++)、
// 直接改 space 出参、日志前不设 item 守卫（P171）。
// [b28d-solo-CItemLock] VERDICT: AE —— 正极性 if(A&&B)+体尾 continue + && 拆嵌套 if。
#include "CItemLock/PopItemLock.inc"
}

// ORIG 0x8542e12：m_lockList 中存在指定 lock 号的引用 → 1，否则 0。
// [b28d-41] 返回类型 int→bool：ORIG VerifyItemLock 调用点 `call; xor $0x1,%eax;
// test %al,%al; je` 是 bool callee 裸形（P248 档①），int 返回恒产 sete 归一。
#include "CItemLock/_findItem.inc"
}

// ORIG 0x8542e98：核对 map 中所有锁定/倒计时条目是否仍被 m_lockList 引用；
// 未被引用的（角色扩展数据加载不全时）退回空闲池并从 map 删除（state==2 时
// 同时 -1 计时器）；发生过修正则 alter。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：三处修复——①`state==1||state==2`
// 正向包裹复现 ORIG 0x8542ede `cmp $1; je; cmp $2; jne`（4.4 对 `!=1 && !=2` 恒
// 合并 je/je 同落点）；②`erase(it++)` 复现 0x8542f47 ppEi（P28）；③_findItem
// 返回类型 bool 复现 0x8542f0e `xor $1,%eax; test %al,%al; je`（P248 档①）。
#include "CItemLock/VerifyItemLock.inc"
}

// ===========================================================================
// 客户端应答包
// ===========================================================================

// ORIG 0x85420e0：包 0xfc = put_header(0,0xfc)、byte(space)、short(slot)。
#include "CItemLock/SendItemUnlock.inc"
}

// ORIG 0x8541ea0：包 0x10f = put_header(1,0x110 族第 1 条)、byte(1)、
// byte(space)、short(slot)。注：ORIG put_header 第二参 0x10f。
#include "CItemLock/SendRequestItemLock.inc"
}

// ORIG 0x8541f5a：包 0x110 = put_header(1,0x110)、byte(1)、byte(space)、
// short(slot)、int(remainSec)。
#include "CItemLock/SendRequestItemUnlock.inc"
}

// ORIG 0x8542026：包 0x111 = put_header(1,0x111)、byte(1)、byte(space)、
// short(slot)。
#include "CItemLock/SendRequestItemUnlockCancel.inc"
}

// ===========================================================================
// 角色扩展数据生命周期（charac_expand::CData 虚接口）
// ===========================================================================

// ORIG 0x8541714：pData 为 100 × 8 字节 DB 区块；自 i=99 向下 PushItemLock
// （lock 号 = i+1），使低号位最终落在栈顶（与空闲号池 LIFO 序一致）。恒真。
// [b28d-solo-CItemLock] VERDICT: AE —— 具型指针 data+i 复现 ORIG 偏移 add 序。
#include "CItemLock/loadData.inc"
}

// ORIG 0x8541958：map 逆编号回写 DB 区块（idx = key-1 ∈ [0,99]；每项 8 字节
// = unlockSec + {state,pad} 4 字节整体）；越界记 LogManager 错误。
// [b28d-solo-CItemLock] 本轮修法：具型 POD 指针 `data + idx` 复现 ORIG 0x85419da
// 偏移 add 序；POD 视图赋值复现 8 字节「先读全再写全」块拷贝（旧 N7/N84/N90
// 判词作废）。
// [b28d-solo-CItemLock] VERDICT: AE —— 具型 POD 指针 + POD 视图块拷贝（先读全再写全）。
#include "CItemLock/getData.inc"
}

// ORIG 0x854176e：SIG_ITEM_LOCK_UPDATE（0x324 字节）出队至 LOGDB→DB_Q(2)：
// << 0x1b8、<< uid；GetInBuffer 清零后 +0 = get_charac_no(-1)，随后与 getData
// 相同的 idx/key-1 校验与写入；越界 LogManager 错误；put(DB_Q) 后析构。恒真。
// [b28d-solo-CItemLock] 本轮修法：SIG 载荷视图 SIG_ITEM_LOCK_UPDATEView
// （unsigned m_characNo + m_locks[0x64]）⇒ characNo store 复现 ORIG 0x854182d
// RHS-first+%edx 中转（unsigned 字段与 int 返回值异型，P211 档③），锁条目复现
// ORIG 0x85418c0 scaled 直存 + POD 块拷贝（旧 N76/N84/N90 判词作废）。
// [b28d-solo-CItemLock] VERDICT: AE —— SIG 载荷视图（unsigned characNo + m_locks[0x64]）+ POD 块拷贝。
#include "CItemLock/_saveData.inc"
}

// ORIG 0x8541a3e：清空锁表 / 待发列表，抽干空闲号池，复位计时器。
#include "CItemLock/_reset.inc"
}

}  // namespace item_lock

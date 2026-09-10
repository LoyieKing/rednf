// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMailBox/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// [b28d-38 判词] 本 TU 3 件最终状态：
//  RemoveMail 0x082f.. DIFF（p167 CLEAR 16/16；byte 层 N 封顶）
//  AddNewMail 0x082f.. DIFF（p167 余 make_pair 首参 const& 形：ORIG make_pair<unsigned int const&, CMail*&>）
//  CMail::MakePacket 0x082f.. DIFF（p167 CLEAR 45/45；byte 层 N 封顶）
// df_game_r WongWork::CMailBox / CMailBox::CMail 方法实现（U2 批次）。
// 依据 docs/class_func_reports/WongWork__CMailBox/ 与 WongWork__CMailBox__CMail/
// 各报告 + ORIG objdump 逐函数转写；类布局见 CGameManager.h（CMail 真实字段）。
// CMailBox 对象仍按 ORIG 偏移访问（+0x00..+0x90，与 CGameManager.cpp 中
// ctor/dtor/Init 的偏移视图一致）。

#include "CGameManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "Inven_Item.h"

#include <algorithm>
#include <cstring>
#include <list>
#include <map>
#include <vector>

namespace
{

template <typename T>
inline __attribute__((always_inline)) T& sig_at(void* base, int off)
{
    return *reinterpret_cast<T*>(static_cast<char*>(base) + off);
}

template <typename T>
inline __attribute__((always_inline)) const T& sig_at(const void* base, int off)
{
    return *reinterpret_cast<const T*>(static_cast<const char*>(base) + off);
}
#define SIG_OFF(T, base, OFF) \
    (*reinterpret_cast<T*>(const_cast<char*>(reinterpret_cast<const char*>(base)) + (OFF)))

typedef std::map<unsigned int, WongWork::CMailBox::CMail*> MailMap;
typedef std::map<int, bool> KeepMap;

const int OFF_LOADED      = 0x00;  // bool 加载状态（IsLoaded/SetLoadState）
const int OFF_LASTLOADIDX = 0x04;  // unsigned int（GetLastLoadIdx/SetLoadState 时间）
const int OFF_NOTLOADED   = 0x08;  // int（Get/SetNotLoadedMailCount）
const int OFF_MAILPOOL    = 0x10;  // StaticPool<CMail,20>
const int OFF_MAILMAP     = 0x40;  // map<unsigned int, CMail*>
const int OFF_KEEP        = 0x64;  // map<int,bool>（LetterKeepCount）
const int OFF_LASTLETTER  = 0x7c;  // unsigned int（GetLastLoadLetterIdx）
const int OFF_LOADCNT     = 0x80;  // int（Get/SetLoadedLetterCount）

// b22 桥清零（P201，Model22）：原「CAvatarItemMgr.h 与 CInventory.h 镜像声明
// 冲突」注记过时——CInventory.h:37 已改 include 权威 CAvatarItemMgr.h（本地
// 最小镜像退役，m_avatarItemMgr 即权威类型 0x84），GetAvatarItemMgrR() 直接
// 返回 const CAvatarItemMgr*，两个 asm 标签桥转真 const 成员调用
// （getJewelSocketData W 0x.../GetExpansionInfo，CAvatarItemMgr.h:58-59 声明，
// mangled 与 ORIG 逐字符一致）。

}  // namespace

namespace WongWork
{

// ============================================================================
// CMailBox::CMail —— 访问器（ORIG 反汇编直译）
// ============================================================================

// ORIG 0x085574d4 (W)
#include "CMailBox/GetLetterId.inc"
#include "CMailBox/GetPeriod.inc"
#include "CMailBox/IsTimeOver.inc"
#include "CMailBox/isUnlimit.inc"
#include "CMailBox/GetSenderName.inc"
#include "CMailBox/GetPackageMoney.inc"
#include "CMailBox/GetPackageItem.inc"
#include "CMailBox/GetSenderCharacNo.inc"
#include "CMailBox/t_398.inc"
}

// ORIG 0x0855759c (W)：m_packageType == 1
#include "CMailBox/isAvatarPackage.inc"
#include "CMailBox/isCreaturePackage.inc"
#include "CMailBox/ResetPackage.inc"
}

// ORIG 0x08551a36 (T)——docs/class_func_reports/WongWork__CMailBox__CMail/MakePacket.md
#include "CMailBox/MakePacket.inc"
}

// ============================================================================
// CMailBox —— 查询 / 计数（偏移视图与 CGameManager.cpp 一致）
// ============================================================================

// ORIG 0x08234566 (W)：map::size @+0x40
#include "CMailBox/GetRecvSize.inc"
}

// ORIG 0x08557922 (W)：StaticPool<CMail,20>::Remain @+0x10
#include "CMailBox/GetRemainSize.inc"
}

// ORIG 0x0823454c (W)
#include "CMailBox/GetLoadedLetterCount.inc"
#include "CMailBox/GetNotLoadedMailCount.inc"
#include "CMailBox/GetLastLoadIdx.inc"
}

// ORIG 0x0855784c (W)
#include "CMailBox/GetLastLoadLetterIdx.inc"
}

// ORIG 0x08557886 (W)：keep map::size @+0x64
#include "CMailBox/GetLetterKeepCount.inc"
}

// ORIG 0x08557866 (W)：clear keep map 并清零 lastLoadLetterIdx
#include "CMailBox/ClearLetterKeepCount.inc"
}

// ORIG 0x084ed222 (W)：map.insert(make_pair(letterId,true))，返回 insert 的 bool
//（新插入 true / 已存在 false；ORIG movzbl insert 返回 pair.second）。
#include "CMailBox/InsertLetterKeepCount.inc"
}

// ORIG 0x08557938 (W)
#include "CMailBox/IsLoaded.inc"
#include "CMailBox/SetLoadState.inc"
}

// ORIG 0x085577ac (W)：id==0 → false；否则在 loadLack 列表中查找
#include "CMailBox/FindPackageLoadLack.inc"
}

// ORIG 0x08557608 (W)：逐项拷贝 lack 列表前 n 项到 out（越界即停）
#include "CMailBox/GetPackageLoadLack.inc"
}

// ORIG 0x085522c4 (T, _ZNK)：map find，未找到返回 0
#include "CMailBox/GetMail.inc"
}

// ORIG 0x08552138 (T)——docs/class_func_reports/WongWork__CMailBox/RemoveMail.md
#include "CMailBox/RemoveMail.inc"
}

// ORIG 0x08551f5a (T)——docs/class_func_reports/WongWork__CMailBox/AddNewMail.md
// stAddNewMailInput 布局（AddNewMail 装载证据，与 CMailBoxHelper 写入一致）：
//   +0x00 mailIndex   +0x04 senderCharacNo  +0x08 packageMoney
//   +0x0c letterId    +0x10 remainSec       +0x14 packageType
//   +0x18 period      +0x1c unlimit(byte)   +0x20 const char* senderName
//   +0x24 Inven_Item*（可空）
#include "CMailBox/AddNewMail.inc"
}

}  // namespace WongWork

// ============================================================================
// user_creature::CCreatureMgr —— MakePacket 宠物期限链依赖（U2 批次补定义）
// ============================================================================

namespace user_creature
{

// ORIG 0x08339292 (_ZNK...16FindCreatureItemEi)：m_creatureItems map 查找。
#include "CMailBox/FindCreatureItem.inc"
}

// ORIG 0x0833d88c (_ZNK...18getRemainTempPeriodEii)
// docs/class_func_reports/user_creature__CCreatureMgr/getRemainTempPeriod.md：
// m_field30 有 key=uid：v<1 直接返回 v；否则 r=v-nowSec，r==0 时取 1。
// 无记录时委托装备宠的 GetRemainDate（BLOCKED: CCreatureItem 未建模 → -1，推断）。
#include "CMailBox/getRemainTempPeriod.inc"
}

}  // namespace user_creature

// ============================ CMailBox::CMail::._398 别名 ============================
// GCC 对 ctor/dtor 的 asm 别名（asm("...")）无效（硬限制）；匿名嵌套 struct 又按本 TU
// 行号计数命名（如 4._N），与 ORIG `5._398` 不一致。故在类内命名 `t_398`，用 `.set`
// 把自然 mangled `5t_398*` 强制改名为 ORIG 带点号 `5._398*`。
// P203 附注（b28 BL9）：`.set` 别名要成为 objdump 标注名，需 .globl+.type+.size
// 四件套（同址选名 binding 强者胜）且自然名 strcmp > 别名（`5t_398` 第 20 字节
// 't' > `5._398` 的 '.'）。尺寸照 ORIG：C1/C2 0x16。
asm(
    ".globl _ZN8WongWork8CMailBox5CMail5._398C1Ev\n"
    ".type _ZN8WongWork8CMailBox5CMail5._398C1Ev, @function\n"
    ".size _ZN8WongWork8CMailBox5CMail5._398C1Ev, 0x16\n"
    ".set _ZN8WongWork8CMailBox5CMail5._398C1Ev, _ZN8WongWork8CMailBox5CMail5t_398C1Ev\n"
    ".globl _ZN8WongWork8CMailBox5CMail5._398C2Ev\n"
    ".type _ZN8WongWork8CMailBox5CMail5._398C2Ev, @function\n"
    ".size _ZN8WongWork8CMailBox5CMail5._398C2Ev, 0x16\n"
    ".set _ZN8WongWork8CMailBox5CMail5._398C2Ev, _ZN8WongWork8CMailBox5CMail5t_398C2Ev");
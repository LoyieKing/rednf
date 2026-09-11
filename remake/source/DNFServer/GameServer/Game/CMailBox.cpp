#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMailBox/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// [AnonLane round8 2026-09-11] 原 8 个 `OFF_*` 偏移常量已删（死代码）：
// 本 TU 内**零使用点**（全文 grep 仅命中定义行），但 `-fkeep-static-consts`（默认开）
// 仍发射 `_ZN12_GLOBAL__N_1L<len>OFF_*E`（我方 r 段 8 条 only-NEW，ORIG 全库无对应名）。
// 访问点已全部改为具名成员/视图访问（§4.3 禁裸偏移），故直接删除（P154 无引用发明符号）。

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

#include "CMailBox/GetLetterId.inc"

#include "CMailBox/GetPeriod.inc"

#include "CMailBox/IsTimeOver.inc"

#include "CMailBox/isUnlimit.inc"

#include "CMailBox/GetSenderName.inc"

#include "CMailBox/GetPackageMoney.inc"

#include "CMailBox/GetPackageItem.inc"

#include "CMailBox/GetSenderCharacNo.inc"

#include "CMailBox/t_398.inc"


#include "CMailBox/isAvatarPackage.inc"

#include "CMailBox/isCreaturePackage.inc"

#include "CMailBox/ResetPackage.inc"


#include "CMailBox/MakePacket.inc"


// ============================================================================
// CMailBox —— 查询 / 计数（偏移视图与 CGameManager.cpp 一致）
// ============================================================================

#include "CMailBox/GetRecvSize.inc"


#include "CMailBox/GetRemainSize.inc"


#include "CMailBox/GetLoadedLetterCount.inc"

#include "CMailBox/GetNotLoadedMailCount.inc"

#include "CMailBox/GetLastLoadIdx.inc"


#include "CMailBox/GetLastLoadLetterIdx.inc"


#include "CMailBox/GetLetterKeepCount.inc"


#include "CMailBox/ClearLetterKeepCount.inc"


#include "CMailBox/InsertLetterKeepCount.inc"


#include "CMailBox/IsLoaded.inc"

#include "CMailBox/SetLoadState.inc"


#include "CMailBox/FindPackageLoadLack.inc"


#include "CMailBox/GetPackageLoadLack.inc"


#include "CMailBox/GetMail.inc"


#include "CMailBox/RemoveMail.inc"


#include "CMailBox/AddNewMail.inc"


}  // namespace WongWork

// ============================================================================
// user_creature::CCreatureMgr —— MakePacket 宠物期限链依赖（U2 批次补定义）
// ============================================================================

namespace user_creature
{

#include "CMailBox/FindCreatureItem.inc"


#include "CMailBox/getRemainTempPeriod.inc"


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
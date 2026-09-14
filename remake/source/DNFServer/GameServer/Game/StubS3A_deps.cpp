// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StubS3A_deps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StubS3A_deps.cpp —— StubS3A 主批 NEEDS_DECL 依赖 producer（StubS3A.S3A_DEPS 批）。
// 全部定义按 ORIG 汇编/报告忠实转写（C++98 / 32 位）。
// 需宿主登记 CMake（source/cmake/game/CMakeLists.txt）。
#include <string.h>

#include "GlobalData.h"      // GlobalData::{s_stream_pool,s_msgq_mgr}
#include "MsgQueueMgr.h"     // MsgQueueMgr::DB_Q / put

#include "CUser.h"           // CUser::{GetCurCharacInventoryRef,GetCharacExpandDataR,
                             // getCurCharacQuestW,IsAbleItem* 声明}
#include "CDataManager.h"    // G_CDataManager()->{find_item,find_quest,m_emotionTags}
#include "CItem.h"           // CItem::{is_stackable,GetItemType,GetAttachType}
#include "Inven_Item.h"      // Inven_Item::{m_field0,m_addInfo,m_amp}
#include "CItemAmplifier.h"  // stAmplifyOption_t::GetLock
#include "CTitleBook.h"      // item_lock::CItemLock::{IsAbleItemLock,IsAbleItemUnlock,IsAbleItemUnlockCancel}
#include "CInventory.h"      // CInventory::{GetInvenSlot,delete_item}（startUseMaterial）
#include "CraneMinigameManager.h"  // CraneMinigameManager::startUseMaterial 声明
#include "Quest.h"           // Quest::m_intDataList（+0x2c）

// 【A 档 GI 顺序对齐】下列头原本在文件中途 include（供 CItemMakingSkill ctor 使用）；
// 该 ctor 前置到 TU 最前后需在此可见。含 include guard ⇒ 中途处重复 include 无副作用。
#include "CItemMakingSkill.h"
#include "PvPTypes.h"                 // CMTRand（CItemMakingSkill ctor）
#include <ctime>

#include "StubS3A_deps/CItemMakingSkill.inc"


#pragma pack(push, 1)
// SIG_ITEM_MAKING_SKILL 协议视图（§4.3）：+0=characNo、+4=skills[11] ushort
struct ItemMakingSkillReqView
{
    // [M2S2 2026-09-11] P211/N76 反形异型化重测：ORIG 0x08545335 该 store 为
    //   RHS-first + `mov %eax,%edx` 中转；同 TU 的 RedeemListHeadView::m_characNo
    //   用同法（int→unsigned）实测翻正 ⇒ 此处同条件重试一次。
    unsigned int m_characNo;         // +0x00（RHS `pUser->get_charac_no(-1)` 为 int）
    // [b28d-63 本 TU] 宽度/符号：ORIG loadData 0x08545234 为 `movzwl; cmp $0x3e7,%ax; jle`
    //   ⇒ 载入零扩展 + 有符号 16 位比较（比较点用 (short) cast 表达，字段仍 unsigned
    //   以与 m_skillLevels 同型、令 else 直落 %ecx，见 loadData 判词）。
    unsigned short m_skills[0xb];    // +0x04（11 项）
};
#pragma pack(pop)

#include "StubS3A_deps/IsAbleItemLock.inc"

// [b28d-63 判词] DIFF→**NEAR 37=37**（p167 CLEAR）：拆嵌套 if + result 预清/判返后
//   逐段同形；唯一残差 = 两处 `call *%eax` vs ORIG `call *%edx`（N7 间接 call 目标
//   寄存器，4.4.7 恒 %eax；4.4.6 后端可翻，路由归 Main）。
// [b28d-63 虚槽核对·Main 全库排查] 本函数两处间接调用槽 = vptr+0xc：
//   _ZTV14CStackableItem@0x08c96ea0 +0x14 = 0x08514a84 = CStackableItem::GetItemType
//   （非 vptr+0x10 IsAvatarItem@0x08514a92 / vptr+0x14 IsCreatureItem@0x08514a9c）
//   ⇒ 槽号与语义一致，无取错。全 TU 另两处 = vptr+0x1c（_reset），
//   _ZTV16CItemMakingSkill@0x08c9c340 +0x24 = 0x085453ba = CItemMakingSkill::_reset ✓。

#include "StubS3A_deps/IsAbleItemUnlock.inc"


#include "StubS3A_deps/IsAbleItemUnlockCancel.inc"


#include "StubS3A_deps/_postCheckForceChangeGrowType4AheadQuest.inc"


// ---------------------------------------------------------------------------
// 5. trim(char*) —— 定义在 GameUtils.cpp（ORIG T 0x0829f0ef），此处不重复。
// ---------------------------------------------------------------------------

#include "StubS3A_deps/startUseMaterial.inc"


// ---------------------------------------------------------------------------
// 7. Redeem_Item::CRedeemItem::_reset —— ORIG T 0x085f7b6e（0x16）
#include "Redeem_Item__CRedeemItem.h"   // Redeem_Item::CRedeemItem
#include "CItemMakingSkill.h"

//    list clear（this+8）。
// 8. Redeem_Item::CRedeemItem::loadData(CUser*, char*) —— ORIG T 0x085f7830
//    （0x8c）：虚调 _reset；count=*(int*)(pData+4)；i=0..count-1：
//    *(int*)(pData + i*0x44 + 0xa) == 0 → alter()；否则 push_back(
//    Inven_Item_Expand_RedeemInfo 副本 @ pData + i*0x44 + 8)。返回 1。
// 9. Redeem_Item::CRedeemItem::_saveData(CUser*) —— ORIG T 0x085f7998
//    （0x1d5）：StreamPool::Acquire("Redeem_Item.cpp",0x108) → guard<<0x1f4
//    → <<GetUID → sig=GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>()（0x59c）→
//    null 则返 0 → memset 0x59c → characNo=get_charac_no(-1) → count=list
//    size → 逐项 rep movsl 0x11 dword → sig+8+i*0x44 → put(s_msgq_mgr,2)
//    → 返 1。
//    SIG_REDEEMINFO_LIST_TO_DB：characNo@0 / count@4 / entries[21]@8
//    （21 * 0x44 = 0x594；8 + 0x594 = 0x59c）。
// ---------------------------------------------------------------------------

#include "PvPTypes.h"                 // CMTRand（CItemMakingSkill ctor）
#include <ctime>
#include "StreamPool.h"

namespace Redeem_Item
{

#pragma pack(push, 1)
// [b28d-63 本 TU] Redeem 行视图：ORIG loadData 以 0x44 步长扫描，
//   check 在 +0xa（`mov 0xa(%eax,%edx,1),%eax`）；头部 count 在 +4
//   （`mov 0x4(%eax),%eax`）。具名视图复现 scaled/disp 寻址（P291/P311）。
union RedeemEntryView
{
    struct Head
    {
        char m_pad00[0xa];   // +0x00..+0x09
        int m_check;         // +0x0a
        char m_pad0e[0x36];  // +0x0e..+0x43
    } m_head;
    struct Body
    {
        char m_pad00[8];     // +0x00..+0x07
        char m_info[0x3c];   // +0x08（push_back 源地址，ORIG `lea 0x8(%eax),%edx`）
    } m_body;
};
struct RedeemListHeadView
{
    // [M2S2 2026-09-11] P211/N76 反形异型化：ORIG 0x085f7a63 对 characNo store 为
    //   RHS-first + `mov %eax,%edx` 中转（`mov %eax,%edx; mov -0x20(%ebp),%eax;
    //   mov %edx,(%eax)`），我方同型（int 字段 ← int 返回）恒 LHS-first。字段改
    //   unsigned（RHS 仍 int = get_charac_no 返回值）⇒ 异型转换中转，翻回 ORIG 序。
    unsigned int m_characNo; // +0x00（RHS `pUser->get_charac_no(-1)` 为 int）
    unsigned int m_count;    // +0x04
};
#pragma pack(pop)
static_assert(sizeof(RedeemEntryView) == 0x44, "Redeem entry stride drift");

#include "StubS3A_deps/_reset.inc"


#include "StubS3A_deps/loadData.inc"


#include "StubS3A_deps/_saveData.inc"


}  // namespace Redeem_Item

// ---------------------------------------------------------------------------
// 10. CItemMakingSkill 生命周期 + CData 纯虚覆盖
//     ctor 0x08545108：CData ctor → vptr → +0x1c = new CMTRand(time(0))。
//     dtor（D1 0x08545192）：置 vptr → delete m_rand → 置空 → 基类 dtor。
//     loadData 0x08545202：虚调 _reset；i=0..0xa：v=*(ushort*)(pData+4+i*2)；
//       v>0x3e7 → m_skillLevels[i]=0x3e8，否则=v。返回 1。
//     _saveData 0x08545276：StreamPool::Acquire("CItemMakingSkill.cpp",0x33)
//       → guard<<0x26e → <<GetUID → sig=GetInBuffer<SIG_ITEM_MAKING_SKILL>
//       （memset 0x1c）→ characNo=get_charac_no(-1) → skills[11] 复制
//       （this+6 ↔ sig+4，ushort ×11）→ put(s_msgq_mgr,2) → 返 1。
//     _reset 0x085453ba：memset(this+6, 0, 0x16)（m_skillLevels[11]）。
// ---------------------------------------------------------------------------

#include "StubS3A_deps/dtor_CItemMakingSkill.inc"


#include "StubS3A_deps/loadData_1.inc"


#include "StubS3A_deps/_saveData_1.inc"


#include "StubS3A_deps/_reset_1.inc"


// ===========================================================================
// [SweepCAutoPunish 2026-09-10] FreeType 收尾 API 占位定义（P210② 跨 TU 迁定义，
// Main 授权；原定义在 GlobalData.cpp，迁出后 GlobalData.cpp 只见声明 ⇒ 4.4 对
// CaptchaGen::~CaptchaGen 0x080de706 的 FT_Done_Face/FT_Done_FreeType 调用判
// 可能抛，复现 ORIG 的 EH landing：push esi/ebx + `_ZNSt6vectorIPvSaIS0_EED2Ev`
// cleanup + `_Unwind_Resume`。探针 /tmp/gdprobe/p2.cpp 实证：被调**同 TU 有定义**
// ⇒ 4.4 判 nothrow 不布 landing；**仅声明** ⇒ 布 landing。nm 全链接集扫描确认
// FT_Done_Face/FT_Done_FreeType 此前仅由 GlobalData.o 定义（无库定义）。
// 函数体逐字节照搬自 GlobalData.cpp。
// ===========================================================================
// [b24FIX · 真 FreeType 运行期绑定] 体改为 dlopen 转发（详见 GlobalData.cpp 同条注释）。
// 本 TU **仍持有定义**这一事实正是 CaptchaGen::~CaptchaGen 0x080de706 EH landing 的
// 成因（GlobalData.cpp 只见声明）⇒ 定义位置不动，仅替换函数体。
// [2026-09-14] 原自创的 `static void* ft32_sym_deps(const char*)` 已删除，改用
//  FT32Bind.h 的 FT32_BIND 宏（与 GlobalData.cpp 同一份，宏不产生符号）。
#include "FT32Bind.h"

extern "C" int FT_Done_Face(void* face)
{
    FT32_BIND(int, (void*));
    if (ft32_fn_ == 0)
        return 6;
    return ft32_fn_(face);
}

// FreeType 库内部收尾函数（ORIG 0x087c3c00 FT_Done_Library / 0x087dc6b0
// FT_Done_Memory）：定义仍在 GlobalData.cpp，此处仅取声明。
extern "C" int FT_Done_Library(void* library);
extern "C" int FT_Done_Memory(void* memory);

// ORIG 0x087bca50（idx49 报告）：library != 0 时读取其首字段 memory（offset 0），
// 依次调用 FT_Done_Library(library) 与 FT_Done_Memory(memory)；恒返 0。
// [b24FIX] 改为转发真 FT_Done_FreeType（其内部即为「取 library->memory → 释放各模块
// → 释放 memory」序列，与 ORIG 语义一致）。
extern "C" int FT_Done_FreeType(void* library)
{
    FT32_BIND(int, (void*));
    if (ft32_fn_ == 0)
        return 6;
    return ft32_fn_(library);
}

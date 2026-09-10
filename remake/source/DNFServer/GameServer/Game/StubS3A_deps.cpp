// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 StubS3A_deps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN16CItemMakingSkillC2Ev`（P327）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// [b28d-62] AE：time(0) 内联实参（临时槽供 const& 绑定）复现 4.1 new 前预取
//   %esi；48=48。ORIG 0x08545108
#include "StubS3A_deps/CItemMakingSkill.inc"
}

#pragma pack(push, 1)
// SIG_ITEM_MAKING_SKILL 协议视图（§4.3）：+0=characNo、+4=skills[11] ushort
struct ItemMakingSkillReqView
{
    int m_characNo;                  // +0x00
    // [b28d-63 本 TU] 宽度/符号：ORIG loadData 0x08545234 为 `movzwl; cmp $0x3e7,%ax; jle`
    //   ⇒ 载入零扩展 + 有符号 16 位比较（比较点用 (short) cast 表达，字段仍 unsigned
    //   以与 m_skillLevels 同型、令 else 直落 %ecx，见 loadData 判词）。
    unsigned short m_skills[0xb];    // +0x04（11 项）
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// 1. CUser::IsAbleItemLock(int,int) const —— ORIG T 0x08646a9c
//    报告 docs/class_func_reports/CUser/IsAbleItemLock.md 全文转写。
//    0x15=21 无物品/无 item 定义；0x13=19 业务不可锁；0x11=17 attach 型不可锁。
// [b28d-62] N82：p167 CLEAR。ORIG 0x08646a9c
#include "StubS3A_deps/IsAbleItemLock.inc"
}
// [b28d-63 判词] DIFF→**NEAR 37=37**（p167 CLEAR）：拆嵌套 if + result 预清/判返后
//   逐段同形；唯一残差 = 两处 `call *%eax` vs ORIG `call *%edx`（N7 间接 call 目标
//   寄存器，4.4.7 恒 %eax；4.4.6 后端可翻，路由归 Main）。
// [b28d-63 虚槽核对·Main 全库排查] 本函数两处间接调用槽 = vptr+0xc：
//   _ZTV14CStackableItem@0x08c96ea0 +0x14 = 0x08514a84 = CStackableItem::GetItemType
//   （非 vptr+0x10 IsAvatarItem@0x08514a92 / vptr+0x14 IsCreatureItem@0x08514a9c）
//   ⇒ 槽号与语义一致，无取错。全 TU 另两处 = vptr+0x1c（_reset），
//   _ZTV16CItemMakingSkill@0x08c9c340 +0x24 = 0x085453ba = CItemMakingSkill::_reset ✓。

// ---------------------------------------------------------------------------
// 2. CUser::IsAbleItemUnlock(int,int,int&) const —— ORIG T 0x08646bfe
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlock.md 汇编为准
//    （反编译 C 的 param_2/param_3 有笔误，栈槽 0xc=space / 0x10=slot / 0x14=outSec）。
#include "StubS3A_deps/IsAbleItemUnlock.inc"
}

// ---------------------------------------------------------------------------
// 3. CUser::IsAbleItemUnlockCancel(int,int) const —— ORIG T 0x08646cae
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlockCancel.md 全文转写。
#include "StubS3A_deps/IsAbleItemUnlockCancel.inc"
}

// ---------------------------------------------------------------------------
// 4. _postCheckForceChangeGrowType4AheadQuest(CUser*,Quest*) —— ORIG T 0x086686ed
//    objdump 0x086686ed..0x08668823 逐指令转写（无独立报告；调用方
//    _postCheckForceChangeGrowType 见 CUser.cpp:8029 区）。
//    quest->m_intDataList(+0x2c, vector<vector<int>>) 每组逐 questIdx：
//    find_quest 后递归本函数，并 getCurCharacQuestW()+4 处 CQuestClear
//    setClearedQuest(questIdx)。
// [b28d-62] N93/P50：ORIG 有 normal_iterator 默认构造 + vector<vector<int>>::
//   operator[]（先声明后赋值形），我方缺；85/82。ORIG 0x086686ed
// [b28d-63 本 TU] p167 三缺口实证（ORIG calls=15 / our=12）：
//   ① MISSING `normal_iterator::C1Ev` ⇒ `iterator it;` 先声明、内层 `it = begin()`（P50 复活）；
//   ② MISSING `vector<vector<int>>::operator[]` ×1 ⇒ 删 groups/group 引用缓存（P183），
//      内层 cond/body 各自重读 `quest->m_intDataList[gi]`；
//   ③ 删 cleared 具名指针（ORIG 无该槽，questIdx 经 %ebx 跨 getCurCharacQuestW 保活 ⇒ P0 内联）。
//   ⇒ 结果 **AE 85/85**（p167 仅剩同址别名 C1Ev/C2Ev 与自调用重定位伪差）；gi/questIdx
//   槽序按 P212 全头部声明（questIdx 先声明占深槽 -0x14、gi -0x10）复现。
#include "StubS3A_deps/_postCheckForceChangeGrowType4AheadQuest.inc"
}

// ---------------------------------------------------------------------------
// 5. trim(char*) —— 定义在 GameUtils.cpp（ORIG T 0x0829f0ef），此处不重复。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 6. CraneMinigameManager::startUseMaterial(CUser*,int) —— ORIG T 0x080ebe06
//    （无独立报告，d_aa_body.cpp objdump 0x080ebe06..0x080ebcbb 转写）。
//    this+0x10（m_field10）=材料 item idx；this+0x14 低 16 位符号扩展=数量。
//    0x11=材料不符；0x16=数量校验/删除失败；0=成功。
// [b28d-62] N82：p167 CLEAR。ORIG 0x080ebe06
// [b28d-63 本 TU] P0/§4.3：删 inven/invenW/needCnt 三个具名局部（ORIG 全内联，
//   帧 0x78→ORIG 0x64），裸偏移改具名字段 m_material(+0x10)/m_field14(+0x14)
//   （ORIG `mov 0x10(%eax),%eax` / `movzwl 0x14(%eax),%eax` 单条 disp）。
#include "StubS3A_deps/startUseMaterial.inc"
}

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
    int m_characNo;          // +0x00
    unsigned int m_count;    // +0x04
};
#pragma pack(pop)
static_assert(sizeof(RedeemEntryView) == 0x44, "Redeem entry stride drift");

#include "StubS3A_deps/_reset.inc"
}

// [b28d-62] 修：3 处 memcpy→直读/结构赋值（P167/N84），去 memcpy call 与
//   多余 ctor。ORIG 0x085f7830
// [b28d-63 本 TU] P0/P48/N84：ORIG 0x085f7848 把 pData 拷入具名局部 -0x10；
//   count 每轮重读（无局部）、check/entry 免槽、push_back 直接传 pData 内地址
//   （无 info 临时对象）；i 为 unsigned（cond `cmp;seta`）。原形多 4 局部 + 0x44
//   临时 + 3 callee-saved push（帧 0x7c vs ORIG 0x28）。
// [b28d-63 判词] 帧/指令数已与 ORIG 对齐（0x28；check 段 7=7、else 段 11=11），
//   verdict 仍 DIFF，残差全为后端形态：①ORIG 0x085f7857 提前载 buf 到 %edx 用
//   `mov 0xa(%eax,%edx,1)`，4.4 恒 `add -0x10(%ebp),%eax; mov 0xa(%eax)`；
//   ②ORIG 0x085f788c `lea 0x8(%eax),%edx`，4.4 恒 `add $0x8,%eax`（N102/N4 族，
//   union 视图/指针算术/数组下标三形均不翻）。gap≤0，非缺段。
#include "StubS3A_deps/loadData.inc"
}

// [b28d-62] 修：CStreamGuard operator*/-> 链 + 非 const list iterator + 结构赋值
//   （P103/P132/P96/N84）。ORIG 0x085f7998
// [b28d-63 判词] 已修：stream 具名局部删（P259 内联实参）、日志串改 ORIG 原文
//   "RedeemItem.cpp"（原 "Redeem_Item.cpp"）、头部字段改 RedeemListHeadView 视图
//   （count 由 lea 改 disp store）。残差 = ①槽号整体 +4（guard -0x30 vs ORIG -0x2c，
//   N95/N126 版本首槽基址常数）；②characNo/count/结构赋值三处 store 序
//   （ORIG RHS-first + %edx/%eax 中转 vs 4.4 LHS-first，N76；字段类型与 RHS cast
//   两旋钮实测不翻）；③循环体先算目标地址后取源（ORIG 相反）。gap≤0，非缺段。
#include "StubS3A_deps/_saveData.inc"
}

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
}

// [b28d-62] 修：memcpy→movzwl 直读（P167），去 memcpy call。ORIG 0x08545202
// [b28d-63 本 TU] P0/P48：ORIG 0x0854521a 把 pData 拷入具名局部 -0x10（源码有 `char* buf`），
//   循环体对源**两次独立直读**（if 与 else 各一次，无 v 槽）；改为视图 scaled 索引
//   `0x4(%eax,%edx,2)` 复现 ORIG 寻址（原 add 链 + v 槽 = 我方多 2 条）。
// [b28d-63 判词] DIFF→**NEAR 37=37**（gap 1→0）：视图字段 + `(short)` 比较点 cast
//   复现 `movzwl; cmp $0x3e7,%ax; jle`，else 直落 %ecx；唯一残差 = _reset 虚调
//   `call *%eax` vs ORIG `call *%edx`（N7）。
#include "StubS3A_deps/loadData_1.inc"
}

// [b28d-62] N18a：历史判词保留（死 regmove 两段，三档类型旋钮已穷举）。
//   ORIG 0x08545276
#include "StubS3A_deps/_saveData_1.inc"
}

#include "StubS3A_deps/_reset_1.inc"
}

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
extern "C" int FT_Done_Face(void* face)
{
    (void)face;
    return 0;
}

// FreeType 库内部收尾函数（ORIG 0x087c3c00 FT_Done_Library / 0x087dc6b0
// FT_Done_Memory）：定义仍在 GlobalData.cpp，此处仅取声明。
extern "C" int FT_Done_Library(void* library);
extern "C" int FT_Done_Memory(void* memory);

// ORIG 0x087bca50（idx49 报告）：library != 0 时读取其首字段 memory（offset 0），
// 依次调用 FT_Done_Library(library) 与 FT_Done_Memory(memory)；恒返 0。
extern "C" int FT_Done_FreeType(void* library)
{
    if (library != 0)
    {
        void* memory = *(void**)library;   // library->memory @ offset 0
        FT_Done_Library(library);
        FT_Done_Memory(memory);
    }
    return 0;
}

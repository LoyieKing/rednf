// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_D/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_D.cpp — game_master GM 命令批次 D（10 类）。
// 还原自 docs/class_func_reports/game_master__*/ 与 df_game_r objdump。
//
// 头文件 GameMasterCmd.h 为多批次共享头；b29 P5 批已按 §8.3 把 CIndependentDrop
// 的 4 个辅助声明由无参占位改为真实带参签名（零布局影响）：
//   - m_str0(+0x08) 占位 std::string；对 ORIG 实际为 POD(int/short/bool) 的类，
//     本 TU 一律以 reinterpret_cast 按偏移访问（见各类注释）。占位 string 由
//     隐式默认构造占用该槽（空串无堆分配），与 ORIG"不构造"仅差一次空串
//     初始化；对象经基类指针删除且基类无虚析构，派生成员析构与 ORIG 相同
//     均不发生。
//   - CIndependentDrop 真实布局 sizeof=0x3c：
//       +0x00 vptr +0x04 m_parser +0x08 bool m_all +0x09..0x0b pad
//       +0x0c std::set<int> m_monsterSet(0x18) +0x24 std::set<int> m_aiSet(0x18)
//     （ctor 0x082a87de objdump：两次 _ZNSt3setIiSt4lessIiESaIiEEC1Ev）
//     头内占位 m_str0/m_f1..m_f11 恰好同尺寸覆盖，本 TU 以 placement new 构造。
//   - CIndependentDrop 的 4 个带参辅助（ResultToStr/PrintItemDropList/printClient/
//     SendChatMsg）已由 GameMasterCmd.h 落真实签名，本 TU 以真实成员定义实现
//     （原 asm-label 自由函数桥已按 §8 清零）。
//
// 【b29 P3/P5 清桥符号账】AI 段原 `std::vector<stMonsterIndependentDrop_t> drops;`
// 默认构造 + extern "C" sret 桥，已改为 ORIG 源形
// `std::vector<stMonsterIndependentDrop_t> drops = ai->getIndepentDropItem();`
// （ORIG 0x084b1283 `lea -0x108(%ebp),%eax` 直传 sret，execute 区间 [0x084b0b4e,
// 0x084b14b8) 无任何 vector C1Ev 调用；ORIG 该族弱符号在 0x0838/0839 段属他 TU）
// ⇒ 本 TU 不再发射 new_allocator/allocator/Vector_base/Vector_impl/vector<...> 的
// C1Ev/C2Ev 共 10 个 ORIG 存在的 STL 弱符号（另 5 个 C5Ev ORIG 本无，属仅NEW 减少）。
// 本 TU compare 账面：identical 96→95、ae 162→158、near 0、diff 5（零新增 DIFF）、
// missing_in_game 69→64；-5 全为该弱符号「借用匹配」消失，非函数语义回归。
// 全量零 only_orig：这 10 个符号仍由 CharacterAIA2.cpp / STMonsterScript.cpp /
// CAICharacterList.cpp 三个 .o 发射（nm 全 .o 扫描实证）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CMonster.h"
#include "STMonsterScript.h"
#include "CParty.h"
#include "CBattle_Field.h"
#include "WarField.h"
#include "CMap.h"
#include "CDungeon.h"
#include "MonsterIndependentDrop.h"
#include "HeroMissionEvent.h"
#include "Arad_DataManager.h"
#include "ServerParameterScript.h"
#include "DisPatcherCore.h"
#include "CEventScriptMng.h"      // EventClassify::CEventScriptMng（桥清零）
#include "CEnvironment.h"
#include "DNFFunctionLib.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <new>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace game_master
{

// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（CParty.h 以 pad 占位、
// 未引入 CBattle_Field 类型；Main 裁定本组 TU 局部视图，与 CParty.cpp 口径一致）。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};

// ---------------------------------------------------------------------------
// asm 桥已清零（§4.5/§8）：
//   - CAICharacter::getIndepentDropItem()/GetName()（ORIG W 0x084b410e/0x084b4138）
//     改用 MonsterIndependentDrop.h 权威镜像类的真实成员声明（定义在
//     LinkMisc4_Data.cpp）；
//   - CIndependentDrop::{ResultToStr,PrintItemDropList,printClient,SendChatMsg}
//     改用 GameMasterCmd.h 的真实签名 + 本 TU 成员定义。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// 本地 POD 镜像：头内占位槽位按 ORIG 真实类型的具名访问（生成 disp 寻址，
// 与 ORIG mov 0x8(%eax) 形态一致；不改动共享头）。
// ---------------------------------------------------------------------------
namespace {

struct GmDBase { void* vptr; void* parser; };                // CCommand +0x00/+0x04

template <typename T>
struct GmDFld : GmDBase { T f08; };                          // 单 POD @+0x08

struct GmDID : GmDBase                                       // CIndependentDrop 0x3c
{
    bool all;                                                // +0x08
    char pad09[3];                                           // +0x09..0x0b
    std::set<int> mons;                                      // +0x0c
    std::set<int> ais;                                       // +0x24
};

// CHeroMissionCommand +0x0c/+0x10 的 enum 字段视图（P254）：ORIG execute
// 0x084b3e91/0x084b3e99 为 `mov disp(%eax),%eax; mov %eax,%esi/%ebx` 两段装载
// （4.1 对 enum 字段保留 eax→callee-saved 中转），4.4 对 unsigned 字段恒折单段。
enum GmDParamEnum { GmD_PARAM_ENUM_0 };
struct GmDHeroMissionCmdView : GmDBase
{
    void* str0;                                              // +0x08 std::string(COW)
    HeroMissionCondition::MissionNo::T no;                   // +0x0c
    GmDParamEnum param;                                      // +0x10
};

} // namespace

// ---------------------------------------------------------------------------
// EUC-KR 字面量（与 df_game_r .rodata 字节一致；源码以转义保真）
// ---------------------------------------------------------------------------
namespace {

// [AnonLane2 2026-09-11 匿名轴] 本处 9 个 EUC-KR 串常量（kFileCreatedFmt
// @0x08c80298 / kNoDropList @0x08c802b4 / kListDrop @0x08c802df / kDifficultyFmt
// @0x08c802ec / kDropRateFmt @0x08c802f7 / kTryCountFmt @0x08c80306 /
// kTotalDropRateFmt @0x08c8031a / kTotalCountFmt @0x08c80335 / kItemList
// @0x08c80342）已删：ORIG 侧全部为匿名 .rodata（对 9 名全库零命中），我方因
// -fkeep-static-consts 发射 game_master::(anonymous)::k* only-NEW 符号；
// 9 处使用点内联同字节字面量（转义保 EUC-KR 原字节）。

} // namespace

// ===========================================================================
// CItemInfoCmd — ORIG ctor 0x082a82ba / parse 0x084aa29e / execute 0x084aa2d2
// 字段：+0x08 std::string m_str0（物品名，真实 string 成员）。
// ===========================================================================

// ORIG ctor：基类 vptr → 自身 vptr(0x8c81a50)；SsC1Ev 构造 +0x08。

#include "GmCmds_D/parse.inc"


#include "GmCmds_D/execute.inc"
{
    struct InfoBuf { char m_b[0xff]; };                      // -0x157（ORIG char info[0xff]）
    InfoBuf info;
    CItem* item = G_CDataManager()->find_item(m_str0.c_str());
    if (item == 0)
    {
        info = InfoBuf();                                    // ORIG 分支内 memset 慢路
        sprintf(info.m_b, "cannot find item (%s) ", m_str0.c_str());
        PacketGuard packet;                                  // -0x50
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(info.m_b)));
        packet.put_str(info.m_b, static_cast<int>(strlen(info.m_b)));
        packet.finalize(true);
        GetUser()->Send(packet);
        return;
    }

    info = InfoBuf();                                        // ORIG else 分支 memset 慢路
    sprintf(info.m_b, "item_index(%d) ", item->get_index()); // ORIG get_index 0x8110c48
    {
        PacketGuard packet;                                  // -0x58（ORIG 析构在 else 块中段）
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(static_cast<int>(strlen(info.m_b)));
        packet.put_str(info.m_b, static_cast<int>(strlen(info.m_b)));
        packet.finalize(true);
        GetUser()->Send(packet);
    }

    const int itemIdx = item->get_index();                   // local_2c
    CUser* user = GetUser();                                 // local_28
    int count = 100;                                         // local_20
    // ORIG 0x084aa642/0x084aa657：getStackableLimit 两次独立调用（比较+赋值，P48 无 CSE）；
    // 嵌套 if 形（Ghidra C 同构）⇒ 末项发 ORIG 的 `cmp mem,%eax; setl %al; test; je`。
    if (item != 0)
    {
        if (item->is_stackable())
        {
            if (static_cast<CStackableItem*>(item)->getStackableLimit() < count)
            {
                count = static_cast<CStackableItem*>(item)->getStackableLimit();
            }
        }
    }
    ENUM_ITEMSPACE space;                                    // local_44（AddItem 出参）
    const int added = user->AddItem(itemIdx, count,
                                    static_cast<eItemAddReason>(6), space, 0);
    // ORIG `js 084aa72e(nop); body; jmp 084aa72f` = 4.1 if-join 尾岛 ⇒ P223 负条件
    // goto 到共享尾空标签（4.4 对正向 if 恒 jump-thread，给不出 nop 岛）。
    if (added < 0) goto trace_end;
    // P227 全表达式临时链：实参在 ctor 前求值并以 callee-saved 跨 ctor 保活
    // （ORIG getCurCharacName→%esi、NumberToString→%ebx，无栈槽往返）。
    cMyTrace(__PRETTY_FUNCTION__, 0x539, 0)(
        "%s:'%s' GM Mode Add Item(%d).",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), added);
    user->SendUpdateItemList(CUser::eSendTarget_1, space, added);
trace_end:;
}

// ===========================================================================
// CInvalidGold — ORIG ctor 0x082a83a8 / execute 0x084ab16e（sizeof=0x8 无字段）
// 行为：金钱短暂改为函数局部静态 invalidMoney(-10000 递减) 触发列表更新后
// 立即恢复原值。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c819a8)。

#include "GmCmds_D/execute_1.inc"


// ===========================================================================
// CLinkSystem — ORIG ctor 0x082a84ea / execute 0x084abdd0（sizeof=0x8 无字段；
// 仅 execute 无 parse）。副本 0x2aff 开关切换。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81888)。

#include "GmCmds_D/execute_2.inc"


// ===========================================================================
// CGetCashInven — ORIG ctor 0x082a85ca / parse 0x084ac80a / execute 0x084ac846
// 字段：+0x08 short（ORIG POD，头占位为 string；按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c817e0)，+0x08 不初始化。

#include "GmCmds_D/parse_1.inc"


#include "GmCmds_D/execute_3.inc"


// ===========================================================================
// CItemDropInDungeon — ORIG ctor 0x082a84b2 / parse 0x084abcb0 /
// execute 0x084abce8。字段：+0x08 int 物品 idx（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c818b8)。

#include "GmCmds_D/parse_2.inc"


#include "GmCmds_D/execute_4.inc"


// ===========================================================================
// CInGameEventSetDay — ORIG ctor 0x082a87a6 / parse 0x084b09d0 /
// execute 0x084b0a08。字段：+0x08 int day（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81630)。

#include "GmCmds_D/parse_3.inc"


#include "GmCmds_D/execute_5.inc"


// ===========================================================================
// CIndependentDrop — ORIG ctor 0x082a87de(sizeof=0x3c) / parse 0x084b0b08 /
// execute 0x084b0b4e / printClient 0x084b14b8 / SendChatMsg 0x084b15c4 /
// ResultToStr 0x084b165c / PrintItemDropList 0x084b1756。
// 真实布局见文件头注释。
// ===========================================================================

namespace {

// b28d-solo-GmCmds_D：P49 —— -O0 下 inline 访问器仍发真实 call 符号
// （p167 EXTRA：indMonsterSet×5/indAiSet×5/indAllFlag×2，ORIG 无）⇒ 改宏，
// 零 call 且寻址形与 ORIG 的 disp 访问一致。
#define GMD_IND_ALL(p)  (((GmDID*)(p))->all)
#define GMD_IND_MONS(p) (((GmDID*)(p))->mons)
#define GMD_IND_AIS(p)  (((GmDID*)(p))->ais)

} // namespace

// ORIG ctor 0x082a87de：基类 vptr(0x8c81600) 后对 +0x0c/+0x24 各构造一次
// set<int>（_ZNSt3setIiSt4lessIiESaIiEEC1Ev）；+0x08 bool 不初始化。

#include "GmCmds_D/parse_4.inc"


#include "GmCmds_D/execute_6.inc"


// ===========================================================================
// CIndependentDropByForce — ORIG ctor 0x082a8876 / parse 0x084b1f8c /
// execute 0x084b1fda。字段：+0x08 bool 强制掉落开关（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c815b8)。

#include "GmCmds_D/parse_5.inc"


#include "GmCmds_D/execute_7.inc"


// ===========================================================================
// CLevelUp — ORIG ctor 0x082a8956 / parse 0x084b388c / execute 0x084b38e2 /
// setLevel 0x084b395c / getLevel 0x084b398e。字段：+0x08 int 等级（POD）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c814f8)。

#include "GmCmds_D/parse_6.inc"


#include "GmCmds_D/execute_8.inc"


// ===========================================================================
// CHeroMissionCommand — ORIG ctor 0x082a89fe / parse 0x084b3db2 /
// execute 0x084b3e5e。字段：+0x08 string 子命令("set"/"reset")；
// +0x0c unsigned int missionNo；+0x10 unsigned int param（两个 POD 头未具名，
// 按偏移访问）。
// ===========================================================================

// ORIG ctor：基类 vptr(0x8c81480) + SsC1Ev 构造 +0x08。

#include "GmCmds_D/parse_7.inc"


#include "GmCmds_D/execute_9.inc"


// （namespace game_master 于文件尾闭合——成员定义体同属该命名空间）

// ---------------------------------------------------------------------------
// 本 TU 提供的 ORIG mangled 符号实体（真实成员定义；供 compare_tu 对比）。
// ---------------------------------------------------------------------------

#include "GmCmds_D/ResultToStr.inc"


#include "GmCmds_D/PrintItemDropList.inc"


#include "GmCmds_D/printClient.inc"


#include "GmCmds_D/SendChatMsg.inc"


#include "GmCmds_D/setLevel.inc"


#include "GmCmds_D/getLevel.inc"


} // namespace game_master
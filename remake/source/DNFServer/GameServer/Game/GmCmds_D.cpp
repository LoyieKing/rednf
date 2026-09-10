// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GmCmds_D/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG 0x08c80298 "%s파일이 생성되었습니다.\n"
const char kFileCreatedFmt[] =
    "%s\xc6\xc4\xc0\xcf\xc0\xcc \xbb\xfd\xbc\xba\xb5\xc7\xbe\xfa\xbd\xc0"
    "\xb4\xcf\xb4\xd9.\n";
// ORIG 0x08c802b4 "출력할 지정드랍 리스트가 없습니다.\n"（memcpy 0x24 字节含 NUL）
const char kNoDropList[] =
    "\xc3\xe2\xb7\xc2\xc7\xd2 \xc1\xf6\xc1\xa4\xb5\xe5\xb6\xf8 \xb8\xae\xbd"
    "\xba\xc6\xae\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\n";
// ORIG 0x08c802df "리스트 드랍\n"（memcpy 0xd 字节含 NUL）
const char kListDrop[] = "\xb8\xae\xbd\xba\xc6\xae \xb5\xe5\xb6\xf8\n";
// ORIG 0x08c802ec "난이도\t%d\n"
const char kDifficultyFmt[] = "\xb3\xad\xc0\xcc\xb5\xb5\t%d\n";
// ORIG 0x08c802f7 "드랍율(%%)\t%f\n"（sprintf 格式 %% → 字面 %）
const char kDropRateFmt[] = "\xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\n";
// ORIG 0x08c80306 "시도수\t%d\t%d\t%d\t%d\n"
const char kTryCountFmt[] = "\xbd\xc3\xb5\xb5\xbc\xf6\t%d\t%d\t%d\t%d\n";
// ORIG 0x08c8031a "총 드랍율(%%)\t%f\t%f\t%f\t%f\n"
const char kTotalDropRateFmt[] =
    "\xc3\xd1 \xb5\xe5\xb6\xf8\xc0\xb2(%%)\t%f\t%f\t%f\t%f\n";
// ORIG 0x08c80335 "총 개수\t%d\n\n"
const char kTotalCountFmt[] = "\xc3\xd1 \xb0\xb3\xbc\xf6\t%d\n\n";
// ORIG 0x08c80342 "아이템 리스트\n"（memcpy 0xf 字节含 NUL）
const char kItemList[] = "\xbe\xc6\xc0\xcc\xc5\xdb \xb8\xae\xbd\xba\xc6\xae\n";

} // namespace

// ===========================================================================
// CItemInfoCmd — ORIG ctor 0x082a82ba / parse 0x084aa29e / execute 0x084aa2d2
// 字段：+0x08 std::string m_str0（物品名，真实 string 成员）。
// ===========================================================================

// ORIG ctor：基类 vptr → 自身 vptr(0x8c81a50)；SsC1Ev 构造 +0x08。

// ORIG parse 0x084aa29e：trim(parser->args)；m_str0 = parser->args。
#include "GmCmds_D/parse.inc"
}

// ORIG execute 0x084aa2d2：按名查物品并回发查询结果包，找到则再 GM 发放一件。
// 【b28d-2026-09-10 重判】旧 N78/N82 判词作废（gap 59→0，助记符 306/306）：
// ① info 真实尺寸 = 0xff（ORIG 慢路 `mov $0xff,%ebx` + 槽 -0x157..-0x59 双证），
//    旧 `char info[0x100]` 多 1 字节（真 bug）；
// ② ORIG 两分支各一次 `memset(info,0,0xff)` 被 4.1 内联为对齐剥离慢路
//    （0x084aa30c/0x084aa48c 各 39 条）⇒ 源码写成 `info = InfoBuf();`（POD 整体赋值，
//    4.4 -O0 发同形慢路，探针 /tmp/b28d_gmd/probe2.cpp fB4/fB5），落点非 4 对齐槽；
// ③ 嵌套 if 形（Ghidra C 同构）复现末项 `cmp mem,%eax; setl %al; test; je`；
// ④ P227 全表达式临时链令 getCurCharacName→%esi、NumberToString→%ebx 跨 ctor 保活；
// ⑤ P223 负条件 goto 复现 `js <nop>; body; jmp join; nop; join:` 尾岛。
// 残余：AddItem 实参装载段 ORIG 提前 `mov -0x28(%ebp),%eax` 并以 %edx 中转 &space/count，
// 4.4 统一用 %eax（1 条 mov 位置差，P75/N7 跨版本寄存器分配，封顶）。
// p167：49=49 CLEAR（b28c-R26 复验；MISSING C2/D2 与 EXTRA C1/D1 为同址别名伪差，
//   P93，ORIG 0x0858dd4c/0x0858de80 同址）。
#include "GmCmds_D/execute.inc"
}

// ===========================================================================
// CInvalidGold — ORIG ctor 0x082a83a8 / execute 0x084ab16e（sizeof=0x8 无字段）
// 行为：金钱短暂改为函数局部静态 invalidMoney(-10000 递减) 触发列表更新后
// 立即恢复原值。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c819a8)。

// ORIG execute 0x084ab16e。
// 【b28d-solo-GmCmds_D 判词 AE】旧「N-P75 参数物化时刻」判词作废：末段
// set_money(invenW, oldMoney) 的 ORIG `mov -0xc(%ebp),%ebx`（receiver call 前
// 装载 callee-saved）经裸 register 中转局部复现（P231/P248④：值跨 call 保活、
// 4.4 不自升、ORIG 目标 %ebx；非 asm 绑定，P0 合规）。探针 /tmp/b28d_gmd/p2.cpp
// f4 与 fA/fC/f5/f6 对照：取址形（*(&old)/volatile）反把 old 挤到深槽。
#include "GmCmds_D/execute_1.inc"
}

// ===========================================================================
// CLinkSystem — ORIG ctor 0x082a84ea / execute 0x084abdd0（sizeof=0x8 无字段；
// 仅 execute 无 parse）。副本 0x2aff 开关切换。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81888)。

// ORIG execute 0x084abdd0。
#include "GmCmds_D/execute_2.inc"
}

// ===========================================================================
// CGetCashInven — ORIG ctor 0x082a85ca / parse 0x084ac80a / execute 0x084ac846
// 字段：+0x08 short（ORIG POD，头占位为 string；按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c817e0)，+0x08 不初始化。

// ORIG parse 0x084ac80a：trim(args)；*(short*)(this+8) = atoi(c_str(args))。
#include "GmCmds_D/parse_1.inc"
}

// ORIG execute 0x084ac846：空函数（push ebp; mov ebp,esp; pop ebp; ret）。
#include "GmCmds_D/execute_3.inc"
}

// ===========================================================================
// CItemDropInDungeon — ORIG ctor 0x082a84b2 / parse 0x084abcb0 /
// execute 0x084abce8。字段：+0x08 int 物品 idx（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c818b8)。

// ORIG parse 0x084abcb0：*(int*)(this+8) = atoi(c_str(trim(args)))。
#include "GmCmds_D/parse_2.inc"
}

// ORIG execute 0x084abce8：可堆叠或 avatar/creature 类型(idx 0x16/0x17) 物品跳过，
// 其余交队伍 drop_item_gm。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR；P0 表达式直读（无 reqIdx/skip 槽）、
// party==0 正向包裹消尾岛（P285/P223 变体）均已按 ORIG 复现。
#include "GmCmds_D/execute_4.inc"
}

// ===========================================================================
// CInGameEventSetDay — ORIG ctor 0x082a87a6 / parse 0x084b09d0 /
// execute 0x084b0a08。字段：+0x08 int day（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c81630)。

// ORIG parse 0x084b09d0：*(int*)(this+8) = atoi(c_str(trim(args)))。
#include "GmCmds_D/parse_3.inc"
}

// ORIG execute 0x084b0a08：event_script_mng->test_process_change_day(day)。
#include "GmCmds_D/execute_5.inc"
}

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

// ORIG parse 0x084b0b08：trim(args)；args=="all" → +0x08=1 否则 0。
#include "GmCmds_D/parse_4.inc"
}

// ORIG execute 0x084b0b4e。
// 【b28d-2026-09-10 重判】jcc 逐位交叉验证揪出 1 处条件反向真 bug：尾段
// `count == 0 ? memcpy(kNoDropList) : printClient+sprintf` 的块序写反（ORIG
// 0x084b140c `cmpl $0; je <memcpy>` = then=printClient 直落）⇒ 改 `if (count != 0)`
// 后 jcc 序列 23=23 全等（P272/P194）。另两处 P0 右值内联：getDungIndex 结果
// `movzwl %ax,%ebx` 直传（0x084b0b95）、field->GetCurrentMapInfo() 内联
// （0x084b0c84 无 field 具名槽）⇒ 助记符 619→617（ORIG 616，净 +1 为
// `mov $0x17318,%edx` 常量中转缺失等寄存器伪影）。
// 文件名分支反写为 `if (!GMD_IND_ALL(this))` 复现 ORIG `xor $0x1` 取反物化 +
// then=非 all 直落块序（P168a/P272，0x084b13b5）。
// 验收三件套（§6.2，orig 616>500）：① 助记符 617 vs 616（+1 寄存器伪影）；② p167 98=98（MISSING/EXTRA 仅
// C1/C2、D1/D2 同址别名，P93）；③ 全 TU call* 虚槽 + jcc 序列交叉验证 0 差。
// 残余为 4.4 槽复用 vs 4.1 独立槽的全帧槽号错位（N82）+ `mov $imm,%edx` 常量中转
// （N76/P238）等 20 个寄存器/块序伪影块，byte 层封顶（语义闭合：缓冲清零、
// hash_map/set/list/vector 迭代器 cv+后置增量、P183 去缓存、P49 宏化访问器均归位）。
// [b28c-R26 复验] p167 98=98 CLEAR（MISSING/EXTRA 仅 C1/C2、D1/D2 同址别名）；
//   gap 616/617 复核不变。
#include "GmCmds_D/execute_6.inc"
}

// ===========================================================================
// CIndependentDropByForce — ORIG ctor 0x082a8876 / parse 0x084b1f8c /
// execute 0x084b1fda。字段：+0x08 bool 强制掉落开关（POD，按偏移访问）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c815b8)。

// ORIG parse 0x084b1f8c：先清 0；trim(args)；args=="on" → 1。
#include "GmCmds_D/parse_5.inc"
}

// ORIG execute 0x084b1fda：回显开关状态并写入发起者 setForceDropFlag。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR；字段 bool 视图 + 无 flag 槽表达式直读
// （P0/P73）、fall-through=on 块序（P47）已按 ORIG 复现。
#include "GmCmds_D/execute_7.inc"
}

// ===========================================================================
// CLevelUp — ORIG ctor 0x082a8956 / parse 0x084b388c / execute 0x084b38e2 /
// setLevel 0x084b395c / getLevel 0x084b398e。字段：+0x08 int 等级（POD）。
// ===========================================================================

// ORIG ctor：仅基类 vptr(0x8c814f8)。

// ORIG parse 0x084b388c：parser 非空时 setLevel(StringToNumber(extra[1]))。
#include "GmCmds_D/parse_6.inc"
}

// ORIG execute 0x084b38e2：栈构 DisPatcher_DebugCommand 后委托设级。
#include "GmCmds_D/execute_8.inc"
}

// ===========================================================================
// CHeroMissionCommand — ORIG ctor 0x082a89fe / parse 0x084b3db2 /
// execute 0x084b3e5e。字段：+0x08 string 子命令("set"/"reset")；
// +0x0c unsigned int missionNo；+0x10 unsigned int param（两个 POD 头未具名，
// 按偏移访问）。
// ===========================================================================

// ORIG ctor：基类 vptr(0x8c81480) + SsC1Ev 构造 +0x08。

// ORIG parse 0x084b3db2：parser 非空时 extra[1]→字符串，extra[2]/extra[3] 经
// StringToNumber 写 +0x0c/+0x10。
#include "GmCmds_D/parse_7.inc"
}

// ORIG execute 0x084b3e5e："set"→processMission，"reset"→setEventData("")，
// 最后统一 sendMissionData 回发任务数据。
// 【b28d-solo-GmCmds_D 判词 AE】旧 N18a 判词作废：ORIG 两处
// `mov 0x10/0xc(%eax),%eax; mov %eax,%esi/%ebx` 两段装载经「enum 字段视图 +
// 裸 register 局部」复现（P254 档③：4.1 对 enum 字段保留 eax→callee-saved 中转；
// 4.4 对 unsigned 字段恒折单段）。探针 /tmp/b28d_gmd/p6.cpp g10 全等。
#include "GmCmds_D/execute_9.inc"
}

// （namespace game_master 于文件尾闭合——成员定义体同属该命名空间）

// ---------------------------------------------------------------------------
// 本 TU 提供的 ORIG mangled 符号实体（真实成员定义；供 compare_tu 对比）。
// ---------------------------------------------------------------------------

// ORIG ResultToStr 0x084b165c：逐条拷贝 stMonsterIndependentDrop_t，
// 按 copy+0x2c（m_rate[10] 主掉率）非 0 决定 PrintItemDropList 详细模式。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR（11=11）：循环前缓存 begin/end（P13）、
// `else if (copy.m_rate[10] != 0)` 二次 test/je（P184 死块照抄）均按 ORIG 复现。
#include "GmCmds_D/ResultToStr.inc"
}

// ORIG PrintItemDropList 0x084b1756：单条独立掉落统计文本追加到 out。
// 【b28d-solo-GmCmds_D 判词 N78/N82/P167】`={0}` rep stos 替代 ORIG 的内联
// memset 慢路（N78）；`if(!detail)` 反形（P168a②）、rate 第一次内联/第二次具名
// float（P252）、内层 begin/end 缓存（P13）、segRate 内联均已按 ORIG 复现。
// p167：EXTRA operator-> ×2 / MISSING operator* ×1（ORIG 单次 `*pit` + 内联 pair
// 拷贝；改用 pair 局部虽 p167 CLEAR 但会丢 ORIG 存在的 operator-> 弱符号
// 0x08193912 使 TU identical −1，故取当前形）。残差另含全帧槽号错位（N82）。
#include "GmCmds_D/PrintItemDropList.inc"
}

// ORIG printClient 0x084b14b8：put_header(0,0x13e)+byte(1)+两段 int/str 包发送。
#include "GmCmds_D/printClient.inc"
}

// ORIG SendChatMsg 0x084b15c4：聊天包(type=2, notify=0xc) 发起者私聊回显。
#include "GmCmds_D/SendChatMsg.inc"
}

// ORIG setLevel 0x084b395c：>70 截 70；<=0 归 1；其余原值。
#include "GmCmds_D/setLevel.inc"
}

// ORIG getLevel 0x084b398e。
#include "GmCmds_D/getLevel.inc"
}

} // namespace game_master
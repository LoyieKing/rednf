// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptLeafA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ScriptLeafA2.cpp — 脚本叶子依赖恢复（独占 TU，任务 ScriptLeafA2）
//
// 覆盖符号（全部经 ORIG df_game_r nm 核实；有报告者全文阅读后转写，
// 报告缺失三处以 objdump 逐指令转写并标注）：
//   convertTabTypeStringToEnum(char const*)                        T 0x08aa7d98
//   loadNPCCommonData(stNPCCommonData_t&, char const*)             T 0x089fafc5
//   stNPCCommonData_t::clear()                                     W 0x089fae9e
//   importRegenerationRandomOptionScript(RandomOptionItem_Regenerator&, char const*)
//                                                                  T 0x08a7e610
//   StrikerScript::importScript(char const*)                       T 0x08a9ddb2
//   StageMapList::loadList()                                       T 0x0836485c
//   StageMapList::compare(...)                                     T 0x08364848
//   APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript(
//       char const*, CActionPointEtcParameter&)                    T 0x08894996
//   eventReward::ImportRewardScript(char const*, char const*)      T 0x080ef3a2
//   importUpgradeTable(char const*, upgrade_table_t*)              T 0x08900c26
// [A16 round5 · C05 四范式复查留痕（fresh 19:38-19:39）] 本 TU 10 件非 identical
//   逐件机械扫描，结论：**无正向命中**。
//   ① F2（ORIG `cmp -N(%ebp),%eax` + setcc 内存直比）命中 0 处 ⇒ 无 P209 反向可用形；
//   ② 尾 fail 块（尾 `mov $0/$1,%eax`）仅 isBoundaryExpValue 命中 1 处（正常
//      `return false` 尾块，与残差无关，该件残差仅 1 条 cmp 操作数序 = N22）；
//   ③ 岛差 5 件**全部命中 C05③ 负例**（ORIG 岛目标重复 ⇒ 4.4 合并/跳板，源层不可控）：
//      StrikerScript::importScript 3 岛同目标 0x8a9de09×3（我方 0 岛，gap -9）、
//      eventReward::ImportRewardScript 2 岛 2 目标（4.4 epilogue-threading，gap -18）、
//      importAchievementRewardParameter 13 岛（0x889b05f×9，gap -12）、
//      importRegenerationRandomOptionScript 0x8a7e660×2（gap +3）、
//      importAdvanceAltarShopParameter 30 岛（8 个目标各重复 2~3 次，gap -6）
//      ⇒ 按 A28/Main 实测口径全部封 N，勿再烧轮次；
//   ④ 数据区解码假象（`0x48 9d 4?` 类荒谬 disp / movsb·cltd·clc 簇）本 TU 未命中。
//   [补·Main 细化口径] `test` 零比较子型（`unsigned`→`int` 可翻转 mem 直比⇔载寄存器）
//   亦复核：本 TU 10 件 ORIG 段 `mov -N(%ebp),%reg; test %reg,%reg` 对**全 0 处**，
//   我方 `cmpl $0x0,-N(%ebp)` 5 处均为两侧同形（StageMapList::loadList ×2、
//   importUpgradeTable ×1、importActionPointSystemEtcScript ×2）⇒ 无该子型命中。
//   [补·轴 B 清单（/tmp/r5/axisB_points.tsv）逐行按地址自校 —— 本 TU 两条**均为
//   序号配对假阳性，记 N 不改源码**（自校三条件见 Main 纪律）]：
//   · importActionPointSystemEtcScript（ORIG 0x08894996）：清单 a 行 setle→seta 的
//     ORIG 点 0x8894bc1 实为 `cmpl $0x4,-0x20(%ebp); setle`（days 循环），其真实同构
//     点是我方 @1d73 `cmpl $0x4,-0x1c(%ebp); setle`（**同为有符号，一致**）；清单 b 行
//     ORIG 0x8895564 `cmp -0x10(%ebp),%eax; setb` 与我方 @1b3c `cmp -0x3c(%ebp),%eax;
//     seta` 是**镜像无符号比较**（`i<limit` vs `limit>i`，等价）⇒ ②③ 不满足 ⇒ N。
//   · importUpgradeTable（ORIG 0x08900c26）：清单 a 行 ORIG 0x8901016 `cmpl $0x4,
//     -0x74(%ebp); setle` 的同构点是我方 @390b `cmpl $0x4,-0x50(%ebp); setle`（一致）；
//     清单 b 行 ORIG 0x8901039 `cmp -0x7c(%ebp),%eax; jbe` ↔ 我方 @3895 `cmp
//     -0x58(%ebp),%eax; setbe`（**同为无符号**，仅 jcc↔setcc 物化形差）⇒ 假配对 ⇒ N。
//   · 真正命中并已修者仅 importAdvanceAltarShopParameter 3 处 size_t→int（setg×3 = ORIG，
//     上下文同构 + ORIG 有符号 + 我方无符号，三条件齐备）。
// [b28d-solo-ScriptLeafA2] APSystem::CActionPointEx 隐式 C1ERKS_(0x08895ebc)/
// aSERKS_(0x0889603a) 判词 = **N（布局妥协，N18a/N76 族）**：
//   ORIG 逐字段粒度 int@0/int@4/short@8/char@a/short@c/char@e/f/10 → vector@14 →
//   int@20 → string@24 → string@28 → char@2c（无 +0x11..+0x13）。正解已探针定案：
//   packed 基类 {int,int,short,char}(0xb) + 派生字段（/tmp/b28d_a2/probe_layout.cpp
//   用例 C4 与 ORIG 0x08895ebc 逐条全等；C1 平铺 8 字段则寄存器形态不符）。
//   落地需把 CActionPointEx 改继承结构，与现有 m_field00/m_record/m_bytes02 平铺
//   视图（APSystemLeafC3.cpp ctor 4 处 + APSystemOnlyOrig.cpp m_record 多处消费）
//   不兼容 ⇒ 跨泳道同步改造；本泳道受「单 .cpp 铁律」约束不越界，已上报 Main
//   请求分派（附探针与改动清单）。现状 84/74、61/51 差 = union 块拷贝 vs ORIG
//   逐字段；语义等价。
//   本波复核（fresh）：C1ERKS_ 84/74、aSERKS_ 61/51 读数不变；ORIG 0x08895ebc
//   从 +0xc 起每成员重载 `mov 0xc(%ebp),%eax`/`mov 0x8(%ebp),%eax`（objdump 实证
//   0x08895ee3/0x08895ef1/0x08895efe/0x08895f0b/0x08895f18/0x08895f30/0x08895f3c），
//   我方 4.4 全程寄存器缓存 ⇒ N18a/N76 命中（隐式拷贝构造形不可源码控制）。
// [b28d-solo-ScriptLeafA2] new_allocator<stNPCCommonData_t::stOneBuff_t>::construct<
// stOneBuff_t>(T*, T&&) ORIG 0x08a04930 size 0x4e / 我方 0x43（gap 2）：
//   ORIG 把 move 源先拷进 3 个栈临时（0x08a04941/46/4c → -0x14/-0x10/-0xc）再拷进
//   placement-new 目标；我方 libstdc++ 头直接 `mov (%ebx),%edx; mov %edx,(%eax)`
//   直拷 ⇒ 库头内部构造形差异（N89/N130 族，禁改头）⇒ 判 N；语义等价。
//   stLevelUpRewardItemScript::clear()                             W 0x083711d6（无报告，objdump）
//   seriaRoomDecoAniEventScript::clear()                           W 0x083732a4（无报告，objdump）
//   rewardSubKey::{rewardSubKey,clear,operator<}                   W 0x080f1190/0x080f11de/0x080f11a2
//   rewardKey::{rewardKey,clear,operator<}                         W 0x080f11f0/0x080f12ac/0x080f121a
//       （objdump 逐指令转写；布局见 CDataManager.h rewardSubKey/rewardKey 区）
//
// 报告路径：
//   docs/class_func_reports/_lt_global_gt_/{convertTabTypeStringToEnum,
//       loadNPCCommonData,importRegenerationRandomOptionScript,
//       importUpgradeTable}.md（## 汇编 + ## 反编译 C 全文已读）
//   docs/class_func_reports/StrikerScript/importScript.md（同上）
//   docs/class_func_reports/StageMapList/{loadList,compare}.md（同上）
//   docs/class_func_reports/APSystem__CActionPointEtcParameter/
//       importActionPointSystemEtcScript.md（同上）
//   docs/class_func_reports/eventReward/ImportRewardScript.md（同上）
//   无报告三处先读调用者/同类报告：_lt_global_gt_/ImportLevelUpRewardItemScript.md、
//       _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md、
//       DisPatcher_RentAvatar/dispatch_sig.md、DisPatcher_BuyItem/get_data.md。
// ============================================================================

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>

#include "ScriptLeafA2/getChangeStatusType.inc"
#include "ScriptLeafA2/ConvertWeekdayStringToInt.inc"
#include "ScriptLeafA2/getItemRarity.inc"
#include "ScriptLeafA2/getCharacterJobToEnumValue.inc"

#include "CDataManager.h"        // stNPCCommonData_t / RandomOptionItem_Regenerator /
                                 // eventReward / StageMapList / APSystem 域权威声明
#include "APSystemTypes.h"       // CActionPointEx / _MedalRewardItem 权威布局
#include "StrikerScript.h"       // StrikerScript 及子类型（ContentsType 等）
#include "CItemAmplifier.h"      // upgrade_table_t / upgrade_info_t 权威布局
#include "CItem.h"               // CItem::GetMedalPrice 声明与 m_field14
#include "DNFLexWrapper.h"       // loadRDARScriptFile / ScanType / ScanInt / ScanFloat / ScanStr
#include "CUserProc.h"          // APSystem::_ActionGroupIndex 完整定义
#include "CEnvironment.h"        // less_TCHAR / SpinLock
#include "LogManager.h"          // LogManager::logFormat

// ORIG B 全局（定义 InitScriptProducers_5.cpp；同布局视图访问：
// STScriptFileList +0x00 即 std::map<int,std::string>，sizeof 0x18）。
namespace
{
class StageScriptFileListView
{
public:
    std::map<int, std::string> m_fileMap;   // +0x00
};
}  // namespace
extern StageScriptFileListView g_stageMapScriptFileList;    // B 0x09494e20
extern char g_npcScriptBaseDirectory[0x100];                // B 0x094fe680

// ============================================================================
// convertTabTypeStringToEnum —— ORIG T 0x08aa7d98
// 报告 _lt_global_gt_/convertTabTypeStringToEnum.md 全文转写。
// 函数级静态 __lookup(map<char const*,int,less_TCHAR>)/__init_lock(SpinLock)/
// __inited 三件套：首调登记 general/specific/despair/event → 0x11/0x14/0x17/0x1a；
// 之后查表命中返回 0..3，未命中返回 4。
// ============================================================================
#include "ScriptLeafA2/convertTabTypeStringToEnum.inc"
}


// ============================================================================
// stNPCCommonData_t::clear —— ORIG W 0x089fae9e（objdump 全文核实）：
// 仅清标量——循环 i=0..9 写 favorExp[i]（movw $0,0x2(%eax,%edx,2)，cmpl $0x9/setle），
// 再按 +0x00→+0x34→+0x30→+0x32 顺序清四个标量；无任何容器 clear。
// ============================================================================
#include "ScriptLeafA2/clear.inc"
}

// ============================================================================
// loadNPCCommonData —— ORIG T 0x089fafc5
// 报告 _lt_global_gt_/loadNPCCommonData.md 全文转写。
// 返回：0x3c=脚本打开失败；其余路径一律 0（解析失败与干净 EOF 同值，ORIG 如此）。
// ============================================================================
#include "ScriptLeafA2/loadNPCCommonData.inc"
}

// ---- ORIG 全局：g_RegenerationRandomOptionScriptBaseDirectory
//      （B 0x095005c0 size 0x100，CU=RDARScriptRegeneration_ROI.cpp，与本函数同 TU）----
// char[0x100] 零初始化脚本目录缓冲（init 流程填充）。ORIG 全二进制零指令、零数据
// 引用（二进制扫描唯一命中在 .symtab 自身）⇒ 死数据；补定义对齐符号账
// （REAL_GAP_SYMBOL），不新增消费点（importRegenerationRandomOptionScript 的路径
// 参数仍按 ORIG 0x08a7e610 实参字面量 "Etc/RandomOption/..." 形式）。
// 位置说明（P327）：本 TU 首符号保持 convertTabTypeStringToEnum（ORIG 另一 TU 的
// GI 键名），故不把本变量前移；ORIG 的
// `_GLOBAL__I_g_RegenerationRandomOptionScriptBaseDirectory`（0x08a7ea66）与本 TU
// 键名互斥，属结构性 REORDER_ONLY。
char g_RegenerationRandomOptionScriptBaseDirectory[0x100];

// ============================================================================
// importRegenerationRandomOptionScript —— ORIG T 0x08a7e610
// 报告 _lt_global_gt_/importRegenerationRandomOptionScript.md 全文转写。
// 返回：0=成功/文件打开失败（ORIG 同值）；0x1d=[choose part] 首扫描失败；
//       0x5d=[regeneration price] 组重复。
// ============================================================================
#include "ScriptLeafA2/importRegenerationRandomOptionScript.inc"
}

// ============================================================================
// StrikerScript::importScript —— ORIG T 0x08a9ddb2
// 报告 StrikerScript/importScript.md 全文转写。成功置 m_loaded=1 并返回 true。
// ============================================================================
#include "ScriptLeafA2/importScript.inc"
}

// ============================================================================
// StageMapList::compare —— ORIG T 0x08364848（比较 +0 uniqueId）。
// ============================================================================
#include "ScriptLeafA2/compare.inc"
}

// ---- stagemap 域镜像（布局取自 loadList 报告汇编偏移；类型 ORIG 真实存在，
//      完整建模归 stagemap 域批次）----
namespace stagemap
{

struct StageData
{
    int m_field0;                            // +0x00
    int m_field4;                            // +0x04 (ORIG StageData stride 0x10)
    int m_field8;                            // +0x08
    int m_fieldC;                            // +0x0c
};

struct DistrictData
{
    int m_field0;                            // +0x00
    std::vector<StageData> m_vec4;           // +0x04 (ORIG StageData vector)
    std::string m_appendImage;              // +0x10 (ORIG std::string size 4)
    int m_field14;                           // +0x14
};

struct AreaData
{
    int m_field0;                            // +0x00
    char m_pad04[0x1c];                      // +0x04
    std::vector<DistrictData> m_vec20;       // +0x20
    int m_field2c;                           // +0x2c
};

class StageMapScript
{
public:
    StageMapScript();                        // ORIG W 0x08373488
    ~StageMapScript();                       // ORIG W 0x0837f782
    void clear();                            // ORIG W 0x08373428

    char m_pad00[0x14];                      // +0x00（未还原字段区）
    std::vector<AreaData> m_areaVec;         // +0x14
};

}  // namespace stagemap

// ORIG T 0x088a9f28（定义待 stagemap 脚本域批次；本 TU 仅声明引用）
#include "ScriptLeafA2/importStageMapScript.inc"
#include "ScriptLeafA2/loadList.inc"
}

// ============================================================================
// APSystem::CActionPointEtcParameter::importActionPointSystemEtcScript
// ORIG T 0x08894996（静态成员）。报告
// APSystem__CActionPointEtcParameter/importActionPointSystemEtcScript.md 全文转写。
// 返回 false=成功/文件失败；错误路径亦为 false（调用方仅判零，语义兼容；
// ORIG 各行级错误码见注释）。
// ============================================================================
namespace APSystem
{

// [b28d-P330 int] ORIG 返回 = **错误行号 int**（callee 尾 `mov %ebx,%eax; ret`，
// ebx 写点含 `mov $0x57..$0xfa,%ebx` 14 处行号常量、0 引用 ×2；caller 4B 全
// store 无 movzbl）。旧 bool 声明把行号截成 0/1 ⇒ trace "%d line" 日志值错。
#include "ScriptLeafA2/importActionPointSystemEtcScript.inc"
}

}  // namespace APSystem

// ============================================================================
// eventReward::ImportRewardScript —— ORIG T 0x080ef3a2
// 报告 eventReward/ImportRewardScript.md 全文转写。
// 令牌派发表为函数级静态 __lookup/__init_lock/__inited 三件套
// （[/reward]=0x8f [job type]=0x93 [sub type]=0xa3 [option]=0xaf
//   [equipment item]=0xb9 [stackable item]=0xd8 [/sub type]=0xfc
//   [/job type]=0x102 [/option]=0x107；未知 -1 忽略）。
// 返回：1=[/reward] 收尾或自然 EOF 且最后 ok=true；0=文件/解析失败
// （自然 EOF 返回最后一次 ScanInt 的 ok 态，ORIG ebx=local_18d 同语义）。
// ============================================================================
#include "ScriptLeafA2/ImportRewardScript.inc"
}

// ============================================================================
// importUpgradeTable —— ORIG T 0x08900c26
// 报告 _lt_global_gt_/importUpgradeTable.md 全文转写。
// 返回 1=成功（含 EOF）；0=失败（文件打开失败/关键扫描失败/键重复/异常捕获）。
//
// 【batch-15/16 验收标注（§6.2 口径）】orig 1119 / ours 1166（gap -132 → -47）。
//   ① 调用点差集（P167）**已归零**：direct 138 ↔ 136，missing 14 / extra 12 全部
//      是 C2/D2 ↔ C1/D1 同体构造/析构符号名对称差 + 1 个 _Unwind_Resume，
//      无任何真实调用点缺口；indirect 0/0。
//   ② 段边界切片法（difflib 在 N82 帧号重排下不可信）逐 token 段核对，修掉两处
//      真语义偏差：[cost weight by upgrade level] 的取数通道（ORIG 用
//      ScanInt(bool*) 返回值 + 循环头 ok 复位，非 ScanInt(int*) 出参形）、
//      [amplification const] 循环入口缺 `ok = true` 复位（orig local_cb）；
//      并删除匿名 namespace 的 inline helper（-O0 逐点发 call，见 N-I 延伸）。
//   ③ 余 -47 集中在三处已定位的 4.1 专属形：0x089003fa 的守卫 bool 汇聚岛
//      （实测命名 bool 反而 +12，已回退）、0x08900d5a 的双段 catch
//      （begin_catch/end_catch + 清理后 rethrow 的 EH 展开表形）、
//      0x0890124x 的成功出口 jmp/nop 中继 ⇒ N3/N82/setcc-fold/跨版本 EH
//      命中 ⇒ 判**语义完成、byte 层封顶**。
//   ④ 三处「看起来更像 ORIG 但实测更远」的改写已回退并在原地留数值记录
//      （get_upgrade_info 逐行重发 gain 471、dword 下标视图 326、
//        命名 bool 汇聚岛 1177），避免后续批次重试。
// ============================================================================
// upgrade_info_t 的表行写入使用**真实字段下标直访**（batch-15 N-I 延伸：
// 匿名 namespace 的 inline helper 在 -O0 下同样逐点发 call，不豁免）；偏移→字段
// 映射按 upgrade_info_t 已建分段具名成员（m_prob00[8]/m_prob20[3]/+0x2c..34 三项/
// m_w38[5]/m_w4c[4]/m_w5c[4]，合计恰 0x6c）。

// [P248 档①] 返回 bool（全部 return 为常量 0/1 ⇒ 机器码与 int 版逐条相同）。
#include "ScriptLeafA2/importUpgradeTable.inc"
}

// ============================================================================
// stLevelUpRewardItemScript::clear —— ORIG W 0x083711d6
// 【报告缺失】无专属函数报告；已先读调用者报告
// _lt_global_gt_/ImportLevelUpRewardItemScript.md 后按 nm 地址 objdump 转写：
//   83711dc mov 0x8(%ebp),%eax ; push eax
//   83711e2 call std::map<LevelUpRewardItemTableKey,LevelUpRewardItemData>::clear
// 即 m_map0.clear()。
// ============================================================================
#include "ScriptLeafA2/clear_1.inc"
}

// ============================================================================
// seriaRoomDecoAniEventScript::clear —— ORIG W 0x083732a4
// 【报告缺失】无专属函数报告；已先读同类报告
// _lt_global_gt_/importSeriaRoomAnimationDecoEvent.md 与 ORIG ctor 0x083731ca
// （objdump）确认五成员布局后转写：
//   vector@+0x00 clear / map@+0x0c clear / map@+0x24 clear /
//   map@+0x40 clear / string@+0x3c clear。
// ============================================================================
#include "ScriptLeafA2/clear_2.inc"
}

// ============================================================================
// CItem::GetMedalPrice —— ORIG W 0x0822c862
// 【报告缺失】无专属函数报告；已先读调用者报告
// DisPatcher_RentAvatar/dispatch_sig.md 与 DisPatcher_BuyItem/get_data.md
// （与 GetCashPrice/get_price 并列的价格访问器）后按 nm 地址 objdump 转写：
//   mov 0x8(%ebp),%eax ; mov 0x14(%eax),%eax ; ret —— 返回 this+0x14 int。
// ============================================================================
#include "ScriptLeafA2/GetMedalPrice.inc"
}

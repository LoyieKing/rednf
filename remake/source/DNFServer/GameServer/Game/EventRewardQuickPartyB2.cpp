// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventRewardQuickPartyB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// EventRewardQuickPartyB2.cpp — 独占 TU（B2 批：eventReward / QuickParty /
// ContentsType 叶子符号恢复）。
//
// 覆盖 ORIG 符号（df_game_r nm 实证，全部 T；重建侧此前无任何定义）：
//   _ZN11eventReward13getSubKeyTypeERSs                                      0x080ef0c0
//   _ZN11eventReward13defaultOptionEcR8itemData                               0x080ef25c
//   _ZN12ContentsType9getStringENS_1TE                                        0x08ac3434
//   _ZN12ContentsType7getTypeEPKc                                             0x08ac34a4
//   _ZN10QuickParty23getRandomBuffTypeByNameERKSs                             0x08a6e100
//   _ZN10QuickParty22importQuickPartyScriptEPKcPNS_14QuickPartyDataE          0x08a6e282
//
// 语义来源（每函数已完整阅读报告 ##汇编 + ##反编译 C 后逐指令转写）：
//   docs/class_func_reports/eventReward/{getSubKeyType,getSubKeyAction,
//                                        getOptionType,defaultOption}.md
//   docs/class_func_reports/ContentsType/{getString,getType}.md
//   docs/class_func_reports/QuickParty/{getRandomBuffTypeByName,
//                                        importQuickPartyScript}.md
// 注：PowerParameterScript::clear（W 0x0837187a）按 Main 归属由
//     CharacterAIA2.cpp 提供，本 TU 不涉及。
// ============================================================================

#include <map>
#include <string>

#include "CDataManager.h"             // eventReward / itemData
#include "CQuickPartyRewardManager.h" // namespace QuickParty / QuickPartyData / import 声明
#include "CEnvironment.h"             // less_TCHAR / SpinLock
#include "DNFLexWrapper.h"            // loadRDARScriptFile / ScanType / ScanInt / ScanStr
#include "StrikerScript.h"            // ContentsType

// ===========================================================================
// 1) eventReward 解析 helper 族（ORIG 非静态成员；this 不参与逻辑）
// ---------------------------------------------------------------------------
// itemData 真实布局见 itemData.h（+0/+1/+2 三字节 + +0x04/+0x08 int，
// sizeof 0x0c）；ORIG 以 movb/movzbl 访问 +0/+1/+2 字节、movl 访问 +4/+8。
// ===========================================================================


// ORIG T 0x080ef0c0：[sub type] 行名 → 子键类型。
#include "EventRewardQuickPartyB2/getSubKeyType.inc"
}

// ORIG T 0x080ef128：[/sub key] 行名 → 发放动作（equip=2 / inven=1 / postal=3）。
#include "EventRewardQuickPartyB2/getSubKeyAction.inc"
}

// ORIG T 0x080ef190：[option] 区选项名 → itemData 字节域写入。
// 返回 false 当且仅当 "upgrade" 的整数扫描失败；未知名返回 true 且不写。
#include "EventRewardQuickPartyB2/getOptionType.inc"
}

// ORIG W 0x080ef25c：动作 2（postal）时重置 ROI 域并置 m_count=1；
// 尾部统一保证强化字节非 0（低字节为 0 → 置 1）。
#include "EventRewardQuickPartyB2/defaultOption.inc"
}

// ===========================================================================
// 2) ContentsType（ORIG 全局类，静态成员）
// ===========================================================================

// ORIG T 0x08ac3434：T(1..12) → 段名字符串；越界/0 → ""（跳转表 ja 0xc 实证）。
#include "EventRewardQuickPartyB2/getString.inc"
}

// ORIG T 0x08ac34a4：线性 i=0..12，双构串后 operator==（name 串 vs getString 串），
// 命中返回 i；穷尽未命中返回 0。
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（N93/N96 族封顶，语义等价）。
// 残差三源均跨版本结构性：①4.1 把 string 单参构造展开的 allocator 临时析构推迟
// 到块尾（槽 -0x15/-0xd），4.4 在 string 构造后立即 ~allocator（显式 allocator
// 局部会新增 ORIG 无的具名槽，P0 禁）；②if(current==expected)return i; 的 CFG：
// ORIG 把 operator== 结果留 %ebx、析构两串后 test %bl；4.4 在测试点即分支并以
// %ebx 作 cleanup 路径标志（多 mov $0/$1,%ebx + 两处 test/reload）；
// ③块级对象槽方向相反（N93）：ORIG expected@-0x1c/current@-0x14，4.4 相反
// （current@-0x18/expected@-0x14），帧 0x30 vs 0x20。
// 实测：continue 形 / 函数级 i / 临时表达式三写法 our_insns 恒 109（ORIG 96）；
// p167 15 vs 16 调用点、0 MISSING（仅 +1 _Unwind_Resume，EH 共享差）。
#include "EventRewardQuickPartyB2/getType.inc"
}

// ===========================================================================
// 3) QuickParty 脚本导入族
// ===========================================================================

namespace QuickParty
{

namespace
{

// importQuickPartyScript 段名 id（ORIG __lookup 表值实证）。
enum
{
    QPSEC_QUICK_PARTY_END    = 0x3e,   // [/quick party]
    QPSEC_UNABLE             = 0x41,   // [unable to quickparty]
    QPSEC_LEVEL_SECTION      = 0x4d,   // [level section]
    QPSEC_BASIC_REWARD       = 0x59,   // [basic reward]
    QPSEC_RANDOM_REWARD      = 0x8f,   // [random reward]
    QPSEC_RANDOM_REWARD_PROB = 0xb8,   // [random reward prob]
    QPSEC_REQUIRE_PARTY_NUM  = 0xce    // [require party num]
};

}  // unnamed namespace

// ORIG T 0x08a6e100：脚本 buff 名 → RandomBuffType；未知 → 11（0xb）。
RandomBuffType getRandomBuffTypeByName(const std::string& name)
{
    RandomBuffType type = (RandomBuffType)0xb;
    if      (name == "[increase exp]")       type = RANDOM_BUFF_INCREASE_EXP;
    else if (name == "[increase stats]")     type = RANDOM_BUFF_INCREASE_STATS;
    else if (name == "[increase speed]")     type = RANDOM_BUFF_INCREASE_SPEED;
    else if (name == "[increase dropprob]")  type = RANDOM_BUFF_INCREASE_DROPPROB;
    else if (name == "[increase allattack]") type = RANDOM_BUFF_INCREASE_ALLATTACK;
    else if (name == "[increase allresist]") type = RANDOM_BUFF_INCREASE_ALLRESIST;
    else if (name == "[increase broken]")    type = RANDOM_BUFF_INCREASE_BROKEN;
    else if (name == "[increase mobhp]")     type = RANDOM_BUFF_INCREASE_MOBHP;
    else if (name == "[increase antidevil]") type = RANDOM_BUFF_INCREASE_ANTIDEVIL;
    else if (name == "[increase coinlimit]") type = RANDOM_BUFF_INCREASE_COINLIMIT;
    else if (name == "[return ticket]")      type = RANDOM_BUFF_RETURN_TICKET;
    return type;
}

// ORIG T 0x08a6e282。双遍结构（ORIG 函数级静态 __lookup/__init_lock/__inited，
// SpinLock 双检建表）：首令牌触发 7 个 (段名,id) 登记后回到派发块重新查表，
// 当前令牌被正常处理（与 importTitleBookEtcScript 同构）。主循环仅以
// [/quick party]（返回 true）或主扫描失败（返回 false）退出；节内子扫描失败
// 一律回到主循环继续取令牌；未知令牌（id==-1）跳过。
//
// 各节写入目标（ORIG 偏移 ↔ QuickPartyData 具名字段，CQuickPartyRewardManager.h）：
//   [level section]        → +0x00 m_vec00（vector<int>）
//   [unable to quickparty] → +0x0c m_dungeonIdxList（vector<int>）
//   [increase gold card]   → +0x18 BasicReward.m_levelDatas（LevelData{率%,档}）
//   [hell ratio]           → +0x30/0x34 m_basicReward.m_field18/m_field1c、
//                            +0x24 BasicReward.m_hellDatas（LevelDataHell）
//   [reward]               → +0x38 m_randomRewards map insert（键=buff 类型）
//   [random reward prob]   → +0x50 m_probs[type][i]（i<7；type≤10 才写）
//   [require party num]    → m_minCounts[dungeonType][diff]
//                            （QuickPartyData +0x1a0 区，行 stride 0x10；
//                            ORIG 寻址 (type*4+diff+0x68)*4 完全一致）
// b28d-solo-EventRewardQuickPartyB2 判词：DIFF（§6.2 三件套达标，byte 层 N 封顶）。
// 已修真缺口：①7 段注册由「循环头统一插入」改 ORIG 的 case 内 fall-through 链
//   （含 case 0 哨兵、goto re_lookup 双检、switch 后 leave+__inited+goto）——
//   消除 pair 值构造/make_pair/临时 ~pair 共 10 类 EXTRA，改 map::value_type
//   引用构造（pair<const char* const,int>::pair<char const(&)[N],int&&>）；
//   ②补 token/value 的 Ss::clear、LevelData entry 提为函数级复用槽、lv 复用
//   -0xf8、ok=ScanStr 两跳槽往返、ScanInt(&lv) 复用；③[reward] 的
//   !=+continue 改 == 包裹（消 operator!= EXTRA / operator== MISSING）；
//   ④while(ScanType) 改 for+if(!ScanType)break（P164 抗循环旋转）。
// 验收：p167 调用点 97=97 CLEAR（0 missing / 0 extra）；栈槽集 42/42 逐项全等；
// gap=2（ORIG 740 / ours 742）。归一化残差 19 行仅剩：LevelData::C1 后 4.4
// EH 边界 jmp+nop（P223/N121 族）、循环内 if(!ok)break 的 je+jmp vs 4.1 jne
// （N122 族）、while 条件块物理位置差（difflib 对齐）。语义与 ORIG 逐块一致。
// 依赖：LevelDataRandomReward::m_field0 int→RandomBuffType（.h 改动，R2 已广播）。
#include "EventRewardQuickPartyB2/importQuickPartyScript.inc"
}

}  // namespace QuickParty

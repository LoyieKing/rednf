// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— InitScriptProducers_3.cpp
//
// CDataManager::init（ORIG 0x08358332..0x0835f1d5，报告 init.md 全量转写）
// 依赖的脚本装载自由函数 · 第 3 组（Guild/Power/Quest 参数、称号簿、PvP 频道、
// 旧装备属性、屏蔽字、物品组表）。
//
// 本 TU 实现的符号（均经 df_game_r nm 核实为真实 T 符号；逐指令转写自
// docs/class_func_reports/_lt_global_gt_/ 对应报告，调用点行号引自
// docs/class_func_reports/CDataManager/init.md）：
//   importOldEquipmentStatInfo   _Z26importOldEquipmentStatInfoPKc        0x08a58174（init.md +0x2af）
//   importPVPChannelList         _Z20importPVPChannelListPcP18live_server_info_ti
//                                                                         0x08901f10（init.md +0x1ce1）
//   importGuildParameterScript   _Z26importGuildParameterScriptP20GuildParameterScriptPKc
//                                                                         0x08978d48（init.md +0x1d56）
//   importPowerParameterScript   _Z26importPowerParameterScriptP20PowerParameterScriptPKc
//                                                                         0x08a592bc（init.md +0x1dcb）
//   importTitleBookEtcScript     _Z24importTitleBookEtcScriptPKcP15stTitleBookInfo
//                                                                         0x08aa8110（init.md +0x1f2a）
//   importQuestParameterScript   _Z26importQuestParameterScriptP20QuestParameterScriptPKc
//                                                                         0x08a6b660（init.md +0x207d）
//   importSlangName              _Z15importSlangNamePcP13SlangNameData    0x089081a3（init.md +0x2204）
//   importItmeGroupTable         _Z20importItmeGroupTablePKcS0_RSt3mapISsi..
//                                                                         0x0890bb25（init.md +0x3e40）
//
// 声明唯一出处 DataManagerInitDepends.h（本文件在 CDataManager.h 之后包含之）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3] 本 TU 判词表（verdict = check_tu_routed.sh
// 干净口径 + p167 调用点差集；.o 单函数读数受 N124 假报影响仅作参考）：
//   stTitleSection::C1 0x08aa979e   AE/IDENTICAL —— CTitleBook.h 内联 ctor 按
//     P239 把 6 个标量移入 ctor 体（ORIG「全部 string/vector 构造 → 标量 store」）。
//   importSlangName 0x089081a3      AE/IDENTICAL —— 修法三则：①块级 `bool ok`
//     无预清 + `for(;;){value=ScanInt(&ok); if(ok) push_back((uchar)value); else
//     break;}`（P0 具名 int value 槽 + prvalue 临时槽；具名 ch+static_cast<T&&>
//     恒多物化一条，P270）；②goto 循环复现 cond 前置 + `xor $1; test; jne exit`
//     （P292 bool callee）；③末段 == 形 + P223 尾空标签复现 nop 岛。P167 CLEAR(18/18)。
//   importOldEquipmentStatInfo 0x08a58174 AE/IDENTICAL —— ①find==end 正极性
//     （P220/P222）+ value_type(itemIndex,info) 引用构造（P192）；②指针局部
//     elementTolerance/activeProperty 退役，改 m_view[2][i] / m_view2[3][j]
//     视图寻址（P311/P30，P167 归零）；③itemIndex 预清 + 4 个 dead int 局部
//     补帧 0x10（P31/P232）；④goto 循环复现 cond 前置。P167 CLEAR(38/38)。
//   importPVPChannelList 0x08901f10 AE/IDENTICAL —— goto 循环复现 cond 前置 +
//     `xor $1; test; jne exit`（P292）。P167 CLEAR(35/35)。
//   importTitleBookEtcScript 0x08aa8110 N 封顶 —— 650/650、P167 CLEAR(94/94)；
//     残差 6 处全为 4.1 独占的 switch 叶块间 nop 占位岛（4.4 恒不发射；§6.2
//     三件套：gap=0 / 调用点归零 / 逐块比对无语义差）。
//   importQuestParameterScript 0x08a6b660 N 封顶 —— 447/446（gap 3→1）、
//     P167 CLEAR(66/66)。[b28d-solo3-2 收口] 两处形态修正：
//     ①`[piece reward]` 循环头改**合并短路** `if (!ScanType(sub,true) ||
//     sub=="[/piece reward]") break;` —— ORIG 0x08a6bace 是 `jne→mov $1 /
//     je→mov $0 / test;jne` 物化（探针逐条同构），两个独立 break 恒产两个
//     直跳出口（少 5 条）；
//     ②`found` 具名 bool 改 `while (appendage != -1) { …; break; }`（P225）
//     —— 精确复现 ORIG 0x08a6bb2a `cmpl $0xffffffff; setne; test; je` 且
//     **零槽**，appendage 由 -0x10 归位 -0xc。修法已落：p 形参缓存（-0x20）+
//     各段块级 ok 槽 + isStr 无预清 + memcpy 1 字节 key + dead1c。
//     残余 1 条 = 4.1 独占循环前 `jmp+nop` 岛（O[42:44]）+ 帧差 0x10/槽号平移
//     4（P332/N115）+ [piece reward table]/清理路径块序（difflib 对齐伪差）。
//   importGuildParameterScript 0x08978d48 N 封顶 —— 507/505、P167 CLEAR(76/76)；
//     残差为 4.1 独占 nop 岛 + 两步寻址伪影（N94：`[level-1][1]` 的 0x60 disp
//     4.4 折单条 scaled mov，ORIG 拆 lea+mov，-1 条）。修法已落：7 处
//     `"lit" == token` 方向修正（P167 MISSING/EXTRA ×7 归零）、pair 直构替
//     make_pair ×2、push_back 左值 ×1、p 形参缓存（-0x34）+ dead30、level/cost
//     局部提升、w/key/value 预清；两类型用户空析构删除（P317，.cpp 定义由
//     Main 代落）。[b28d-solo3 二次收口] 删 lv 预清（504）与删 6 处 done 复位
//     （499）均使 gap 反向扩大（ORIG 依赖其形），已回退保持 505。
//   importPowerParameterScript 0x08a592bc N 封顶 —— 638/639、P167 CLEAR(101/101)；
//     [b28d-solo3 二次收口] token 链按 ORIG 汇编顺序重排（noti time/kill count
//     effect range/power up bonus/power war channel/power up point mod/join cost
//     modify/victorious reward time/penalty/break bonus/rejoin panalty 十段归位）
//     + [pc room bonus] 具名局部预清中转（ORIG +0x60a..+0x628）
//     + [straight victory penalty] 改 `while(true){…if(!done)break;}` 四守卫
//     （复现 ORIG 逐处 `movzbl; xor $1; test; je +2; jmp`）+ [break bonus]
//     去 value 预清 —— gap 9→-1（639 vs 638）。唯一残差 = 4.4 各段尾 continue
//     公共回边中继 jmp（+1；ORIG 直跳循环头 +0x56）。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"         // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头，与 CDataManager.cpp 口径一致）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr/ScanTypeOrStr
#include "LogManager.h"          // LogManager::logFormat

#include <map>
#include <string>
#include <string.h>              // memcpy（ORIG importQuestParameterScript [difficulty] 段）
#include <utility>
#include <vector>

// ---- 外部依赖声明（ORIG T 符号，定义归属各自批次；此处仅引用）----

// _Z13importRewardsPSt6vectorIP18stTitleRewardBasisSaIS1_EESs 0x08aa8b79
#include "InitScriptProducers_3/importRewards.inc"
#include "InitScriptProducers_3/importTitleSection.inc"
#include "InitScriptProducers_3/convertTabTypeStringToEnum.inc"
#include "InitScriptProducers_3/loadAppendageType.inc"
#include "InitScriptProducers_3/loadValueType.inc"
extern "C"
{
std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;
}

// ============================================================================
// importOldEquipmentStatInfo —— ORIG 0x08a58174（../RDARScriptOldEquip.cpp）
// 报告 _lt_global_gt_/importOldEquipmentStatInfo.md。
// 逐条读取 itemIndex + STEquipmentStatInfo 全部 26 个 int 字段；任一缺失即
// logFormat 并返回 false；重复 index 打印 Redundant 后跳过插入；
// ScanInt 收尾失败（正常 EOF）返回 true。
// ============================================================================
#include "InitScriptProducers_3/importOldEquipmentStatInfo.inc"
}

// ============================================================================
// importPVPChannelList —— ORIG 0x08901f10
// 报告 _lt_global_gt_/importPVPChannelList.md；调用点 init.md +0x1cce：
//   importPVPChannelList("Etc/pvpChannelList.etc", this+0xb4ec, 0)。
// [PVP CHANNEL INFO] 段：grade=ScanInt、count=ScanInt；每条 ScanStr(名字) +
// pvp_channel_info_t 的 9 个 int（扫描序→偏移见 CDataManager.h 注释）+
// 尾随 ScanStr；完成后把 map[grade]=vector insert 进 live_server_info_t 的
// m_pvpChannelMap。第三形参 flag 在 ORIG 函数体内未使用。
// 任一 channel 字段缺失 → 清理后返回 false。
// ============================================================================
#include "InitScriptProducers_3/importPVPChannelList.inc"
}

// ============================================================================
// importGuildParameterScript —— ORIG 0x08978d48
// 报告 _lt_global_gt_/importGuildParameterScript.md；调用点 init.md +0x1d53：
//   importGuildParameterScript(this+0xa50c, "Etc/Guild.etc")。
// 返回 bool：仅当读到 [guild parameter end] 为 true；加载失败 / ScanType 失败 /
// [guild exp weight] 遇 m_field8==0 均返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 507/505、P167 CLEAR(76/76)；残差 =
// 4.1 nop 占位岛 + 两步寻址伪影（详见文件头判词表）。
#include "InitScriptProducers_3/importGuildParameterScript.inc"
}

// ============================================================================
// importPowerParameterScript —— ORIG 0x08a592bc
// 报告 _lt_global_gt_/importPowerParameterScript.md；调用点 init.md +0x1dc8：
//   importPowerParameterScript(this+0xa628, "Etc/Power.etc")。
// 返回 bool：仅当读到 [power parameter end] 为 true；加载失败 / ScanType 失败 /
// [power war channel] 第二值缺失 返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 638/629、P167 CLEAR(101/101)；残差 =
// 4.1/4.4 break 块序（N122/P168a）+ nop 岛（详见文件头判词表）。
// [2026-09-10 D06 · N] 残差 = 我方多 1 条**不可达中继** `jmp <while_head>`（位于
//   `jmp <return done>` 之后的死区，ORIG 该处为 `nop`）⇒ 4.4 的循环出口中继
//   （A28 边界）；实测 `while(true)`↔`for(;;)` 同值（631 vs 630），`break`↔`goto
//   scan_end` 同值 ⇒ 源码层不可控，封顶。
#include "InitScriptProducers_3/importPowerParameterScript.inc"
}

// ============================================================================
// importQuestParameterScript —— ORIG 0x08a6b660
// 报告 _lt_global_gt_/importQuestParameterScript.md；调用点 init.md +0x207a：
//   importQuestParameterScript(this+0x8cc8, "n_Quest/questParameter.etc")。
// [difficulty] 用 ScanTypeOrStr：类型令牌取 sub[0] 为键、ScanInt 为值插入
// m_map0；字符串令牌仅识别 [/difficulty] 终止符。[piece reward] 中首个
// ScanInt 失败即整体返回 false（asm +0x445）。
// ============================================================================
// [b28d-solo3-2 收口] 447/446、P167 CLEAR(66/66)、gap 3→1；两处形态修正
// （合并短路 break / while(appendage!=-1){…break;} 零槽 setne）见文件头判词表。
#include "InitScriptProducers_3/importQuestParameterScript.inc"
}

// ============================================================================
// importTitleBookEtcScript —— ORIG 0x08aa8110
// 报告 _lt_global_gt_/importTitleBookEtcScript.md；调用点 init.md +0x1f23：
//   importTitleBookEtcScript("Etc/TitleBook.etc", this+0xa788)。
// 双遍结构（ORIG __lookup/__init_lock/__inited 三个函数级静态）：
//   首令牌以 id=0 进入 switch，case 0 与首个 case 0x39 合并，之后逐 case
//   **贯穿**登记 6 个 (键,id)（0x39/0x4d/0x7f/0xc6/0xe4/0xf4），落到 default
//   后释放锁、置位 __inited 并 continue，以当前令牌重新查表派发——
//   因此首令牌同样会被正常处理。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 650/650、P167 CLEAR(94/94)；残差 =
// 4.1 switch 叶块 nop 占位岛 ×6（4.4 恒不发射；详见文件头判词表）。
// [2026-09-10 D06 · N 封顶复核] 本轮三形实测（读 `fnx.py` 归一化口径）：
//  ① 原形（内层 `for(;;)` + `continue`）：643 vs ORIG 642（我方多 1 条 jmp）；
//  ② 内层+外层全 goto 形（`titlebook_retry:`/`outer_retry:`）：642 = 642（gap 0）
//     但残差变为「入口 `jmp <outer_head>`（我方多）↔ 置位处 `jmp +0x61`（我方少）」
//     两处位置互换；③ 单 goto 出口形：同 ②。⇒ 属 4.4 循环头/出口块布局（P223/A28
//     epilogue-threading 族），源码层不可分裂；保留 ② 形（gap 0 优于 ① 的 +1）。
#include "InitScriptProducers_3/importTitleBookEtcScript.inc"
}

// ============================================================================
// importSlangName —— ORIG 0x089081a3
// 报告 _lt_global_gt_/importSlangName.md；调用点 init.md +0x21fd：
//   importSlangName("Etc/slangname.etc", this+0x86a0)。
// [SLANG]/[one byte skip char]/[two byte skip char] 三段分别回填
// SlangNameData 的 vector<string>(+0x00)/vector<uchar>(+0x0c)/
// vector<ushort>(+0x18)。
// ============================================================================
#include "InitScriptProducers_3/importSlangName.inc"
}

// ============================================================================
// importItmeGroupTable —— ORIG 0x0890bb25
// 报告 _lt_global_gt_/importItmeGroupTable.md；调用点 init.md +0x3e40：
//   importItmeGroupTable("Etc/", "ItemGroupName.tbl", getItemGroupNameTable())。
// 循环读 (id, 名称) 对插入 table；返回 0=成功、0x1ebd=ScanStr 失败、
// 0x1eb2=加载失败（错误码与 ORIG 一致）。
// ============================================================================
#include "InitScriptProducers_3/importItmeGroupTable.inc"
}

// stCustomConditionMsg::stCustomConditionMsg —— ORIG C1 0x8aa9928：string 成员
// 默认构造后两 int 置 -1（importTitleBookEtcScript [custom condition message]）。
#include "InitScriptProducers_3/stCustomConditionMsg.inc"
}

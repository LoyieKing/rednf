// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 W8_EventClassifyB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— EventClassify 域仅 ORIG 符号批次（W8_Mid.EventBatch）。
// 覆盖：
//   CObjectBringUp::{verifyGiveBringUpReward 0x0810fe1a, verifyMaterialItemByRule
//     0x0810fcea, sendBringUpReward 0x0810fe90}
//   CEventScriptMng::{send_event_init_record_data 0x0810cb2c,
//     process_update_ingame_history 0x0810c82e}
//   CPcRoomPlayTime::processEventPcRoomPlayTime 0x0811089c
//   CUsedFatigueGiveItem::get_user_type 0x08110eec（objdump·报告缺失）
//   CExchangeRandomItemReward::{verifyMaterialItemByRule 0x0810eb30,
//     deleteUsedMaterialItem 0x0810eede, getRewardItem 0x0810ee40,
//     sendRandomItemReward 0x0810f26e}
//   CAvatarDisjointRandomReward::{checkAvatarDisjointError 0x0810f3a4,
//     deleteAvatarItem 0x0810f4e2, SendDisjointAvatarStatistic 0x0810f5f4,
//     getRewardItem 0x0810f694, SendCreateEmblemStatistic 0x0810f742,
//     sendRandomItemReward 0x0810f88c}
// 每函数依据 docs/class_func_reports/EventClassify__*/ 对应 md 的
// ## 汇编 + ## 反编译 C 完整转写；类布局见 EventClassify_Action.h /
// InGameEventScripts.h / CEventScriptMng.h。
// [b28d-W8EventB 回边专项核对（Main 高优先级排查）] 全 17 个游戏函数逐位核对
// jmp/jcc **目标下标**（非仅助记符）：回边（BACK）数量与助记符两侧全等
// （sendBringUpReward 1/1、verifyMaterialItemByRule 1/1、process_update_ingame_history
// 1/1、CExchange::verifyMaterialItemByRule 4/4、deleteUsedMaterialItem 2/2、
// SendCreateEmblemStatistic 1/1，其余 0/0），且回边目标处指令均为**循环体头**
// （+0x72 / +0xc4 / +0x35 / …），非函数出口 ⇒ **不存在**「goto 退出标签 ⇒ 循环只
// 解析/迭代一轮」模式。本 TU 无 goto；2 处 continue（sendBringUpReward）与 2 处
// break（CExchange::verifyMaterialItemByRule、SendCreateEmblemStatistic）均编译为
// 正常前向跳转/回边，与 ORIG 一致。
// ----------------------------------------------------------------------------
// [b28d-D03 本波 判词]（fresh `check_tu_routed.sh`：identical=87 ae=148（合计 235）
//   near=1 diff=3；shift 探针对 5 件全跑）
//   · verifyMaterialItemByRule 0x0810fe1a —— 95/94：shift **PURE SHIFT c=0**（帧 0x24、
//     槽直方图全等）；唯一残差 = ORIG 比我们多 1 条孤立 `nop`（中段对齐产物，N112：
//     4.4 循环出口 nop 填充旗标无效、compare 口径已剥离尾部对齐 nop）⇒ 封 N112。
//   · deleteUsedMaterialItem 0x0810eede —— 223/223：shift PURE SHIFT c=0；残差 = 同内容
//     指令块相对序（difflib 重排），属 N104/EH 尾块相对序族 ⇒ 封顶。
//   · SendCreateEmblemStatistic 0x0810f742 —— 99/99（**NEAR**）：shift PURE SHIFT c=0；
//     残差 = 索引折形 `lea 0x8(%ebx),%edx; mov %eax,-0x4c(%ebp,%edx,4)` vs
//     `lea 0x4(%ebx),%edx; mov %eax,-0x3c(%ebp,%edx,4)`（算术等价：-0x4c+4*(ebx+8) ≡
//     -0x3c+4*(ebx+4)）⇒ P30/N4「add-vs-lea/scaled 索引」封顶；另 2 处数据符号立即数
//     （N132 AE 口径）。
//   · sendBringUpReward 0x0810fe90 —— 331/335：shift MIXED，帧 0x60 vs 0x70（我方多 4 槽，
//     push 数相同）；残差三类（历轮已锁定）：a) 帧/槽 = N82/N115；b) varargs 求值序
//     ORIG 右起（m_itemIdx→getCurCharacNo→get_acc_id）vs 4.4 恒左起（P75/P87，P226
//     语句表达式实测无效）；c) std::copy/back_inserter 内联块序（+1 jmp 同目标）。
//     p167 58/58 CLEAR。
//   · process_update_ingame_history 0x0810c82e —— 98/102：shift **PURE SHIFT c=0**；
//     残差 = 4 处 sret 调用点的寄存器角色（ORIG `lea -0xNN(%ebp),%edx; mov %eax,0x4(%esp)`
//     vs 4.4 `mov %eax,%edx; lea …,%eax`）⇒ N7（sret 地址/返回值寄存器角色互换，
//     4.4.7 后端固有，P230 判据）封顶。

#include <cstring>
#include <utility>

#include "CEventScriptMng.h"         // 实体类 + CEventScriptMng
#include "InGameEventScripts.h"      // ObjectBringUpScript / ExchangeRandom... 等
#include "EventClassify_Action.h"    // CEventActionMng
#include "CUser.h"                   // CUserCharacInfo getter / Send / get_acc_id
#include "CInventory.h"
#include "CItem.h"                   // GetItemType/get_grade/get_index
#include "CAvatarItemMgr.h"          // UnRegistItem/GetExpireDate
#include "CStatisticServerProxy.h"
#include "Packet_PcRoomPlayTimeReward.h"      // 全局 Packet_PcRoomPlayTimeReward（ORIG W C1 0x08110c0a）
#include "Packet_Emblem_Create_Statistic.h"   // 全局 Packet_Emblem_Create_Statistic（ORIG W C1 0x08110bdc）
#include "Packet_Avater_Disjoint_Statistic.h" // 全局 Packet_Avater_Disjoint_Statistic（ORIG W C1 0x08110bae）

#include "CDataManager.h"            // G_CDataManager/find_item/m_eventAvatarList
#include "GlobalData.h"              // s_stream_pool/s_msgq_mgr/s_*_proxy_mgr
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "PacketGuard.h"             // PacketGuard + PacketHeader（传递可见）
#include "SigTypes.h"                // SIG_OBJECT_BRING_UP
#include "CEnvironment.h"            // G_CEnvironment / m_serverEnvir.m_serverGroup
#include "GameWorld.h"               // IsIntegratedPvPBaseChannel / G_GameWorld
#include "GameTypes.h"               // cMyTrace
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy
#include "CMonitorServerProxy.h"
#include "DB_UpdateAvatarHistory.h"
#include "DB_UpdateInGameEventHistory.h"

// ORIG 全局作用域真实类 DB_SavePcRoomPlayTimeEvent
// （_ZN26DB_SavePcRoomPlayTimeEvent11makeRequestEjjh，T 0x08447bd0；
//   权威类定义与实例注册在 W8LT_DBTask.cpp，makeRequest 定义同 TU）。
struct DB_SavePcRoomPlayTimeEvent
{
    static void makeRequest(unsigned int accId, unsigned int totalTime,
                            unsigned char rewardCnt);
};

namespace EventClassify
{

// ============================================================================
// CObjectBringUp
// ============================================================================

// verifyGiveBringUpReward（ORIG 0x0810fe1a）
// ORIG 分支形态：multiple==0 分支 if/return（P56）；否则 OR 短路链直接 jcc
// 汇聚统一 fail 出口（P133），Ghidra else-if 链。
#include "W8_EventClassifyB/verifyGiveBringUpReward.inc"
}

// verifyMaterialItemByRule（ORIG 0x0810fcea）
// 不缓存 script（P36：ORIG 每次重读 this+0x1c）；flag!=0 分支 ORIG 对
// size()/getCurUseBringUpMaterialTotal 各调用两次（P48 无 CSE），条件先 size
// 后 total（setbe 方向）。
// [b28d-L14 N82附] ORIG 0x0810e7c4；p167 CLEAR（10/10）；残差=bool 汇聚物化+槽深。
// [b28d-W8EventB 本波归零 AE 94/94] 两处旋钮（旧判词「N 封顶 11 条」作废）：
// ①结构：flag!=0 分支只做 `if (idx!=item) return false;`，命中路径落出
//   if/else 到函数尾 `return true;` ⇒ 复现 ORIG 0x810fe0f 尾部 `mov $1,%eax`
//   fall-through 到 epilogue（P191；旧「就地 mov $1;jmp」差异消除）；
// ②idx 类型 int→unsigned int：ORIG 0x810fd65 `lea 0x10(%eax),%edx` 先物化
//   容器基址再载 idx（this-first），int 下标 4.4 恒 idx-first（P218 探针）。
// 语义/调用点全等（p167 CLEAR 10/10）。
// [L14 round2 复核] 95/94：唯一差异 = ORIG 0x0810fcea 段在**循环回边目标前**多 1 个
// 4.1 对齐 nop（orig[37]，紧跟 `call GetInvenSlot; movzbl; mov %eax,-0xc` 之后），
// 4.4 不发同形填充 ⇒ N112 封顶（§6.1 口径）。
#include "W8_EventClassifyB/verifyMaterialItemByRule.inc"
}

// sendBringUpReward（ORIG 0x0810fe90）
// ORIG 固定索引 1 比较 pair.second+0x2c = Action_SendMail::m_itemIdx（+0x20，
// 相对 pair 为 +0x2c；非 m_gold）。比较装载序：先 operator[]([1]) 后
// operator->(当前元素)。getBringUpRewardFlag() 用 if(flag) continue 布局
// （P59：call;test;jne 循环尾）。日志参数与 __PRETTY_FUNCTION__ 照 ORIG。
// [b28d-L14 N82附] ORIG 0x0810e2b0；p167 CLEAR（58/58）；残差=槽深/常量装载。
// [b28d-W8EventB 本波] 已修三处（DIFF 残差 329/333，语义/调用点全等）：
// ①result 提到循环外 + `int deadSlot = 1; (void)deadSlot;`：ORIG 0x810fed4
//   `movl $0x0,-0x14`（result 预清 P198①，两分支共用同槽）+ 0x810fedb
//   `movl $0x1,-0x10`（死局部，全函数无读取点 ⇒ P171 死写入照抄）⇒ 头部两条
//   store 与 result/sig 槽（-0x14/-0x10/-0xc）逐槽对齐；
// ②SIG_OBJECT_BRING_UP::m_serverGroup 1B→4B（SigTypes_D.h，Main 已批）：
//   ORIG 0x0810ffb8 `movzbl %al,%edx; mov %edx,0x8(%eax)` 与 CUser::UpdateData
//   0x08654089 同形，4B 写；sizeof 0x10 与 +0x0c/+0x0d 偏移不变；
// ③残余仅三类跨版本产物，已逐条核对语义等价（jcc 目标序列、p167 58/58 CLEAR）：
//   a) 帧 0x70 vs 0x60 与 it/actionMng 槽整体浅 4（N82/N115/N126 版本常数）；
//   b) varargs 求值序：ORIG 左起（m_itemIdx→getCurCharacNo→get_acc_id），4.4 恒
//      右起；P226 语句表达式 `({ it->second.m_itemIdx; })` 实测无效（保持原形）；
//   c) std::copy/back_inserter 内联块序（ORIG 把 `lea -0x48;call` 前置，4.4 后置，
//      多 1 条 jmp 到相同目标）。
// 【b28d-D03 更正 · 该头依赖已闭合】原记「getBringUpRewardFlag 返回 uchar（头依赖，
//   已上报 Main）」为**过时信息**：Main 已在 S350 代落 `CUserCharacInfo.h:567
//   m_bringUpRewardFlag` 与 `getBringUpRewardFlag()` 双 bool 化（1 字节/偏移/mangled 不变）。
//   本波 fresh 实测：调用点两侧均为 `call; test %al,%al; jne`（**无 setne 差异**）⇒ 该项
//   不再是本件残差来源。C05④ 四步落点（留档）：①无消费者死比较？否 ②值槽化/表达式
//   物化？否（两侧同寄存器直测）③callee 声明不一致？**否（已两侧 bool）**⇒ 归因 1 不成立
//   ④本件残差 = 帧 0x60 vs 0x70（我方多 4 槽）+ varargs 求值序 ORIG 右起 vs 4.4 左起
//   （P75/P87）+ std::copy/back_inserter 块序，均属跨版本结构性 ⇒ 维持 N82/N115 封顶。
#include "W8_EventClassifyB/sendBringUpReward.inc"
}

// ============================================================================
// CUsedFatigueGiveItem —— get_user_type（ORIG 0x08110eec；报告缺失，
// objdump 直接转写：movzbl 0xc(script)=UsedFatigueGiveItemScript::m_userType）
// ============================================================================
#include "W8_EventClassifyB/get_user_type.inc"
}

// ============================================================================
// CPcRoomPlayTime::processEventPcRoomPlayTime（ORIG 0x0811089c）
// ============================================================================
// [b28d-L14 N82附] ORIG 0x0811089c；p167 CLEAR（24/24）；残差=槽深/寄存器分配。
// [b28d-solo-W8_EventClassifyB 判词·AE 达成] 本波修正两处：
// ①packet.m_field0a 三目 → if/else 两臂各自 store（ORIG +0x121/+0x13c 两次
//   `mov %al,-0x31(%ebp)`；三目被 4.4 合并为单一汇合 store）；
// ②CUser::getTotalPcRoomPlayTime 返回类型 int→unsigned int（Main 代落 CUser.h）
//   复现 ORIG 0x081108c4 `cmp %edx,%eax; setb`（call 结果留 %eax 无中转，P302）。
// check_tu 已归 ae（116=116）。
#include "W8_EventClassifyB/processEventPcRoomPlayTime.inc"
}

// ============================================================================
// CEventScriptMng
// ============================================================================

// send_event_init_record_data（ORIG 0x0810cb2c）
// history 强制 const map*（ORIG begin/end 走 const 版本，无 iterator 转换）。
#include "W8_EventClassifyB/send_event_init_record_data.inc"
}

// process_update_ingame_history（ORIG 0x0810c82e）
// updates/history 均强制 const map*（ORIG begin/end/find 全为 const 版本，
// _Rb_tree_const_iterator 直返无转换）。makeRequest 参数装载：右→左
// hit->second.second、hit->second.first、it->first（updates 迭代器 key）、
// get_acc_id —— 与 ORIG +0x0b8..+0x0ed 压栈一致（当前实现曾误用 hit->first
// 且 second.first/second.second 颠倒）。ORIG 的 ++it 位于外层 if 体内
// （second==false 时不递增——忠实转写 ORIG 控制流）。尾部不清 update 表。
// [b28d-L14 N82附] ORIG 0x0810f5b8；p167 CLEAR（18/18）；残差=槽深/寄存器分配。
// [b28d-solo-W8_EventClassifyB 判词·N7 封顶，残差 17] hend 内联后槽位全对齐
// （it -0x24 / hit -0x28 / 内层 end -0x1c / 循环条件 end -0x20，与 ORIG 逐槽一致）；
// 残差为 4 处 sret 地址装载形：
//   ORIG `lea -0xXX(%ebp),%edx; mov %eax,0x4(%esp); mov %edx,(%esp)`
//   vs 4.4 `mov %eax,%edx; lea -0xXX(%ebp),%eax; mov %edx,0x4(%esp); mov %eax,(%esp)`
// —— 4.4 的 lea 目标恒 %eax（call 结果须先中转 %edx），4.1 恒 %edx；属 N7
// 寄存器角色族（P230 补分诊：N7 判词限定 4.4.7 后端）。「先声明后赋值」
// 形实测反劣（17→21，且引入默认 ctor 风险）。语义/调用点全等。
// [b28d-W8EventB 复核] jcc 归一化交叉验证：6 条 jcc 助记符与目标偏移序列
// 逐位一致（仅整体 +8 常数位移，源于 4.4 sret 装载多 4 条）；p167 18/18 CLEAR。
#include "W8_EventClassifyB/process_update_ingame_history.inc"
}

// ============================================================================
// CExchangeRandomItemReward
// ============================================================================

// getRewardItem（ORIG 0x0810ee40）
// result 声明于循环前（ORIG local_10=0）；非 const iterator（P96）。
#include "W8_EventClassifyB/getRewardItem.inc"
}

// verifyMaterialItemByRule（ORIG 0x0810eb30）
// flag08==0：内层命中即收集并 break；外层在 materials 非空时终止整个双重循环。
// flag08!=0：全量收集；收集数!=规则数 → 0x16。不缓存 script/rule（P36/P48：
// ORIG 对 pairs0c[r] 三次 operator[]）。命中条件两元素装载：first 先、second 后。
// [b28d-solo-W8_EventClassifyB 判词·归零] 修法=块序反写：ORIG 0x0810eb46
// `test %al,%al; je +0x190` ⇒ flag08!=0 分支直落在前、flag08==0（含 break）
// 分支后置；条件反写 + 两分支体互换后归一化 ndiff=1（仅尾部对齐 nop，§6.1 豁免）。
#include "W8_EventClassifyB/verifyMaterialItemByRule_1.inc"
}

// deleteUsedMaterialItem（ORIG 0x0810eede）
// ORIG 无 failed 标志：delete_item 失败路径内联处理（回包循环 + logFormat
// 0x640 + return 8），成功路径 fall-through push_back+++i。回包循环
// 内 newSlot<0 时 logFormat 0x63d。全部 __PRETTY_FUNCTION__。
// [b28d-solo-W8_EventClassifyB 判词·归零（ae）] 本波真语义修正五处：
// ①item.m_addInfo2 = it->second 直写（ORIG +0x16c `mov %eax,-0x9b(%ebp)`
//   写 item+0x07 4B），并回读该字段作 create_item 第三参与 0x63d 日志实参
//   （原实现第三参直传 it->second、日志重调 it->first/second）；
// ②失败路径 logFormat 改两次独立 materials[i] 取用（ORIG +0x2f5/+0x302
//   两次 operator[]，P48 无 CSE；原实现缓存 `const pair& bad` 致少一次调用）；
// ③returnItems.size() != 0（P197④：empty() 恒 out-of-line call 无 setne 段）；
// ④delete_item 改正条件 if/else（ORIG +0x0b0 `test %al,%al; je +0x112`
//   成功路径直落，原 `if(!...)` 反置块序）；
// ⑤newSlot 循环外预清（ORIG +0x12a `movl $0x0,-0xc(%ebp)`，P198①）。
// check_tu 已归 ae；残余仅 make_pair 模板实参 `IRmRiE`(unsigned long&) vs
// `IRjRiE`(unsigned int&) —— Inven_Item::m_addInfo 声明宽度的共享头旋钮，
// 已上报 Main 决策（本 TU 单文件不可改）。
// [L14 round2 复核] 223/223、strict ndiff=**0**：前轮 3 行「差异」全为 .o 未解析
// 重定位的显示形（ORIG `$0x8b4c2c0/$0x8b4d400/$0x8b4c264` 串地址 vs 我方节内偏移
// `$0x2f/$0x240/$0xe8`）⇒ 实为等价，工具 NEAR 为伪差（P264/.o 口径）。
#include "W8_EventClassifyB/deleteUsedMaterialItem.inc"
}

// sendRandomItemReward（ORIG 0x0810f26e）
// set_item(mail,p.first,p.second) → 发信(true) → ++flag；结束把 mail.m_vec38
// （脚本 +0x5c）移交 recordVec 并 clear。非 const iterator（P96）。
#include "W8_EventClassifyB/sendRandomItemReward.inc"
}

// ============================================================================
// CAvatarDisjointRandomReward
// ============================================================================

// checkAvatarDisjointError（ORIG 0x0810f3a4）
// get_grade 两次独立调用（P48：ORIG +0x059/+0x068 双 call）。
// 注意：ORIG 虚调用 getUsableItemType 在 vtable+0x24，当前 CItem.h 虚序生成
// +0x20 —— 共享头（CItem.h）虚函数序与 ORIG vtable 差 2 槽，需全局修复，
// 见报告；此处保持头现状。
#include "W8_EventClassifyB/checkAvatarDisjointError.inc"
}

// deleteAvatarItem（ORIG 0x0810f4e2）
#include "W8_EventClassifyB/deleteAvatarItem.inc"
}

// SendDisjointAvatarStatistic（ORIG 0x0810f5f4）
// ORIG 用 if/else 赋值局部 m_gradeIdx（rarity==2 ? 2 : grade-1）——P47 按
// if-return/if-else 形态；GetServerProxy(s_statistic_proxy_mgr,0) 直链。
#include "W8_EventClassifyB/SendDisjointAvatarStatistic.inc"
}

// getRewardItem（ORIG 0x0810f694）
// grade 为 char 局部直存（ORIG mov %al,-0x11）；map 迭代器非 const。
#include "W8_EventClassifyB/getRewardItem_1.inc"
}

// SendCreateEmblemStatistic（ORIG 0x0810f742）
// ORIG 写入序：先 put_grade（slots[i+1]）后 put_value（slots[i]）——get_grade
// 调用在 value 读取之前（+0x09c vs +0x0ae）。size>5 判断（cmp $5;seta）。
// [b28d-L14 P30/N82附] ORIG 0x0810f742；p167 CLEAR（13/13）；数组下标 add-vs-lea
// 组合差（ORIG `mov n,%ebx` 直用 vs 4.4 `lea 1(%eax),%ebx`）+ 基址 -0x40/-0x44。
// [b28d-solo-W8_EventClassifyB N94 封顶] 改用具名视图 m_grades[n]/m_values[n] 后
// grade 侧已逐条全等（`mov n,%ebx; mov %eax,-0x40(%ebp,%ebx,4)`）；value 侧
// ORIG `lea 0x8(%ebx),%edx; mov %eax,-0x4c(%ebp,%edx,4)`（disp=packet+0x2、lea+8）
// vs 4.4 `lea 0x4(%ebx),%edx; mov %eax,-0x3c(%ebp,%edx,4)`（disp=packet+0x12、lea+4）
// —— 4.4 对成员数组偏移 0x18 恒拆 disp=偏移&0xf=8 / lea=4，ORIG 4.1 拆 disp=2/lea=8；
// 地址与语义完全等价（均 = packet+0x22+4n），助记符序列全等（NEAR）。
// 合法 C++ 无法把 union 基址移到 +0x2（PacketHeader 非平凡 ctor 不可作 union 成员）⇒ N94。
// [b28d-W8EventB 复核] jcc 归一化 11/11 逐位一致；`call *` 虚槽 vptr+0xc
// （`mov (%eax),%eax; add $0xc,%eax; mov (%eax),%edx`）两侧逐条全等。
#include "W8_EventClassifyB/SendCreateEmblemStatistic.inc"
}

// sendRandomItemReward（ORIG 0x0810f88c）
// m_mail @ 脚本+0x20；发信(false)；非 const iterator。
#include "W8_EventClassifyB/sendRandomItemReward_1.inc"
}

}  // namespace EventClassify

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventScriptsCompleteB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// EventScriptsCompleteB2.cpp —— 事件脚本/升级支持域独占恢复 TU（EventScriptsCompleteB2 批次）。
//
// 覆盖符号（全部经 ORIG nm 实证存在，地址随各项标注）：
//   A. 10 个事件脚本真实 ctor（W）+ ExchangeRandomItemRewardScript::reset（W）
//   B. LevelupSupportSciprt / LevelupSupport2ndScript 的 C1/D1（T）
//   C. importActionMailScript（T 0x0897f147）
//   D. 10 个 import*EventScript（T）
//
// 证据等级：
//   - LevelupSupportSciprt/LevelupSupport2ndScript C1/D1：docs/class_func_reports/
//     LevelupSupportSciprt/ 与 LevelupSupport2ndScript/ 下对应 .md 报告已完整阅读
//     （## 汇编 + ## 反编译 C），按报告转写。
//   - 其余函数无独立报告 → 按 source/toolchain 规则用 ORIG objdump 反汇编逐指令
//     转写；编辑前已完整阅读同类/调用者报告：
//     InGameEventScriptLoader/importInGameEventScript.md（调用者，全文）、
//     EventClassify__C*/README.md 全部、EventClassify__CObjectBringUp/
//     verifyGiveBringUpReward.md（ObjectBringUpRule 字段语义）、以及同域已交付
//     EventLevelupA2.cpp（基类/邮件动作/装载器定义）。各函数头部均已标记。
//
// 解析器形态说明：ORIG 各 import 函数内的「static map<tag,id> + SpinLock +
// __cxa_guard」是函数级静态惰性建表（map 构造/SpinLock 构造各带 guard +
// __cxa_atexit，__inited 零初始化 bool）；构建完成前首个 token 以 key=0 进入
// case 贯穿注册链，表尾释放锁置位后回 guard 重派发同一 token。batch-9 起，
// importAttendanceScript / importPcBangPlayTimeScript /
// importUsedFatigueGiveItemEventScript / importExchangeRandomItemRewardScript
// 四函数按报告逐指令还原该真实双相形（不再用直接 token 比较简化形）；其余
// 同族函数暂保留简化形（各自报告到位后同法收敛）。
//
// 【2026-09-10 真 bug 修正（jcc 目标下标判据）】原 10 个 import*EventScript
// （Attendance/PcBangPlayTime/FatigueQuantity/AddItemCountDB/AccountFirstLogin/
// Exchange/UsedFatigueGiveItem/AvatarDisjointRandomReward/PurchaseCashItemBonus/
// ObjectBringUp）循环体末尾写 `goto scan_exit;`，而 `scan_exit:` 标签落在
// `if(!ScanType(...)){...}` 体内 ⇒ 编译为 `jmp scan_done`（**退出循环**），
// 脚本每文件只解析首个 token 就 return；ORIG 同处为回边 `jmp <循环头>`
// （Attendance 0x08981dc7 → 0x0898157e，其余 +0x42/+0x45/+0x4f 同族）。
// 旧判 AE/IDENTICAL 系 clean() 把 jcc 目标归一为 `<T>` 造成的**假匹配**
// （本轮新暴露的判据盲区）。已删 10 处 `goto scan_exit;` 改自然回边，
// 语义与 ORIG 一致（jcc 目标 = 循环头）。
//
// 【2026-09-10 收敛：内层 for(;;) 重试循环消 entry jmp】判据① 复验残留的唯一
// 目标差是 10 函数共有的「4.4 对**含内层后跳 goto** 的循环在循环头前插 1 条
// entry jmp（`jmp <L+nop>`）」，ORIG(4.1) 则「入口直落 + 尾部回边岛」。
// 探针族定界（/tmp/l18p/{p,q,r,s}.cpp，routed 4.4 实测）：entry jmp 由
// 「循环体内**后跳** goto 到循环内标签」触发（f1/f2/f4/f5 有、f3/f6 前跳无、
// fa–fd 无 goto 无）；普通循环回边（continue）不触发（g1）。
// ⇒ 修法：把 `if(!ScanType){ scan_exit: goto scan_done; }` + `redo_lookup:`
// 标签 + 两处 `goto redo_lookup;` 改写为 `if(!ScanType) goto scan_done;` +
// **内层 `for(;;)` 重试循环**（guard 竞争路径 `continue;`；重派发块写
// `if(!__inited){leave;__inited=true;} else {break;} continue;` —— continue
// 置于内层体末使 4.4 直出单条 `jmp <guard>`，否则多一条 latch 跳板）。
// 10 函数（FatigueQuantity/AccountFirstLogin/AddItemCountDB/PcBangPlayTime/
// Exchange/AvatarDisjointRandomReward/Attendance/UsedFatigueGiveItem/
// PurchaseCashItemBonus/ObjectBringUp）全部 DIFF→AE，raw 指令数亦与 ORIG 全等。
// ObjectBringUp 另有 [get eventitem fatigue] 的内层后跳 `goto send_sec_head;`
// 两处，改「内层 break + 外层 continue」后同样 AE。
// TU 四数：297/562(859)/1/13 → 297/572(869)/1/3。
// 残 3 = importLevelUpRewardScript（N82/N96：ORIG 的 next 标志在 %ebx 寄存器、
// 4.4 -O0 恒槽化；声明序/帧 0x170↔0x1a0 镜像）+ 本 TU
// `__static_initialization_and_destruction_0ii`（N123：每 TU 同名 local 副本，
// compare_tu 按名索引撞名）+ `std::_Vector_base<…Reward_Item…>::D1`（N89：4.1
// out-of-line / 4.4 内联差）。判据① 复验：全 TU 分支方向不匹配 = 0。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "DNFLexWrapper.h"             // loadRDARScriptFile/ScanType/ScanInt/ScanStr
#include "CEnvironment.h"              // less_TCHAR / SpinLock（__lookup 双相派发依赖）
#include "InGameEventScripts.h"        // InGameEventScript 域权威声明（本批次补字段）
#include "LevelupSupportEventManger.h" // LevelupSupportSciprt / LevelupSupport2ndScript

// CDataManager.cpp 提供（ORIG T 0x089bbc0f，声明亦见 InitScriptProducers_1.cpp）。
#include "EventScriptsCompleteB2/getCharacterJobToEnumValue.inc"
#include "EventScriptsCompleteB2/LevelupSupportSciprt.inc"
#include "EventScriptsCompleteB2/LevelUpRewardScript.inc"
#include "EventScriptsCompleteB2/UsedFatigueGiveItemScript.inc"
}

// ORIG W 0x0898597a：全成员构造 → str(+8).clear → vec(+0x10).clear →
// mail.reset → int+0xc=0 → int+0x24=0。
#include "EventScriptsCompleteB2/AttendanceScript.inc"
}

// ORIG W 0x08985fb8。
#include "EventScriptsCompleteB2/PcBangPlayTimeScript.inc"
}

// ORIG W 0x089858da：基类 → string+0x8 → mail+0xc → str.clear → mail.reset。
#include "EventScriptsCompleteB2/AccountFirstLoginScript.inc"
}

// ORIG W 0x08985ae0：字节+8=0 → vec<pair>+0xc clear → vec<int>+0x18 clear →
// mail+0x24 reset。
#include "EventScriptsCompleteB2/reset.inc"
}

// ORIG W 0x08985b1a：基类 → vec<pair>+0xc → vec<int>+0x18 → mail+0x24 → reset()。
#include "EventScriptsCompleteB2/ExchangeRandomItemRewardScript.inc"
}

// ORIG W 0x08985be2：基类 → map<char,uint>+0x8 → mail+0x20 → mail.reset。
#include "EventScriptsCompleteB2/AvatarDisjointRandomRewardScript.inc"
}

// ORIG W 0x08985c74：基类 → string+0x8 → 字节+c=0 → str.clear。
#include "EventScriptsCompleteB2/FatigueQuantityScript.inc"
}

// ORIG W 0x08985e0a：基类 → string+0x8 → vec<int>+0x10 →
// vector<pair<vector<uint>,mail>>+0x1c → vector<pair<Rule,mail>>+0x28 →
// str.clear → 字节+c=0。
#include "EventScriptsCompleteB2/ObjectBringUpScript.inc"
}

// ORIG W 0x08986134：基类 → vec<int>+0x8 → vec<int>+0x14 → string+0x20。
#include "EventScriptsCompleteB2/AddItemCountDBScript.inc"
#include "EventScriptsCompleteB2/ObjectBringUpRule.inc"
#include "EventScriptsCompleteB2/reset_1.inc"
}
// PurchaseBonusRule：ORIG W 0x08985ce6（调 reset）/ T 0x08985cfa（三 int = -1）。
#include "EventScriptsCompleteB2/PurchaseBonusRule.inc"
#include "EventScriptsCompleteB2/reset_2.inc"
}

// ORIG W 0x08985d1c：基类 → string+0x8 → set+0xc → vector<pair<Rule,mail>>+0x24
// → string+0x8 clear（无 mail.reset，忠实保留）。
#include "EventScriptsCompleteB2/PurchaseCashItemBonusScript.inc"
}

// ============================================================================
// B. LevelupSupportSciprt / LevelupSupport2ndScript C1/D1（报告转写）
// ============================================================================

// 报告 ~LevelupSupportSciprt.md @0x08ab5138：仅析构该 map。
#include "EventScriptsCompleteB2/dtor_LevelupSupportSciprt.inc"
#include "EventScriptsCompleteB2/LevelupSupport2ndScript.inc"
#include "EventScriptsCompleteB2/dtor_LevelupSupport2ndScript.inc"
#include "EventScriptsCompleteB2/importActionMailScript.inc"
}

// ============================================================================
// D. import*EventScript ×10
// ============================================================================

// ---- importFatigueQuantityEventScript T 0x08982fc6（1248B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importFatigueQuantityEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(title)→explain→
// db table→user type，公共尾 +0x474。id：[/event]0x478 [title]0x47a
// [explain]0x47d [db table]0x480 [user type]0x483。[user type] ScanInt(NULL)
// 字节截断值先物化存 +0xc（+0x46c mov %eax,%edx / mov %dl）；-0x1d bool 槽
// 初值 0 仅作 ScanType 失败返回（+0x4a1 movzbl -0x1d），此外从不写。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importFatigueQuantityEventScript.inc"
}

// ---- importAccountFirstLoginEventScript T 0x0897f7bf（1242B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAccountFirstLoginEventScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→LAB(0x fab4)→(fb12)→
// (fb73)→(fbd1)，公共尾 +0x46e。id：[/event]0xc4 [title]0xc6 [explain]0xc9
// [db table]0xcc [send mail]0xcf；mail 存 +0xc；-0x1d bool 槽仅作 ScanType
// 失败返回（+0x49b）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importAccountFirstLoginEventScript.inc"
}

// ---- importAddItemCountDBEventScript T 0x0897fc99（1480B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAddItemCountDBEventScript.md
// （双段完整阅读）。双相派发同族：6 键注册链 +0x2a8→+0x30f→+0x36d→+0x3ce→
// +0x42f→+0x4c4，公共尾 +0x559。id：[/event]0xe9 [title]0xeb [explain]0xee
// [db table]0xf1 [item index]0xf4 [quest index]0xff。[item index]/[quest
// index] 均为「槽清 0→ScanInt(&ok)→失败回主循环→push_back→重循环」的收集
// 循环（+0x47d/+0x512，此前简化形漏循环=语义缺口），vec 分别 +0x8/+0x14；
// [db table] 串存 +0x20。ok(-0xa9) 共享即返回值。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importAddItemCountDBEventScript.inc"
}

// ---- importPcBangPlayTimeScript T 0x0898512c（1661B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importPcBangPlayTimeScript.md
// （双段完整阅读）。同 importAttendanceScript 双相派发形：8 个 (键,id) 注册链
// +0x2d3→+0x33a→+0x3a7→+0x417→+0x47b→+0x4e2→+0x549→+0x5ae，公共尾 +0x611。
// id：[/event]0x5d2 [title]0x5d7 [explain]0x5da [db table]0x5dd [play time]
// 0x5e0 [number of time]0x5e3 [start tag]0x5e6 [end tag]0x5e9。[play time]
// +0xc(int)/[number of time]+0x10(byte mov %dl)/[start tag]+0x12(word mov
// %dx)/[end tag]+0x14(word) 均 ScanInt(NULL) 直存；-0x1d bool 槽初值 0 仅作
// ScanType 失败返回（+0x63e movzbl -0x1d），此外从不写。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importPcBangPlayTimeScript.inc"
}

// ---- importExchangeRandomItemRewardScript T 0x08981e0f（1701B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importExchangeRandomItemRewardScript.md
// （双段完整阅读）。双相派发同族：6 个 (键,id) 注册链 +0x2a8→+0x30f→+0x376→
// +0x3d7→+0x540→+0x5d7，公共尾 +0x636。id：[/event]0x2e8 [title]0x2ea
// [explain]0x2ed [material]0x2f0 [random reward item]0x308 [send mail]0x312。
// [material] 节（+0x428）：先 ScanType 一次（失败 mov $0,%ebx 直返假，+0x445）；
// 该 token 经 std::operator== 与 "[exchange rule]" 直比（+0x460，非查表）；
// 相等则 ScanInt(&ok) 字节存 +0x8，失败跳公共尾回主循环；两路均汇入 uint
// 整数对循环（+0x491：槽先清 0，make_pair<unsigned&,unsigned&> 引用对临时
// （P61/P81）→ pair<int,int>::pair<unsigned,unsigned> 转换构造 →
// vector<pair<int,int>>::push_back）。[random reward item]（+0x58e）int 槽每轮
// 清 0 后 ScanInt(&ok)，成功 push_back 至 +0x18；ok 共享变量即返回值（+0x663）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importExchangeRandomItemRewardScript.inc"
}

// ---- importAvatarDisjointRandomRewardScript T 0x089824b4（1429B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAvatarDisjointRandomRewardScript.md
// （双段完整阅读）。双相派发同族：5 键注册链 +0x29d→+0x304→+0x365→+0x3c6→
// +0x4ca，公共尾 +0x526。id：[/event]0x32e [title]0x330 [explain]0x333
// [random reward item]0x336 [send mail]0x346。[random reward item]（+0x417）：
// char/uint 双槽每轮先清 0，key=(char)ScanInt 字节截断存槽（+0x436 mov %al），
// 成功对经 make_pair&lt;char&amp;,unsigned&amp;&gt; 引用对临时+value_type 转换+
// map::insert 三段（P134 嵌套实参形），任一 ScanInt 失败回主循环。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importAvatarDisjointRandomRewardScript.inc"
}

// ---- importAttendanceScript T 0x08981539（2262B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importAttendanceScript.md（汇编+
// Ghidra C 双段完整阅读）。ORIG 为「函数级静态 __lookup(map<const char*,int,
// less_TCHAR>)/__init_lock(SpinLock)/__inited(bool) 三件套 + case 贯穿建表链」
// 双相派发（同 importTitleBookEtcScript 形）：首令牌以 key=0 进入注册链，自
// case 0x26f（与 case 0 合并，+0x306）起逐 case insert 全部 12 个 (键,id)
// （+0x306→+0x36d→+0x3da→+0x44a→+0x4ba→+0x52e→+0x59e→+0x60e→+0x6d1→+0x736→
// +0x7ad→+0x80b），落公共尾（+0x867）leave+置位后回 guard 重派发同一令牌。
// id：[/event]0x26f [title]0x271 [explain]0x274 [db table]0x277 [fatigue]0x27a
// [reward explain]0x299 [final reward explain]0x29c [reward]0x29f
// [final day condition]0x2ad [final reward]0x2b0 [send mail]0x2c7
// [msg overbtn]0x2cb。[reward] 节为 pair 默认构造+ScanInt(&ok) 成对循环
// （+0x65c..+0x6ca，ok 即 -0x121 返回值共享槽）；其余数值节 ScanInt(NULL)
// 直存；ScanType 失败返回 ok（+0x893 movzbl -0x121）。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importAttendanceScript.inc"
}

// ---- importUsedFatigueGiveItemEventScript T 0x0898491b（2065B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importUsedFatigueGiveItemEventScript.md
// （双段完整阅读）。同双相派发形：10 个 (键,id) 注册链 +0x2ec→+0x355→+0x3c2→
// +0x432→+0x4a2→+0x531→+0x5af→+0x609→+0x6b5→+0x72f，公共尾 +0x7a2。
// id：[/event]0x58f [title]0x592 [explain]0x595 [db table]0x598 [user type]
// 0x59c [used fatigue]0x5a1 [charc event]0x5a7 [apply job type]0x5aa
// [used fatigue limit]0x5af [send mail]0x5b5。result(-0xf9) 初值 true；
// [/event] 返回 result 本体（+0x349 movzbl -0xf9，非恒真）；[user type]
// ScanInt(&result) 字节存 +0xc、[used fatigue] int 存 +0x10、[used fatigue
// limit] int 存 +0x1c，三者失败即 mov $0,%ebx 返回假（+0x527/+0x5a5/+0x725）；
// [charc event] 置字节 +0x14=1；[apply job type] 局部 string → ScanStr →
// getCharacterJobToEnumValue 存 +0x18；[send mail] 失败仅把 result 置假并继续。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importUsedFatigueGiveItemEventScript.inc"
}

// ---- importObjectBringUpEventScript T 0x08983e48（2771B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importObjectBringUpEventScript.md
// （双段完整阅读）。双相派发同族：7 键注册链 +0x2bf→+0x326→+0x393→+0x403→
// +0x473→+0x58b→+0x626，公共尾 +0xa64。id：[/event]0x509 [title]0x50b
// [explain]0x50e [db table]0x511 [material item]0x515 [eventitem send]0x529
// [send]0x54c。[material item]（+0x4d3）：节内 ScanType 失败 mov $0,%ebx 直返
// 假（+0x4f5）；[exchange rule] 字节存 +0xc，读失败退出节回主循环（+0x536）；
// 两路汇入同一 int 收集循环（+0x53c 槽清 0，失败回主循环）。[eventitem send]
// （+0x5eb）：局部 vector<uint> fatigues；[get eventitem fatigue] 为内层收集
// 循环（+0x637），ScanInt 失败回节内头重 ScanType（+0x664，非退节）；
// [send mail] 成功 pair(vector,mail) 值临时 push +0x1c 后 fatigues.clear()
// （+0x754）并继续节内，失败退节（+0x786 je→退）；[/eventitem send] 或
// ScanType 失败或未知 token 后判定退节/继续如汇编；节退一律 dtor fatigues。
// [send]（+0x832）：局部 ObjectBringUpRule rule；三个 [reward * ] 数值节
// ScanInt(&ok) 成功→回节内头、失败→直接公共尾（rule 不清理）；[send mail]
// 失败仅退节不复位 rule（+0x997），成功三字段均 !=-1 时 push +0x28、
// rule.reset() 后回节内（+0xa58）；ScanType 失败直接公共尾。此前简化形漏
// 「fatigue 内层循环/clear 复位/mail 失败不复位 rule」三处语义，已按 ORIG 修正。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importObjectBringUpEventScript.inc"
}

// ---- importLevelUpRewardScript T 0x08980261（4824B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importLevelUpRewardScript.md
// （ORIG 汇编 + Ghidra 反编译 C 两段已完整阅读）。标签主循环 4 个：
// [recv check db table]/[level up]/[send mail]/[level up reward end]。
// ORIG 反汇编（报告 /tmp/LevelUpReward.md，已完整读 ## 汇编 + ## 反编译 C）：
// [level up] 节内是 11 个职业标签的线性 operator== 比较链，GCC 对每个分支
// 复制一整块解析代码（ScanType 子标签 + ScanInt×4 + ScanStr×2 + push_back +
// pair(char&,cond&) 构造 + map::insert + pair 析构），故 ORIG 函数体达 4824B、
// 调用 164 个。本文件按同构形态逐分支展开转写。
// 控制流要点（全部经汇编实证）：
//   - 节顶 ScanType 失败（EOF）→ 直接回主循环头（不建局部对象）；
//   - 职业分支内子标签 ScanType 失败 → ebx=0 → 销毁局部、退出本节回主循环
//     （不是返回假！此前重建误写 return false，已修正）；
//   - [/level up] → ebx=0 退出本节；未知 token → ebx=1 继续节内；
//   - 职业命中后子 token 不比较、直接丢弃；cond.m_filter0=立即数职业号，
//     filter[1]=(char)ScanInt(&ok)；reward 循环三 ScanInt 短路检查（任一失败
//     即跳出到 insert），随后两 ScanStr + push_back；
//   - insert 键经 pair<char&,Condition_LevelUp&> 模板推导（ORIG 符号
//     _ZNSt4pairIKi17Condition_LevelUpEC2IRcRS1_EEOT_OT0_），故键实参必须是
//     char 左值（本地 char 变量持有 filter[0] 值）；
//   - ok(=result) 为跨全部分支共享的局部 bool，ScanInt 全写它，且
//     [level up reward end] 置真后作为返回值。
// 残差归因（N 类）：ORIG(4.1) 把 token(-0x140) 与 11 个职业分支各自的
// cond/ri/临时槽在深区按活跃性交错复用（帧 0x170），4.4 分配器为每变量
// 固定独立槽（帧 0x1a0、token -0x68），几乎每条含槽指令文本错位——跨版本
// 栈槽分配器差异（P112/N76 族），无法由合法 C++ 复现；语义与指令序列已
// 逐分支对照 ORIG 汇编一致。
#include "EventScriptsCompleteB2/importLevelUpRewardScript.inc"
}
// ---- importPurchaseCashItemBonusEventScript T 0x089834a6（2404B）----
// 报告 docs/class_func_reports/_lt_global_gt_/importPurchaseCashItemBonusEventScript.md
// （双段完整阅读）。双相派发同族：8 键注册链 +0x2e0→+0x347→+0x3b4→+0x424→
// +0x494→+0x54a→+0x5be→+0x6aa，公共尾 +0x933。id：[/event]0x4a1 [title]0x4a3
// [explain]0x4a6 [db table]0x4a9 [item idx]0x4ac [item idx start]0x4b6
// [item idx end]0x4b9 [send]0x4c6。函数级 uint start(-0x13c) 初值 0（+0x45）。
// [item idx]（+0x4f1）uint 槽清 0→ScanInt(&ok)→set<unsigned>+0xc insert，
// 失败回主循环；[item idx start] ScanInt(NULL) 存 start；[item idx end]
// （+0x61e）uint end=ScanInt(&ok)，ok&&start!=0&&end!=0 时
// `for(;start<=end;++start) set.insert(start)`（+0x695 setbe 无符号界）。
// [send] 节（+0x6fe）：局部 rule ctor 后节内 while(ScanType)，四标签经
// std::operator== 直比（不入查表）：三个 [purchase * count] ScanInt(&ok)
// 成功→节内 continue、失败→退出节回主循环（+0x776/+0x778，此前简化形方向
// 写反=语义缺口，现按 ORIG 修正）；[send mail] 局部 mail ctor+reset，
// importActionMailScript 失败立即退出节（rule 不复位保留原值，+0x861..868），
// 成功则三字段均 !=-1 时 push_back(pair(rule,mail) 值临时+~pair)、随后
// rule.reset() 后回节内（+0x924 test ebx/jne +0x71b）。未知 token 忽略
// 继续；节内 ScanType 失败退出节回主循环。ok(-0x135) 共享即返回值（+0x960）。
// 残差归因（N 类，等计数 563=563）：4 节 ok 检查点 je/jne 朝向（4.1 先
// xor 物化再反判 vs 4.4 直接 test 反转目标，setcc-fold 族）、节头 4 枚
// 前跳 nop;jmp 岛（+0x70e..+0x717，jump-threading 族）、pair 构造实参
// %eax↔%edx（N7）、make_pair sret 后 `sub $0x4` 伴随（P75 族）均不可由
// 合法源码翻正；[item idx end] 的 `xor;test;jne` 已用 != true 形复现。
// 2026-09-10：`redo_lookup` 后跳 goto 改写为内层 for(;;) 重试循环（消 4.4
// entry jmp；判据① 全 TU 分支方向差 = 0）⇒ DIFF→AE。
#include "EventScriptsCompleteB2/importPurchaseCashItemBonusEventScript.inc"
}

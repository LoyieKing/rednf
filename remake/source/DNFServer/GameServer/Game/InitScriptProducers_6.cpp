// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_6/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// InitScriptProducers_6 —— CDataManager::init（ORIG 0x08358332..0x0835f1d5，
// docs/class_func_reports/CDataManager/init.md）依赖的脚本装载自由函数·第 6 批。
//
// 本 TU 独家提供下列 ORIG T 符号的真实定义（逐指令转写自 df_game_r，
// 各函数 ORIG 地址见实现处注释；节名键串取自 .rodata 实测）：
//   importAvatarRouletteList            0x089116eb (+ Detail 0x089115b1)
//   AvatarRoulette::{C1,D1,clear}       0x08911b98 / 0x0837e484 / 0x08911cf6
//   importBloodDungeonScript            0x08909016
//   importCharacLinkSystemScript        0x08909529
//   importCharacterCreateCreatureGiftScript 0x0896001a
//   importCharacterCreateGiftScript     0x0895eda1
//   importConditionEventChkDungeonScript 0x08913891
//   importConditionEventScript          0x0890e868
//   importDBLogItem                     0x0890dbba
//   importDailyScheduleEtcScript        0x0891826c
//   importDeathTowerScript              0x089089f3
//   importDimensionActivationData       0x089160f0
//   importEmotionTypeTagsScript         0x0890de85
//   importEquipmentGradeSampleInfo      0x0890e18d
// 及其共享叶子依赖（ORIG 均为 T，源码此前无任何定义，已广播认领）：
//   ConvertStringToTime                 0x088c2341
//   ConvertHHMMSSStringToInt            0x088c25df
//   ConvertWeekdayStringToInt           0x088c284a
//   getCharacterJob                     0x088be57e
//   getGrowTypeIndex                    0x088be63d
//   makeFairPVPEquipmentPackageKey      0x088945ad
//   tokenizeStr                         0x088945e2
//   getCharacterJobToEnumValue          0x089bbc0f
//
// 统一模式：loadRDARScriptFile("", path) → ScanType 分派节名 → ScanInt/
// ScanFloat/ScanStr 字段回填。ORIG 中 string→分支 的静态 map<const char*,int>
// （__lookup/__init_lock/__inited BSS 桩）是编译器对「按行号分派」的缓存 lowering，
// 观测语义等价于顺序串比较——重建一律采用 if/else 链，不引入全局可变状态。
// ============================================================================

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <utility>
#include <vector>
#include <map>

#include "CDataManager.h"             // 脚本类型权威布局（stDeathTower_t 等）
#include "sync_script.h"             // STEquipmentPartSetPiece/STEquipmentScript
                                     // （DataManagerInitDepends.h 前置要求）
#include "DataManagerInitDepends.h"   // 本批 import* 声明 + AvatarRoulette 完整视图
#include "DNFLexWrapper.h"            // loadRDARScriptFile/unload/Scan*/ScanTypeOrInt
#include "CEnvironment.h"             // less_TCHAR / SpinLock（getGrowTypeIndex __lookup）

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

// ============================================================================
// 共享叶子 helper
// ============================================================================

// ----------------------------------------------------------------------------
// ConvertStringToTime —— ORIG T 0x088c2341。"YYYY-MM-DD HH:MM:SS"（19 字符）
// → mktime。substr(0,4)/(5,2)/(8,2)/(11,2)/(14,2)/(17,2) 各 atoi 后直接写入
// 栈上 struct tm 对应槽位（tm_sec..tm_year；wday/yday/isdst 不初始化，ORIG 如此，
// mktime 会归一化）。长度非 19 返回 false；成功写 out 并返回 true。
// ----------------------------------------------------------------------------
#include "InitScriptProducers_6/ConvertStringToTime.inc"
}

// ----------------------------------------------------------------------------
// ConvertHHMMSSStringToInt —— ORIG T 0x088c25df。"HH:MM:SS"（8 字符）：
// h=atoi(substr(0,2))、m=atoi(substr(3,2))、s=atoi(substr(6,2))。
// 长度非 8 返回 false。
// ----------------------------------------------------------------------------
#include "InitScriptProducers_6/ConvertHHMMSSStringToInt.inc"
}

// ----------------------------------------------------------------------------
// ConvertWeekdayStringToInt —— ORIG T 0x088c284a。SUN..SAT→0..6、ALL→7；
// 未命中返回 false 且不触碰 out（ORIG 仅在命中路径写 (%edx)）。
// ----------------------------------------------------------------------------
// b28d-solo-InitScriptProducers_6: P63 共享尾（else-if 链 + 尾部统一 return true）。
// ORIG 用 `s.compare("X") == 0`（int compare → `test %eax,%eax; sete %al; test; je`），
// 非 operator==(bool)。ORIG 0x088c284a。
#include "InitScriptProducers_6/ConvertWeekdayStringToInt.inc"
}

// ----------------------------------------------------------------------------
// tokenizeStr —— ORIG T 0x088945e2。先求首/尾锚点并 clear 输出容器，随后
// while(end!=npos || start!=npos) 循环 push_back(substr(start,end-start)) 并
// 推进双锚点。循环条件为「或」（88946cd 处 jne/je 组合实证），与常规实现不同。
// ----------------------------------------------------------------------------
#include "InitScriptProducers_6/tokenizeStr.inc"
}

// ----------------------------------------------------------------------------
// getCharacterJob —— ORIG T 0x088be57e（定义在 SyncScriptDeps.cpp，本 TU 仅声明
// 引用）。全局作业名表 g_characterJobNames（ORIG .data 0x93f4d40，12 项实测）：
// "[swordman]".."[creator mage]" 共 11 项 + "[all]"；i=0..10 命中返回 i，否则 11。
// ----------------------------------------------------------------------------
#include "InitScriptProducers_6/getCharacterJob.inc"
#include "InitScriptProducers_6/makeFairPVPEquipmentPackageKey.inc"
#include "InitScriptProducers_6/getCharacterJobToEnumValue.inc"
typedef std::map<const char*, int, less_TCHAR> GrowTypeMap;

// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — p167 调用点差集 CLEAR（71=71，
// 语义完整）；残差 N82（帧/槽号 673 vs 687）+ N89（libstdc++ 模板全内联）。
// [SweepInitProd9 本轮] 负极性反例实测（勿再试）：把 25 个 case 体统一改为
// `if (!__inited) { insert } else { return N; }`（P333 负极性形，与同 TU
// importDimensionActivationData 的 break 版不同——此处 else 是 return）后
// ourN 687→**737**（gap −14→−64）、jcc 目标块差异段 13→13 且 jcc 数 91/109→
// 91/127，明显反劣 ⇒ 已回滚。P333 的负极性守卫形仅在 else 为 `break` 时成立。
// [SweepInitProd9 本轮·回边方向审计] ORIG backward 2 / 我方 1：两条 continue
// （0x7b2 guard 重试 / 0x1343 尾部置位）汇聚共享尾块 0x134c → `jmp 687`(+0x13
// 循环头)，ORIG 为两条独立 jmp 直达 +0x13 ⇒ 共享块合并，**回边未丢失**。
// [C03CryptoInit68 复核·2026-09-10] p167 双向差集 71=71，MISSING/EXTRA 全部是
// C1↔C2 同址别名伪差（`_ZNSt4pairIKPKciEC2I…` vs `…C1I…`、`_Rb_tree_const_iterator`
// 的 C1/C2、`_ZNSt3mapI…EC2Ev` vs `C1Ev`、`SpinLockC2Ev` vs `C1Ev`）⇒ 无真语义缺口（P93）。
// ORIG 0x088be63d。
#include "InitScriptProducers_6/getGrowTypeIndex.inc"
}

// ============================================================================
// AvatarRoulette 生命周期（ORIG C1 0x08911b98 / D1 0x0837e484 / clear 0x8911cf6）
// ============================================================================

#include "InitScriptProducers_6/AvatarRoulette.inc"
}

#include "InitScriptProducers_6/dtor_AvatarRoulette.inc"
}

// clear —— ORIG 0x8911cf6：j=0..10 双层清空 avatar 数组；stiker 数组不动。
// b28d-21: IDENTICAL（55=55）— P183 删 `vector& bucket = m_avatars[j]` 引用缓存
// （ORIG 每轮重发 j*12+0x14 全链）+ 具名 int n 槽 + 有符号比较（setl）。
#include "InitScriptProducers_6/clear.inc"
}

// ============================================================================
// importAvatarRouletteListDetail —— ORIG T 0x089115b1
//   bool (AvatarRoulette&, std::string&, ENUM_CHARACTERJOB)
// 内层装载器：至多 9 次 ScanTypeOrInt 取数值（遇 TYPE 提前终止），累计入局部
// pairs{value,3}；pairs 非空时整体 push 进 m_avatars[job]，再 ScanInt 一枚
// sticker push 进 m_stikers[job]。恒返回 true。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（97=97）— ORIG 0x089115b1。value(-0x28，预清 0
// 且 ScanInt 结果复用)、isType(-0x29)/isInt(-0x2a)；pair 直构
// `pair<unsigned long,int>(value, 3)`——第二实参须为**右值字面量**（p167 实证
// ORIG 为 `C1IRiiEEOT_OT0_` = pair<int&,int&&>，具名 three 左值会选错重载）；job 形参直用。
#include "InitScriptProducers_6/importAvatarRouletteListDetail.inc"
}

// ============================================================================
// importAvatarRouletteList —— ORIG T 0x089116eb
//   bool (char* path, AvatarRoulette&)
// [avatar coin info]/[rare clone avatar rate]/[rare avatar rate]/
// [upper avatar rate]/[normal avatar rate] 五个标量 + 十个 job 节
// ([swordman]..[DemonicSwordman])，每节 do{Detail}while(token==[/节名])。
// EOF（ScanTypeOrInt 非型且返 0）或 load 失败之外的出口恒为 true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089116eb。真语义 bug 已修：job 节 do-while 继续条件原写
// `token == "[/节名]"`（方向反，会多吃一个节头），按 ORIG 0x089118a5 `je body`
// 实证改为 `!(token == "[/节名]")`；十个 job 节为**独立 if 序列**（非 else-if）。
// 本轮：十节 do-while 改 `for(;;){Detail; if(token=="[/节名]")break;}` 复现 4.1
// 形 `jmp body; nop; body…; test %al,%al; je body`（do-while 在 4.4 恒产
// `xor $1; test; jne body`）⇒ gap 21→2（raw 328 vs 326，norm 307 vs 305）。
// 残差 2 条：① m_avatarCoinInfo 首节 store ORIG `mov %eax,%edx; mov this,%eax;
// mov %edx,(%eax)` RHS-first+中转 vs 我方 LHS-first（T1–T4 无字段类型直证，
// P321 T5 禁以装载序改类型）⇒ N76。
// [SweepInitProd9 本轮] ② 已翻正：旧判「空 else/continue 两形不闭合」被 P223
// **尾空标签**形推翻——五个标量节尾各 `goto after_sections;`（ORIG 0891179f/
// 089117cd/089117fb/08911829/08911857 的 `jmp 08911b5c`），循环体尾置
// `after_sections:;`（=ORIG 08911b5c 共享中继岛 `jmp 主循环头`）。实测：
// jcc 48=48、目标块签名差异段 6→**0**（逐位全等）；ourN 305→306、ORIG 307
// ⇒ gap 2→1。剩余唯一条 = N76 store 序（`mov %eax,%edx` 中转）。
// p167 CLEAR（47=47）。
#include "InitScriptProducers_6/importAvatarRouletteList.inc"
}

// ============================================================================
// importDeathTowerScript —— ORIG T 0x089089f3
//   int (const char*, stDeathTower_t&)，0=成功，否则 init.md Fail(%d) 行号码。
// 八个节：单值节带错误码；四个 200 表节从下标 1 开始连续 ScanFloat/ScanInt
// 直至失败（ORIG 无边界检查，忠实保留）；未知节名 → 0x18cd 错误。
// 所有 Scan 均为「先写入字段、后校验 ok」（fstp/mov 先于 ok 判定；
// [reward item prob] 三列前缀和亦同），与 ORIG 指令顺序一致。
// ============================================================================
// b28d-21 复验 — ORIG 0x089089f3。已修：ScanType 失败走 break + 循环外 return 0
// （ORIG `jne <返回0岛>` 0x08908d0f）；表节循环 `field[i]=Scan; ++i; if(!ok)break`
// （ORIG `fstps; addl; xor; test; je body`）；[reward item prob] 三列前缀和无具名
// 中转局部。p167 CLEAR（24=24）。残差 gap 1（raw 243 vs 242，norm 234 vs 233）：
// ① 3 处 store ORIG 为 RHS-first+`mov %eax,%edx` 中转（[apc exp weight] 首节
// 08908a73 段 + [reward card num] 08908b06 段 + [illusion reward card num]
// 08908ba8 段）vs 我方 LHS-first——m_rewardItemProb/m_rewardCardNum 的 int/unsigned
// 无 T1–T4 直证，且改型需动 CDataManager.h + 4 个消费 TU（P321 T5 禁）⇒ N76；
// ② [reward item prob] 两个 += 段我方 `lea (%edx,%eax,1); mov %eax,%edx` vs ORIG
// `add %eax,%edx`（N4/P30）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 233（gap +1）；jcc 40=40 且
// **目标块签名差异段 = 0**（逐位交叉验证全等，含全部条件方向/落点）——控制流
// 与 ORIG 完全同构，残差纯为上述 N76 store 序 + N4/P30 与 1 条 nop。
#include "InitScriptProducers_6/importDeathTowerScript.inc"
}

// ============================================================================
// importBloodDungeonScript —— ORIG T 0x08909016
//   int (const char*, stBloodDungeon_t&)，0=成功。
// [ultimate reward prob] 以 map find 判重：key 已存在 → 0x196e 错误。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x08909016。已修：
// [ultimate reward prob] 改 `it == end()` 正极性 + `make_pair(key, prob)` 经
// pair<const int,T> converting ctor（P61/P178），p167 由 3 MISSING/2 EXTRA 转
// CLEAR（33=33）；[reward exp weight] 改 `++i` 先于 ok 检查。残差 N112（nop ×）
// + P211 store 序 + 槽（271 vs 264）⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 263 / ourN 265（gap −2），jcc 36=36、
// 目标块差异 7 段（全部为 ORIG nop 中继岛/块首落点差，jcc 方向与数量逐位一致）。
// [C03CryptoInit68 修·2026-09-10] 按 P0/配方⑦ 删 4 处「ORIG 无槽」具名 int 局部：
//  `[exp standard monster cnt]` 与 `[reward item prob]` 三次 ScanInt 的结果在 ORIG
//  均为 `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0xN(%eax)` **直存**
//  （0x089090e4/0x0890912e/0x08909164/0x0890919a），无 v/v1/v2/v3 槽 ⇒ 全部内联。
//  读数：ourN 265→**264**、本次报告 origN 271（新 r_sdc4 口径），jcc 目标偏移与 ORIG
//  逐位对齐（`je +0x45`/`+0x162` 等全等）；帧由 0x70 回到 0x60 邻域。
// 残余（跨版本族）：ORIG 直存的 RHS-first 中转（`mov %eax,%edx`）vs 我方 LHS-first
// 直存（P211/P76：字段与 ScanInt 返回同型 ⇒ 4.4 恒 LHS-first，源侧不可择）；
// 以及 3 处 ORIG `nop; jmp` 中继岛（N112/P223）⇒ 记 N 封顶。
#include "InitScriptProducers_6/importBloodDungeonScript.inc"
}

// ============================================================================
// importCharacLinkSystemScript —— ORIG T 0x08909529
//   int (const char*, stCharacLinkSystem_t&)，0=成功。
// [link level] 写 +0x00；四个 bonus 节各自选定目标 vector 后 do-while 连续装
// 4×ScanInt 记录；未匹配节名静默跳过（ORIG al=0 → 回外层）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08909529 — 第二个 string 局部
// unused(-0x18，仅 ctor/dtor)；[link level] ScanInt 直存无局部；四 bonus 节
// 合并 `||` 短路链后**四路** else-if 选 target（第 4 路仍比较，无裸 else）。
#include "InitScriptProducers_6/importCharacLinkSystemScript.inc"
}

// ============================================================================
// importCharacterCreateGiftScript —— ORIG T 0x0895eda1
//   bool (createChracScript*, const char*)
// [apply job] 记忆当前 job（getCharacterJobToEnumValue）；[/apply job] 把累积的
// rewardInfo 挂入 m_map0[job] 并复位累积器；[/create character gift] 返回 true；
// EOF 未遇结束符 → false；时间串转换失败 → false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: IDENTICAL（634=634）— 清单标注 NEAR2 为旧读数；
// 本波 fresh 复验已闭合（双相派发 + 循环内静态三件套 P177 形）。ORIG T 0x0895eda1。
#include "InitScriptProducers_6/importCharacterCreateGiftScript.inc"
}

// ============================================================================
// importCharacterCreateCreatureGiftScript —— ORIG T 0x0896001a
//   bool (CharacterCreateCreatureGiftScript*, const char*)
// ORIG 双相派发（与 importTitleBookEtcScript/importAttendanceScript 同构）：
//   函数级静态 __lookup(map<const char*,int,less_TCHAR>)/__init_lock(SpinLock)/
//   __inited(bool) 三件套位于外层 token 循环体内（每轮穿过 guard 测试，
// ORIG +0x8e/+0x108），注册完成 SpinLock::leave + __inited=1 后 jmp +0x8e 以
// 当前令牌重查表派发。case 贯穿链按 [event job]→[start]→[end]→[/event job]→
// [change able creature lev]→[change infinity creature]→[client infinity
// creature] 顺序 insert(0x277/0x27d/0x281/0x285/0x28c/0x290/0x2a2)。
// 收敛口径：orig=ours=487，助记符序列全等、栈槽逐条一致；残余差异仅为
// .o 未链接的 guard/静态对象/串字面量重定位显示（.rodata 内容与 5 个局部
// 静态符号名 _ZZ…E8__lookup/11__init_lock/8__inited+双 guard 实测与 ORIG
// 一致），链接后按 AE 口径判等。
// ============================================================================
// b28d-solo-InitScriptProducers_6: IDENTICAL（482=482）— 清单标注 NEAR2 为旧读数；
// 本波 fresh 复验已闭合（P164 EOF 落点形）。ORIG T 0x0896001a。
#include "InitScriptProducers_6/importCharacterCreateCreatureGiftScript.inc"
}

// ============================================================================
// importConditionEventChkDungeonScript —— ORIG T 0x08913891
//   bool (const char*, ConditionLevelChkDungeon*)
// [apply level] 写 +0x0c；[unable dungeon] 清空后 do-while push_back；
// 无 unloadRDARScriptFile（ORIG 如此）；out 为空指针时返回 false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: ORIG 0x08913891 — load 检查在最前，之后按
// token(-0x10)/unused(-0x14，仅 ctor/dtor 零引用，P171)/ok(-0x15) 声明；
// 循环出口用 break（ORIG `jne exit`）而非 return true。
#include "InitScriptProducers_6/importConditionEventChkDungeonScript.inc"
}

// ============================================================================
// importConditionEventScript —— ORIG T 0x0890e868
//   bool (const char*, stConditionEventInfo*)
// [id]→+0x00；[condition match]→map+0x08（key,(v1,v2)）；
// [condition reward]→multimap+0x20；[condition quick party add reward]→
// multimap+0x38。两条 reward 节均扫 1 key+4 值，末两值 setne 成 bool。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890e868。已修：load 检查
// 最前，token(-0xa0)/unused(-0xa4)/ok(-0xa5) 置于 load 之后。p167 CLEAR（39=39）、
// 323=323、助记符序列剥离 nop 后全等；残差仅 N112（循环入口/失败岛 nop ×3）⇒ N。
#include "InitScriptProducers_6/importConditionEventScript.inc"
}

// ============================================================================
// importDBLogItem —— ORIG T 0x0890dbba
//   bool (char*, std::vector<stDBLogItem>*)
// 外层每轮消耗两个节头：第一 ScanType 匹配 [dblog buyitem]（flag=0 批量）、
// 第二匹配 [dblog getitem]（flag=1）；记录={id,ConvertStringToTime(str),flag}，
// id ScanInt 失败即节尾。任一 ScanStr/时间转换失败 → 整体 false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890dbba。已修：删两处
// `std::string tmp(str)` 中转，直接 `ConvertStringToTime(str, ...)`（by-value 形参
// 自身拷贝即可），p167 由 6 EXTRA 转 CLEAR（28=28）。
// [SweepInitProd9 本轮] ORIG 0x0890dbba 逐位复验 + 三处 P0/P171 修复：
// ① `int id = ScanInt(&ok); item.m_itemIdx = (unsigned)id;` → 内联
//    `item.m_itemIdx = (unsigned)ScanInt(&ok);`（ORIG 0890dc70/0890dd82
//    `mov %eax,-0x34/-0x40` 直存，无 id 槽往返）——删 2×2 条；
// ② `bool convOk = (ConvertStringToTime(...) == 1); if (!convOk)` → 裸
//    `if (!ConvertStringToTime(...))`（ORIG 0890dcc5 `mov %eax,%ebx; xor $1,%ebx`
//    无 bool 槽）——删 2×1 条；
// ③ 补 ORIG 三处死写入（P171）：0890dc03 `movl $0x0,-0x14`（prologue）、
//    0890dc43 `movl $0x0,-0x10`（buyitem 入口）、0890dd55 `movl $0x0,-0xc`
//    （getitem 入口）——补 3 条；声明序按 ORIG 槽浅→深排 deadA/token/str/ok，
//    result（4.4 共享尾替代 ORIG 的 ebx 承载）声明最后。
// 读数：ourN 202→199 = ORIG 199（gap 0）；jcc 28/27（ORIG 尾部 EOF 岛
// 0890de2a `jmp 0890de2d` 被 4.4 jump-thread，P223 尾标签法在此跨非平凡
// 局部声明不可用）+ 8 段目标块序差（块首 string-ctor 块 vs memset 块）+ 槽整体
// 深 4（result 槽，ORIG 用 ebx）⇒ N82/N95/P223 封顶。
// [C03CryptoInit68 修·2026-09-10] 上条「槽整体深 4」的真因已定位并翻正（P0/P244/
// P231 三配方联用，ourN 199→203、ORIG 204）：
//  ① `register bool result;`（裸 register，非 asm 绑定，P231/P307 合法）⇒ result
//     免槽进 **%ebx**，token/str/ok/deadA 槽位与 ORIG 逐值一致（-0x20/-0x24/-0x25/-0x14）；
//  ② result **不带初值**、两处 ScanType 失败改 `goto eof`、`eof: result = true;`
//     ⇒ `mov $0x1,%ebx` 落在循环之后（ORIG 0x0890de2d），prologue 无多余 `movb $1,槽`；
//  ③ `int deadA` 声明下移到两个 string 之后（ORIG 0890dbfc 在 SsC1Ev×2 之后）。
// 残余（唯一）：尾部 EH 清理块相对序 —— ORIG 0x0890de49 处一条 `jmp +0x28f` 领起
// cleanup 链，我方该跳与后续块序互换（N104「EH 尾块相对序」族，difflib 放大为 4 行）。
// 语义等价，按 N104/P223 封顶（本件已由 DIFF 降为「仅尾块序」）。
#include "InitScriptProducers_6/importDBLogItem.inc"
}


// ============================================================================
// importDailyScheduleEtcScript —— ORIG T 0x0891826c
//   int (const char*, std::vector<_DailySchedule>&)，0=成功。
// [schedule] 内 do-while 连续装配 _DailySchedule（0x24）：3×ScanInt +
// 5×ScanStr（后四者失败带 0x3721..0x3737 行号码）+ 两次 ConvertStringToTime
// （+0x0c/+0x10）+ weekday（+0x14）+ 两组 ConvertHHMMSSStringToInt（+0x18..）。
// ============================================================================
#include "InitScriptProducers_6/importDailyScheduleEtcScript.inc"
}

// ============================================================================
// importDimensionActivationData —— ORIG T 0x89160f0
// ORIG 结构（/tmp/importDimAct.md 汇编+Ghidra C 双段实证）：
// 外层节名 = 函数级静态 __lookup map<char const*,int,less_TCHAR> +
// __init_lock(SpinLock) + __inited 双检贯穿 insert 建表链（5 条：
// "[play day count]"0x3488→"[coin info]"0x348d→"[section]"0x34b7→
// "[select group]"0x34ca→"[give group]"0x34ff），建表后 find 派发；
// coin info 难度 = 第二张静态 __lookup_0（4 条：[easy]0x3499→0、
// [medium]0x349e→1、[hard]0x34a3→2、[ultimate]0x34a8→3、未命中保持
// 入口 -1 且**照常**执行公共尾 insert），两表键均 less_TCHAR 大小写无关。
// 数据落点：[play day count]→out+0x00（ScanInt 返回值直写，不查 flag）；
// coinMap(+0x04) insert(coinType, DimensionCoinInfo{+0=首 ScanInt int、
// +4=二 ScanInt!=0 的 setne 字节})；[section] 装配 EventRewardSection 入
// m_rewardData.m_vec0(+0x1c)；[select group] 按 makeFairPVPEquipmentPackageKey
// (getCharacterJob,idx) 入 m_mapC(+0x28)；[give group] 按 (job,growIdx) 入
// m_map24(+0x40)。节内 ScanStr 失败 → return false；EOF(ScanType 失败) → true。
// ============================================================================
// b28d-21 复验 — ORIG 0x089160f0（0x0890c1f0 段为同 TU 邻居）。p167 CLEAR（175=175）。
// 本轮修真语义 bug + 归位形态，指令数 1143 vs 1122 → 1143 vs 1128（norm 1127 vs 1128）：
// ① **[section] 循环条件方向反**（真 bug）：ORIG 089169da `movzbl ok; xor $1; test;
//    je 记录块` ⇒ 原码 `if (!ok) break;`；旧写 `if (ok) break;` 在成功读到记录时反而
//    停止读 m_field1/m_field2/count/items。
// ② 9 处静态表守卫改负极性 `if (!__inited) { insert } else { 派发 }`（ORIG 089163b5/
//    0891642a/08916958/08916ac7/08916fa6/089166e9/08916755/089167c1/0891682a 均
//    `movzbl; xor $1; test; je 派发`）⇒ 各补回 1 条 xor + 1 条块尾 jmp。
// ③ 两张表的 guard 循环头改正极性 `if (__inited) { find } else { 锁 }`（ORIG 08916272/
//    089165b1 `movzbl; test; je 锁`，无 xor）。
// ④ 两处公共尾改 `if (!__inited) { leave; inited=1; continue; } break;`（ORIG 08916891/
//    0891735b `movzbl; xor $1; test; je …`）。
// ⑤ `int v = 0;` 单变量跨节复用（ORIG Ghidra local_1ac@-0x1a8：prologue 清零 +
//    coin-info used + section count + give-group value），旧拆 4 个局部致全帧槽号漂移。
// ⑥ 两串声明移到 load 检查之后（ORIG 08916130/0891613e ctor 在 load 后）；EOF 改
//    `break` + 循环外 `return true`（P164 尾部共享块）；`int first;` 去预清；
//    give-group 死存补具名 `dead20`（ORIG 0891710f `mov %eax,-0x1c`）。
// 残差 1 条（我方多）：块序差异（items 构造/tokenizeStr 段位置）+ 槽号整体深 4
//（N95/N126 版本首槽基址常数）+ 1 条尾 jmp ⇒ N82/N95 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 1143 / ourN 1141（gap +2）；jcc 155/156、
// 目标块差异 9 段（delete/insert 成对，均为静态表守卫块与 [section]/[give group]
// 内层循环的 nop 中继岛落点差，jcc 方向无互换）⇒ 维持 N82/N95，无真反。
// [SweepInitProd9 本轮·回边方向审计] ORIG backward 16 / 我方 14：差的 2 条 = 4.4
// 把 4 条 continue（leave 点 0x494d/0x4c8b/0x4e99/0x5960）合并到 2 个共享尾块
// （0x5974→jmp 4773=+0x8e 主表 guard 头；0x4ea6→jmp 4ab1=+0x3cc 子表 guard 头），
// ORIG 为 4 条独立 jmp 直达同目标 ⇒ **无回边丢失、无「退出循环」误写**。
// [C03CryptoInit68 复核·2026-09-10] p167 双向差集 175=175；MISSING 全为 D2/C2 与
// 我方 D1/C1 的**同址别名伪差**（`_ZNSsD2Ev`/`_ZNSsC2Ev`、`_ZN19EventRewardItemInfoD2Ev`、
// `_ZNSaIcED2Ev`、`_ZN18EventRewardSectionD2Ev`、`_ZNSt3map…C2Ev`、`SpinLockC2Ev`、
// `_ZNSt4pairIKPKciEC2I…` 等），EXTRA 侧为其 C1/D1 对应物 ⇒ 无真语义缺口（P93）。
#include "InitScriptProducers_6/importDimensionActivationData.inc"
}

// ============================================================================
// importEmotionTypeTagsScript —— ORIG T 0x0890de85
//   bool (const char*, stEmotionTypeTagsInfo*)
// [EMOTION INFO] 内：tag 入 m_tags(+0x00) 且记入 stOneEmotionData.m_0；
// 随后 4 值/3 布尔/字符串族填充元素后 push 入 m_data(+0x0c)；$END 结束
// m_18 字符串列表。任一 Scan 失败 → false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890de85。已修：load 检查
// 最前；函数级 token(-0x10)/unused(-0x14)/ok(-0x15)，out 判空在声明之后；`str`
// 改为 [EMOTION INFO] 块内局部（ORIG -0x1c）。p167 由 3 MISSING 转 CLEAR（29=29）。
// 残差 N86/N89（string 构造/析构族）+ N82 槽 ⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 245（gap −11），jcc 38/43
// （多 5 个）。逐位核对：3 处 `data.m_10/11/12 = (v==1)?1:0` 的 ORIG 形是
// `mov slot,%eax; cmp $1; sete %al`（无分支），4.4 恒折 `cmpl $1,slot; jne;
// mov $1; jmp; …`（每处 +1 jne +1 jmp）⇒ 属 P197④/setcc-fold 负例（4.1 槽比
// sete 不可复现，探针族已证），非语义差；其余 2 段为块首落点差。语义等价。
// [C03CryptoInit68 修·2026-09-10（推翻 N197④ 判词）] 原 `data.m_1x = (v==1) ? 1 : 0`
// 产**分支形**（`cmpl $1,slot; jne; mov $1; jmp; mov $0`）；ORIG 是
// `mov -0x20(%ebp),%eax; cmp $0x1,%eax; sete %al; mov %al,-0x34(%ebp)`。
// **钥匙 = 直接 bool→字节字段赋值**：`data.m_10 = (v == 1);`（不加三目、不加
// static_cast）⇒ 4.4 逐条复现 ORIG 的 `cmp/sete/直存`，三处同改后
// **ourN 248 → 239 = ORIG（gap −11 → 0）**。
// （反例留痕：`static_cast<char>(v == 1)` 与三目同产分支形；`unsigned int v` 局部
//  不改变内存直比；补 `int v4` 具名中转 ⇒ 239→241 且全槽整体深 4，已回退。）
// 残余（跨版本族，语义等价）：① 比较形 —— ORIG `mov -0x20,%eax; cmp $1,%eax`
// 载寄存器 vs 我方 `cmpl $1,-0xc` 内存直比（P209/P286 族，局部实参 int 不可择）；
// ② 失败路径返回寄存器 %ebx(ORIG) vs %esi(我方)（N7/N18a 寄存器角色）；
// ③ ORIG 0x0890dedf 一处 `jmp+nop` 岛（P223/N112）。
// p167 复核：本件 MISSING/EXTRA 全为 C1↔C2 同址别名伪差（`_ZNSsC2Ev` vs `_ZNSsC1Ev`、
// stOneEmotionData C1/C2），无真语义缺口（P93）。
#include "InitScriptProducers_6/importEmotionTypeTagsScript.inc"
}

// ============================================================================
// importEquipmentGradeSampleInfo —— ORIG T 0x0890e18d
//   bool (const char*, stEquipmentGradeSample*)
// [grade_0]..[grade_4] 五节分别向五个 vector<unsigned int> push（push_backEOj
// 右值重载，值序一致）；out 判空在 load 之后（ORIG 0x890e1d3）。
// ============================================================================
// b28d-solo-InitScriptProducers_6: AE（190=190）— ORIG 0x0890e18d。load 检查最前，
// 随后 token(-0x34)/unused(-0x38，零引用)/ok(-0x39) 声明，再 out 判空；五节内层
// `for(;;){ v=ScanInt(&ok); if(!ok)break; push_back((unsigned int)v); }`
// （ORIG `xor $1; test; je body` 形；push_back 取右值重载 EOj，p167 CLEAR）。
#include "InitScriptProducers_6/importEquipmentGradeSampleInfo.inc"
}

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_8/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— InitScriptProducers_8.cpp
// [C03CryptoInit68 复核·2026-09-10] 本 TU 的 4 个 DIFF 件
// （importUnlimitChallengeScript / importVendingMachineInfoCore /
//  importUseJobFatigueGiftScript / importSeriaRoomAnimationDecoEvent）
// 经 p167 双向调用点差集复核（121=121 / 39=39 / 62=62）：ORIG-only 与 our-only
// **全部是 C1↔C2 / D1↔D2 同址别名伪差**（`_ZNSsC2Ev` vs `_ZNSsC1Ev`、
// `stUnlimitChallengeMessageInfoC2Ev` vs `C1Ev`、`_ZN22stVendingMachineInfo_tC2Ev`、
// `useCharacterJobFatigue::fatigueMatchGiftC2Ev`、`_ZNSt4pairIKPKciEC2I…` vs `C1I…`、
// `_ZNSt23_Rb_tree_const_iteratorI…EC2E…` vs `C1E…` 等），**无真语义缺口**（P93）。
// 残差均属跨版本形态族：N85 x87 链 / N82 槽复用 / N112·P223 nop 中继岛 /
// P211 store 装载序 / N130 右值重载；语义等价，按 §6 记 N 封顶。
// CDataManager::init()（ORIG 0x08358332..0x0835f1d5，docs/class_func_reports/
// CDataManager/init.md）依赖的 12 个脚本装载自由函数 + 被调依赖
// importVendingMachineInfoCore（本 TU 定义）。importDeathTowerScript 唯一定义
// 在 InitScriptProducers_6.cpp（重复强符号已清理），本 TU 仅声明供
// importTowerInfo 调用。逐函数自 ORIG objdump
// 反汇编与 docs/class_func_reports/_lt_global_gt_/ 对应报告转写，非固定返回：
//
//   importResealTable                _Z17importResealTablePKcP14reseal_table_t      T 0x089008b8
//     init.md +0x5431 调用 ("Etc/Reseal.etc", &m_resealTable)
//   importVillageAttackedScript      _Z27importVillageAttackedScriptPKcR19stVillageAttacked_t T 0x089093ee
//     init.md +0x1984 调用 ("Etc/VillageAttacked.etc", m_villageAttacked)，0=成功
//   loadPvpMapBasicDataScript        _Z25loadPvpMapBasicDataScriptPKcPSt6vectorI11PVPMAP_DATASaIS2_EE T 0x0890a427
//     init.md +0x37f6 调用 ("Etc/pvpmapbasicdata.etc", 栈局部 vector)
//   importVendingMachineInfoCore     _Z28importVendingMachineInfoCorePKcS0_R18stVendingMachine_t T 0x0890b2a3
//   importVendingMachineInfo         _Z24importVendingMachineInfoPKcS0_RSt6vectorI18stVendingMachine_tSaIS2_EE T 0x0890b74e
//     init.md +0x3300 调用 ("Etc/", "VendingMachine.lst", m_vendingMachine)，返回值弃
//   importTowerInfo                  _Z15importTowerInfoPKcS0_RSt3mapIj14stDeathTower_t.. T 0x0890b8d0
//     init.md +0x0b6c 调用 ("Etc/", "TowerList.lst", m_deathTowerMap)，0=成功
//   importUnlimitChallengeScript     _Z28importUnlimitChallengeScriptPKcP22stUnlimitChallengeInfo T 0x0890bc63
//     init.md +0x553e 调用 ("Etc/UnlimitChallenge.etc", &m_unlimitChallenge)
//   importRankSystemScript           _Z22importRankSystemScriptPKcP16stRankSystemInfo T 0x0890ed7f
//     init.md +0x0c2d 调用 ("Etc/RankSystemInfo.etc", &m_rankSystem)
//   importUltimateDungeonList        _Z25importUltimateDungeonListPKcRSt3setIiSt4lessIiESaIiEE T 0x8913c7e
//     init.md +0x173c 调用 ("Etc/UltimateDungeonList.etc", m_ultimateSet)
//   importSeriaRoomDecoEvent         _Z24importSeriaRoomDecoEventP24seriaRoomDecoEventScriptPKc T 0x0895e4bf
//     init.md +0x0501 调用 (&m_seriaRoomDeco, "Event/SeriaRoomDecoEvent.evt")
//   importUseJobFatigueGiftScript    _Z29importUseJobFatigueGiftScriptP22useCharacterJobFatiguePKc T 0x0895f817
//     init.md +0x0660 调用 (&m_useJobFatigue, "Event/JobFatigue.evt")
//   importSeriaRoomAnimationDecoEvent _Z33importSeriaRoomAnimationDecoEventP27seriaRoomDecoAniEventScriptPKc T 0x08960861
//     init.md +0x074a 调用 (&m_seriaAniDeco, "Event/SeriaRoomDecoAnimation.evt")
//   AvatarVariation::importAvatarColorVariation _ZN15AvatarVariation26importAvatarColorVariationEPNS_15AvatarColorInfoE T 0x088d444f
//     init.md +0x6a51 调用 (&m_avatarColor)——路径常量 "Etc/AvatarColorVariation.etc"
//
// 本 TU 同时提供上述域内 POD 的生命周期定义（ORIG W 弱符号逐条转写）：
//   seriaRoomDecoEventSoundInfo C1 0x089623ba / clear 0x089623ee / D1 0x083dd54a
//   useCharacterJobFatigue::fatigueMatchGift C1 0x0896251c
//   seriaRoomDecoAniInfo C1 0x089625e8 / clear 0x08962606 / D1 0x083cf9ba
//   stVendingMachineInfo_t C1 0x0891b36a / clear 0x0891b3fe / D1 0x083f2f3a
//   stVendingMachine_t C1 0x0891c820 / D1 0x083e636c
// 及 ORIG 数据符号 PVPMAP_BIG_IMAGE(0x93f4f00)/PVPMAP_SMALL_IMAGE(0x93f4f60)/
// PVPMAP_ONLY_TOURNAMENT_EXPOSURE(0x94f70f4)（其余三个 PvP 全局由 PvP_Room.cpp
// 提供，本 TU 仅 extern 引用，不重复定义）。
// ============================================================================

#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"        // 目标类型：reseal_table_t/stDeathTower_t/
                                 // stRankSystemInfo/stUnlimitChallengeInfo/
                                 // seriaRoomDeco*/useCharacterJobFatigue/
                                 // stVendingMachine_t/PVPMAP_DATA/AvatarColorInfo
#include "sync_script.h"        // STEquipmentPartSetPiece（DataManagerInitDepends.h 需要）
#include "LogManager.h"          // LogManager::logFormat
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr
#include "CEnvironment.h"        // less_TCHAR/SpinLock（SpinLock.cpp 提供）

// ---- 外部被调（ORIG 真实自由函数，定义在各权威 TU）----
#include "InitScriptProducers_8/getCharacterJobToEnumValue.inc"
#include "InitScriptProducers_8/getDrawLayer.inc"
#include "InitScriptProducers_8/importDeathTowerScript.inc"


#include "InitScriptProducers_8/loadRDARScriptListFile.inc"
extern bool PVPMAP_DEATHMATCH_MODE[0x14];                     // ORIG B 0x94f70e0
// [b28d-Main 代落·S215] 元素 char→bool（定义侧 PvP_Room.cpp 已改，P248 档②；
// 同符号同宽，读侧已按 bool 读 0/1）。
extern bool PVPMAP_ONLY_NORMAL_EXPOSURE[0x14];       // ORIG B 0x94f7108
extern bool PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14];     // ORIG B 0x94f711c

// ---- PvP map 全局表（ORIG 数据符号；外部链接）----
// D 0x93f4f00 [0x50]：int[20]，仅 [0]=-1 其余 0（.data 字节实证）。
int PVPMAP_BIG_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// D 0x93f4f60 [0x50]：同上。
int PVPMAP_SMALL_IMAGE[0x14] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// B 0x94f70f4 [0x14]：char[20] 零初始化 BSS。
char PVPMAP_ONLY_TOURNAMENT_EXPOSURE[0x14] = {0};

// ============================================================================
// importResealTable —— ORIG 0x089008b8
// 报告 docs/class_func_reports/_lt_global_gt_/importResealTable.md
// [rarity weight]→m_arr1[0..5](+0xc)；[extended rarity weight]→m_arr2[0..5]
// (+0x24)；[grade]→m_vec push_back 直至 Scan 失败。Scan 失败仍写入后仅记日志。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089008b8：clean 162/162（fresh .o 口径，
// ORIG 165 insns）；p167 CLEAR。已对齐：死 string×2（P171）、bool ok 独立槽
// （-0x2d/-0x2e/-0x2f）、三段 ScanInt(bool*) 循环、push_back(unsigned int)。
#include "InitScriptProducers_8/importResealTable.inc"
}

// ============================================================================
// importUltimateDungeonList —— ORIG 0x08913c7e
// set 清空后扫描；[apply ultimate] 段内逐 int insert；遇到 [/apply ultimate]
// 或任意其他 token 连续吞至该结束标记；EOF 与正常结束均返回 1，打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 08913c7e：clean 89/89（ORIG 92 insns）；
// p167 CLEAR。已对齐：set::clear、ok 每轮重置、insert(int)、EOF 与 [/apply ultimate]
// 共享尾块 return 1、打开失败日志返回 0。
#include "InitScriptProducers_8/importUltimateDungeonList.inc"
}

// ============================================================================
// importVillageAttackedScript —— ORIG 0x089093ee
// [attacked map info] 段内每行 4 个 int 构成 stAttackedMapInfo_t push_back；
// 第 2/3/4 列 Scan 失败分别返回 0x1990/0x1994/0x1998；打开失败 0x197f；
// 成功返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 089093ee：clean 96/96（ORIG 99 insns）；
// p167 CLEAR。已对齐：4 列 ScanInt 独立错误码 0x1990/0x1994/0x1998、首列失败回标签、
// stAttackedMapInfo_t 逐字段 + push_back、打开失败 0x197f。
#include "InitScriptProducers_8/importVillageAttackedScript.inc"
}

// ============================================================================
// loadPvpMapBasicDataScript —— ORIG 0x0890a427
// [pvp map order]：idx 入 PVPMAP_DATA::m_mapIdx（m_name ctor 后 clear）push_back；
// [pvp map basic data]：首列弃读 + 6 列入全局表（3 int 表 + 3 bool 化 char 表），
// 计满 0x14 条打印超限日志并仍返回成功；EOF 返回成功；打开失败返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890a427：clean 160/160（ORIG 164 insns）；
// p167 CLEAR。已对齐：首列弃读、6 列全局表写、entryIdx>0x13 日志 + break、
// [pvp map order] while(out!=0) 形、PVPMAP_DATA ctor + clear + push_back。
#include "InitScriptProducers_8/loadPvpMapBasicDataScript.inc"
}

// ============================================================================
// importVendingMachineInfoCore —— ORIG 0x0890b2a3
// 单台售货机脚本解析。[item group]…[/item group] 为一栏：
//   [group num]/[material a b] 直写；[output item rate count flag] 与
//   [bonus output …] 分别累计 m_giveCount/m_bonusCount 并 push
//   {itemId, 新累计值, count}；flag==1 时 (itemId,count) 追加 m_lottery。
// 错误码：0x1e0b 打开失败、0x1e27/0x1e2d/0x1e30 组头、0x1e3d/0x1e40/0x1e44
// output、0x1e55/0x1e58/0x1e5c bonus；组间未知标签记日志继续；EOF 返回 0。
// 注意：ORIG clear() 不清 m_lottery（仅 vecGive/vecBonus），忠实保留。
// ============================================================================
namespace
{

const char* const kVendOutputTag = "[output]";
const char* const kVendBonusTag = "[bonus output]";

}  // namespace

// [b28d-solo-InitScriptProducers_8 N] ORIG 0890b2a3：p167 CLEAR（39/39 调用点全等）；
// clean 320/321（fresh .o 口径，ORIG 327 insns，净 1 条）。已对齐：删多余死 string
// （ORIG 无第二 string ⇒ 帧 0xb0→0xa0、ok 回 -0x35）、ok!=true 的 xor 物化 ×14 与
// ORIG 全等、output/bonus 改 for(;;)+if(!ok)break 形（P165 两份物理独立块）、
// make_pair<unsigned long&,unsigned int&>、push_back(O&&)、P171 越界读照抄。
// 残余为 4.1/4.4 块序（ORIG 两连 jmp 中继 vs 4.4 jump-thread）——N104/N121 封顶。
#include "InitScriptProducers_8/importVendingMachineInfoCore.inc"
}

// ============================================================================
// importVendingMachineInfo —— ORIG 0x0890b74e
// 列表文件 → map<idx,name>；逐项构造 stVendingMachine_t{键, Core 解析}，
// 成功 push_back，Core 失败返回 0x1e7c。空列表 0x1e73，列表加载失败 0x1e83。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b74e：clean 118/118（ORIG 118 insns）；
// p167 CLEAR。已对齐：P214①/P257 正向包裹、iterator 双缓存、Core 结果内联实参（P0）。
#include "InitScriptProducers_8/importVendingMachineInfo.inc"
}

// ============================================================================
// importTowerInfo —— ORIG 0x0890b8d0
// 列表文件遍历：sprintf("%s%s") 拼 per-file 路径 → importDeathTowerScript →
// insert(key, tower)。列表加载失败 0x1eab、空列表 0x1e8e、单文件失败 0x1ea2。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0890b8d0：clean 160/160（ORIG 160 insns）；
// p167 CLEAR。已对齐：P214① 正向包裹、sprintf 路径拼接、importDeathTowerScript +
// insert(key,tower)、三档错误码 0x1eab/0x1e8e/0x1ea2。
#include "InitScriptProducers_8/importTowerInfo.inc"
}

// ============================================================================
// importRankSystemScript —— ORIG 0x0890ed7f
// [dungeon clear time average]：7 int 行 → m_vec38；前 2 列为 key、后 5 列为
//   value 写 m_dungeonRank.m_map0，key 已存在则整体返回失败；
// [dungeon clear time reduction bonus]：pair(a,b) 入 m_map44 且 {a,b} 入
//   m_dungeonRank.m_vec18；[rank grade]：逐值 push m_vec5c 且前 9 个写
//   m_rankLevel[0..8]；[combo interval boundaries]：4 int 入 m_vec68；
// [combo practice bonus inerval]：连续 int 覆写 m_field74 直至 Scan 失败。
// 打开失败返回 0，EOF 返回 1。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap0] ORIG 0890ed7f：p167 CLEAR（48/48）；fresh .o
// 379/379（gap=0）。本轮修：①[dungeon clear time average] 的重复键守卫改 P222 正向
// 包裹 `if(it==end){insert}else return false;`（ORIG 0x890efd8 call eq; test; je
// 失败块；旧 `if(!(it==end))` 多一条 xor $1）；②[combo practice bonus inerval] 由
// do-while 改为单值 `out->m_field74=ScanInt(&ok); if(!ok) return true;`（ORIG
// 0x890f293..2b3 只读一值即回主循环头 0x890edcd，非段内循环）。已对齐：死 string
// （-0x70，P171）、stClearTimeAverage ctor 声明（P104）、[rank grade]/[reduction
// bonus] for(;;)+if(!ok)break 形、用户空 dtor 删除（P317）。残余仅 ScanType 失败块
// 极性（ORIG jne 尾块 vs 4.4 je+就地物化，0x890f2b5）——N104 封顶。
#include "InitScriptProducers_8/importRankSystemScript.inc"
}

// ============================================================================
// importUnlimitChallengeScript —— ORIG 0x0890bc63（报告
// _lt_global_gt_/importUnlimitChallengeScript.md 汇编+Ghidra C 双段转写）。
// 无 tag 静态表：直接字符串比较 if/else 链；七职业段为 7 个独立 else-if 块
// （ORIG 每块 info/key/item 临时各占独立栈槽，subIdx 立即数 0..6，
//  如 0x0890be87 swordman info @-0x118、0x0890bfee fighter @-0x124 …），
// 忠实按 ORIG 展开。reward_server 行 = [等级, int, int] → m_vec1c；
// reward_client 行 = 同三列 → multimap（make_pair(key&, item&) 左值模板，
// ORIG 0x0891fb9b _ZSt9make_pairIR22stUnlimitChallengeKeyR... 实证）。
// 各段 while 头四 int 链式置零（0x0890be2e `movl $0,-0x20;mov -0x20,%eax;
// mov %eax,-0x24;…` 链形 = 源码 `int a=0,b=a,c=b,d=c;`）。
// onoff：ScanInt 失败仍 return true（ORIG 0x0890bd1a jne → return 1）；
// 成功则 movb 0/1 写 m_field0（bool）。server_id/level 首列失败 = 段结束回
// 主循环；reward 段第二/三列失败 = return false。message 段 do-while：
// 每轮构造 msg1/msg2（0x0890cc53/-0x1bc），id=ScanInt 失败结束段。
// ============================================================================
// 残余 DIFF 归因（N 类豁免，语义已全等、指令数 1085/1093）：
//  ① 4.1 while/if 块排布镜像：ORIG `call ScanType; xor$1; test; jne
//     exit-island(nop;jmp)`（0x0890bcd3..bcd9）+ body fallthrough；4.4.7 O0
//     恒生成反向 `je body` + exit fallthrough（范式库负例“空 else nop;jmp 岛
//     vs jump-threading”，无合法源码形态可翻转 BB 排布）；
//  ② message 段 ebx 汇聚 flag（0x0890cd42 `mov $0x1,%ebx` / 0x0890cd6f
//     `test %ebx,%ebx`）= 4.1 统一 dtor epilogue；4.4 每路径独立 dtor
//     （负例“EH 双份 dtor epilogue vs merge”）；
//  ③ 其余为 .o 重定位地址显示差（链接后 AE 口径消除）。
// [b28d-solo-InitScriptProducers_8 N] ORIG 0890bc63：p167 CLEAR（121/121 调用点全等）；
// fresh .o 1093/1092（gap=-1）。本轮修：①全部 `if(ok!=true)` → `if(!ok)`（45 处；
// ORIG 0x890bd0e 等为 `movzbl; xor $1; test` 负极性物化）；②[reward_server]/
// [reward_client] 的 `while(ScanType(sub,true))` → `for(;;){ if(!ScanType(sub,true))
// break; ... }`（ORIG 0x890be0c 循环头 `call; xor $1; test; je body`）；③[message]
// 段改 `if(!ok) break;` 正向后置（ORIG 0x890cc8c je 继续）。残余仅 ScanType 失败块
// 极性（ORIG jne 尾块 vs 4.4 je+mov $1,%ebx 就地物化，0x0890bcd3/0x0890cdb4）与
// 循环入口旋转岛——N104/N121 封顶，语义全等。
#include "InitScriptProducers_8/importUnlimitChallengeScript.inc"

}

// ============================================================================
// importSeriaRoomDecoEvent —— ORIG 0x0895e4bf
// [object max count]/[fatigue]/[reward item mail expire] 单 int 直写；
// [using item] 清 vector<int> 后逐 int push；[reward list] 二元组入
// m_matchEventMap（第二列 Scan 结果不校验）；[sound info] 清 m_soundList 后
// 每行“序号(弃)+int+int+string+int”构造元素 push。打开/空指针返回 0。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 AE] ORIG 0895e4bf：clean 212/212（ORIG 217 insns）；
// p167 CLEAR。已对齐：逐段 ScanInt/ScanStr 形、seriaRoomDecoEventSoundInfo 生命周期、
// 未知标签日志继续。
#include "InitScriptProducers_8/importSeriaRoomDecoEvent.inc"
}

// ============================================================================
// importUseJobFatigueGiftScript —— ORIG 0x0895f817
// 函数级静态 tag→id 表（ORIG __lookup 0x94f75f8/guard+__init_lock
// 0x94f7610/__inited 0x94f7614）：[/job fatigue]=0x217、[job type]=0x21b、
// [fatigue title]=0x229、[fatigue text]=0x22d、[reward list]=0x231、
// [/job type]=0x252。[/job type] 时 insert(jobType,gift) 并复位累计态。
// [reward list] 行校验：第 2 列 >=0、第 3 列 >0，违规立即返回失败。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap1] ORIG 0895f817：p167 CLEAR（62/62）；fresh .o
// 472/473（gap=1）。本轮按 P333 状态机重写惰性注册链：首轮 v=0 沿 case 0→0x217→
// 0x21b→0x229→0x22d→0x231→0x252 fallthrough 逐条 insert（ORIG 0x895fae7..fe75），
// 每 case 负极性 `if(!__inited){insert}else{...break;}`（ORIG `xor $1; test; je`；
// 旧一次性建表形净缺 45 条）；case 0/0x217 命中即 `return true`（ORIG 0x895fb44；
// 旧"无操作"是语义错）；EOF 改 break+循环外 `return ok`（P164 配套，ORIG 尾共享块
// 0x895ffaf 返回 ok 槽）。已对齐：value_type 转发构造（P315）、三件套移入循环体
// （P177）、fatigueMatchGift ctor throw()（P216）。残余仅循环入口旋转 jmp/nop 岛
// 与尾部岛——N104/N121 封顶。
#include "InitScriptProducers_8/importUseJobFatigueGiftScript.inc"
}

// ============================================================================
// importSeriaRoomAnimationDecoEvent —— ORIG 0x08960861
// [using item] pair 序列清+填 m_itemMap；[reward item] 第三列弃读的二元组
// push m_rewardList；[ani list] 行 “key,string,string(layer 名),int×3” 经
// getDrawLayer 转层号入 m_aniList；[finish sound] 直写 m_finishSound；
// [finish ani] 循环 “[front]/[back]” 选层（静态表 0x318/0x31b），随后行
// string(name)+string(layer)+int×3 入 m_finishAniMap；未知层名返回失败。
// 该段仅能经 EOF（ScanStr 失败）退出。打开/空指针返回 0，EOF 返回 1。
// ============================================================================
// [b28d-solo-InitScriptProducers_8 gap0] ORIG 08960861：p167 CLEAR（90/90）；fresh .o
// 628/628（gap=0）。本轮修真 bug：①[reward item] 第三列由 `(void)ScanInt(&value)`
// 改为 `if(!ScanInt(&reward.m_field8)) return false;`（ORIG 0x8960a36..a53 写对象
// +0x08 并检查返回值——旧码写错目标且缺失败门禁）；②key/val 提升为函数级局部
// （ORIG 0x89608bd/c7 入口初始化 -0xec/-0xf0，[using item]/[ani list] 复用）。
// 并按 P333 重写 [finish ani] 惰性注册链：首轮 v=0 沿 case 0→0x318→0x31b
// fallthrough 建表（ORIG 0x8960efb/0x8960f55），case 体负极性 if(!__inited)，
// default 在 __inited 时 return false（未知层名），三件套声明移入子循环（P177，
// ORIG 重扫 0x8960fc9 jmp 0x8960cf2）。残余仅 ScanType/ScanInt 失败块极性（ORIG
// jne 尾块 vs 4.4 je 就地物化）——N104 封顶。
#include "InitScriptProducers_8/importSeriaRoomAnimationDecoEvent.inc"

}

// ============================================================================
// AvatarVariation::importAvatarColorVariation —— ORIG 0x088d444f
// 固定路径 "Etc/AvatarColorVariation.etc"；清 m_colorMap 后 [avatar color map]
// 段每行 “key r g b name”（RGB 字节宽度 mov %al 实证）；name 扫描成功时同时
// insert colorMap/nameMap，失败退出该段；主循环 EOF 返回成功。
// ============================================================================
namespace AvatarVariation
{

// [b28d-solo-InitScriptProducers_8 AE] ORIG 088d444f：clean 192/192（ORIG 194 insns）；
// p167 CLEAR。已对齐：m_colorMap/m_nameMap 双 insert、RGB 字节宽度 mov %al 实证、
// name 扫描失败退段。
#include "InitScriptProducers_8/importAvatarColorVariation.inc"
}

}  // namespace AvatarVariation

// ============================================================================
// 域内 POD 生命周期定义（ORIG W 符号转写；声明见 CDataManager.h）
// ============================================================================

// ORIG C1 0x089623ba：+0/+4/+0xc 清零，string 默认构造。
#include "InitScriptProducers_8/seriaRoomDecoEventSoundInfo.inc"
}

#include "InitScriptProducers_8/dtor_seriaRoomDecoEventSoundInfo.inc"
#include "InitScriptProducers_8/clear.inc"
}

// ORIG fatigueMatchGift C1 0x0896251c：成员默认构造后 +0x18=0、map/string clear。
#include "InitScriptProducers_8/fatigueMatchGift.inc"
}

// ORIG C1 0x089625e8：string 默认构造后调 clear（+0x04=7，其余 int 清零）。
#include "InitScriptProducers_8/seriaRoomDecoAniInfo.inc"
}

#include "InitScriptProducers_8/dtor_seriaRoomDecoAniInfo.inc"
#include "InitScriptProducers_8/clear_1.inc"
}

// ORIG C1 0x0891b36a：三个 vector 成员默认构造后调 clear（尾部 call 0x891b3fe）。
#include "InitScriptProducers_8/stVendingMachineInfo_t.inc"
}

#include "InitScriptProducers_8/dtor_stVendingMachineInfo_t.inc"
#include "InitScriptProducers_8/clear_2.inc"
}

// ORIG C1 0x0891c820：仅构造 +0x04 vector（m_id 保持未初始化）。
#include "InitScriptProducers_8/stVendingMachine_t.inc"
#include "InitScriptProducers_8/dtor_stVendingMachine_t.inc"

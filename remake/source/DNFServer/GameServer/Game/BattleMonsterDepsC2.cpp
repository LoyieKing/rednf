// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleMonsterDepsC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// BattleMonsterDepsC2.cpp — CBattle_Field 怪物装配/换图剩余方法（独占 TU，
// 宿主登记 CMake；不改 GameStubs，不重复既有定义）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field6getMapEi                          T 0x08300ed4
//   _ZN13CBattle_Field20setDungeonClearTraceEiiii        T 0x0830107e
//   _ZN13CBattle_Field20getAddChampionMobCntEv           T 0x083010ee
//   _ZN13CBattle_Field16get_monster_listERSt4listI11_mapMonsterSaIS1_EE
//                                                        T 0x083012a4
//   _ZN13CBattle_Field35add_monster_list_bossroom_conditionERSt4listI11_mapMonsterSaIS1_EEii
//                                                        T 0x083013e2
//   _ZN13CBattle_Field24add_monsters_current_mapERSt4listI11_mapMonsterSaIS1_EERSt6vectorIiSaIiEER11map_monsterRiSB_SB_
//                                                        T 0x0830162e
//   _ZN13CBattle_Field22add_monster_hell_partyER11map_monsterRbRiS3_
//                                                        T 0x08301b8c
//   _ZN13CBattle_Field17add_monster_bloodER11map_monsterRiS2_
//                                                        T 0x08301bf6
//   _ZN13CBattle_Field18add_monster_APC_AIERi            T 0x08301d76
//   _ZN13CBattle_Field26add_monster_APC_AI_by_rateER11map_monsterRiii
//                                                        T 0x083020b6
//   _ZN13CBattle_Field31add_monster_additional_championERSt6vectorIiSaIiEEi
//                                                        T 0x083022a8
//   _ZN13CBattle_Field17add_monster_eventERiii           T 0x083023ec
//   _ZN13CBattle_Field19set_monster_abilityEv            T 0x083026da
//   _ZN13CBattle_Field29insert_tournamentRoundMonsterEv  T 0x083029b8
//   _ZN13CBattle_Field17add_item_curr_mapERiS0_          T 0x08302bc6
//   _ZN13CBattle_Field13moveMap_1_LogEii                 T 0x0830766e
//   _ZN13CBattle_Field13moveMap_2_LogEii                 T 0x08307740
//   _ZN13CBattle_Field13moveMap_3_LogEii                 T 0x08307812
//   _ZN13CBattle_Field13moveMap_4_LogEiiPK10GridScript   T 0x083078e4
//   _ZN13CBattle_Field14check_boss_mapEii                T 0x083079ba
//   _ZN13CBattle_Field19check_movemap_rangeEii           T 0x08307b2a
//   _ZN13CBattle_Field18get_candidator_mapEPK8CDungeonPK10GridScriptRiRSt6vectorIiSaIiEEiii
//                                                        T 0x08308214
//   _ZN13CBattle_Field15set_visited_mapEiib              T 0x083086d2
//   _ZN13CBattle_Field10set_map_idEPK10GridScriptRiRSt6vectorIiSaIiEE
//                                                        T 0x08308882
//   _ZN13CBattle_Field20set_visited_map_gotoEii          T 0x08308a24
//   _ZN13CBattle_Field36choose_random_bossmap_from_dummymapsEPK8CDungeonPK10GridScriptRiRbiii
//                                                        T 0x08307eca
//   _ZN13CBattle_Field17check_visited_mapEiiRb           T 0x08307d7c
//   _ZN13CBattle_Field20get_invade_directionEii          T 0x08307e44
//   _ZN13CBattle_Field10onStartMapEv                     T 0x0830dc74
// 桥接（本 TU 独有定义点）：
//   _ZN16CAICharacterList10getByLevelEj                  T 0x0834aa60
//     （工程头以 static getByLevel(void*,unsigned) 桥接声明；实现按
//       CDeathTower.cpp 中 get 桥接同型转写，objdump 0x834aa60 逐行核对）
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C 全文）：
//   docs/class_func_reports/CBattle_Field/{getMap,setDungeonClearTrace,
//     getAddChampionMobCnt,get_monster_list,add_monster_list_bossroom_condition,
//     add_monsters_current_map,add_monster_hell_party,add_monster_blood,
//     add_monster_APC_AI,add_monster_APC_AI_by_rate,
//     add_monster_additional_champion,add_monster_event,set_monster_ability,
//     insert_tournamentRoundMonster,add_item_curr_map,moveMap_1_Log,
//     moveMap_2_Log,moveMap_3_Log,moveMap_4_Log,check_boss_map,
//     check_movemap_range,get_candidator_map,set_visited_map,set_map_id,
//     set_visited_map_goto,choose_random_bossmap_from_dummymaps,
//     check_visited_map,get_invade_direction,onStartMap}.md
//   CAICharacterList::getByLevel 无独立报告 → 先读同类（CDeathTower.cpp get 桥接、
//     CAICharacterList 布局注释）后 objdump 0x0834aa60 转写。
// ============================================================================

#include <cstdlib>
#include <cstring>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h      // CBattle_Field + WarField/CMap/CDungeon/CParty/
                                // CMonster/STMonsterScript/GridScript/MazeScript 权威链
#include "CDataManager.h"       // G_CDataManager/find_*/get_hellparty_groupOrder_map/
                                // CAICharacterList（get/getByLevel 桥接声明）
#include "CItemGenerator.h"     // WongWork::stGenerateRefData_t/stGenerateResult_t/CItemGenerator
#include "CItemGeneratorMgr.h"  // WongWork::CItemGeneratorMgr::eGenerateType_t/getGenerator
#include "GlobalData.h"         // GlobalData::s_systemTime_
#include "MonsterIndependentDrop.h"  // CAICharacter 完整镜像（getLevel/getIdx 内联）

using WongWork::stGenerateRefData_t;
using WongWork::stGenerateResult_t;
using WongWork::CItemGenerator;
// ============================ CAICharacterList =============================

// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（桥接签名 static (void*, uint)）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（成员 this）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
#include "BattleMonsterDepsC2/getByLevel.inc"
}

// ============================ CBattle_Field ================================

// ORIG 0x08300ed4：find_map(mapIndex)；失败按当前图种类记日志返回 0；
// 困难 4 且地图 m_b4 非零时改写当前图号后二次查找（再失败同样记日志 0x4c4）。
#include "BattleMonsterDepsC2/getMap.inc"
}

// ORIG 0x0830107e：四段直写 m_dungeonClearTracer。
// [b28d-D9] N95：ORIG 4.1 dead 局部 -0x4(%ebp) 单条 store；4.4 槽基恒 ebp-8（P232/P190 均不可上移），残 1 条（ORIG 0x0830107e sz 0x0000006f）
#include "BattleMonsterDepsC2/setDungeonClearTrace.inc"
}

// ORIG 0x0830dc74：当前图 m_playInfo.m_startTick(+0x7c) ← 系统 tick。
#include "BattleMonsterDepsC2/onStartMap.inc"
}

// ORIG 0x083012a4：前置指针检查；锦标赛副本清空轮次表后走 ConsistTournament 并
// 初始化 boss 计数/轮次；普通副本直接拷贝 CMap 怪物表。
#include "BattleMonsterDepsC2/get_monster_list.inc"
}

// ORIG 0x083013e2：boss 房条件副本（m_pDungeon->m_bossRoom90c.m_0==1）时，把
// 与条件坐标匹配的怪物从条件怪表并入 mobList。
// [b28d-solo-BattleMonsterDepsC2] P214①/P13（已修 168→175，正文逐条全等）：ORIG 无 matched
// 具名 bool/无 break——匹配即执行内层 list 拷贝+push_back 后继续遍历（`mov $1/$0` 短路物化）；
// 外层 const_iterator 转换构造 + begin/end 各一次（4 槽），内层 const_iterator 缓存 end；
// conditions 为 const 引用绑定临时（ORIG local_10 指针槽，无 4.4 值拷贝）。
// [SweepBattleMonster 2026-09-09] 残差 N121：尾部 4.4 三层循环出口各发 nop+jmp 岛（ORIG 2 岛，
// 第三层直落 lea esp）⇒ clean 171 vs 172（+1 条）。
#include "BattleMonsterDepsC2/add_monster_list_bossroom_condition.inc"
}

// ORIG 0x0830162e：把 mobList 按 spawnType 展开生成到当前图。
// 命名怪条件：m_pDungeon->m_vec8f0 非空且 _mapMonster.m_champion 时查已清场表跳过。
// [SweepBattleMonster 2026-09-09] 已修（399 vs 398，clean 397=397，DIFF→near）：①P167 end()
// 每轮回边重求值（不缓存 itEnd）；②P214① 正极性 `m_villageMonsterId!=0 && m_party!=NULL`
// 包裹队伍分支、普通分支内 `ref.m_flag04!=0` 在前；③真 bug：flag==0 分支等级来源应为
// ref+0x8（m_field08，ORIG 0x08301893 `mov 0x8(%eax),%eax`），旧写 m_levelAdd(+0x10)；
// ④P226 语句表达式复现 `!empty() && champion` 物化岛；⑤P324 case 升序(0,1,2)；⑥P225
// `(signed char)dim>0`+if/else 复现 setg；⑦__PRETTY_FUNCTION__；⑧P231 register 复用 uid。
// 残差仅 uid 值落 %ebx vs ORIG %eax（N7 寄存器角色，指令数/序列全等）。
#include "BattleMonsterDepsC2/add_monsters_current_map.inc"
}

// ORIG 0x08301b8c：血战开启且组序表存在时转调 add_hell_party_monster。
#include "BattleMonsterDepsC2/add_monster_hell_party.inc"
}

// ORIG 0x08301bf6：血战系统/阶段时间自 CMap 复制到本对象后转调 add_blood_monster。
// [b28d-solo-BattleMonsterDepsC2] P197④/P225/P0：ORIG `call size; test %eax; setne %al;
// test %al; je` ⇒ 源写 `size() != 0`（empty() 生 test %al 直跳）；且 ORIG 无 ok 具名局部
// （`mov %eax,%ebx; xor $1,%ebx` 保活跨析构）⇒ 条件内联表达式，去掉 `const bool ok` 槽。
#include "BattleMonsterDepsC2/add_monster_blood.inc"
}

// ORIG 0x08301d76：按地图 AI 编排表逐条生成 APC 怪物。
// [b28d-solo-BattleMonsterDepsC2] 已修：`mob.m_instId = seq++`（ORIG 单次读 seq + lea 0x1）；
// m_level 三目改 (signed char)dim > 0 形（ORIG `test %al; setg %al; test; je`）。
// [SweepBattleMonster 2026-09-09] 已修（213 vs 212，clean 211 vs 209）：①删 `map_monster tmp`
// 直接 `Add_Mob(mob)`（省一个 0x350 临时，帧 0xa30→0x6f0，ORIG 0x6e0）；②m_field11 字节视图
// 复现 +0x11/+0x12 双 movb（WarField.h 匿名 union）；③`!m_dummyBossMap`（字段 bool 化）。
// [SweepBattleMonsterDeps 2026-09-10] 归零（211=211，ndiff 152→0，AE）：①apcIdx 声明提到
// arranges 之前 + 循环前重置（ORIG 0x08301da3/0x08301de2 双清零，4.1「先声明更深」）；
// ②begin/end 各调一次并缓存 itEnd（P13；ORIG -0x368/-0x36c 两独立槽，循环尾只 operator++/!=）；
// ③`if (!empty())` 正向包裹（P214①，ORIG `xor $1; test; je`）；④删 info 具名槽（P0，内联
// GetCurrentMapInfo()）；⑤m_level 改语句表达式 + if/else 复现 `setg` 物化（P226/P225）。
// 帧 0x6f0→0x6e0=ORIG，p167 27=27 CLEAR（仅 map_monster C1/C2、D1/D2 同址别名差）。
#include "BattleMonsterDepsC2/add_monster_APC_AI.inc"
}

// ORIG 0x083020b6：概率生成单只 APC（mobId 固定 0x40，roleType 5）。
// [b28d-solo-BattleMonsterDepsC2] 已修：uid 递增改 `uid = uid + 1`（ORIG `lea 0x1(%eax)` 复用
// 寄存器；`mob.m_instId + 1` 会重读 mob+0x4）；m_level 条件加 (signed char)。
// [SweepBattleMonster 2026-09-09] 已修（140 vs 146）：①`int mobId=0x40` 具名局部 + `mobId++`
// 后置（ORIG 0x083020d2 store / 0x08302296 `addl $0x1,-0x10`）；②P0 删 `map_monster tmp` 直接
// `Add_Mob(mob)`（帧 0x6b0→0x370=ORIG）；③m_field11 字节视图双 movb；④P231 register 复用 uid。
// 残差 N：rate()!=0 的 `setne` 与 rate()>rand 的 `setg` 物化（P226 语句表达式实测 +5 条反劣）、
// dim>0 的 if/else 尾部三 jmp 岛、uid 落 %ebx（N7）。
#include "BattleMonsterDepsC2/add_monster_APC_AI_by_rate.inc"
}

// ORIG 0x083022a8：从候选表中随机抽 count 只追加为宝箱怪（每轮一次 rand 取槽）。
// [b28d-solo-BattleMonsterDepsC2] P197④/P225：ORIG `call size; test %eax; sete %al; test %al;
// jne` ⇒ 源写 `size() == 0`（empty() 直跳少 2 条）；`begin()+slot` 不套 static_cast（ORIG
// 直接把 slot 槽址绑 operator+ 第二引用参，cast 会多一条 -0x1c 临时）。
#include "BattleMonsterDepsC2/add_monster_additional_champion.inc"
}

// ORIG 0x083023ec：事件怪按 rival 表实例号区间（0x2710..）落到匹配坐标的事件位。
// [b28d-solo-BattleMonsterDepsC2] P167/P13（已修 215→213）：ORIG 调 operator*（deEv）并把结果
// 存具名 -0x10（`(*ev)` 形，非 it->）；begin/end 各一次并缓存；`mob.m_instId = seq++` 复现
// 单次读 seq + `lea 0x1`（写成 seq+1 / mob.m_instId+1 均重读）。
// [SweepBattleMonster 2026-09-09] 已修（188 vs 193，clean 185 vs 187）：①P0 删 `map_monster
// tmp` 直接 `Add_Mob(mob)`（省 0x350 临时，帧 0xa20→0x6d0=ORIG）；②m_field11 字节视图双 movb。
// 残差：`if (m_eventMonsterStatus.empty()) return;` ORIG 为 `!empty()` 正向包裹形
// （`call; xor $1; test; je`，P214①），改包裹需重排整个主流程 ⇒ 记 N + 尾部 2 条。
#include "BattleMonsterDepsC2/add_monster_event.inc"
}

// ORIG 0x083026da：直接遍历 GetCurrentMapInfo()->m_monsterMap（+0xc）原图，
// 能力写入落在真实地图怪物上；srand/冠军分支均重新 GetCurrentMapInfo。
// [b28d-solo-BattleMonsterDepsC2] P13/P196（已修 206→209）：ORIG begin/end 各调一次并缓存
// （-0x2c/-0x30）；条件 `movzbl 0x8; cmp $0x4,%al; jg` 为**有符号**比较 ⇒
// (signed char)mm.m_roleType > 4（unsigned 版产 ja 反极性）。
// [SweepBattleMonster 2026-09-09] 已修（213=213，clean 210=210，DIFF→near）：P0 删具名
// `ActiveStaticInfo* active`（ORIG 每处 `lea 0x38(%eax)` 重算，无具名槽）⇒ 槽图整体回正。
// 残差仅 `movsbl` 落 %eax vs ORIG %edx（N7 寄存器角色）。
#include "BattleMonsterDepsC2/set_monster_ability.inc"
}

// ORIG 0x083029b8：锦标赛副本把当前图怪物按轮次 1..4 分桶进 multimap。
// [b28d-solo-BattleMonsterDepsC2] N82/P225/N113：ORIG 增量段为后置 ppEi（带 -0x20 临时槽），
// 但源写 `it++` 实测 150→157 反劣（4.4 为后置 ++ 另发 sret 拷贝链）；条件 `n<memberCount &&
// it!=end` 的 4.1 bool 物化（mov $1/$0 + test）4.4 恒折直跳。封顶。
#include "BattleMonsterDepsC2/insert_tournamentRoundMonster.inc"
}

// ORIG 0x08302bc6：按地图物件表与副本指定掉落生成地面物品。
// refData 三份上下文 blob 的字节布局按 ORIG 偏移直写（+0x34 区为生成器读取的
// 调用上下文；其中两处保存指针宽度数据，见各注释）。
// 注意：ORIG 在「assign==-1 且标准等级越界」分支直接跳回循环头而不推进迭代器
// （08302de5 jne 083030ad→083030ae，++it 位于 08303080 正常路径上）；脚本校验
// 保证标准等级恒在 1..200，该路径实际不可达。此处忠实保持同一控制流。
// [b28d-D9] N82/N86：帧 0x20c vs 0x21c + 槽序差（ORIG 0x08302bc6 sz 0x0000060b）
#include "BattleMonsterDepsC2/add_item_curr_map.inc"
}

// ---- moveMap_*_Log 族（0x0830766e / 0x08307740 / 0x08307812 / 0x083078e4）----

#include "BattleMonsterDepsC2/moveMap_1_Log.inc"
}

#include "BattleMonsterDepsC2/moveMap_2_Log.inc"
}

#include "BattleMonsterDepsC2/moveMap_3_Log.inc"
}

#include "BattleMonsterDepsC2/moveMap_4_Log.inc"
}

// ORIG 0x083079ba：终点位 boss 房条件未满足时报错码 7。
// [b28d-solo-BattleMonsterDepsC2] P13/P283：ORIG begin/end 各转换构造一次后 `jmp` 底部条件块
// （it=-0x14、itEnd=-0x18 两独立槽）⇒ 源写「两个具名 const_iterator + for(;it!=itEnd;++it)」；
// 条件里直接调 end() 使 4.4 复用 -0x14 槽并每轮重调 end()（-5 条）。
#include "BattleMonsterDepsC2/check_boss_map.inc"
}

// ORIG 0x08307b2a：相邻格（含斜向禁止）越界检查。
// [b28d-solo-BattleMonsterDepsC2] P0（已修）：ORIG 无 MapInfo* 具名局部 ⇒ 两处调用内联
// （消 -0x1c 槽，帧 0x5c→0x4c 与 ORIG 全等）。
// 残差判 N87/N18a：ORIG 绝对值段=单赋值寄存器链 8 条（`sub %eax,%edx; mov %edx,%eax;
// sar $0x1f,%eax; xor %eax,%edx; mov %edx,-0x20; sub %eax,-0x20`）；4.4 两语句形先存槽后
// 读回且把 `-s` 归约为 `+(s&1)`（14 条/处）、单表达式展开 3 次（115）、裸 register 免槽但
// 使 curX/curY 槽浅 4（99，与 ORIG 有槽往返的 P0 证据冲突）。条件块第 4 跳 4.4 恒反极 je。
// 三档类型旋钮（P248）试尽，封顶。
#include "BattleMonsterDepsC2/check_movemap_range.inc"
}

// ORIG 0x08307e44：以当前位置为基准的单向方向判定。
#include "BattleMonsterDepsC2/get_invade_direction.inc"
}

// ORIG 0x08307d7c：按副本是否每图进入选用 goto 位或普通访问位判定。
#include "BattleMonsterDepsC2/check_visited_map.inc"
}

// ORIG 0x083086d2：写目标格/当前层 visited 标记。
// [b28d-solo-BattleMonsterDepsC2] P272/P136：ORIG 短路链 `jne<else>`+`je<then>`（else 直落、
// then 在后）⇒ 源写德摩根 `if (curX != x || curY != y) {else} else {then}`；字段直赋 bool 值
// 复现 `movzbl -0x1c(%ebp),%edx; mov %dl,0x1(%eax)`（原三目 `?1:0` 每处 +5 条物化）。
#include "BattleMonsterDepsC2/set_visited_map.inc"
}

// ORIG 0x08308a24：写目标格/当前层 visitedGoto 标记。
#include "BattleMonsterDepsC2/set_visited_map_goto.inc"
}

// ORIG 0x08307eca：dummy 图序列推进 / boss 位传送。
// [b28d-solo-BattleMonsterDepsC2] P0：ORIG 对 operator[] 结果**值拷贝**成具名 pair 局部
// （-0x18/-0x14 两槽）再写 m_endX/m_endY；MapInfo* info 无槽 ⇒ 判空内联调用表达式。
// 残差判 N（P292/N18a）：IsHaveDummyMap() 返回 uchar，ORIG 调用点 `call; test %al; je` 无
// setne，4.4 恒加（改 bool 需动 CDungeon.h 共享头，收益 2 条，本波不动）；m_dummyMapPosIdx
// ORIG 两段 `mov 0xcc; mov %eax,%ebx` 而 4.4 单段直落（N18a 反向）；m_visited(char) 判空
// 4.4 多 setne（字段 bool 化会破坏 set_visited_map 已 AE 的写入形）。
#include "BattleMonsterDepsC2/choose_random_bossmap_from_dummymaps.inc"
}

// ORIG 0x08308214：候选图收集（终点/起点任务/hell/普通四路分支）。
// [b28d-D9] P225/P197④：ORIG test+sete 物化岛；4.4 直跳（ORIG 0x08308214 sz 0x000004bd）
#include "BattleMonsterDepsC2/get_candidator_map.inc"
}

// ORIG 0x08308882：确定最终图号并写回当前图信息。
// [b28d-solo-BattleMonsterDepsC2] P197④/P0：ORIG `call size; test %eax; setne %al; test %al;
// je` ⇒ `candidator.size() != 0`（empty() 生 xor $1 两段）；get_rand_int 结果具名存 -0x1c
// （ORIG 有槽往返）；logFormat 内联 GetMapInfoFromPos（具名 info 使调用提前，破坏右起求值序）。
#include "BattleMonsterDepsC2/set_map_id.inc"
}

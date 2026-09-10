// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InitScriptProducers_7/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r Game/ —— CDataManager::init() 脚本装载自由函数（第 7 批）。
//
// 本 TU 承载 DataManagerInitDepends.h 声明的以下 12 个 import*（全部为 ORIG nm
// 真实 T 符号；逐函数按 objdump 反汇编忠实转写，调用点见
// docs/class_func_reports/CDataManager/init.md）：
//
//   importEventAvatarListScript        0x0895e1ed  init.md +0x688b
//   importEventEtcScript               0x0895e30f  init.md +0x6998
//   importFairPVPEquipment             0x088ffbfc  init.md +0x2328
//   importGrowthEquipmentScript        0x0895eb3f  init.md +0x0576
//   importHellPartyScript              0x08909857  init.md +0x0d30
//   importInGameAdvertisementEtcScript 0x0897e9c4  init.md +0x048c
//   importInGameEventInfoList          0x0897eb59  init.md +0x674d
//   importLimitItemUsageInfo           0x0890d871  init.md +0x562b
//   importMercenarySystemEtcScript     0x089f6c50  init.md +0x6b19
//   importNPCRelationEventScript       0x0890d44f  init.md +0x5a5f
//   importPcBangEquipmentRendar        0x08915606  init.md +0x6be8
//   importQuestShopScript              0x0890fa3d  init.md +0x10d4
//
// 另提供上述函数直接依赖、当前无任何定义的 ORIG 小符号：
//   makeFairPVPEquipmentPackageKey 由 InitScripts_2 提供（勿在此重复定义）
//   tokenizeStr / ConvertStringToTime / getGrowTypeIndex 由 InitScripts_6 提供
//   loadAppendageType / loadValueType 沿用 extern 声明（定义归其报告域批次）
//   stLimitItemUsageInfo::Add 0x891b4a8 / stResetItemInfo::Add 0x891b4dc /
//   stRefillItemInfo::Add 0x891b502
//   stQuestShopStatus C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826
//   stStatusLevelInfo C1 0x891b8b4；stQuestShoptInitCost C1 0x837109a
//   stNPCRelationEventInfo C1 0x891c946
//   stMercenaryRewardGroup C1 0x89f7a9c / stMercenaryCompetitionArea C1 0x89f7b28
//   stHellPartyMonsterGroupList C1 0x891b1d6 / stHellParty_t C1 0x891b242 D1 0x891c7e2
// ============================================================================

#include "CDataManager.h"        // DataManagerInitDepends.h 形参类型宿主，必须先包含

#include "DataManagerInitDepends.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "DNFLexWrapper.h"
#include "EventAvatarListScript.h"

#include "CEventScriptMng.h"   // InGameEventInfo / InGameEventInfoList

// ---- 外部脚本助手（定义在兄弟 TU，此处仅引用）----
#include "InitScriptProducers_7/loadAppendageType.inc"
#include "InitScriptProducers_7/loadValueType.inc"
#include "InitScriptProducers_7/tokenizeStr.inc"
#include "InitScriptProducers_7/getCharacterJob.inc"
#include "InitScriptProducers_7/ConvertStringToTime.inc"
#include "InitScriptProducers_7/getGrowTypeIndex.inc"
#include "InitScriptProducers_7/importMercenarySystemEtcScript.inc"
}

// ============================================================================
// importEventAvatarListScript — ORIG 0x0895e1ed（init.md +0x688b 调用，
// "Event/EventAvatarList.evt" → m_eventAvatarList）。
// 语义：loadRDARScriptFile("",path)；clear；扫描 "[avatar list]" 后连续
// ScanInt 入 set（ScanInt 失败回主循环）；"[event avatar list end]" 置返回
// 标记。返回该标记（未见到 end 段返回 false）。
// ============================================================================
#include "InitScriptProducers_7/importEventAvatarListScript.inc"
}

// ============================================================================
// importEventEtcScript — ORIG 0x0895e30f（init.md +0x6998，"Event/EventEtc.evt"
// → m_eventEtc）。"[seria room tree material list]" 连续 int 入 m_vec0；
// "[seria room deco material list]" 按 (值, 序号) 写 m_mapC（序号自增）；
// "[christmas event end]" 结束并返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0895e30f
//   P164 分离形 `while(run){ if(!ScanType) break; }` + p 缓存槽(-0x10) +
//   make_pair<int&,int&> 左值对（P61）。orig/ours=128/128。
#include "InitScriptProducers_7/importEventEtcScript.inc"
}

// ============================================================================
// importFairPVPEquipment — ORIG 0x088ffbfc（init.md +0x2328，
// "Etc/FairPVPEquipment.etc" → m_stackableMap）。
// "[group]"：ScanStr 职业→getCharacterJob；ScanInt((bool*)0) 段位；
// key=makeFairPVPEquipmentPackageKey(job,grade)。随后收集若干
// "[package selection]" 包（成对 itemId/count 直至 ScanInt 失败），整组赋给
// (*out)[key]。[/fair pvp package] 或流结束均返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x088ffbfc
//   P164 外层/内层 `for(;;)+break` 双改 + itemId 去预清（槽 -0x2c 对齐）；
//   p167 95/95 CLEAR。
#include "InitScriptProducers_7/importFairPVPEquipment.inc"
}

// ============================================================================
// importGrowthEquipmentScript — ORIG 0x0895eb3f（init.md +0x0576，
// "Event/GrowthWeapon.evt" → m_growthWeapon）。五个标量段直写字段（任一
// ScanInt 失败提前结束但返回 true）；"[growth equipment reward list]"
// 成对 (idx, reward) 入 m_rewardMap。
// 字段偏移与 growthEquipmentScript 布局一致（+0x18/+0x1c/+0x20/+0x22/+0x24）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0895eb3f
//   P164 循环头 ScanType + P223 多源 goto 返回岛（done1..done5）+ 独立 if
//   序列（非 else-if）。orig/ours=183/183。
#include "InitScriptProducers_7/importGrowthEquipmentScript.inc"
}

// ============================================================================
// importInGameAdvertisementEtcScript — ORIG 0x0897e9c4（init.md +0x048c，
// "Etc/ingame_advertisement.etc" → m_inGameAd）。clear() 将前 151 字节置 1；
// "[in game advertisement expose condition of teen]" 后每个 idx（ScanInt
// 成功且 <=150）清 0 一字节，idx>150 结束本段。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0897e9c4
//   P164 分离形 `while (run) { if (!ScanType(token, true)) break; }`（条件头
//   仅测 run、ScanType 检查落体内首）。
#include "InitScriptProducers_7/importInGameAdvertisementEtcScript.inc"
}

// ============================================================================
// importLimitItemUsageInfo — ORIG 0x0890d871（init.md +0x562b，
// "Etc/Chn_Server_LimititemUsageinfo.etc" → m_limitItemUsage）。
// 三段分别填充 Ex 结构的三个子表；日期串经 ConvertStringToTime 转秒。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0890d871
// 按 ORIG 逐段重排：
//  ① [Limit Item Usage Info] 首个 ScanInt 存 char 槽(-0x1d) ⇒ 源码首参是 char kind，
//    第二/三参是 int a(-0x1c)/b(-0x18)（原重建把 kind/a 类型写反）；
//  ② 循环头 ScanType 检查（P164）+ ok 失败 break/return 双出口；
//  ③ itemIdx(-0x14)/itemId(-0x10)/count(-0xc) 循环外声明并预清（ORIG movl $0）；
//  ④ ORIG -0x24 循环外 dead int 预清（P171/P232）补齐帧。
#include "InitScriptProducers_7/importLimitItemUsageInfo.inc"
}

// ============================================================================
// importNPCRelationEventScript — ORIG 0x0890d44f（init.md +0x5a5f，
// "Event/NPCRelationEvent.evt" → m_npcRelation）。
// 顶层仅接受 "[NPC]"（其他 token 直接成功返回）；块内 "[index]" new 0x48 节点
// 并 AddNPCInfo；"[good]/[soso]" 进入好感段：每 kind token 先 GetFavorIndex，
// "[item]" 成对入 AddFavorItemInfo、"[reward]" 单条入 AddRewardItemInfo；
// "[can not give item]" 连续 id 入 AddDislikeItemInfo。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/N112] ORIG 0x0890d44f
// 按 ORIG 重排（已修）：
//  ① sub(-0x4c) 移入 [NPC] 内层循环内声明（ORIG 每轮 C1/D1，非函数级）；
//  ② favorKind 改裸短路 if 条件（ORIG eax 物化 mov$1/mov$0，非 int 槽）；
//  ③ 内层循环改 `for(;;)`；curIdx 循环内预清（去函数头重复 store）；
//  ④ 真 bug：`token != "[NPC]"`/`sub != "[can not give item]"` 改 operator==
//     正判（p167 实测 MISSING eq×2 / EXTRA ne×2，ne 选错重载）；
//  ⑤ [NPC] 非匹配与循环 EOF 共享 done 返回岛（goto done）。
// 残余 DIFF 归因（N 封顶，p167 40/40 CLEAR、orig/ours=309/314）：
//  sub 槽 -0x4c vs -0x54（4.4 块内 string 槽序）+ 4.4 continue 标志编码
//  `mov $0x2,%ebx` vs 4.1 `mov $0x1,%ebx` + AddFavor/RewardItemInfo 实参
//  寄存器 edx↔ecx 角色差（N82/N112/N7 族）。
#include "InitScriptProducers_7/importNPCRelationEventScript.inc"
}

// ============================================================================
// importQuestShopScript — ORIG 0x0890fa3d（init.md +0x10d4，
// "Etc/questshop.etc" → m_questShop）。返回 0=成功，否则错误码：
//   0x2815 装载失败 / 0x2829 [quest status] 序号 ScanInt 失败 /
//   0x284f loadAppendageType==-1 / 0x286d [quest point] ScanInt 失败。
// "[initialization costs]" 三值写 m_initCost（失败按 ORIG 返回 0）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/P71] ORIG 0x0890fa3d
//  已修：status 提函数头（-0xcc）；[quest status]/[initialization costs] 改
//  if/else 链（p167 MISSING eq → == 正判）；map 插入改 make_pair
//  （p167 MISSING make_pair/pair C1 OS_ 变体，P61/P192）。
//  残余：p167 仅 EXTRA `stStatusLevelInfoD1Ev` ×2 —— P71 用户空析构跨版本
//  trivial 判定差（CDataManager.h 共享头，不改）；槽/临时分配 N82。
// [b28f-SweepDeps 复核] 主循环 `while (ScanType(token,true))`：ORIG 0x0890fa9d
//  为**头置条件**（`movl $0x1,0x4(%esp); call ScanType; xor $0x1,%eax; test;
//  jne exit`，入口 0x0890fa9a `jmp +2` 跳过对齐 nop），4.4 恒旋转为底部条件
//  （entry `jmp <尾条件>`）。P292/P283 形（for(;;)+`if(!ScanType) break;`）实测
//  **反劣**：our 370→372、gap −27→−29（diff 行 525→517 不改变 verdict）⇒ 已回退，
//  按 P164 环旋转（4.4 固有）封顶。同族 importPcBangEquipmentRendar 内层
//  items 循环已记「三形穷举不可同时保 xor$1 与头置」。
// [b28f-SweepDeps 线索] 另 2 处 EXTRA `stStatusLevelInfoD1Ev` 可试 P334 trivial
//  镜像视图（本 TU anonymous namespace 同字段序 POD 镜像 + 容器实参位
//  reinterpret_cast<const stStatusLevelInfo*>），未实测（超本波预算）。
#include "InitScriptProducers_7/importQuestShopScript.inc"
}

// ============================================================================
// importPcBangEquipmentRendar — ORIG 0x08915606（init.md +0x6be8，
// "Etc/PcBangRentalService.etc" → m_pcBangRentar）。
// "[section]"：EventRewardSection 三字节 + count 个 uchar 入 m_vec0；
// "[group]"：job/grade→key，收集 "[package selection]" 包（itemId、count、
// short、"逗号串"→tokenize/getGrowTypeIndex 逐项 push_back），[/group] 时
// insert 进 m_mapC。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N78/N7/N104] ORIG 0x08915606
//  逐 jcc/调用点核对：无语义缺口。jcc 27:27 一一对应，唯一极性差 = [section]
//  do-while 的 more 标志（ORIG 0x089159e7 `test %ebx,%ebx; je` 寄存器化 vs
//  我方 `cmpb $0,-0x29(%ebp); jne` 槽化，do-while 语义等价）；内层 items 循环
//  4.4 旋转（条件落尾 0x2b46 vs ORIG 落头 0x08915b02）；growText/delim 临时
//  析构的 EH 双份 epilogue vs 4.1 merge（N104）。
//  p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；orig/ours=582/581。
// [F2e-Singles2 C05② 复核 2026-09-10] cmp 差 2 处：①0x08915cb9 `cmp -0x1c(%ebp),%eax;
//  setg` —— 左操作数是**跨调用返回值**（0x08915cb4 call 结果直用）⇒ 边界表 ❌；
//  ②0x08915998 `cmp -0x30(%ebp),%eax; setl` 我方 -0x34 槽（槽差，属 N82 平移）。
//  ⇒ 非 C05② 命中形，维持 N78/N7/N104 封顶（our 581 vs ORIG 582，gap +1）。
#include "InitScriptProducers_7/importPcBangEquipmentRendar.inc"
}

// ============================================================================
// importHellPartyScript — ORIG 0x08909857（init.md +0x0d30，
// "Etc/HellParty.etc" → *Get_instance_stHellParty_t()）。
// 返回 0=成功；错误码段：0x1a24 装载 / 0x1a2e 首.ScanType /
// 0x1a37..0x1aa3 基础值各 Scan / 0x1aba 组下标 / 0x1acd..0x1aed 组名 /
// 0x1aed 未知组名 / 0x1b08 组下标重复 / 0x1b1a..0x1b30 difficulty 段。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N121/N82] ORIG 0x08909857
//  本波逐 jcc/字段偏移核对，修真语义 bug（判据⑮ 多余门禁 / 写入位置 / P0 / P171 /
//  P293）：
//   ①[group] 怪物循环原写 `for (i < slotCount)`，slotCount=out.m_field72[tag] ——
//     ORIG 0x08909f9e..0x08909fee 是**无上界** for(;;)，仅 ScanInt 后 ok 失败
//     break（越界照抄）；我方多读 out.m_field72[tag] 并提前退出 ⇒ 删门禁；
//   ②[group index] 原先判 ok 再写 out.m_groupIndex —— ORIG 0x08909dc2 先
//     `mov %dx,0x50(%eax)` 写字段、0x08909dc6 才判 ok；
//   ③groupName/rowName 的 ScanStr 改 `ok = ScanStr(...); if (!ok)`，复现 ORIG
//     0x08909e2d/0x0890a197 的 `mov %al,-0xd9; movzbl -0xd9` ok 槽往返（P0）；
//   ④补 tag 分支死写入 out.m_field6c[tag]/out.m_field72[tag] → 两个死局部
//     （ORIG 0x08909e6a..0x08909f86，P171 死写入照抄）+ tag 改 uchar（movb）；
//   ⑤m_groupMap.find 改具名 iterator ⇒ 复现 ORIG 先 find(0x0890a011) 后
//     end(0x0890a02c) 的求值序（P293）；
//   ⑥[hellparty script end] 与 [/hellparty basic values]/[/hellparty monster
//     group] 收尾比较移至链末（ORIG 0x0890a2ae / 0x08909d21 / 0x0890a0f5）；
//     difficulty 循环改 `i < rows+1` 复现 ORIG `add $1; cmp; setg`。
//  残余（N 封顶）：4.4 把 11 个 else-if 回边 jmp 集中并各带 1 字节 nop 对齐
//  （N112/N121 块重排，源码不可控）+ 全帧槽号平移（N82/N126）。
//  p167 别名口径 CLEAR（MISSING 全 C2/D2 与 EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；ratio 0.9357、gap=-10（全部为上述 nop/块序）。
#include "InitScriptProducers_7/importHellPartyScript.inc"
}

// ============================================================================
// importInGameEventInfoList — ORIG 0x0897eb59（报告 _lt_global_gt_/
// importInGameEventInfoList.md 汇编+Ghidra C 双段转写；init.md +0x674d）。
// 双相派发真形：函数级静态 __lookup map<char const*,int,less_TCHAR>
// （BSS 0x094f7ba8）+ __init_lock SpinLock（0x094f7bc0）+ __inited（0x094f7bc4），
// guard 每轮穿（P177：static 位于重派发循环头）；注册轮持锁 id=0 → switch
// 全 miss → 公共尾 0x0897f0b3 leave+置位+jmp 0x0897ebca（guard 重查同 token）。
// 分派 = `cmpl $0x37; ja; shl $2; jmp *table` 跳表（P163：case 0 并入
// [/event_list] leaf 撑 range）。token→id 逐例 insert（非链式，每块独立）：
// [/event_list]=0x24（命中→result=true 经共享退出岛 0x0897f0fe 返回）、
// [idx]=0x27（ScanInt(int*)→info+0x00 + ScanStr→info+0x08，返回值不判）、
// [record type]=0x2b(+0x04)、[start]=0x2e(+0x0c)、[end]=0x31(+0x10)、
// [state]=0x34(+0x14)、[/event]=0x37（insertEventInfo(info.m_field4, info)
// 0x0897f0ae）。info/token 为函数级临时（-0xd8/-0xc0，循环外构造一次）；
// result(-0x1d) 仅 ScanType EOF / [/event_list] 两退出路径读取（P164 scan_exit
// 岛共享尾：`movzbl -0x1d,%ebx` + dtor 统一块）。
//
// 本波已修（P 范式命中）：
//  P309 key 提函数头声明（`const char* key;` 无 init）⇒ token 槽 -0xc0 对齐；
//  P168a② 公共尾负极性 `if (!__inited) { leave; 置位; goto redo; }` ⇒ 复现
//   ORIG `movzbl __inited; xor $1; test; je <nop 岛>`；
//  [/event_list] 路径改裸 `return true;` ⇒ 4.4 直 `mov $0x1,%ebx; jmp 共享退出岛`
//   （与 ORIG 0x0897ee2b 同形，旧 `result=true; goto` 的槽写已消）。
//
// 残余 DIFF 归因（N 类封顶，语义完成：p167 50/50 CLEAR、orig/ours=364/363）：
//  ① 4.4 循环入口对齐填充 `jmp +2; nop`（info C1 之后、ScanType 之前），ORIG
//     4.1 直接 fall-through（N112 变体，jmp 非 nop 不被 strip_nop_padding 剥离）；
//  ② find 的 sret 临时槽与 key 槽互换：ORIG key -0xb8 / temp -0xbc，4.4 编译器
//     临时恒优先占最浅槽 ⇒ key -0xbc / temp -0xb8（N82/N86 槽分配族；穷举
//     具名 iterator/声明序互换/内联实参三形均不闭合）。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N82] orig/ours=364/363
//   jcc 20:20 逐位同构（含 0x37 跳表前置 `cmpl $0x37; ja`），仅 3 处 jmp/nop
//   岛位置差（我方循环入口多 `jmp +2; nop`、ORIG 尾多 `jmp; nop`；N112/N121）；
//   p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 配对，P93）；
//   无 call *（无虚调用）；无语义缺口。
#include "InitScriptProducers_7/importInGameEventInfoList.inc"
}

// ============================================================================
// 三个子类型构造器为头文件内联（CDataManager.h，ORIG 具名 C1 0x837c6be/6e6/70e）。
// ORIG 0x891b4a8 W：临时元素 {kind,a,b} 后 push_back。
#include "InitScriptProducers_7/Add.inc"
}

// ORIG 0x891b4dc W。
#include "InitScriptProducers_7/Add_1.inc"
}

// ORIG 0x891b502 W。
#include "InitScriptProducers_7/Add_2.inc"
}

// ORIG 0x837109a W：三 int 逆序清零（全局类型，mangled 无 stQuestShop 前缀）。
#include "InitScriptProducers_7/stQuestShoptInitCost.inc"
}

// ORIG stNPCRelationEventInfo C1 0x891c946 W：两个 FavorAndReward 子对象 +
// vector<int> @+0x3c 默认构造。
#include "InitScriptProducers_7/stNPCRelationEventInfo.inc"
#include "InitScriptProducers_7/stMercenaryRewardGroup.inc"
}

// ORIG stMercenaryCompetitionArea C1 0x89f7b28 W：ushort 清零 + vector 构造
// + int@0x10 清零 + ushort@0x14 清零。
#include "InitScriptProducers_7/stMercenaryCompetitionArea.inc"
}

// ORIG stHellPartyMonsterGroupList C1 0x891b1d6 W：memset(-1) 两段 + 三标量清零。
#include "InitScriptProducers_7/stHellPartyMonsterGroupList.inc"
}

// ORIG stHellParty_t C1 0x891b242 W：map@0x54 构造；标量清零；三段 memset；
// 难度表五组循环清零；+0x90=0。
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0891b242
// 4.1 发射序 = map C1(+0x54) → +0x50 →
// +0x0..0xc → +0x4c → +0x10 → +0x14 → memset 0x18/0x28/0x3c → +0x90 → 清零环
//（P239：类成员 meminit 前置、其余标量 store 段按 ORIG 序落 ctor 体）；
// 原全 meminit 形在 4.4 按声明序混合发射且 m_difficultyRates()/m_partyRates()
// 另发 value-init 循环（重复清零），删除并把标量移入体内。
#include "InitScriptProducers_7/stHellParty_t.inc"
}

// ORIG D1 0x891c7e2 W：仅析构 map 成员（其余 POD 无动作）。
#include "InitScriptProducers_7/dtor_stHellParty_t.inc"

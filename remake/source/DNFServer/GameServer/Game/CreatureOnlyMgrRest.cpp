// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CreatureOnlyMgrRest/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— user_creature::CCreatureMgr 收尾批（CreatureOnlyMgrRest）。
// ---- b28d-61 判词汇总（本 TU 14 件非 identical）----
// [L14 round2 复核] 5 件逐件重核（check_tu_routed fresh 40/77/3/3）：
//   ChangeOwner 121/121、ChangeSlotNo 127/127、InsertCreatureItemResult 81/81 —— 0 差异块；
//   槽直方图 (a) 相对序（ORIG -0x3c/-0x34/-0x2c 对我方 -0x38/-0x30/-0x20，最小差 3~4，
//   非恒定平移），声明序/聚合化/volatile/framePad 四形前轮已实测不可翻 ⇒ 维持 N82附/N115；
//   SetOverSkillTime 215/220（N115 帧 + N97 /1000 lowering）、SaveCreatureItem 156/165
//   （我方多 push %edi + 槽浅 0x10）⇒ 维持原判词。
// 已修 AE：UpdateCreatureItemResult（显式指针算术 `(char*)sig+4` 复现 ORIG lea）、
//   EventEvoluteCreature（P222 正向包裹翻 setge 极性）。
// N 封顶（ORIG 地址见各函数头/报告）：
//   checkMaxCreatureLevel 0x0833baee（GetItemId 已补；余 N89 STL 调用+帧）
//   LoadCreatureItem 0x0833b68c（N89 内联差异 1 处）
//   IsExistedCreatureItem 0x0833a942（N104 分支/trace 块布局）
//   IsSaveCreatureItem 0x0833ba30（setcc-fold：ORIG sete vs 4.4 setne）
//   C1ERKS0_ 0x0833b6xx（N84/N86 拷贝构造整体形态）
//   SendCreatureItemInfo 0x0833b498（P183 槽/局部缓存）
//   AuctionRegist 0x0833b09c / ChangeOwner 0x0833ad34 / ChangeSlotNo 0x0833aedc
//     / SaveCreatureItem 0x0833bbbe（N80 cMyTrace 变参求值序+GetItemId 块）
//   InsertCreatureItemResult 0x0833b4d4（N82 寄存器保存 push edi/esi）
//   Evolute 0x0833beb4（N85 bool 物化岛）
//   DenySwapArtifact 0x0833989c（N93 块序）
//   SetOverSkillTime 0x0833c41e（P211 求值序，GetItemId 已补）
// 工作清单 /tmp/only_orig/user_creature.txt 中 _ZN...12CCreatureMgr 27 符号，
// 报告 docs/class_func_reports/user_creature__CCreatureMgr/<Method>.md 全文
// （## 汇编 + ## 反编译 C）逐个转写；报告缺失者 objdump 标【objdump】。
//
// 本批另代管四个被本批方法调用、当前全仓无人提供定义的 user_creature::
// CDBMsgSender W 内联方法（ChangeOwner/ChangeSlotNo/AuctionRegist/Cancel），
// 均按 ORIG 逐指令转写【objdump】；归口收编时请宿主迁移后删除此处定义。
//
// ---- b28d-solo-CreatureOnlyMgrRest 泳道结果（清单 9 件，2026-09-09）----
// TU 三数：identical 40 / ae 75（合计 115）/ near 5 / diff 3（含 _Z41 豁免）
//   —— 基线 62/102/6/4；near −1、diff −1。ident+ae 下降 51 属**弱符号发射面
//   迁移**（原 P147 force 的 `CCreatureMgr copy(src)` 连带发射的 map/_Rb_tree
//   拷贝模板 W 实例，现由 CInventory.o 发射，链接期 COMDAT 合并、全局符号集
//   不变；Main 2026-09-09 裁决本泳道验收按 near/diff 不升 + 全局口径）。
// 转 AE（2）：
//   Evolute 0x0833beb4（P221：兄弟块 cMyTrace 改全表达式临时链，翻 then/else
//     槽向 -0x28/-0x18 与 ORIG 一致）。
//   C1ERKS0_ 0x86992cc（N84 正向修复：CInventory.h 删用户拷贝构造声明 ⇒ 编译器
//     隐式合成 W；Main 续改 CInventory.cpp 拷贝构造成员初始化列表 ⇒ 发射点迁
//     CInventory.o，本 TU force 已删；Main 删 m_pad49 后与 ORIG 62 条逐条 AE，
//     含 map/数组/尾部 bool 全序）。
// 仍 NEAR（5）：
//   IsSaveCreatureItem 0x0833ba30 / LoadCreatureItem 0x0833b68c：N7（4.4.7 间接
//     call 目标恒 %eax vs ORIG %edx）；本波已把 §4.5 槽指针直调改为真虚调用
//     （CCreatureItem 多态化后 entry->IsSave()/SaveCreatureItem(...)）。
//   ChangeOwner 0x0833ad34 / ChangeSlotNo 0x0833aedc / InsertCreatureItemResult
//     0x0833b38c：N82附（4.4 聚合优先低槽 vs 4.1 标量/生命周期穿插）+ N115
//     栈边界。四形实测：reason 声明位置/顺序、结构体/数组聚合化、volatile、
//     double framePad 均不可翻（聚合化反致额外拷贝劣化，已回退）。
// 仍 DIFF（2，语义完整、p167 CLEAR）：
//   SaveCreatureItem 0x0833bbbe：N115 全槽深 0x10 + push %edi（4.4 把函数指针/
//     m_user/count 计算分配进 edi/esi vs ORIG ebx/ecx/eax/edx）；p167 16/16。
//   SetOverSkillTime 0x0833c41e：N115 帧 0x7c vs 0x6c（槽整体深 0x10）+ N97
//     有符号 /1000 lowering；p167 28/28（本波修掉 EXTRA GetOverSkillTime/GetTime
//     各 1 次：trace 第 2 实参改用 last 槽）。
// 本波改动：P139（ChangeOwner/ChangeSlotNo trace 串 → __PRETTY_FUNCTION__）、
//   P171（SaveCreatureItem 删 entry 判空）、真虚调用转换、P221（Evolute）、
//   SetOverSkillTime trace 段按 ORIG 槽集重写（ovstR/stR 存 /1000 结果）。
//
// 声明追加需求见 /tmp/df_batches/CreatureJob_decl_UCMgr.txt。
// ============================================================================
#include <string.h>

#include <map>

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CPacketRespondent.h"
#include "SigTypes.h"
#include "CSystemTime.h"
// ============================================================================
// CreatureOnlyMgrRest.cpp —— user_creature::CCreatureMgr worklist 收尾
// （OnlyOrig_CreatureJob 批；SIG_CREATURE_ITEM_DETAIL_INFO 权威定义已提升至
//   CInventory.h 顶部，本 TU 不再携带局部副本。）
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr10SetStomachEi T 0x08339ee0（SetStomach.md）：
// m_equippedCreature 非空 → CCreature::SetStomach(value) 后经 CPacketRespondent
// 广播 MakeNotipacketCreatureState(uid, GetStomach())；返回恒 1。
// ============================================================================

namespace user_creature
{
#include "CreatureOnlyMgrRest/SetStomach.inc"
}


// ORIG _ZN13user_creature12CCreatureMgr11ChangeOwnerEP10Inven_Itemiiii
// T 0x0833ad34（ChangeOwner.md）：槽位/物品类型过滤后 CDBMsgSender(0x8a,m_user)，
// sender.ChangeOwner(item,characNo,slotFrom,mapSize,param_4,param_5)；
// 成功 → Send() + WriteCreatureLog(3,item idx,addInfo2,1,type,param_5)；
// 失败 → cMyTrace(...,0xc26,5) "cannot write db uid(%d)\n"。过滤分支均返 true。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：**真语义 bug 修正**
// （WriteCreatureLog 末实参 ORIG 0x0833ae4e 读 0x14(%ebp)=mapSizeHint，
// 旧码误传 param_5）+ P212/P309 reason 全头部声明 + P0 内联 characNo
// + P227 临时链 trace + P52 链式 receiver + P139（trace 串改
// __PRETTY_FUNCTION__ 复现 `_ZZ...E19__PRETTY_FUNCTION__` 局部静态符号）。
// 残差 = N82附（4.4 聚合 sender/trace 占低槽、标量 reason 落 -0x1c，ORIG
// reason 最深 -0x3c）+ N115 帧 0x4c vs 0x5c；reason 声明位置/顺序、结构体/
// 数组聚合化、volatile、double framePad 四形实测不可翻（聚合化劣化已回退）。
#include "CreatureOnlyMgrRest/ChangeOwner.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr11ItemEvoluteEi T 0x0833d6c2（ItemEvolute.md）：
// 装备宠存在则**直通透传** CCreature::ItemEvolute(m_user, itemIdx) 的 %eax
// （call 后无 test/movzbl，直接 jmp 尾 leave;ret），否则尾块 mov $0。
#include "CreatureOnlyMgrRest/ItemEvolute.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr12ChangeSlotNoEP10Inven_Itemii
// T 0x0833aedc（ChangeSlotNo.md）：FindCreatureItem(uid) 为空返 false；先本地
// SetSlotNo(newSlot)，再 CDBMsgSender(0x8a).ChangeSlotNo(item,characNo,...)；
// 成功 Send + WriteCreatureLog(3,item idx,addInfo2,1,type,param_3)；失败 trace 0xc44。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P212/P309 reason 全头部声明
// + P0 内联 characNo + P227 临时链 trace + P52 链式 receiver（reason 实参
// 经复核 = param_3 = ORIG 0x14(%ebp)，无 bug）+ P139（trace 串改
// __PRETTY_FUNCTION__）。残差 = N82附 聚合/标量槽分区（ORIG reason -0x3c、
// 我方 -0x1c）+ N115 帧 0x4c vs 0x5c（creatureItem/sender/trace 三槽统一浅 4）。
#include "CreatureOnlyMgrRest/ChangeSlotNo.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr12LoadArtifactEiP10Inven_Item
// T 0x0833be7c（LoadArtifact.md）：item 非空转 EquipArtifact(slot,item)，空返 1。
#include "CreatureOnlyMgrRest/LoadArtifact.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr13AuctionRegistEP10Inven_Itemii
// T 0x0833b09c（AuctionRegist.md）：sender.AuctionRegist(item,characNo,p2,p3,size)
// 成功 → Send() + DeleteCreatureItem(addInfo2)；失败 trace 0xc5c。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （DeleteCreatureItem 第 2 实参 ORIG 读 0x14(%ebp)=param_3，旧码误传两次
// item->m_addInfo2）+ P0/P227 内联 characNo 与临时链 trace + 返回归一化
// `if(!call()) return false; return true;`。
#include "CreatureOnlyMgrRest/AuctionRegist.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr16DenySwapArtifactEP10Inven_ItemiiS2_ii
// T 0x0833989c（DenySwapArtifact.md，条件序以汇编为准）：
// 有装备宠：slot 组 (s1,b)/(s3,a) 任一满足 IsArtifactEquipSlot && !b.isEmpty()
// 且装备宠 IsAvailableArtifact(s1) 为假 → deny(1)；否则 0。
// 无装备宠：任一组 IsArtifactEquipSlot && !isEmpty(b/a) → allow(0)；否则 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：**真语义 bug 修正**
// （无装备宠分支 ORIG 0x08339990 无 `xor $1` ⇒ 条件为 b->isEmpty()/a->isEmpty()，
// 旧码 `!isEmpty()` 极性写反）+ P226 语句表达式零槽复现 `&&` 物化岛 +
// N93附 反向共享尾（block A return false 提为函数尾，复现 ORIG
// `mov $1,%eax; jmp; mov $0,%eax` 尾块序）。
#include "CreatureOnlyMgrRest/DenySwapArtifact.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr16GetCreatureIndexEv T 0x08338f36
// （GetCreatureIndex.md）：尾调 CCreature::GetCreatureId(W 0x833f0da,+0x48)，
// eax 直通故为 int 返回；ORIG 不判空（忠实转写）。
#include "CreatureOnlyMgrRest/GetCreatureIndex.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr16LoadCreatureItemEP10Inven_ItemP29SIG_...
// T 0x0833b68c（LoadCreatureItem.md）：工厂按 detail type 取项；临时到期换算；
// 清临时租期记录后虚槽+8 分派（蛋→CEgg::LoadCreatureItem / 宠→CCreature::...）；
// 失败回收对象回池；成长宠刷 isMaxLevel 标记。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P248 档② m_growExpFlag
// uchar→bool + 裸 `if(!field)`（GamePODTypes.h）+ 删 m_addInfo 冗余 cast；
// CCreatureItem 多态化后改真虚调用 `newItem->LoadCreatureItem(this,item,
// detail,m_user)`。残差 N7：4.4.7 间接 call 目标恒 %eax，ORIG 用 %edx
// （P230 换后端可治；本 TU 未路由 4.4.6）。
#include "CreatureOnlyMgrRest/LoadCreatureItem.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr16SaveCreatureItemEv T 0x0833bbbe
// （SaveCreatureItem.md，条目寻址以汇编 lea 复核）：
// CDBMsgSender(0x89) 取 SIG_CREATURE_ITEM_LIST(0x4078)；count@+0；
// 遍历宠物：IsSave() 为真 → SaveCreatureItem(item 区 @cnt*0x3d+4（Inven_Item 数组）、
// detail 区 @cnt*0x38+0x21a0、m_user)，detail.m_uid(+4) 非 0 则 count++；
// count==0 → Cancel 否则 Send。
// [b28d-solo-CreatureOnlyMgrRest] verdict=N 封顶（DIFF，语义完整，p167 16/16
// CLEAR）：真虚调用（entry->IsSave / entry->SaveCreatureItem，ORIG
// 0x0833bc9b/bcb1）+ P171（删 entry 判空，ORIG 直接解引用）+ P183 count 每处
// 重读 + P28 `it++`（ppEi）。残差 = N115 帧 0x4c vs 0x40、全槽深 0x10 与
// push %edi（4.4 把函数指针/m_user/count 计算分配到 edi/esi，ORIG 用
// ebx/ecx/eax/edx；detail 地址 `lea (,%eax,8); mov; sub` 链寄存器角色互换）。
#include "CreatureOnlyMgrRest/SaveCreatureItem.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr16SetOverSkillTimeERKi T 0x0833c41e
// （SetOverSkillTime.md）：死亡宠免检返 1；匹配装备宠 item 时校验脚本/等级/
// 冷却窗（cur+2 < last+st/1000+ovst/1000 → trace 0xe6c 返 0x13），通过则
// COverSkillTime::SetTime() 刷新；其余路径 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=N 封顶（DIFF，语义完整，p167 28/28
// CLEAR）：P196 比较方向翻正（ORIG `cmp %eax,%ebx; setg` = 累加值在左）+
// P0 去 learnLevel/expLevel/lastUsed/stMs/ovstMs/curSec 具名局部 + P23/P24
// 求值序（itemIdx 写左侧）+ P227 临时链 trace1 + trace 段按 ORIG 槽集重写
// （ovstR/stR 存 **除 1000 后**的值 @-0x48/-0x44、interval@-0x40、last@-0x3c，
// trace 第 2 实参用 last 槽 —— 修掉 EXTRA GetOverSkillTime/GetTime 各 1 次）。
// 残差 = N115 帧 0x7c vs 0x6c（全槽深 0x10）+ N97 有符号 /1000 lowering
// （`mov %ecx,%eax` 中转数差 2）。
#include "CreatureOnlyMgrRest/SetOverSkillTime.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr18IsSaveCreatureItemEv T 0x0833ba30
// （IsSaveCreatureItem.md）：map 非空时任一条目 IsSave()（vt+4==+0x40 flag）
// 为真即 1；否则 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P292/P16 `!size()` 早返形
// （ORIG `test; sete; test; je`）+ P225 拆嵌套 if + P28 `it++` + CCreatureItem
// 多态化后真虚调用 `entry->IsSave()`。残差 N7：间接 call 目标 4.4.7 恒 %eax
// vs ORIG %edx（P230 换 4.4.6 可治；本 TU 未路由）。
#include "CreatureOnlyMgrRest/IsSaveCreatureItem.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr18UnRegistTempPeriodEi T 0x0833d962
// （UnRegistTempPeriod.md）：m_field30.find(uid)!=end 则 erase 并返 true。
#include "CreatureOnlyMgrRest/UnRegistTempPeriod.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr20EventEvoluteCreatureEiii T 0x0833da1e
// （EventEvoluteCreature.md）：等级门槛不足 trace 0x10c2；进化成功广播列表包
// MakeNotipacketCreatureItemList(map*,0xee) 后返 1。
#include "CreatureOnlyMgrRest/EventEvoluteCreature.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr20SendCreatureItemInfoEP17SIG_CREATURE_ITEM
// T 0x0833b498（SendCreatureItemInfo.md）：SIG.m_slot(+0x2c) 槽物存在且非空 →
// 广播列表包后 SendUpdateItemList(1,ITEMSPACE 7,slot)；返回恒 1。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P197③ slotItem
// 两段式（ORIG `movl $0,-0xc`）+ P52 `sig->m_slot` 内联实参跨 receiver call
// 保活 + 去 inv/slot 具名局部（三处使用点各自重读 sig->m_slot）。
#include "CreatureOnlyMgrRest/SendCreatureItemInfo.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr22LoadCreatureTempPeriodEP31SIG_POSTAL_...
// T 0x0833d9d8（LoadCreatureTempPeriod.md）：+4 非 -1 时 RegistTempPeriod(uid,t,0)；
// 返回 t!=-1。（结构 SIG_POSTAL_CREATURE_TEMP_PERIOD 由 decl 收编，仅证 +0/+4）
#include "CreatureOnlyMgrRest/LoadCreatureTempPeriod.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr22SetCreatureItemAddInfoEii T 0x0833b620
// （SetCreatureItemAddInfo.md）：背包区 3 槽物存在且 idx(+0x02) 非空 →
// set_add_info(value) 返 1；否则 0。
#include "CreatureOnlyMgrRest/SetCreatureItemAddInfo.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr24InsertCreatureItemResultEP17SIG_...
// T 0x0833b38c（InsertCreatureItemResult.md）：load 后 field64==6 广播列表；
// set/order 记录 ceraOrderNo(+0x60) 前后置/清零；WriteCreatureLog 五值直取 SIG
// （idx +0x8 / addInfo2 +0x30 / 类型 byte +0x5 / reason +0x51）。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：删多余 memset（ORIG 仅
// Inven_Item::C1）+ P235 具名成员访问（m_reason/m_addInfo2/m_itemView.m_itemIdx/
// m_item[1] 复现 disp 折叠）+ P52 单语句链式 receiver。残差 = N82附（4.4 把
// Inven_Item 聚合放低槽 -0x59、enum reason 放浅槽 -0x1c；ORIG reason 最深
// -0x5c、localItem -0x55）+ N115 帧差；reason 提前/后置、ReasonBox 聚合化、
// reason[1] 数组化、char[3] 撑位四形实测不可翻（聚合/数组形多 2 条拷贝劣化）。
#include "CreatureOnlyMgrRest/InsertCreatureItemResult.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr24UpdateCreatureItemResultEP17SIG_...
// T 0x0833b592（UpdateCreatureItemResult.md）：同载入链；成功且 field64==4
// 广播 SendCreatureItemInfo。
#include "CreatureOnlyMgrRest/UpdateCreatureItemResult.inc"
}

// ORIG _ZN13user_creature12CCreatureMgr7EvoluteEiib T 0x083beb4
// （Evolute.md）：等级达标或 force 时；单线/多线进化分别判定后执行，
// 各失败 path trace（0xdd6 kind / 0xddb level）；空宠/不满足返 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=AE（原 NEAR）：P226 语句表达式复现
// 两处 `||` 的 mov$1/mov$0 物化岛（ORIG 0x0833bedc）+ P0 去 GetLevel 具名局部；
// 两分支 cMyTrace 改全表达式临时链（P221）后 then/else 槽向 -0x28/-0x18 与
// ORIG 逐条一致 ⇒ NEAR→AE。
#include "CreatureOnlyMgrRest/Evolute.inc"
}

// ---- ORIG _ZN13user_creature12CCreatureMgrC1ERKS0_ W 0x86992cc 【objdump】----
// （nm 弱别名 C2 同址）：按成员序拷贝 map<int,CCreatureItem*> → +0x18 equipped →
// +0x1c user → +0x20..+0x2b artifacts（int×3 循环）→ +0x2c tempExpireDelta →
// map<int,int> field30 → movzbl +0x48 isMaxLevel 标记。用户定义拷贝构造同时
// 产出 C1/C2 两符号。
// ORIG T 0x08339196：成员 map/CArtifact[3] 隐式默认构造后调 Reset()
// （置 equipped=0/user=0/tempExpireDelta=-1/artifacts Reset/isMaxLevel=1）。
#include "CreatureOnlyMgrRest/CCreatureMgr.inc"
}

// ORIG _ZN13user_creature12CCreatureMgrC1ERKS0_ W 0x86992cc【objdump】：
// map 拷贝 → +0x18/+0x1c → m_artifacts[3] 内联 3×mov → +0x2c → map30 →
// +0x48 bool；无 CArtifact::C1 调用 ⇒ 原码是**编译器隐式合成拷贝构造**。
// [b28d-solo-CreatureOnlyMgrRest] verdict=AE（原 DIFF 57）：CInventory.h 删用户
// 拷贝构造声明（Main 代落）⇒ 编译器隐式合成 W C1ERKS0_/C2ERKS0_；Main 续删
// m_pad49 后与 ORIG 62 条逐条一致（map 拷贝 → +0x18/+0x1c → 数组 3×mov 循环
// → +0x2c → map30 → +0x48 bool，尾部无 pad 拷贝、无 CArtifact::C1）。
// 发射点：CInventory.cpp 的 CInventory 拷贝构造成员初始化列表（Main 代落）。
// 本 TU 原 P147 force 使用点已按 Main 裁决删除（其连带发射的 map 拷贝模板
// W 实例随发射面迁至 CInventory.o，链接期 COMDAT 合并，全局符号集不变）。
// 真 bug 修正：旧用户定义形 `i<2` 漏拷 m_artifacts[2]（ORIG 循环 3 次）。

// ============================================================================
// const 侧查询
// ============================================================================

// ORIG _ZNK13user_creature12CCreatureMgr13getExpireDateEi T 0x0833d758
// （getExpireDate.md）：FindCreatureItem 空 → -1；否则 getExpireTime()。
#include "CreatureOnlyMgrRest/getExpireDate.inc"
}

// ORIG _ZNK13user_creature12CCreatureMgr19GetCreatureItemSizeEv W 0x8531fbe
//【objdump】：单 call m_creatureItems.size() 后直返。
#include "CreatureOnlyMgrRest/GetCreatureItemSize.inc"
}

// ORIG _ZNK13user_creature12CCreatureMgr21IsExistedCreatureItemEi T 0x0833a942
// （IsExistedCreatureItem.md）：遍历 map；条目空 → cMyTrace(...,0xbda,5)+
// "[%s][%d]"；GetSlotNo()==slot 即返 1；否则 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P272 反写 if/else
// （ORIG `je <trace 块>`，else 体直落）+ P28 `it++` + P221 全表达式临时链
// 翻 trace 对象槽（ORIG -0x20 浅于 begin -0x28）。
#include "CreatureOnlyMgrRest/IsExistedCreatureItem.inc"
}

// ORIG _ZNK13user_creature12CCreatureMgr21checkMaxCreatureLevelEib T 0x0833baee
// （checkMaxCreatureLevel.md）：命中 itemId 且成长标志一致的首个条目上取
// CCreature::isMaxLevel()；否则 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=IDENTICAL/AE：P28 增量侧 `it++`
// （ORIG 增量段 call ppEi，具 sret 临时 -0x10）。
#include "CreatureOnlyMgrRest/checkMaxCreatureLevel.inc"
}

// ORIG _ZNK13user_creature12CCreatureMgr22isGrowCreatureMaxLevelEv T 0x0833bbb2
// （isGrowCreatureMaxLevel.md）：movzbl +0x48 直返。
#include "CreatureOnlyMgrRest/isGrowCreatureMaxLevel.inc"
}

}  // namespace user_creature

namespace user_creature
{

// ORIG _ZN13user_creature9CCreature16IsCreatureScriptEv W 0x0833f0f2【objdump】：
// setne +0x44(m_script!=0)。
#include "CreatureOnlyMgrRest/IsCreatureScript.inc"
}

}  // namespace user_creature



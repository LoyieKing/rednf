// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MonsterWrapLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// MonsterWrapLCB.cpp — LinkClosureB 批 B 怪物包装传递依赖收口（唯一 owner TU）
//
// 全部函数依据 df_game_r objdump/docs/class_func_reports 报告逐指令转写。
// 覆盖符号：
//   _ZNK12CAICharacter13IsHellMonsterEv                      W @0x08539de8
//   _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834a15e
//   _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834f8a6
//   _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE
//                                                            T @0x0834f8e8
//   _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
//                                                            T @0x08349da0 (0x1ab)
//   _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
//                                                            T @0x08349f4b (0x11a)
// CLuckPoint::UseLuckPoint(int,int,int,int) @0x08550b14 与
// CLuckPoint::sendItemDropRate(char const*) @0x08550ef0 已由 CClearReward.cpp
// 提供唯一 owner（LinkKillChain 批次，语义经 ORIG 报告逐分支核对一致），
// 本 TU 不重复定义。
// ============================================================================
#include "MonsterIndependentDrop.h"

#include "CItemList.h"   // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMonster.h"   // CMonster::getMonsterScript / STMonsterScript 完整类型

// 全局生成函数自由声明（此前仅 mangled 符号存在于 ORIG；本 TU 为权威 owner）
#include "MonsterWrapLCB/generateDropAvatarItem.inc"
#include "MonsterWrapLCB/generateCatchMonsterItem.inc"
#include "MonsterWrapLCB/IsHellMonster.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834a15e _ZNK12CAICharacter22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   lea 0x30(this) 取 m_dropAvatarInfo 后转发自由函数。
#include "MonsterWrapLCB/generateDropAvatarItem_1.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8a6 _ZNK8CMonster22generateDropAvatarItemEhhRSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0xcf8 —— 即 m_script(+0x60) 内
//   m_dropAvatarInfo(+0xc98) 子对象，具名访问后转发自由函数。
#include "MonsterWrapLCB/generateDropAvatarItem_2.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x0834f8e8 _ZNK8CMonster24generateCatchMonsterItemERSt6vectorI10Inven_ItemSaIS1_EE：
//   call getMonsterScript(this) 后 lea 0x70(script) 取 m_catchItems 转发。
#include "MonsterWrapLCB/generateCatchMonsterItem_1.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349da0 _Z22generateDropAvatarItemRK23stEnamyDropAvatarInfo_thhRSt6vectorI10Inven_ItemSaIS3_EE
// （size 0x1ab）头像随机掉落主逻辑：
//   ① 入门阈值：threshold = (int)((double)m_field00 * T_A[(u8)rarity]
//                                 * T_B[(u8)grade - 1])；
//     T_A 基址 info+0x18（flds 0x8(%eax,%edx,4)，edx=(u8)rarity+4）、T_B 基址
//     info+0x8、下标 (u8)grade-1 —— 两片均为 clear 清零区浮点表的重叠视图，
//     按指针视图逐指令转写；fistpll 截断取低 dword。
//     get_rand_int(100000) > threshold 时直接返回。
//   ② 共 rounds = *(unsigned*)(info+0x04)（无符号 seta 比较）轮：
//       每轮 r = get_rand_int(*(int*)(info+0x2c))；
//       自 m_elements.begin() 重新遍历：首个 elem.m_field04 > r（无符号）时，
//       Inven_Item{}；item.m_addInfo(ORIG 写 +0x02 视图) ← elem.m_field00；
//       G_CDataManager()->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)1,
//       item, 1)；out.push_back(item)；命中后不推进迭代器即退出本轮
//       （ORIG jmp +0x170 → 轮数 +1 继续）。
// [b28d-L14 N18a] ORIG 0x08598f34；p167 缺 1 次 const_iterator ptEv；残差=槽深/寄存器。
// [F2e-Singles2 C05② 2026-09-10] 旋钮①删 rounds 具名局部（P0：ORIG 尾测每轮重读
//   `mov 0x8(%ebp),%eax; mov 0x4(%eax),%eax; cmp -0x10(%ebp),%eax; seta`，而我方缓存
//   rounds 于 -0x14 并折 `setb`）⇒ 尾测与 threshold 槽(-0x18→-0x14)一次对齐 ORIG；
//   旋钮②base/tableA/tableB 引用内联 + longlong 整数域（fildll/fistpll）⇒ thresholdLL。
//   实测 our 129→121（ORIG 122，gap **−7→−1**），TU 四数 27/38/0/2 两态逐位一致。
//   残余 = 4.4 寻址折叠（我方 `add $0x6; shl $0x2; add; flds (%eax)` vs ORIG
//   `add $0x4,%edx; flds 0x8(%eax,%edx,4)`，复现需 .h 成员数组视图，本任务禁改 .h；
//   N125 族）+ 槽号差（N82）。
#include "MonsterWrapLCB/generateDropAvatarItem_3.inc"
}

// ----------------------------------------------------------------------------
// ORIG T 0x08349f4b _Z24generateCatchMonsterItemRKSt6vectorI13CatchItemInfoSaIS0_EERS_I10Inven_ItemSaIS5_EE
// （size 0x11a）：CatchItemInfo 三成员均 int 存储，第 3 成员由 flds 按 float
// 位型消费，故以 reinterpret 视图承载。
//   count(m_field4)==0 → 跳过；
//   rate(m_field8 视 float)==0.0f → 跳过（fnstsw+sahf 仅判 ZF：NaN 继续走）；
//   r = get_rand_int(0x64)；(double)rate > (double)r 才生成；
//   Inven_Item{}；item.m_addInfo = m_field0；create_item((T)1,item,1)；
//   push_back count 次（signed setg 比较）。
// [b28d-L14 P167/N90] ORIG 0x08598d2c；已改 m_itemList 直取（消 EXTRA getItemList）；
// 残差=12B 结构体按值拷贝形（N90 族）。
// [F2e-Singles2 C05③ 2026-09-10] D08 `||` 合并两守卫（m_field4==0 ‖ rate==0.0f）
//   **实测零效果**：我方汇编改前/改后逐条相同、our_insns 93=93、岛数不变
//   （3×`jmp+nop` 仍在）⇒ 已回滚（属 4.4 块布局/A28 族，非「多守卫同目标」可治）。
#include "MonsterWrapLCB/generateCatchMonsterItem_2.inc"
}

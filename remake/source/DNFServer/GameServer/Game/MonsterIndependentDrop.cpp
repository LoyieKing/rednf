// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MonsterIndependentDrop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// MonsterIndependentDrop.cpp — 独立掉落生成域（df_game_r 忠实转写）
//
// 全部函数按 ORIG objdump 逐指令转写（自由函数无 Ghidra 报告，逐指令还原；
// CAI/CMonster 版本为纯转发）。ORIG 地址：
//   generateIndependentItem      0x0834989a
//   independentItemRateControl   0x0834972f
//   CAICharacter::generateIndependentItem    0x0834a13a
//   CMonster::generateIndependentItem        0x0834f876
// ============================================================================
// [b28d-56 SweepMonsterDrop] 本 TU 判词表（verdict = check_tu_routed.sh fresh .o +
// game_func_report --no-compile 读汇编）：
//   TU 四数：identical=40 ae=62 near=0 diff=3 missing_in_game=20
//     （基线 42/61/0/3/19）。identical −2 / missing +1 成因 = pairList 迭代器改
//     const_iterator 后本 TU 不再发射 2 个 ORIG 本 TU 未使用的非 const 实例
//     （ppEv / ne<iterator,iterator>），同时发射 ORIG 实有的 const 转换构造
//     （C2 共同 + C5 为 4.4 独有别名）；P167 差集由 7 项（4 MISSING + 6 EXTRA）
//     降到 5 项且全为 P93 同址别名（C1/C2、D1/D2、C1IPS2/C2IPS2）。
//   generateIndependentItem 0x0834989a：gap 20→4→2（our 343→347→349 / ORIG 351）。
//     真 bug①：genCount 下标 groupIdx+3 → groupIdx+4（ORIG 0x8349aaf 绝对地址
//       drop+0x14+4g；旧读 drop+0x10+4g，少 1 个 int 元素）。
//     修②：groupIdx / premium goldBase 改走 +0x3e / +0x3c 独立字节视图（P196；
//       旧 movzwl 0x3e / 0x3c 多读一字节）。
//     修③：premium 补 (int)(float) 恒等往返（ORIG 0x83499f8 fildl / 0x8349a1a fistpl）。
//     修④：删 product 局部（P0 无槽 ⇒ 原码两次独立 imul）＋ 删 else 分支多余
//       hitCount 清零 ＋ 缓存 end 迭代器（P13）。
//     修⑥：m_field28 已 enum 化（CItemGenerator.h，Main 批准；探针
//       /tmp/enum_probe3.cpp int 单段 vs enum 两段，4.1.2/4.4.7 双证；
//       sizeof 0x48 / +0x28 偏移 / 对齐 / mangled 零变化），补回 2 条
//       `mov 0x28(%eax),%eax; mov %eax,%edx`。
//     残余 = 2×`sub $1`（ORIG `sub $1; add $4` 两步，4.1.2/4.4.7 探针 6 形
//       （(g-1)+4 / 指针算术 / int·uint 局部 / 中间变量 / 显式 cast）均折
//       `lea 3/4` 一步 ⇒ 判 N/P75 跨版本折叠；最终跳转落点差 6 字节与此吻合）。
//   independentItemRateControl 0x0834972f：真 bug⑤ —— 守卫条件反向。ORIG 0x83497c7
//     `cmp diff; setle %al; test; je <循环尾>`（Ghidra C 同判 `if (field04 <= diff)
//     {衰减}`）⇒ 仅 field04 <= diff 执行衰减；旧源码写成 `<= diff` 时 continue，
//     衰减被施加到错误条目。修后 verdict 114/115，P167 8/8 CLEAR；残余 =
//     P223 岛数（ORIG 2 岛+1 直跳 vs 4.4 3 岛）、N85 x87 比较 lowering、N82/N126
//     槽基差 4（-0x18/-0x12/-0x11 ↔ -0x1c/-0x16/-0x15）。
//   stMonsterIndependentDrop_t::C1ERKS_ 0x08376bb2：52/58，N90 数组分块拷贝封顶
//     （原判词，未动）。
//   CAICharacter 0x0834a13a / CMonster 0x0834f876 转发件：13/13、15/15（.o 阶段
//     call 标签未解析差 = N124，链接后 identical）。
// ============================================================================
#include "MonsterIndependentDrop.h"

#include <algorithm>

#include "CItemList.h"     // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMonster.h"      // CMonster::getMonsterScript
#include "GameRand.h"      // get_rand_int

// ----------------------------------------------------------------------------
// ORIG 0x0834972f _Z26independentItemRateControlR26stMonsterIndependentDrop_t
//                    RKN8WongWork19stGenerateRefData_tE
// 遍历 G_CDataManager()+0xa89c 的 vector<stIndependentDropRateControl>
// （即 m_independentDrop.m_vec18，+0xa884 + 0x18 实证）：
//   ctrl.m_itemId == drop.m_itemIdx 且 drop.m_rate[jobIdx] != 0 时，
//   diff = (u8)ref.m_field3f - (u8)ref.m_field34；仅当 ctrl.m_field04 <= diff：
//   f = (float)(10000 - ctrl.m_field08*diff)；f==0 → f=1.0f；
//   drop.m_rate[jobIdx] = (int)(rate * (f / 10000.0f))（fistpl 舍入）。
#include "MonsterIndependentDrop/independentItemRateControl.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0834989a 自由函数。逐指令语义：
//   jobIdx = (u8)ref[+0x36]（掉率档位）；chan = (u8)ref[+0x34]；
//   groupIdx = (u8)ref[+0x3e]（数量档位）。
//   对每个 drops 元素拷贝一份后：
//   1) 若 m_rangeMin/m_rangeMax 非 0：ref.m_field25 非 0 时要求
//      m_rangeMin <= ref.m_field2c <= m_rangeMax，否则要求
//      m_rangeMin <= chan <= m_rangeMax，不满足则跳过该项。
//   2) independentItemRateControl(copy, ref)。
//   3) premium：m_rate[10]>0 && ref.m_premiumDropRate>0 && ref.m_goldBase==3 时
//      m_rate[jobIdx] = m_rate[jobIdx]*(premium+100)/100。
//   4) ref.m_field25 && ref.m_field30>1：m_rate[jobIdx] *= m_field30，
//      上限 0xf4240。
//   5) 数量：itemIdx==0 && m_rate[10]==0 → count=m_rate[groupIdx+4]
//      （ORIG 0x8349ab9 基址 drop+0x14）；否则循环 m_rate[groupIdx+4] 次，
//      每次 ref.m_field24 直接过、或 m_rate[jobIdx] > get_rand_int(1000000)
//      计一次命中；count = min(hits, m_rate[9])。
//   6) m_rate[10]!=0：过滤通过后循环 count 次：r=get_rand_int(m_rate[10])，
//      取 m_pairList 中第一个 second>r 的 first 写 item.m_addInfo；
//      create_item(TYPE_1,item,1) 后 push_back。
//      m_rate[10]==0：过滤通过后 item.m_addInfo=m_itemIdx，create_item 一次；
//      若 m_itemIdx==0 则 item.m_addInfo2=count 后入一个，否则入 count 个。
#include "MonsterIndependentDrop/generateIndependentItem.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0834a13a：lea 0x24(%eax) 取容器后直接转发自由函数。
#include "MonsterIndependentDrop/generateIndependentItem_1.inc"
}

// ----------------------------------------------------------------------------
// ORIG 0x0834f876：call CMonster::getMonsterScript(this)
// （_ZNK8CMonster16getMonsterScriptEv @0x0830e6c6），lea 0x3ac(%eax) 取脚本内
// 独立掉落容器（STMonsterScript::m_independentDrops）后转发自由函数。
#include "MonsterIndependentDrop/generateIndependentItem_2.inc"
}

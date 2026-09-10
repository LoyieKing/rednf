// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CItemAmplifier/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CItemAmplifier 还原（2026-08-16）。
// 构造/checkInvestableItem/generateAmplifyItem 依 ORIG 反汇编；
// stAmplifyItemScript_t / upgrade_table_t 构造为数据表占位（后续批次细化）。

#include "CItemAmplifier.h"

#include <algorithm>
#include <string.h>

#include "CItemList.h"

// ===================== 数据表占位构造（ORIG 0x82aa510 / 0x89000ca） =====================

// ORIG 0x0822aea6：+0x00 m_amount / +0x04 m_value 均清 0。
// ----------------------------------------------------------------------------
// [b28d-E03 2026-09-10 本波复核] 四数 92/193/2/2 不变（4 件均封顶，无源码旋钮）：
//   · upgrade_table_t::C1 200=200 DIFF：PURE SHIFT c=0（槽直方图全等）⇒ 残差 = 装载序
//     + P30 常量拆分（ORIG `mov idx,%edx; mov this,%eax; lea 0x1c(%edx),%ecx` + disp 0x4
//     vs 我方 `mov idx,%eax; lea 0x1a(%eax),%ecx; mov this,%eax` + disp 0xc，**同址等价**）。
//   · _generateOption 帧 72 vs 56（MIXED）：N82 具名 float 槽序（三形穷举不可达）。
//   · getInvestMaterialInfo 60=60 NEAR：PURE SHIFT c=0（寄存器/装载序）。
//   · get_upgrade_info 371=371 DIFF：PURE SHIFT c=0。
#include "CItemAmplifier/stInvestAmplifyOption_t.inc"
}

#include "CItemAmplifier/stAmplifyItemScript_t.inc"
}

// ORIG 0x089000ca：按偏移初始化概率/等级混合表。
// [b28d-solo-CItemAmplifier] verdict=DIFF（200/199，归一 diff 47）：语义已逐条
//   对齐（含修 2 处真 bug：+0x74/+0x274 两处 1.0f 写入的 4 字节偏移 + 补
//   m_machineFrames.clear()）。残差全为 P30/N4 族「索引寄存器 + add-vs-lea」：
//   ORIG `mov idx,%edx; mov this,%eax; add $K,%edx` vs 4.4 `mov idx,%eax;
//   lea K(%eax),%ecx; mov this,%eax`（7 个初始化循环 × 2-3 条；穷举 lea/add/
//   常量拆分均不齐）⇒ 结构性封顶，无源码旋钮。
// [b28d-solo-CItemAmplifier2] P238 修复 i7 权重区：+0x240 是 float 字段
//   （m_additionalWeight），ORIG 0x089002ba/0x089002bf 发 `mov $0x0,%edx;
//   mov %edx,(%eax,%ecx,4)` 中转形；旧版两句都走 I32 视图 ⇒ 第一句被 4.4 折成
//   `movl $0x0,0xc(%eax,%edx,4)` 直写（少 1 条）。改 F32 视图 + 0.0f 后
//   gap 1→0（orig/our 208/208），TU 四数 92/193/2/2 零变差；p167 调用点
//   14=14 CLEAR（MISSING/EXTRA 仅 C1↔C2、D1↔D2 同址别名伪差，P93）。
#include "CItemAmplifier/upgrade_table_t.inc"
}

// ===================== CItemAmplifier =====================

#include "CItemAmplifier/CItemAmplifier.inc"
}

// [b28d-solo-CItemAmplifier] verdict=NEAR@4.4.7（139/139）/ **AE@c6446r**
//   （compare_tu 官方口径：identical=92 ae=193 near=2 diff=2；本件与
//   generateAmplifyItem 两件由 NEAR→AE，零回归）。残差仅 N7：4.4.7 间接 call
//   目标恒 %eax 而 ORIG/4.4.6 用 %edx（3 处 GetItemType 虚调 + 1 处
//   isAmplifyPollutionItem）。路由需求已附净效果证据上报 Main。
//   本轮真语义修复：rarity 守卫极性反转（ORIG 0x08234a19 物化 `rarity==0||
//   rarity==1` → return false；旧源码 `!=0 && !=1` 判反）+ flag 分支合并 &&
//   （ORIG bool 物化且 flag==0 也走 getUsableLevel 臂）+ grade/rarity/base 去具名
//   局部（P0）。
#include "CItemAmplifier/checkInvestableItem.inc"
}

// [b28d-solo-CItemAmplifier] verdict=NEAR@4.4.7（55/55）/ **AE@c6446r**。
//   残差仅 N7（isAmplifyPollutionItem 间接 call 目标 %eax vs %edx）；
//   P63 正向包裹已消 ORIG 不存在的 2 条 jmp 中继。
#include "CItemAmplifier/generateAmplifyItem.inc"
}

// [b28d-solo-CItemAmplifier] verdict=NEAR（115/115，归一 diff 40）：N82 族封顶。
//   4.4 把具名 float 局部放最浅槽（-0xc/头部声明 -0x14）而 ORIG 放最深 -0x24，
//   连带 it/end/temp/fpucw 槽组 -4..-0x10 漂移、帧 0x48 vs 0x38；三形实测
//   （if 内声明 -0xc/48、头部声明 -0x14/40、头部带初始化 -0x14/42）均不可达
//   ORIG 槽位。已修：outOfRange 去具名 bool（P0）、rand2 移到 begin/end 之后
//   （ORIG 0x08515031）、m_type 走同宽 reinterpret 免 int→enum 中转。
#include "CItemAmplifier/_generateOption.inc"
}


// ===================== 净化材料查询（ORIG W 0x08234d20 / 0x08234cd4 / 0x08234cfa） =====================
// 两行净化材料在 stAmplifyItemScript_t 中按 int[2] 行布局（ORIG 表查找即
// 类内 int 数组寻址 arr[type]，gcc 拆 idx+0x24 + disp8 0xc/0x4 形）：
//   type 0 = m_purifyMatItem[0](+0x9c)/m_purifyMatCount[0](+0xa4)（[purify material]）
//   type 1 = m_purifyMatItem[1](+0xa0)/m_purifyMatCount[1](+0xa8)（[purify only material]）

// ORIG 0x08234d20：i∈[0,1]，表项 == itemIdx 则返回 i；否则 -1。
#include "CItemAmplifier/getPurifyMaterialType.inc"
}

// ORIG 0x08234cd4：type∈[0,1] → *(int*)(amp + 0x9c + type*4)；越界返回 0。
#include "CItemAmplifier/getPurifyMaterialItemIdx.inc"
}

// ORIG 0x08234cfa：type∈[0,1] → *(int*)(amp + 0xa4 + type*4)；越界返回 0。
#include "CItemAmplifier/getPurifyMaterialItemCount.inc"
}

// ===================== 强化投资（ORIG 0x08234d5e / 0x08234c3c / 0x08234b30） =====================

// ORIG 0x08234d5e：按 type 索引 m_script.m_investMap[type]，查找 level。
// 命中则把 value.{m_amount,m_value} 写入 info，返回 1；未命中返回 0。
// [b28d-20] 返回类型 int→bool：ORIG dispatch_sig 0x08219cd4 为 xor $1（bool callee）；
// 返回类型不入 mangling，无 sizeof/偏移/vtable 变化。
// [b28d-solo-CItemAmplifier] verdict=NEAR（60/60，归一 diff 4）：N98 封顶。
//   ORIG 两个 hash_map 迭代器 it(-0x1c)/last(-0x14) 为「声明序=先声明深」，
//   4.4 按首次使用序「先用浅」⇒ 仅两处 end() 的 lea 槽号互换；两序互试
//   （it 先/8 行差、last 先/4 行差）已取优。
#include "CItemAmplifier/getInvestMaterialInfo.inc"
}

// ORIG 0x08234c3c：返回 ENUM_ERROR 码（0 = 允许）。
//   0:  item 无能力且未强化 → 0；有强化 → 0x12；有(已识别)能力 → 0x14
//   1:  有能力 → (有强化 → 0x12；否则 0)；无能力 → 0x15
//   2:  恒 0
//   其他（>2）: 恒 1
#include "CItemAmplifier/enableInvestAmplifyOption.inc"
}

// ORIG 0x08234b30：在 m_script.m_options 中匹配 option.m_amount 类型的选项
// （stAmplifyOptionInfo_t::m_type == option.m_amount；option.m_value 不参与，
// 命中项自身的 +0x06 m_value 才是基数），命中则按 rarity 缩放并写
// inven.m_amp._setAbility(type, value, identified=true)。
#include "CItemAmplifier/investAmplifyOption.inc"
}

// ============================================================================
// upgrade_table_t::get_upgrade_info @ 0x8900436
//   按 rarity/growthGrade/upgrade/equipType 计算 upgrade_info 到 out。
//   返回 0=越界/失败, 1=成功。
// ============================================================================

// [b28d-solo-CItemAmplifier] verdict=DIFF（371/371，归一 diff 122）：语义已逐条
//   对齐（含 3 处真语义修复，见函数内注释）。残差 = ①P30/N4「索引寄存器 +
//   add-vs-lea」（ORIG `movzbl rarity,%edx; add $0x20,%edx` vs 4.4 `movzbl
//   rarity,%eax; lea 0x20(%eax),%edx`，16 行缩放 + 2 行折扣共 20 处）；
//   ②N1 常量折叠（ORIG `flds -100.0f; faddp` vs 4.4 `flds +100.0f; fsubrp`，
//   2 处）。已修形：dword 下标视图（P30）、gain 逐行重读（P0/P183）、bias 用
//   函数内 static（非 const，复现 _ZZ…E9pointFive/E3two 的 d 段符号与 x87 实乘）、
//   (int)(long long) 复现 fistpll、dead `int maxUpgradeLevel=0x12` 复现 ORIG
//   0x089007f9 死槽并使帧 0x6c/全部槽位对齐、out[] 走 upgrade_info_t 新增
//   float 视角（消 lea/add 取址）。
#include "CItemAmplifier/get_upgrade_info.inc"
}

//   返回 3 或 upgrade_info +0x24 惩罚类型字段（importUpgradeTable 的
//   infoInt(info,0x24) 脚本导入值；ORIG 反汇编 mov 0x24(%eax),%eax 实证，
//   非 m_discountBase(+0x34)）。
// ============================================================================
#include "CItemAmplifier/GetPenaltyType.inc"
}

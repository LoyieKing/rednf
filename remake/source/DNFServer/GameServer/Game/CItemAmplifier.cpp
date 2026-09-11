// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemAmplifier/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// ----------------------------------------------------------------------------
// [L3-b 2026-09-11 本波核账] 四数 92/193/2/2 不变，落地 1 项**真语义 bug**：
//   · get_upgrade_info 两处折扣缩放：ORIG 0x08900780 段为 `fildl 字段; flds 表系数;
//     fmulp; flds 0x8e122e8; faddp`，0x089007c7 段同形（0x8e122e8 = .rodata 常量池
//     `00 00 00 3f` = **0.5f**）⇒ 源码是 `积 + 0.5f`（fistpll 截断前的就近舍入偏置），
//     旧写法 `-100.0f + 积` 既错常量又错方向（我方 .o 实证发 `flds 100.0f;
//     fsubrp %st,%st(1)`，而 GNU as AT&T 下 `fsubrp %st,%st(1)` ≡ ST(1)−ST(0)，
//     c6root g++4.4.7 探针 `-100.0f + y` 生成同形可证）⇒ 已改 `积 + 0.5f`。
//     归一 diff 136→132 行、our 371 不变（gap 0 保持），四数零变差。
//   · 残差（N 族，穷举记录不变）：C1 我方多 1 条 `_Unwind_Resume`（EH 表族，24 vs 25
//     调用点，指令总数 200=200）+ 7 组 `mov idx,%edx; mov this,%eax; add $K,%edx` vs
//     4.4 `mov idx,%eax; lea K-2(%eax),%ecx; mov this,%eax`（同址等价）；
//     _generateOption/getInvestMaterialInfo 的槽序整体浅/深互换（两序互试已存档）。
// ----------------------------------------------------------------------------
#include "CItemAmplifier/stInvestAmplifyOption_t.inc"
{
    m_amount = 0;
    m_value = 0;
}

#include "CItemAmplifier/stAmplifyItemScript_t.inc"
{
    // ORIG 0x82aa510：仅构造 map/vector/hash_map 容器，数据区不初始化
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
    : m_machineFrames(0xa)   // ORIG C1 0x089000ca：_Hashtable 桶数 0xa
{
    // [b28d-solo-CItemAmplifier] 7 个循环各有**独立计数器**：ORIG 槽 -0x24..-0xc
    // 七个 int（Ghidra C 的 local_28..local_10），且 P212 全头部声明序=先声明深。
    // 旧版共用一个 i 使帧 0x30（ORIG 0x40）且计数器槽错位。
    int i1, i2, i3, i4, i5, i6, i7;
    // 初始化循环一律用 **dword 下标** 形（UPG_TBL_F32/I32 + `i + K`），复现 ORIG
    // 0x089001xx..0x089003a0 的 `mov %edx,%eax? ; add $K,%edx ;
    // movl $imm,disp(%eax,%edx,4)` 两步寻址（P30/N81：字节偏移表达式会被 4.4
    // 折成 lea/shl/add 链，实测本函数因此多 41 insn）。K 值 = (绝对偏移-0xc)/4。
    for (i1 = 0; i1 <= 3; ++i1)           // +0x0c..：1.0f  ⇒ k = i + 0x3
        UPG_TBL_F32(this, i1) = 1.0f;
    for (i2 = 0; i2 <= 0x15; ++i2)        // +0x1c..：1.0f  ⇒ k = i + 0x7
        UPG_TBL_F32(this, i2 + 0x4) = 1.0f;
    for (i3 = 0; i3 <= 5; ++i3)
    {
        // [b28d-solo-CItemAmplifier] 语义修复：ORIG 0x089001bb `lea 0x1c(%edx),%ecx
        // + mov %edx,0x4(%eax,%ecx,4)` = this+0x74+4i；旧 k=i+0x1b 落 +0x78+4i，
        // 漏写 +0x74 且误写 +0x8c（与第二句重叠）。
        UPG_TBL_F32(this, i3 + 0x1a) = 1.0f;   // +0x74..
        UPG_TBL_F32(this, i3 + 0x20) = 1.0f;   // +0x8c..
    }
    for (i4 = 0; i4 <= 0x63; ++i4)        // +0xa4..：整数 1
        UPG_TBL_I32(this, i4 + 0x26) = 1;
    UPG_TBL_I32(this, 0x8a) = 0;              // +0x234 公告阈值
    // [b28d-solo-CItemAmplifier] 语义修复：ORIG 0x0890022d `lea 0x9c(%edx),%ecx
    // + mov %edx,0x4(%eax,%ecx,4)` = this+0x274+4i；旧 k=i+0x9b 落 +0x278+4i。
    for (i5 = 0; i5 <= 0x63; ++i5)        // +0x274..：1.0f
        UPG_TBL_F32(this, i5 + 0x9a) = 1.0f;
    for (i6 = 0; i6 <= 5; ++i6)
    {
        UPG_TBL_I32(this, i6 + 0xfe) = -1;    // +0x404..
        UPG_TBL_I32(this, i6 + 0x104) = -1;   // +0x41c..
    }
    UPG_TBL_I32(this, 0x8b) = -1;             // +0x238 附加分解物 item
    UPG_TBL_I32(this, 0x8c) = 0;              // +0x23c 等级差阈值
    for (i7 = 0; i7 <= 5; ++i7)
    {
        // [b28d-solo-CItemAmplifier2] P238：ORIG 0x089002ba/0x089002bf 对 +0x240
        // 权重区发 `mov $0x0,%edx; mov %edx,(%eax,%ecx,4)`（edx 中转），而紧随的
        // +0x25c 基准等级区是直写 `movl $0x0,0xc(%eax,%edx,4)`。两区字段真型不同
        // （float 权重 / int 基准），故权重区走 float 视图 + 0.0f。
        UPG_TBL_F32(this, i7 + 0x8d) = 0.0f;   // +0x240.. 权重区（float）
        UPG_TBL_I32(this, i7 + 0x94) = 0;      // +0x25c.. 基准等级区（int）
    }
    UPG_TBL_I32(this, 0x93) = 0;              // +0x258 强化等级下限
    m_floatMap.clear();
    // [b28d-solo-CItemAmplifier] ORIG 0x0890030d `lea 0x4c4(%eax),%eax; call
    // _Hashtable::clear` 实证：构造后**照抄**清空 m_machineFrames（旧版漏此调用，
    // gap=5 主因）。
    m_machineFrames.clear();
    // 逐条直写：ORIG 0x089003a0 域是 4 条 `movl $0,0x4b4+4i(%eax)`，走 memset()
    // 会多出 ORIG 不存在的 memset 符号（P183 反向：库调用回退为物化）。
    m_zero4b4[0] = 0;
    m_zero4b4[1] = 0;
    m_zero4b4[2] = 0;
    m_zero4b4[3] = 0;
}

// ===================== CItemAmplifier =====================

#include "CItemAmplifier/CItemAmplifier.inc"
{
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
                                         const Inven_Item& inven,
                                         int flag) const
{
    if (item->is_stackable())
    {
        return false;
    }

    // [b28d-34] P282：vptr+0xc 槽 = _ZTV5CItem+0x14 = word[5] = CItem::GetItemType()
    // （CItem.h:45 纯虚；基类 _ZTV5CItem word[5] 是 __cxa_pure_virtual 占位，真身在
    // 派生类槽：_ZTV14CStackableItem word[5]=0x08514a84=CStackableItem::GetItemType）。
    // ORIG 三处 `mov (%eax),%eax; add $0xc; mov (%eax),%edx; call *%edx`
    // （0x082349a5/0x082349bc/0x082349ed）；旧函数指针形把目标缓存进栈槽并多 2 insn。
    // [b28d-solo-CItemAmplifier2] 收口 P282 复核：我方 .o 三处立即数同为 0xc，
    // generateAmplifyItem 一处同为 0x58（=isAmplifyPollutionItem）⇒ 槽号全对齐。
    // [b28d-34] 修后 138/139，残差 N7（间接 call 目标 %eax vs %edx）+ 帧 0x24/0x34。
    if (item->GetItemType() <= 9 || item->GetItemType() > 0x15)
    {
        return false;
    }
    if (item->GetItemType() == 0xb)
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] 语义反转修复：ORIG 0x08234a19 `test%eax; je
    // 8234a2d`，8234a2d = `mov $0x1` 岛 ⇒ 物化的是 **rarity==0 || rarity==1**
    // （两臂各自重调 get_rarity），命中即 return false；旧源码写成
    // `!=0 && !=1` 恰把可强化稀有度判反（0/1 放行、≥2 拒绝）。
    if (item->get_rarity() == 0 || item->get_rarity() == 1)
    {
        return false;
    }

    CEquipItem* equip = (CEquipItem*)item;
    if (!equip)
    {
        return false;
    }
    if ((int)inven.m_fieldb < equip->get_endurance())
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] ORIG 0x08234980 实证：`cmpl $0,flag; je <bool 物化>`
    // 后 mov$1/mov$0 + test ⇒ 源码是**合并 && 条件**（flag==0 也落 getUsableLevel
    // 臂），旧嵌套 if 形既少一条语义检查又无 bool 物化段（P214①/P133）。
    if (flag != 0 && equip->isExistUpgradableLevelTag())
    {
        if (flag <= 0x36)
        {
            return false;
        }
    }
    else if (item->getUsableLevel() <= 0x36)
    {
        return false;
    }

    // [b28d-solo-CItemAmplifier] P0：ORIG grade 在 %ebx、rarity 在 %edx、base 在
    // %eax（均无槽往返）⇒ 原码无这三个具名局部，表达式内联；仅 diff 占 -0x14。
    // 表址形 `add $0xd4,%edx; mov 0xc(%eax,%edx,4)` = this+0x100(m_upgrade)+0x25c。
    int diff = item->get_grade() -
               m_upgrade.m_bodyView.m_named.m_baseGrade[item->get_rarity()];
    // 钳零保持 std::max 形：ORIG 0x08234b0a 实参序 `lea -0x14(diff)→0x4(%esp);
    // lea -0x10(0)→(%esp)` ⇒ 原码是 std::max(**0, diff**)（arg0 在 (%esp)）。
    return std::max(0, diff) >= m_script.m_field3c;
}

// [b28d-solo-CItemAmplifier] verdict=NEAR@4.4.7（55/55）/ **AE@c6446r**。
//   残差仅 N7（isAmplifyPollutionItem 间接 call 目标 %eax vs %edx）；
//   P63 正向包裹已消 ORIG 不存在的 2 条 jmp 中继。
#include "CItemAmplifier/generateAmplifyItem.inc"
                                         Inven_Item& inven) const
{
    ENUM_AMPLIFY_TYPE type = ENUM_AMPLIFY_TYPE_NONE;
    unsigned short value = 0;

    _generateOption(item, type, value);
    // [b28d-solo-CItemAmplifier] P63：ORIG 0x08514ef2/0x08514efb 两个守卫
    // **同跳一个**出口 0x08514f6e（leave），且主流程尾块直落 epilogue。旧形
    // `if(...) return;` 让 4.4 给每个早退各发一组 `nop; jmp` 中继（+2 jmp）。
    // 正向包裹后守卫直接 je epilogue、内层 else 直落。
    if (type != ENUM_AMPLIFY_TYPE_NONE && value != 0)
    {
        // [b28d-34] P282 槽序还原：ORIG 0x08514f02 `add $0x58,%eax; mov (%eax),%edx;
        // call *%edx` 的 vptr+0x58 槽 = _ZTV5CItem+0x60 word[24] = CItem::
        // isAmplifyPollutionItem() const（0x085136ac，nm 实证）。ORIG 后随
        // `xor $1; test %al; je` ⇒ 源形为负极性 `if (!item->isAmplifyPollutionItem())`
        // （fn()==false 走 true 臂，0x8514f16 第三参=1；fn()==true 落 0x8514f43 第三参=0）。
        if (!item->isAmplifyPollutionItem())
        {
            inven.m_amp._setAbility((unsigned char)type, value, true);
        }
        else
        {
            inven.m_amp._setAbility((unsigned char)type, value, false);
        }
    }
}

// [b28d-solo-CItemAmplifier] verdict=NEAR（115/115，归一 diff 40）：N82 族封顶。
//   4.4 把具名 float 局部放最浅槽（-0xc/头部声明 -0x14）而 ORIG 放最深 -0x24，
//   连带 it/end/temp/fpucw 槽组 -4..-0x10 漂移、帧 0x48 vs 0x38；三形实测
//   （if 内声明 -0xc/48、头部声明 -0x14/40、头部带初始化 -0x14/42）均不可达
//   ORIG 槽位。已修：outOfRange 去具名 bool（P0）、rand2 移到 begin/end 之后
//   （ORIG 0x08515031）、m_type 走同宽 reinterpret 免 int→enum 中转。
#include "CItemAmplifier/_generateOption.inc"
                                     ENUM_AMPLIFY_TYPE& type,
                                     unsigned short& value) const
{
    float baseValue;   // [b28d-solo-CItemAmplifier] 头部先声明（ORIG 槽 -0x24 最深）
    // ORIG 0x08514f76..0x08514fa3：范围守卫**两次独立 get_rarity**（0x08514f7c
    // 与 0x08514f8b）+ `mov$1/mov$0` 汇聚岛 + `test%al; je`，且 bool 值**无槽
    // 往返**（P0）⇒ 原码是表达式右值（`||` 前端物化），非具名 bool 局部；
    // 旧 `const bool outOfRange` 形多 `mov %al,-0x15; cmpb` 两步 + 帧 +0x10。
    if (item->get_rarity() < 0 || item->get_rarity() > 5)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    int rand = get_rand_int(0x2710);
    if (m_script.m_rarityProb[item->get_rarity()] <= rand)
    {
        type = ENUM_AMPLIFY_TYPE_NONE;
        value = 0;
        return;
    }

    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    // ORIG 0x08515031：rand2 在 begin()/end() **之后**求值（槽 -0xc 浅于 it/end）。
    int rand2 = get_rand_int(0x2710);
    while (it != end)
    {
        if (it->m_prob > rand2)
        {
            // [b28d-solo-CItemAmplifier] ORIG 0x0851506c `mov (%eax),%edx` 单段
            // 直载：m_type 为 int 字段，`(ENUM_AMPLIFY_TYPE)it->m_type` 转换在
            // 4.4 多一条 `mov %eax,%edx`（P254 反向）；改用同宽 reinterpret 视图
            // 免转换（m_type 类型不动，investAmplifyOption 的 int 比较段不受影响）。
            type = reinterpret_cast<const ENUM_AMPLIFY_TYPE&>(it->m_type);
            // ORIG 0x0851508x：先 `fildl; fstps -0x24` 把 m_value 落成 float 槽，
            // 再**第三次** get_rarity → `flds m_rarityRate[r]`（被乘数在前）→
            // `fmuls -0x24` → 控制字 ROUND → `fistpl`。故 rarityRate 作左操作数、
            // m_value 走具名 float 局部（P176 ROUND 辨伪 + P183 重物化）。
            baseValue = (float)it->m_value;
            value = (unsigned short)(
                m_script.m_rarityRate[item->get_rarity()] * baseValue);
            return;
        }
        ++it;
    }
}


// ===================== 净化材料查询（ORIG W 0x08234d20 / 0x08234cd4 / 0x08234cfa） =====================
// 两行净化材料在 stAmplifyItemScript_t 中按 int[2] 行布局（ORIG 表查找即
// 类内 int 数组寻址 arr[type]，gcc 拆 idx+0x24 + disp8 0xc/0x4 形）：
//   type 0 = m_purifyMatItem[0](+0x9c)/m_purifyMatCount[0](+0xa4)（[purify material]）
//   type 1 = m_purifyMatItem[1](+0xa0)/m_purifyMatCount[1](+0xa8)（[purify only material]）

// ORIG 0x08234d20：i∈[0,1]，表项 == itemIdx 则返回 i；否则 -1。
#include "CItemAmplifier/getPurifyMaterialType.inc"
{
    // [b28d-34] AE（22=22）：字段 unsigned int + 表值在左翻正比较段（P274/P211）。
    for (int i = 0; i <= 1; ++i)
    {
        // [b28d-34] 字段 unsigned + 表值在左 ⇒ ORIG 两段物化（`mov 表项,%edx;
        // mov 形参,%eax; cmp %eax,%edx`）；探针 /tmp/p34e.cpp f4 逐条命中。
        if (m_script.m_purifyMatItem[i] == itemIdx)  // +0x9c 表查找
            return i;
    }
    return -1;
}

// ORIG 0x08234cd4：type∈[0,1] → *(int*)(amp + 0x9c + type*4)；越界返回 0。
#include "CItemAmplifier/getPurifyMaterialItemIdx.inc"
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatItem[type];
    }
    return 0;
}

// ORIG 0x08234cfa：type∈[0,1] → *(int*)(amp + 0xa4 + type*4)；越界返回 0。
#include "CItemAmplifier/getPurifyMaterialItemCount.inc"
{
    if (type >= 0 && type <= 1)
    {
        return m_script.m_purifyMatCount[type];
    }
    return 0;
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
// [W15ANearHunt1 2026-09-11 收尾复核] routed 复编（10:27）n1_v=NEAR 60/60 gap+0
//   ndiff2，残差**仅**两处 end() 的 sret 槽互换：ORIG 0x08234d7f `lea
//   -0x1c(%ebp),%eax`（首次 end）/ 0x08234d94 `lea -0x14(%ebp),%eax`（二次 end），
//   我方顺序相反；0x08234dd1/0x08234dd8 的比较用 lea 随动。4.4 按「首次使用序」
//   先浅(-0x14)后深(-0x1c)，4.1 按「声明序」先深后浅 ⇒ 交换声明序两向均不可翻转
//   （本批 W13G `CalculateExpertJobMaterials`、W13A `get_limit_cube_item` 同族件
//   独立复现同一结论）⇒ 维持 N98 封顶，未改动代码。
#include "CItemAmplifier/getInvestMaterialInfo.inc"
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type,
    unsigned long level,
    stInvestAmplifyOption_t& info) const
{
    // ORIG 0x08234d64..0x08234e1b 逐条转写（无 md 报告，二进制切片）：
    //   ① 表址 = this + ((type*4+type)*4 + 0xa0) + 0xc（无边界检查，P140）；
    //   ② hash_map::end() **两次**（-0x1c 先作 it 初值、-0x14 作 last），
    //      find 结果以 POD 双 dword 覆盖 -0x1c（4.1 迭代器赋值发形）；
    // [b28d-34] N98 封顶（ndiff 8→2 NEAR）：4.1 槽序=声明序「先声明深」
    // （it=-0x1c/last=-0x14），4.4 按首次使用序「先用浅」且不可由声明序翻
    // （两序互试实测）。交换声明序（last 先）后仅剩两处 end() 的 lea 槽号
    // 互换（-0x14↔-0x1c），find 结果槽与后段逐条全等，语义等价。
    //   ③ it == last → 0；
    //   ④ 命中段**逐成员**赋值：两次 _Hashtable_const_iterator::operator->
    //      分别取 pair+4 / pair+8（⇒ `info = it->second` 整体拷贝会丢一次
    //      operator-> 并改走结构复制路径）。
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>&
        table = m_script.m_investMap[(int)type];
    const __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator
        last = table.end();
    __gnu_cxx::hash_map<unsigned long, stInvestAmplifyOption_t>::const_iterator it =
        table.end();
    it = table.find(level);
    if (it == last)
    {
        return 0;
    }
    info.m_amount = it->second.m_amount;
    info.m_value = it->second.m_value;
    return 1;
}

// ORIG 0x08234c3c：返回 ENUM_ERROR 码（0 = 允许）。
//   0:  item 无能力且未强化 → 0；有强化 → 0x12；有(已识别)能力 → 0x14
//   1:  有能力 → (有强化 → 0x12；否则 0)；无能力 → 0x15
//   2:  恒 0
//   其他（>2）: 恒 1
#include "CItemAmplifier/enableInvestAmplifyOption.inc"
    const Inven_Item& inven,
    ENUM_ITEM_AMPLIFY_INVEST_TYPE type) const
{
    switch (type)
    {
    case ENUM_ITEM_AMPLIFY_INVEST_TYPE_2:
        break;
    case ENUM_ITEM_AMPLIFY_INVEST_NONE:
        if (inven.m_amp.hasAbility())
        {
            return 0x14;
        }
        if (inven.GetUpgrade())
        {
            return 0x12;
        }
        break;
    case ENUM_ITEM_AMPLIFY_INVEST_TYPE_1:
        if (!inven.m_amp.hasAbility())
        {
            return 0x15;
        }
        if (inven.GetUpgrade())
        {
            return 0x12;
        }
        break;
    default:
        return 1;
    }
    return 0;
}

// ORIG 0x08234b30：在 m_script.m_options 中匹配 option.m_amount 类型的选项
// （stAmplifyOptionInfo_t::m_type == option.m_amount；option.m_value 不参与，
// 命中项自身的 +0x06 m_value 才是基数），命中则按 rarity 缩放并写
// inven.m_amp._setAbility(type, value, identified=true)。
#include "CItemAmplifier/investAmplifyOption.inc"
    const CItem* item,
    Inven_Item& inven,
    const stInvestAmplifyOption_t& option) const
{
    if (option.m_amount == 0)
    {
        return;
    }

    std::vector<stAmplifyOptionInfo_t>::const_iterator it =
        m_script.m_options.begin();
    std::vector<stAmplifyOptionInfo_t>::const_iterator end =
        m_script.m_options.end();
    for (; it != end; ++it)
    {
        // [b28c-16] P222 正向包裹：ORIG `cmp; sete; je <++it 岛>`（命中 fall-through），
        // 写 `!= continue` 4.4 反产 setne/je 命中块；type 不设局部（ORIG 直接
        // movzbl %al,%eax 内联进实参）。
        if (it->m_type == option.m_amount)
        {
            unsigned short value =
                (unsigned short)((float)it->m_value *
                                 m_script.m_rarityRate[item->get_rarity()]);
            inven.m_amp._setAbility((unsigned char)option.m_amount, value, true);
            return;
        }
    }
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
                                       unsigned char rarity,
                                       unsigned char growthGrade,
                                       unsigned char upgrade,
                                       ENUM_EQUIPMENTTYPE equipType) const
{
    // [b28d-solo-CItemAmplifier] ORIG 0x08900486/0x08900498 是两条**独立**守卫
    // （各自 `mov 0x1c(%ebp),%eax; cmp; jle/jg` 直落续判）；合并 `||` 会被 4.4
    // 折成反向块序（P133/P191），拆两条即同形。
    if (upgrade >= m_infos.size())
        return 0;
    if (rarity >= 6)
        return 0;
    if (equipType > 0x15)
        return 0;
    if (equipType < 10)
        return 0;
    if (growthGrade >= 100)
        return 0;

    // [b28d-solo-CItemAmplifier] ORIG 0x089004b7 的 `flds &_ZZ…E9pointFive;
    // flds &_ZZ…E3two; fmulp; fstps -0x20` 实证：bias 是**函数内 static float
    // 常量**的 x87 实乘（非编译期折叠常量），符号名即 pointFive/two。
    static float pointFive = 0.5f;
    static float two = 2.0f;
    const float bias = pointFive * two;
    // ORIG 0x089004xx：拷贝是 **27 个 dword 的块移动展开**（Ghidra
    // `for (iVar6 = 0x1b; ...; *pfVar7 = *pfVar5)`），不是 memcpy ⇒ 写结构赋值
    // 让编译器自行展开/发块拷（batch-14 访问器专项连带项）。
    *out = m_infos.at(upgrade);

    // [b28d-solo-CItemAmplifier] dword 下标视图（P30）：ORIG 每行
    // `movzbl rarity,%edx; mov this,%eax; add $0x20,%edx; flds 0xc(%eax,%edx,4)`
    // 为「索引+常数 + disp 0xc」两步形，gain **逐行重读**（无具名缓存，P0/P183）；
    // 分母 this+0xc..0x18 亦直读 disp。旧版缓存 gain + 字节偏移宏是 352/326 伪形。
    out->m_f00[0] = ((float)(int)growthGrade / UPG_TBL_F32(this, 0) + bias) *
                   out->m_f00[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[1] = ((float)(int)growthGrade / UPG_TBL_F32(this, 1) + bias) *
                   out->m_f00[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[2] = ((float)(int)growthGrade / UPG_TBL_F32(this, 2) + bias) *
                   out->m_f00[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[3] = ((float)(int)growthGrade / UPG_TBL_F32(this, 3) + bias) *
                   out->m_f00[3] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[4] = out->m_f00[4] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[5] = out->m_f00[5] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[6] = out->m_f00[6] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[7] = out->m_f00[7] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[0] = ((float)(int)growthGrade / UPG_TBL_F32(this, 0) + bias) *
                    out->m_f4c[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[1] = ((float)(int)growthGrade / UPG_TBL_F32(this, 1) + bias) *
                    out->m_f4c[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[2] = ((float)(int)growthGrade / UPG_TBL_F32(this, 2) + bias) *
                    out->m_f4c[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[3] = ((float)(int)growthGrade / UPG_TBL_F32(this, 3) + bias) *
                    out->m_f4c[3] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[0] = out->m_f5c[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[1] = out->m_f5c[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[2] = out->m_f5c[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[3] = out->m_f5c[3] * UPG_TBL_F32(this, rarity + 0x20);
    // [b28d-solo-CItemAmplifier] 语义修复（旧版整段缺失 + 键用错）：
    //   ① ORIG 0x0890076d `add $0x28,%edx; mov 0x4(%eax,%edx,4),%edx;
    //      mov %edx,0x34(%eax)` ⇒ 先把折扣基数**播种**为
    //      this[0xa4+4*growthGrade]（旧版直接读复制来的 out+0x34 值）；
    //   ② 两次缩放各从字段重读（无具名 disc 局部，P0）；转换目标是 long long
    //      （`fistpll` 实证），写 (int)(long long)(…)；
    //   ③ m_floatMap 键 = **clamp(upgrade,0x12)**（ORIG 0x08900800
    //      `cmpb $0x12,-0x44(%ebp)`，-0x44 是 upgrade 形参槽；旧版误用 equipType）。
    out->m_discountBase = UPG_TBL_I32(this, growthGrade + 0x26);
    // [L3-b 语义修复] ORIG 0x08900780 先 `fildl -0x48` 把字段转 float、再取表
    // 系数 `fmulp`、随后 `flds 0x8e122e8(=0.5f); faddp` ⇒ 源码是
    // `积 + 0.5f`（fistpll 的截断前就近舍入偏置），**不是** `-100.0f + 积`
    // （旧写法的 100.0f 常量与减法方向均错：ORIG 常量池此位为 0.5f，
    // 且 faddp 为加法）。同理第二处 0x089007da/0x089007e0。
    out->m_discountBase =
        (int)(long long)((float)out->m_discountBase *
                             UPG_TBL_F32(this, equipType + 4) +
                         0.5f);
    out->m_discountBase =
        (int)(long long)((float)out->m_discountBase *
                             UPG_TBL_F32(this, rarity + 0x1a) +
                         0.5f);
    // ORIG 0x089007f9 `movl $0x12,-0x1c(%ebp)` 是死槽（后续从不读）⇒ 照抄
    // 一个"声明+赋值"两段式局部（P171/P198①）。
    int maxUpgradeLevel;
    maxUpgradeLevel = 0x12;
    (void)maxUpgradeLevel;
    if (upgrade > 0x12)
        upgrade = 0x12;
    std::map<int, float>::const_iterator it = m_floatMap.find(upgrade);
    if (it != m_floatMap.end())
    {
        // ORIG 0x08900864 先 `fildl; fstps -0x5c` 把字段落 float 槽再 call
        // operator->（左操作数先求值），随后 `fmuls -0x5c` ⇒ 字段写在乘法左侧。
        out->m_discountBase = (int)(long long)((float)out->m_discountBase *
                                               it->second);
    }
    return 1;
}

//   返回 3 或 upgrade_info +0x24 惩罚类型字段（importUpgradeTable 的
//   infoInt(info,0x24) 脚本导入值；ORIG 反汇编 mov 0x24(%eax),%eax 实证，
//   非 m_discountBase(+0x34)）。
// ============================================================================
#include "CItemAmplifier/GetPenaltyType.inc"
                                    const upgrade_info_t* info) const
{
    // ORIG 两条件各自重读表项（无 th 局部），upgrade 物化 byte 后 movzbl；
    // 表 = +0x404 基 int 数组按 rarity 直接下标（两步 add/disp 形）。
    // 分支形为「反条件 return 3 前置」（pen2 实验：cmp;jl → then 后置）。
    if (m_penalty[rarity] != -1 && (int)upgrade >= m_penalty[rarity])
        return 3;
    return info->m_prob20[1];   // +0x24：ORIG `mov 0x24(%eax),%eax` 单条访存
}

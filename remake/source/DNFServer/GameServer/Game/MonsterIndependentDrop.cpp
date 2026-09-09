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
void independentItemRateControl(stMonsterIndependentDrop_t& drop,
                                const WongWork::stGenerateRefData_t& ref)
{
    std::vector<stIndependentDropRateControl>& ctrls =
        G_CDataManager()->m_independentDrop.m_vec18;

    const unsigned char jobIdx = (unsigned char)ref.m_rateSlot;    // movzbl 0x36
    const unsigned char base34 = (unsigned char)ref.m_field34;     // movzbl 0x34

    for (std::vector<stIndependentDropRateControl>::iterator it = ctrls.begin();
         it != ctrls.end(); ++it)
    {
        if (it->m_itemId != drop.m_itemIdx)                        // setne → next
            continue;
        if (drop.m_rate[jobIdx] == 0)                              // test → next
            continue;

        // ORIG 0x83497a5 `movzbl 0x3f(%eax),%eax` ⇒ +0x3f 独立字节视图（P196
        // 字段宽度；旧 `m_goldFactor>>8` 恒产 movzwl+shr）。
        const int diff = (int)ref.m_field3f - (int)base34;          // +0x3f（等级上界）
        // ORIG 0x83497c7 `cmp diff; setle %al; test; je <循环尾>` ⇒ 仅当
        // m_field04 <= diff 时**执行**衰减体（Ghidra C 同判：`if (field04 <= diff) {...}`）。
        // 旧源码写成 `<= diff` 时 continue ⇒ 条件反向真 bug（衰减作用到错误条目）。
        if (it->m_field04 > diff)
            continue;

        int value = 10000 - it->m_field08 * diff;                  // imul + 0x2710 sub
        float factor = (float)value;                               // fildl/fstps
        if (factor == 0.0f)                                        // fucompp fldz
            factor = 1.0f;
        // flds 0x8c349ec = 10000.0f；fdivrp/fmulp → rate*(factor/10000.0f)
        drop.m_rate[jobIdx] = (int)((float)drop.m_rate[jobIdx] *
                                    (factor / 10000.0f));          // fistpl 回写
    }
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
void generateIndependentItem(
    const std::vector<stMonsterIndependentDrop_t>& drops,
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out)
{
    const unsigned char jobIdx = (unsigned char)ref.m_rateSlot;     // movzbl 0x36
    const unsigned char chanByte = (unsigned char)ref.m_field34;    // movzbl 0x34
    // ORIG 0x83498bc `movzbl 0x3e(%eax),%eax` ⇒ +0x3e 独立字节视图（旧
    // `(unsigned char)m_goldFactor` 恒产 movzwl 0x3e，P196 字段宽度）。
    const unsigned char groupIdx = ref.m_field3e_byte;              // movzbl 0x3e

    // ORIG 0x83498d0/0x83498e5 在循环初始化区各求值一次 begin/end（P13 缓存
    // end，非每次迭代重调）。
    std::vector<stMonsterIndependentDrop_t>::const_iterator it = drops.begin();
    const std::vector<stMonsterIndependentDrop_t>::const_iterator endIter =
        drops.end();
    for (; it != endIter; ++it)
    {
        stMonsterIndependentDrop_t drop(*it);                       // copy ctor

        // ---- 1) 范围过滤 ----
        if (drop.m_rangeMin != 0 || drop.m_rangeMax != 0)
        {
            if (ref.m_field25)                                      // movzbl/test
            {
                if (ref.m_field2c < drop.m_rangeMin)                // jl → skip
                    continue;
                if (!(ref.m_field2c <= drop.m_rangeMax))            // jle 取反
                    continue;
            }
            else
            {
                if (drop.m_rangeMin > (int)chanByte)                // jg → skip
                    continue;
                if (drop.m_rangeMax < (int)chanByte)                // jl → skip
                    continue;
            }
        }

        // ---- 2) 掉率衰减控制（命中计数器先清零，ORIG -0x34 槽）----
        int hitCount = 0;
        independentItemRateControl(drop, ref);

        // ---- 3) premium 掉率加成 ----
        // ORIG 0x83499b6 `movzbl 0x3c(%eax),%eax` ⇒ +0x3c 独立字节视图（P196）。
        if (drop.m_rate[10] > 0 &&
            ref.m_premiumDropRate > 0 &&
            ref.m_field3c_byte == 3)
        {
            // imul + 0x51eb851f/sar5 有符号除 100，结果再经 fildl/fistpl 恒等
            // 往返（ORIG 0x83499f8/0x8349a1a）⇒ 源码含 (int)(float) 转换。
            drop.m_rate[jobIdx] = (int)(float)(
                drop.m_rate[jobIdx] * (ref.m_premiumDropRate + 100) / 100);
        }

        // ---- 4) 倍率放大并封顶 ----
        if (ref.m_field25 && ref.m_field30 > 1)
        {
            // ORIG 0x8349a49 / 0x8349a75 两次独立 imul（无 product 局部槽，P0：
            // 原码无具名局部 ⇒ 比较与赋值各写一遍完整表达式）；两侧均先装 rate
            // 再装 field30（0x8349a4d/0x8349a57）⇒ rate 为左操作数。
            if (drop.m_rate[jobIdx] * ref.m_field30 > 0xf423f)      // 1000000
                drop.m_rate[jobIdx] = 0xf4240;
            else
                drop.m_rate[jobIdx] = drop.m_rate[jobIdx] * ref.m_field30;
        }

        // ---- 5) 生成数量 ----
        int genCount = 0;
        if (drop.m_itemIdx == 0 && drop.m_rate[10] == 0)
        {
            // ORIG 0x8349aaf `movzbl g; sub $1; add $4; mov -0xc0(%ebp,%eax,4)`
            // ⇒ 绝对地址 drop+0x14+4g = m_rate[groupIdx+4]（m_rate 基址 +0x04）。
            // 旧 `groupIdx+3` 读 drop+0x10+4g，少 1 个元素 = 真语义 bug。
            genCount = drop.m_rate[groupIdx + 4];
        }
        else
        {
            // ORIG 0x8349ac8 仅发 j=0（hitCount 已在 0x834997b 调用
            // rateControl 前清零一次，else 分支不重复清零）。
            for (int j = 0; drop.m_rate[groupIdx + 4] > j; ++j)
            {
                if (ref.m_field24)                                  // movzbl/test
                    ++hitCount;
                else if (drop.m_rate[jobIdx] >
                         get_rand_int(0xf4240))                     // 1000000
                    ++hitCount;                                     // setg
            }
            genCount = std::min(hitCount, drop.m_rate[9]);          // _ZSt3minIi
        }

        // ---- 6) 物品生成 ----
        Inven_Item item;                                            // Inven_ItemC1Ev
        if (drop.m_rate[10] != 0)
        {
            if (ref.m_field25 &&
                drop.m_levelFilter >= 0 &&                          // js → 不过滤
                ref.m_field28 != drop.m_levelFilter)
                continue;

            for (int k = 0; k < genCount; ++k)
            {
                const int roll = get_rand_int(drop.m_rate[10]);
                // ORIG 0x8349bad 段：begin() 返回 iterator 经 const_iterator 转换
                // 构造（P96 翻案——ORIG 侧确有转换临时）；循环体用 operator*（deEv）
                // 而非 operator->（P167 新判据），并整对拷贝到 8 字节局部
                // -0x40/-0x3c（0x8349be7..0x8349bef）。
                for (std::vector<std::pair<int, int> >::const_iterator p =
                         drop.m_pairList.begin();
                     p != drop.m_pairList.end(); ++p)
                {
                    const std::pair<int, int> pr = *p;
                    if (pr.second > roll)
                    {
                        item.m_addInfo = pr.first;                  // +0x02 ← first
                        break;                                      // 未命中沿用旧值
                    }
                }
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_1, item, 1);
                out.push_back(item);
            }
            continue;
        }
        else
        {
            if (ref.m_field25 &&
                drop.m_levelFilter >= 0 &&
                ref.m_field28 != drop.m_levelFilter)
                continue;

            item.m_addInfo = drop.m_itemIdx;                        // +0x02 ← +0x00
            G_CDataManager()->m_itemList->create_item(
                ENUM_ITEM_CREATE_TYPE_1, item, 1);
            if (item.m_addInfo == 0 && drop.m_rate[10] == 0)
            {
                item.m_addInfo2 = genCount;                         // +0x07 ← count
                out.push_back(item);
            }
            else
            {
                for (int k = 0; k < genCount; ++k)
                    out.push_back(item);
            }
        }
        // 循环尾：局部 copy/item 析构由编译器生成（与 ORIG 8349d63 一致）。
    }
}

// ----------------------------------------------------------------------------
// ORIG 0x0834a13a：lea 0x24(%eax) 取容器后直接转发自由函数。
void CAICharacter::generateIndependentItem(
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out) const
{
    ::generateIndependentItem(m_independentDrops, ref, out);
}

// ----------------------------------------------------------------------------
// ORIG 0x0834f876：call CMonster::getMonsterScript(this)
// （_ZNK8CMonster16getMonsterScriptEv @0x0830e6c6），lea 0x3ac(%eax) 取脚本内
// 独立掉落容器（STMonsterScript::m_independentDrops）后转发自由函数。
void CMonster::generateIndependentItem(
    const WongWork::stGenerateRefData_t& ref,
    std::vector<Inven_Item>& out) const
{
    // ORIG 0x0834f876：`call getMonsterScript; lea 0x3ac(%eax)` 无局部中转。
    // static_cast 结果直接取容器内联传参（P0/技巧36），声明 script 局部多一
    // -0xc 槽令帧 0x28→0x18 失配。
    ::generateIndependentItem(
        static_cast<STMonsterScript*>(getMonsterScript())->m_independentDrops,
        ref, out);
}

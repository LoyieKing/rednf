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
//   diff = (u8)ref.m_field34 - (u8)ref.m_field34；若 ctrl.m_field04 > diff：
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

        const int diff = (int)((ref.m_goldFactor >> 8) & 0xff) - (int)base34;  // +0x3f（旧 m_levelHigh：等级上界）
        if (it->m_field04 <= diff)                                 // setle → skip
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
//   5) 数量：itemIdx==0 && m_rate[10]==0 → count=m_rate[groupIdx+3]
//      （ORIG 下标表达式 (g-1)+4）；否则循环 m_rate[groupIdx+3] 次，
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
    const unsigned char groupIdx = (unsigned char)ref.m_goldFactor;  // movzbl 0x3e

    for (std::vector<stMonsterIndependentDrop_t>::const_iterator it =
             drops.begin();
         it != drops.end(); ++it)
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
        if (drop.m_rate[10] > 0 &&
            ref.m_premiumDropRate > 0 &&
            (unsigned char)ref.m_goldBase == 3)
        {
            // imul + 0x51eb851f/sar5 有符号除 100（中间 fildl/fistpl 恒等往返）。
            drop.m_rate[jobIdx] =
                drop.m_rate[jobIdx] * (ref.m_premiumDropRate + 100) / 100;
        }

        // ---- 4) 倍率放大并封顶 ----
        if (ref.m_field25 && ref.m_field30 > 1)
        {
            const int product = ref.m_field30 * drop.m_rate[jobIdx];
            if (product > 0xf423f)                                  // 1000000
                drop.m_rate[jobIdx] = 0xf4240;
            else
                drop.m_rate[jobIdx] = product;
        }

        // ---- 5) 生成数量 ----
        int genCount = 0;
        if (drop.m_itemIdx == 0 && drop.m_rate[10] == 0)
        {
            genCount = drop.m_rate[groupIdx + 3];                   // (g-1)+4
        }
        else
        {
            hitCount = 0;
            for (int j = 0; drop.m_rate[groupIdx + 3] > j; ++j)
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
                for (std::vector<std::pair<int, int> >::iterator p =
                         drop.m_pairList.begin();
                     p != drop.m_pairList.end(); ++p)
                {
                    if (p->second > roll)
                    {
                        item.m_addInfo = p->first;                  // +0x02 ← first
                        break;                                      // 未命中沿用旧值
                    }
                }
                G_CDataManager()->getItemList()->create_item(
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
            G_CDataManager()->getItemList()->create_item(
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

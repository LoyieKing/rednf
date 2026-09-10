// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureA_User/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件：p167（getUserQuestRewardItem CLEAN、
//   gold MISSING=EXTRA=1 别名）⇒ 无语义缺口。逐件：
//   · CalculateExpertJobMaterials（NEAR）：交换 ci/mi 声明序试验四数不变
//     （31/40/2/5）⇒ 后置 ++ 的 sret 临时 -0x10/-0x14 相对序属 4.4 分配产物（N86），已回滚。
//   · compute_quest_basic_reward_exp：帧 0x44↔0x54 + 槽整体浅 4（N95/N115/P232）。
//   · compute_quest_basic_reward_gold：`xor $1; test; jne` ↔ ORIG `test; je` 直判
//     （P292/P133 族，否定早返物化方向）+ itemIdx 槽 -0xc↔-0x18。
//   · getUserQuestRewardItem / GetCurCharacInventoryRef：仅尾 nop 岛（P223 已试）与
//     .rodata addend 显示差 ⇒ AE 等价。

#include "CUser.h"
#include "Quest.h"
#include "CDataManager.h"      // G_CDataManager / m_questParameter(+0x8cc8)
#include "CInventory.h"
#include "CExpandEquipslot.h"
#include "CTitleBook.h"
#include "CUserCharacInfo.h"

// ---------------------------------------------------------------------------
// CUser::GetCurCharacInventoryRef（ORIG T 0x08680f2e 报告全文已读）
// switch 跳表区域编码：case0→GetInvenRef(1)、case1→GetInvenRef(2)、
// case2→GetCargoRef、case3→GetInvenRef(0)、case7→GetInvenRef(3)、
// case0x12→GetCharacExpandData(kServerItemExpansion=9).GetInvenRefW(5)、
// case0x13..0x16→GetCharacExpandData(kTitleBook=0xe).getTitleSlot(area-0x13)。
// ORIG 共享出口 +0x172；default/guard 均 mov $0 进同一出口。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/GetCurCharacInventoryRef.inc"
                                            unsigned short slot)
{
    // b28d-65：ORIG 0x08680cab `test;setne;test;je` = 正极性 `if (getCurCharacR())`
    // 物化形（P292 表）；负极性 `==0` 在 4.4 生 `sete`。改正向包裹、尾部统一 return 0。
    if (getCurCharacR())
    {
        switch (area)
        {
        case 0:
            return getCurCharacInvenW()->GetInvenRef(1, slot); // [0x868062 GetInvenRefEii]
        // b28d-65：ORIG 叶块排放序（P46）= 源码 case 书写序 0,2,3,7,1,0x12,0x13..0x16
        // （0x044 GetInvenRef1 / 0x076 GetCargoRef / 0x08c GetInvenRef0 / 0x0b4 GetInvenRef3 /
        //  0x0e6 GetInvenRef2 / 0x101.. / 0x141..）⇒ case 1 移到 case 7 之后。
        case 2:
            return getCurCharacCargoW()->GetCargoRef(slot);    // [0x8680fa4 GetCargoRefEi]
        case 3:
            return getCurCharacInvenW()->GetInvenRef(0, slot); // [0x8680fba]
        case 7:
            return getCurCharacInvenW()->GetInvenRef(3, slot); // [0x868100a]
        case 1:
            return getCurCharacInvenW()->GetInvenRef(2, slot); // [0x8680f9a]
        case 0x12:
        {
            // b28d-65 [P0]：ORIG 0x08680d99 段把 GetCharacExpandData 结果存槽 -0xc 再重载
            // （槽往返 ⇒ 原码有具名局部），链式写法会让 4.4 改把 slot 存 %ebx 跨调用。
            CExpandEquipslot* pExpand = GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
            return pExpand->GetInvenRefW(static_cast<INVEN_TYPE>(5), slot); // [0x868102f]
        }
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            return reinterpret_cast<CTitleBook*>(GetCharacExpandData(
                       static_cast<ENUM_CHARAC_EXPAND_TYPE>(14)))
                       ->getTitleSlot(
                           static_cast<ENUM_TITLE_BOOK_CATEGORY>(area - 0x13),
                           slot);                             // [0x8681062 ebx=area-0x13]
        default:
            return 0;                                          // [0x8681094 mov $0]
        }
    }
    return 0;
}

// ORIG T 0x08680c8a（const 重载）：guard/getter 全部走 const R 版
// （getCurCharacInvenR/getCurCharacCargoR/GetInvenRefR/GetCargoRef const/
// GetCharacExpandDataR/GetInvenRefR）——ORIG 无 enableSaveInven 副作用，
// 仅 CTitleBook::getTitleSlot 直调非 const 版（_ZN…E 实证）。
#include "LinkClosureA_User/GetCurCharacInventoryRef_1.inc"
                                                  unsigned short slot) const
{
    // b28d-65：与非 const 版同修（ORIG 0x08680c8a 正极性 setne 物化 + 叶块序
    // 0,2,3,7,1,0x12,0x13..0x16 + case 0x12 具名槽往返 + default 独立 return 0）。
    if (getCurCharacR())
    {
        switch (area)
        {
        case 0:
            return getCurCharacInvenR()->GetInvenRef(1, slot);
        case 2:
            return getCurCharacCargoR()->GetCargoRef(slot);
        case 3:
            return getCurCharacInvenR()->GetInvenRef(0, slot);
        case 7:
            return getCurCharacInvenR()->GetInvenRef(3, slot);
        case 1:
            return getCurCharacInvenR()->GetInvenRef(2, slot);
        case 0x12:
        {
            CExpandEquipslot* pExpand = GetCharacExpandDataR(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(9));
            return pExpand->GetInvenRefR(static_cast<INVEN_TYPE>(5), slot);
        }
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
            // ORIG 直调非 const getTitleSlot（_ZN…E）且用非 RE 重载
            // GetCharacExpandData(ENUM)（_ZNK5CUser19GetCharacExpandDataE23…）；
            // 经 const_cast 去 CV。
            return const_cast<CTitleBook*>(reinterpret_cast<const CTitleBook*>(
                       GetCharacExpandData(
                           static_cast<ENUM_CHARAC_EXPAND_TYPE>(14))))
                ->getTitleSlot(
                    static_cast<ENUM_TITLE_BOOK_CATEGORY>(area - 0x13), slot);
        default:
            return 0;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// CUser::UpdateRating（ORIG W 0x08568e9c 报告缺失；objdump 六条 call 序忠实）:
// enableSaveCharacStat(); SetRating(rating);
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/UpdateRating.inc"
{
    enableSaveCharacStat();                                // [0x8568ea8]
    SetRating(rating);                                     // [0x8568eba SetRatingEf]
}

// ---------------------------------------------------------------------------
// CUser::getUserQuestRewardItem（ORIG T 0x0866e914 报告全文已读）
// 字段全部位于 Quest：m_fieldDC(+0xdc vector<int>)/m_fieldE8(+0xe8)/
// m_rewardItems(+0xec)/m_titleReward(+0xc4)；其余探查经由第二参 const CUser*
// 的 getCurCharacQuestR()->isClearQuest(idx)。
//   mode==1：命中任一已清除 idx 即返回 &m_rewardItems；
//   否则：全部 idx 已清除才返回 &m_rewardItems；
//   空 idx 表或不满足一律返回 &m_titleReward。
// ---------------------------------------------------------------------------
// b28d-66 [P281]：CUser.h:3055 已 static（Main 代落）⇒ 实参 0x8/0xc(%ebp) 与 ORIG 一致。
//   本轮修正：ORIG 每个分支各自 `const_iterator it = quest->m_fieldDC.begin()/end()`
//   （const vector ⇒ 一次 call 直构，槽 else=-0xc/-0x10、then=-0x14/-0x18），无 const_cast
//   非 const begin 引起的 iterator→const_iterator 转换构造；else 分支为
//   `for(; it!=end; ++it) if(!isClearQuest(*it)) break;`（ORIG xor $1 证 break 形）。
#include "LinkClosureA_User/getUserQuestRewardItem.inc"
    const Quest* quest, const CUser* other)     // ORIG _ZN5CUser22…PKS_ 非 const
{
    // b28d-66 [P0]：ORIG 0x0866e914 全程直接重读形参 0x8/0xc(%ebp)（无槽往返）
    // ⇒ 原码无具名 `Quest* q` 局部，const_cast 仅用于返回值地址表达式。
    if (!quest->m_fieldDC.empty())                      // [0x866e91b xor $1]
    {
        // b28d-66：ORIG 布局 = `cmp $1; je <后置块>`（后置块槽 -0x14/-0x18）⇒ 原码
        // 条件取 `!= 1`（break-块 fall-through 槽 -0xc/-0x10），4.4 生同一极性。
        if (quest->m_fieldE8 != 1)                      // [cmp $1; je else(后置块)]
        {
            std::vector<int>::const_iterator it = quest->m_fieldDC.begin();  // [-0xc]
            std::vector<int>::const_iterator end = quest->m_fieldDC.end();   // [-0x10]
            for (; it != end; ++it)
            {
                if (!other->getCurCharacQuestR()->isClearQuest(*it))         // [xor $1]
                {
                    break;
                }
            }
            // b28d-66：ORIG 0x0866e9e3 为 `call eq; test; je +0x16f`（目标 = 空块
            // nop 中继岛，then 块尾另有 `jmp +0x170` 跳过该岛）——本形逐条匹配前 3 条，
            // 仅缺 4.1 的对齐 nop 岛+jmp（P223 试形产 `eq;xor;test;jne` 反劣，已回退）。
            if (it == end)                                 // [operator== P7]
            {
                return &const_cast<Quest*>(quest)->m_rewardItems;
            }
        }
        else
        {
            std::vector<int>::const_iterator it = quest->m_fieldDC.begin();  // [-0x14]
            std::vector<int>::const_iterator end = quest->m_fieldDC.end();   // [-0x18]
            for (; it != end; ++it)
            {
                if (other->getCurCharacQuestR()->isClearQuest(*it))
                {
                    return &const_cast<Quest*>(quest)->m_rewardItems;  // [lea +0xec]
                }
            }
        }
    }
    return &const_cast<Quest*>(quest)->m_titleReward;  // [lea +0xc4]
}

// ---------------------------------------------------------------------------
// CUser::getAddExpRateFromPremium（ORIG W 0x085bfd76 _ZN5CUser24getAddExpRateFromPremiumE17ENUM_PREMIUM_TYPE）
// this+0x7121c（m_premium）→ WongWork::CUserPremium::GetAddExpBonus(type)
// （ORIG W 0x085bf940）。转发链逐指令转写。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/getAddExpRateFromPremium.inc"
{
    return m_premium.GetAddExpBonus(static_cast<int>(type));
}

// ---------------------------------------------------------------------------
// CUser::compute_level_penalty（报告全文已读）
// m_questParameter(+0x8cc8).m_field18(+0x18 → +0x8ce0) /
// m_field1c(+0x1c → +0x8ce4)。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/compute_level_penalty.inc"
{
    // P196：ORIG `cmpl $6; jle / cmpl $11; jg / call 0x8ce0 / cmpl $11; jle / call 0x8ce4 / mov $100`
    if (levelDelta > 6 && levelDelta <= 11)
        return G_CDataManager()->m_questParameter.m_field18;   // [0x8ce0]
    else if (levelDelta > 11)
    {
        return G_CDataManager()->m_questParameter.m_field1c;   // [0x8ce4]
    }
    return 100;
}

// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_exp（报告全文已读）
// ---------------------------------------------------------------------------
// b28d-65：m_field130 已按 P248档② bool 化（Quest.h）并改裸 if(!field) ⇒ xor$1 对齐；
//   残余槽整体平移 4（ORIG -0x24..-0x10 vs 4.4 -0x28..-0x14，N95/P232 族）⇒ 封顶。
#include "LinkClosureA_User/compute_quest_basic_reward_exp.inc"
{
    if (quest->m_type == 4)                                // [quest+0x8]
    {
        return 0;                                          // [mov $0 早退]
    }

    int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x24]
    int penalty = 100;                                     // [-0x20]
    if (!quest->m_field130)                                // [xor $1 双测 → P248档② 裸形]
    {
        penalty = compute_level_penalty(levelDelta);       // [0x866e439]
    }

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（getRewardWeight +
    // getRewardExp 两分支，共 3 次）。
    int weight = G_CDataManager()->m_questParameter.getRewardWeight(  // [-0x1c movsbl char]
        static_cast<char>(quest->m_field102));

    int exp = 0;                                           // [-0x18]
    if (quest->m_field101 != 0 || quest->m_field130 != 0)
    {
        exp = G_CDataManager()->m_questParameter.getRewardExp(get_charac_level()); // [0x866e4a8]
    }
    else
    {
        exp = G_CDataManager()->m_questParameter.getRewardExp(quest->m_levelMin);  // [0x866e4c9]
    }

    exp = exp * weight / 100;                              // [imul + div 0x64 sar5]
    exp = exp * penalty / 100;

    int premiumBonus = 0;                                  // [-0x14]
    if (isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x4f))) // [cmp $0x4f]
    {
        int addRate = getAddExpRateFromPremium(
            static_cast<ENUM_PREMIUM_TYPE>(0x4f));         // [ecx]
        premiumBonus = setPremiumBonusExp(exp, addRate);   // [_ZN15CUserCharacInfo18setPremiumBonusExpEji]
    }

    STExpReward growthView;                                // [ctor 085bed52 -0x28]
    int growthBonus = 0;                                   // [-0x10]
    growthView.m_reward = get_growth_power_exp_reward().m_reward;  // [sret 临时 + mov dword]
    // [G5 2026-09-10 widthscan 线索·实测反劣已回滚] ORIG 0x0866e58a 为
    //   `movzbl -0x25(%ebp),%eax; movzbl %al,%eax`——`-0x25 = -0x28+3` = m_reward 槽
    //   最高字节**直读**（非 `>>24` 移位；widthscan `ext_movzbl orig=6 our=5` 实证）。
    //   纯 cast 字节视图形实测反劣（153→155 insns，gap +3→+5：4.4 未折叠成单条
    //   `movzbl -0x25(%ebp)`）⇒ 需 STExpReward 的 union 双视角（头改动）才可能收益，回滚。
    float growthRate =
        static_cast<float>(static_cast<short>(
            static_cast<unsigned char>(
                static_cast<unsigned int>(growthView.m_reward) >> 24))) /
        100.0f;                                            // [filds %ax / fdiv 100]
    growthBonus =
        static_cast<int>(static_cast<float>(exp) * growthRate); // [fld/fistpl ROUND]

    set_charac_seria_buf_bonus_exp(growthBonus);           // [0x866e5cc]

    return exp + premiumBonus + growthBonus;               // [-0x18 两加]
}

// ---------------------------------------------------------------------------
// CUser::compute_quest_basic_reward_gold（报告全文已读）
// m_b8 gate: quest+0xb8==0 或 ==0x19 才发放；遍历 m_titleReward(+0xc4) 首个
// m_itemIdx==0 元素计算金币；quest+0x104=m_field104 为金币倍率。
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/compute_quest_basic_reward_gold.inc"
{
    if (!(quest->m_fieldB8 == 0 || quest->m_fieldB8 == 0x19))
    {
        return 0;                                          // [0x866e608]
    }

    // b28d-65 [N86]：ORIG 0x0866e61b 段为 begin()→临时 iterator 槽 + 转换构造两段
    // （4.1 不做 copy-elision）；4.4 直构进目标槽 ⇒ 少 2 条 + 迭代器槽号连锁。
    // 实测三形（=初始化 / ()初始化 / 先声明后赋值）均不可翻（后两形更劣 142 insn）。
    std::vector<RewardItemInfo>::const_iterator it = quest->m_titleReward.begin();
    std::vector<RewardItemInfo>::const_iterator end = quest->m_titleReward.end();

    do                                                     // [do-while P25]
    {
        if (it == end)
        {
            return 0;                                      // [P16 早退]
        }
        // b28d-66 [P0]：ORIG `mov (%eax),%eax; mov %eax,-0xc; cmpl $0,-0xc; je`
        // ⇒ 原码有具名局部（itemIdx 占 -0xc 槽），非内联 test/sete 形。
        int itemIdx = it->m_itemIdx;                       // [-0xc]
        // b28d-66 真语义修正 + 布局：ORIG `cmpl; je +0xea`（命中块为跳转目标，
        // 以 return 结束），未命中遍历到末尾走回边后的 `return 0`；旧代码把计算
        // 放循环外，导致「遍历完未找到」也返回金币值。
        if (itemIdx != 0)
        {
            ++it;
        }
        else
        {
            int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x18]
            int penalty = 100;                             // [-0x14]
            int gold = 0;                                  // [-0x10]
            // ORIG `movzbl 0x130; xor $1; test; je <后置块>` ⇒ 原码条件为
            // `if (!m_field130)`（field==1 跳后置 getRewardGold(get_charac_level())）。
            if (!quest->m_field130)                        // [xor $1 极性]
            {
                penalty = compute_level_penalty(levelDelta);   // [0x866e713]
                gold = G_CDataManager()->m_questParameter.getRewardGold(quest->m_levelMin);
            }
            else
            {
                gold = G_CDataManager()->m_questParameter.getRewardGold(get_charac_level());
            }

            return gold * penalty / 100 * quest->m_field104 / 100; // [两段 imul/div]
        }
    } while (!(it == end));                                // [尾判回环，ORIG operator==]

    return 0;
}

// ---------------------------------------------------------------------------
// CUser::quest_basic_reward（报告全文已读）
// ---------------------------------------------------------------------------
#include "LinkClosureA_User/quest_basic_reward.inc"
                               int& qpOut, int& pieceOut, bool send)
{
    expOut = compute_quest_basic_reward_exp(quest);        // [0x866e7c1]
    goldOut = compute_quest_basic_reward_gold(quest);      // [0x866e7d8]

    int ratio =                                            // [-0xc uint 除法，比较有符号 jle]
        getCurCharacHelpAbuseComputedRatio() / 10000u;     // [mul 0xd1b71759 shr 0xd]
    if (ratio > 100)                                       // [cmpl $0x64 jle]
    {
        ratio = 100;
    }

    if ((int)G_CDataManager()->m_questParameter.m_field20 < (int)ratio) // [+0x8ce8 setl]
    {
        expOut = expOut * (100 - ratio) / 100;             // [imul 0x51eb851f sar5]
        goldOut = goldOut * (100 - ratio) / 100;
    }

    qpOut = static_cast<int>(static_cast<short>(quest->m_field138)); // [movzwl/movswl]
    if (static_cast<short>(quest->m_field138) > 0)
    {
        incQuestPoint(static_cast<int>(static_cast<short>(quest->m_field138)),
                      quest->m_index, send);               // [0x866e8c0 incQuestPointEiib]
    }

    pieceOut = static_cast<int>(static_cast<short>(quest->m_field13a));
    if (static_cast<short>(quest->m_field13a) > 0)
    {
        incQuestPiece(static_cast<int>(static_cast<short>(quest->m_field13a)),
                      quest->m_index, send);               // [0x866e90d incQuestPieceEiib]
    }
}

// ---------------------------------------------------------------------------
// CUser::CalculateExpertJobMaterials（报告全文已读）
// copy 向量上按 GetExpertJobCompoundMaterialVariation/100 缩减后再并回原表。
// P48 双迭代器形态照录（ci 在 copy 上、mi 在实参向量上同步推进）。
// ---------------------------------------------------------------------------
// b28d-65：残余 P0（jobLevel/jobType ORIG 直落 %ebx/%esi 无槽）+ N86（迭代器两段）+
//   槽向族；orig 165 vs ours 153，四形实测不可翻 ⇒ 封顶。
#include "LinkClosureA_User/CalculateExpertJobMaterials.inc"
    std::vector<std::pair<int, int> >& materials)
{
    // b28d-66 [P0]：ORIG jobLevel/jobType 直落 %ebx/%esi 且无栈槽 ⇒ 原码为内联右值，
    // 实参求值序（右到左）= level(含 GetCurCharacExpertJobExp) → type → 对象。
    float variation =
        getCurCharacInvenR()->GetExpertJobCompoundMaterialVariation(
            static_cast<ENUM_EXPERT_JOB_TYPE>(GetCurCharacExpertJobType()),
            GetCurExpertJobLevel(GetCurCharacExpertJobExp()));
                                                            // [fstps -0xc]
    std::vector<std::pair<int, int> > reduced(materials);   // [-0x28 vector copy]
    variation /= 100.0f;                                    // [fdiv _DAT_08cf3954]

    // b28d-66 [N86]：ORIG 迭代器 = 先声明（C1Ev ×2）后赋值（begin 经 -0x3c 临时中转
    // 再 mov 拷贝到 -0x2c/-0x30），非 4.4 的拷贝初始化直构。
    // [G5 2026-09-10 判据⑩试验·无效] 交换 ci/mi 声明序（期望翻转后置 ++ 的 sret 临时
    // -0x10/-0x14 相对序）实测四数不变（31/40/2/5）⇒ 已回滚，维持原声明序。
    std::vector<std::pair<int, int> >::iterator ci;         // [-0x2c]
    std::vector<std::pair<int, int> >::iterator mi;         // [-0x30]
    ci = reduced.begin();
    mi = materials.begin();

    // b28d-66：ORIG 循环 = while(ci!=end && mi!=end)（无 xor $1 短路取反）+
    // 后置 ci++/mi++（_ZN…ppEi，sret 临时 -0x10/-0x14），非 do-while+break+前置 ++。
    while (ci != reduced.end() && mi != materials.end())   // [双 != 判定]
    {
        ci->second = static_cast<int>(                     // [fld/fistpl ROUND]
            static_cast<float>(ci->second) * variation);
        mi->second += ci->second;                          // [lea (%esi,%eax)]
        if (mi->second == 0)                               // [test/sete]
        {
            mi->second = 1;
        }
        ci++;                                              // [operator++(int) ×2]
        mi++;
    }
}

// [B01 铁律1 / Main 批准] 原 `_forceLinkClosureAWeakSymbols`（static + used）为**发明符号**
// （ORIG `_ZL29_forceLinkClosureA…` 不存在），其目的（P147 强制发射 7 个弱模板实例）已由
// 其它 TU 自然发射覆盖（本机 nm 普查 841 个 .o）：`_ZNK…normal_iteratorIPi…4baseEv` 116 个、
// `…IPKi…C1ERKS2_` 82 个、`…C1IPiE`(enable_if 转换构造) 16 个、`pair 迭代器 ppEv` 16 个、
// `vector<int>::begin/end` 116/116、`vector<pair<int,int>>::begin` 98 个 ⇒ 删定义不丢符号。

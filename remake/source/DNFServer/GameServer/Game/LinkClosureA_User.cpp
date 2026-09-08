// ============================================================================
// LinkClosureA_User.cpp — undef_bizA 收口批 A：CUser 用户域符号
//
// 完整阅读报告（docs/class_func_reports/CUser/）：
//   GetCurCharacInventoryRef_08680f2e.md（含 Ghidra C 全文）
//   quest_basic_reward.md / compute_quest_basic_reward_exp.md /
//   compute_quest_basic_reward_gold.md / compute_level_penalty.md
// objdump 转写（报告缺失，nm -S 定址）：
//   UpdateRating(float)            W 0x08568e9c
//   GetUserGainItem 域外自由核对：CLuckPoint 三方法落 CClearReward.cpp
// const 重载版本（_ZNK5CUser24GetCurCharacInventoryRefEtt T 0x08680c8a，
// OnlyOrig_CreatureJob 批声明）亦按 objdump 同构转写。
// ============================================================================

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
Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short area,
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
const Inven_Item* CUser::GetCurCharacInventoryRef(unsigned short area,
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
void CUser::UpdateRating(float rating)
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
std::vector<RewardItemInfo>* CUser::getUserQuestRewardItem(
    const Quest* quest, const CUser* other)     // ORIG _ZN5CUser22…PKS_ 非 const
{
    Quest* q = const_cast<Quest*>(quest);   // ORIG 直接 lea 非常量视图
    if (!q->m_fieldDC.empty())              // [0x866e91b xor $1]
    {
        std::vector<int>::const_iterator it = q->m_fieldDC.begin();
        std::vector<int>::const_iterator end = q->m_fieldDC.end();

        if (q->m_fieldE8 == 1)                         // [cmp $1]
            for (; it != end; ++it)
            {
                if (other->getCurCharacQuestR()->isClearQuest(*it))
                {
                    return &q->m_rewardItems;          // [lea +0xec]
            }
        }
        else
        {
            while (it != end &&
                   other->getCurCharacQuestR()->isClearQuest(*it))
            {
                ++it;
            }
            if (it == end)                                 // [operator== P7]
            {
                return &q->m_rewardItems;
            }
        }
    }
    return &q->m_titleReward;                          // [lea +0xc4]
}

// ---------------------------------------------------------------------------
// CUser::getAddExpRateFromPremium（ORIG W 0x085bfd76 _ZN5CUser24getAddExpRateFromPremiumE17ENUM_PREMIUM_TYPE）
// this+0x7121c（m_premium）→ WongWork::CUserPremium::GetAddExpBonus(type)
// （ORIG W 0x085bf940）。转发链逐指令转写。
// ---------------------------------------------------------------------------
int CUser::getAddExpRateFromPremium(ENUM_PREMIUM_TYPE type)
{
    return m_premium.GetAddExpBonus(static_cast<int>(type));
}

// ---------------------------------------------------------------------------
// CUser::compute_level_penalty（报告全文已读）
// m_questParameter(+0x8cc8).m_field18(+0x18 → +0x8ce0) /
// m_field1c(+0x1c → +0x8ce4)。
// ---------------------------------------------------------------------------
int CUser::compute_level_penalty(int levelDelta)
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
int CUser::compute_quest_basic_reward_exp(Quest* quest)
{
    if (quest->m_type == 4)                                // [quest+0x8]
    {
        return 0;                                          // [mov $0 早退]
    }

    int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x24]
    int penalty = 100;                                     // [-0x20]
    if (quest->m_field130 == 0)                            // [xor $1 双测]
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
int CUser::compute_quest_basic_reward_gold(Quest* quest)
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
        if (it->m_itemIdx == 0)                            // [local_10]
        {
            break;
        }
        ++it;
    } while (!(it == end));                                // [尾判回环，ORIG operator==]

    int levelDelta = get_charac_level() - quest->m_levelMin; // [-0x18]
    int penalty = 100;                                     // [-0x14]
    int gold = 0;                                          // [-0x10]
    if (quest->m_field130 == 1)
    {
        gold = G_CDataManager()->m_questParameter.getRewardGold(get_charac_level());
    }
    else
    {
        penalty = compute_level_penalty(levelDelta);       // [0x866e713]
        gold = G_CDataManager()->m_questParameter.getRewardGold(quest->m_levelMin);
    }

    return gold * penalty / 100 * quest->m_field104 / 100; // [两段 imul/div]
}

// ---------------------------------------------------------------------------
// CUser::quest_basic_reward（报告全文已读）
// ---------------------------------------------------------------------------
void CUser::quest_basic_reward(Quest* quest, int& expOut, int& goldOut,
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
void CUser::CalculateExpertJobMaterials(
    std::vector<std::pair<int, int> >& materials)
{
    int jobLevel = GetCurExpertJobLevel(GetCurCharacExpertJobExp()); // [ebx]
    int jobType = GetCurCharacExpertJobType();                       // [esi]
    float variation =
        getCurCharacInvenR()->GetExpertJobCompoundMaterialVariation(
            static_cast<ENUM_EXPERT_JOB_TYPE>(jobType), jobLevel);
                                                            // [fstps -0xc]
    std::vector<std::pair<int, int> > reduced(materials);   // [-0x28 vector copy]
    variation /= 100.0f;                                    // [fdiv _DAT_08cf3954]

    std::vector<std::pair<int, int> >::iterator ci = reduced.begin();   // [-0x2c]
    std::vector<std::pair<int, int> >::iterator mi = materials.begin(); // [-0x30]

    do
    {
        if (!(ci != reduced.end() && mi != materials.end())) // [双 != 判定]
        {
            break;
        }
        ci->second = static_cast<int>(                     // [fld/fistpl ROUND]
            static_cast<float>(ci->second) * variation);
        mi->second = mi->second + ci->second;              // [lea (%esi,%eax)]
        if (mi->second == 0)                               // [test/sete]
        {
            mi->second = 1;
        }
        ++ci;                                              // [operator++ ×2]
        ++mi;
    } while (true);
}

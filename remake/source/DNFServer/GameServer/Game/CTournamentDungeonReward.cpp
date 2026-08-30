#include "CTournamentDungeonReward.h"
#include <cstring>

#include "CDungeon.h"          // TournamentRewardRate/TournamentRewardItemRate 权威定义
#include "CBattle_Field_shared.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDataManager.h"
#include "CItemList.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameRand.h"          // get_rand_int

// ORIG ctor 0x8284770：写虚表 0x8c0cbf0；D1 0x82847dc；ResetReward 0x8284820。
// 本 TU 仅提供 CBattle_Field 构造/析构链与 ResetReward 所需符号（ORIG 0x8284820）。
// 其余成员（MakeSelectPacket/_MakeRewardPacket/MakeRewardPacket 等）在头为 G2
// 占位布局时未声明成员，故不在此定义，属后续批次。
CTournamentDungeonReward::CTournamentDungeonReward()
    : m_rewards()
{
    // ORIG 0x8284770：构造 2x2 m_rewards 后调 ResetReward()（各 Clear + m_field04=0）
    ResetReward();
}

CTournamentDungeonReward::~CTournamentDungeonReward() {}

// ORIG 0x8284820：对 2x2 个 TournamentRewardType（+0x8 起）逐个 Clear，
// 最后清零 +0x04。构造函数中的 memset 占位同步移除（成员自动初始化）。
void CTournamentDungeonReward::ResetReward()
{
    for (int c = 0; c < 2; ++c)
        for (int r = 0; r < 2; ++r)
            m_rewards[c][r].Clear();
    m_field04 = 0;
}

// ORIG 0x828502e：type∈{0,1}、maxItems<3 校验后，按 cardNo 在
// m_rewards[type][i].m_flag08 中匹配卡片号：
//   m_id==0  → items[0] 置 add_info=m_field04（固定物品），items[1] 空；
//   m_id!=-1 → items[1].m_addInfo=m_id 后经 CItemList::create_item 生成，
//              items[0] 空；m_id==-1 → 双空返回 0（未命中）。
#include "CDataManager.h"
#include "CItemList.h"   // create_item（G_CDataManager()->getItemList()）

int CTournamentDungeonReward::GetItemFromCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                                              int cardNo, Inven_Item* items,
                                              unsigned int maxItems)
{
    if (maxItems < 3 && (int)type >= 0 && (int)type < 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            TournamentRewardType& slot = m_rewards[(int)type][i];
            if ((char)slot.m_flag08 == (char)cardNo)
            {
                if (slot.m_id == 0)
                {
                    items[0].reset();
                    items[0].set_add_info(slot.m_field04);
                    items[1].reset();
                    return 1;
                }
                if (slot.m_id != -1)
                {
                    items[0].reset();
                    items[1].reset();
                    items[1].m_addInfo = slot.m_id;
                    G_CDataManager()->getItemList()->create_item(
                        ENUM_ITEM_CREATE_TYPE_1, items[1], slot.m_field04);
                    return 1;
                }
                items[0].reset();
                items[1].reset();
                return 0;
            }
        }
    }
    return 0;
}

// ============================================================================
// ORIG 0x8284a24：两张卡槽各按 rate 掷点决定奖励（未过首档则 m_id=0）。
// ============================================================================
void CTournamentDungeonReward::DecisionRewardType(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE card, TournamentRewardRate& rate,
    const TournamentRewardItemRate* itemRate)
{
    for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
    {
        int roll = get_rand_int(rate.m_c + 1);
        if (rate.m_0 == 0 || rate.m_0 < roll)
        {
            if (roll <= rate.m_0 + rate.m_4)
            {
                int pick = get_rand_int(itemRate->m_c);
                int accum = 0;
                for (size_t i = 0; i < itemRate->m_items.size(); ++i)
                {
                    accum += itemRate->m_items[i].m_data[1];
                    if (pick + 1 <= accum)
                    {
                        m_rewards[(int)card][slotIdx].m_id =
                            itemRate->m_items[i].m_data[0];
                        m_rewards[(int)card][slotIdx].m_field04 =
                            itemRate->m_items[i].m_data[2];
                        break;
                    }
                }
            }
        }
        else
        {
            m_rewards[(int)card][slotIdx].m_id = 0;
        }
    }
}

// ============================================================================
// ORIG 0x8284890：round=victory?round:round-1（夹取 0..4）；查 rates 表命中
// 后掷卡槽 0；终局轮（round==4 且胜利）再以表项 5 掷卡槽 1。
// ============================================================================
void CTournamentDungeonReward::MakeReward(
    const std::map<int, TournamentRewardRate>* rates,
    const TournamentRewardItemRate* itemRate, int round, bool victory)
{
    if (rates == 0)
        return;

    int curRound = victory ? round : round - 1;
    if (curRound < 0)
        curRound = 0;
    else if (4 < curRound)
        curRound = 4;

    std::map<int, TournamentRewardRate>::const_iterator it = rates->find(curRound);
    if (it == rates->end())
        return;

    TournamentRewardRate rate = it->second;
    DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, rate, itemRate);
    if (round == 4 && victory)
    {
        std::map<int, TournamentRewardRate>::const_iterator itFinal = rates->find(5);
        if (itFinal != rates->end())
        {
            TournamentRewardRate rateFinal = itFinal->second;
            DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, rateFinal,
                               itemRate);
        }
    }
}

// ============================================================================
// ORIG 0x8284b8e：空奖励槽（m_id==0）以生成结果首个掉落物的 add_info2
// （金币额）乘金币率写入 m_field04。
// ============================================================================
void CTournamentDungeonReward::SetRewardGold(
    WongWork::stGenerateResult_t& result, float goldRate)
{
    for (int card = 0; card < 2; ++card)
    {
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            if (m_rewards[card][slotIdx].m_id == 0 &&
                !result.m_dropItems.empty())
            {
                // ORIG x87 RC=truncate 直接截断（fildl+fmuls+fistpl，无 +0.5）
                m_rewards[card][slotIdx].m_field04 =
                    (int)((float)result.m_dropItems.begin()->get_add_info()
                          * goldRate);
            }
        }
    }
}

// ORIG 0x82852e4：单卡型奖励包体（byte2 + 每卡槽 int/int/short0）
void CTournamentDungeonReward::_MakeRewardPacket(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, PacketGuard* packet)
{
    if ((int)type < 2)
    {
        packet->put_byte(2);
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            packet->put_int(m_rewards[(int)type][slotIdx].m_id);
            packet->put_int(m_rewards[(int)type][slotIdx].m_field04);
            packet->put_short(0);
        }
    }
}

// ORIG 0x8284caa：卡型 0 必发；flag 时补卡型 1，否则 byte0 占位
void CTournamentDungeonReward::MakeRewardPacket(PacketGuard* packet, bool flag)
{
    _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, packet);
    if (flag)
        _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, packet);
    else
        packet->put_byte(0);
}
// ============================================================================
// ORIG 0x08284ee4（报告 SelectCard.md）：cardA/cardB ∈ [0,1]、slotNo ∈ [0,3]
// 校验后，m_rewards[(int)cardA][(int)cardB].m_flag08（movzbl 零扩展读）== 0xff（未选）
// 才写入 slotNo 低字节并返回 1；否则返回 0。
// ============================================================================
bool CTournamentDungeonReward::SelectCard(int slotNo, char cardA, char cardB)
{
    if (cardA < 0 || cardA > 1)
        return false;
    if (cardB < 0 || cardB > 1)
        return false;
    if (slotNo < 0 || slotNo > 3)
        return false;

    // ORIG cmp $0xff; jne fail —— 命中未选槽（==0xff）直落写入体，失败块居尾
    if (static_cast<unsigned char>(
            m_rewards[(int)cardA][(int)cardB].m_flag08) == 0xff)
    {
        m_rewards[(int)cardA][(int)cardB].m_flag08 = static_cast<char>(slotNo);
        return true;
    }
    return false;
}
// ============================================================================
// ORIG 0x08285284（报告 CTournamentDungeonReward/isMemberSelectCard.md）：
// 遍历该卡型两槽，m_flag08（char 符号扩展）== cardNo 即 1。
// ============================================================================
bool CTournamentDungeonReward::isMemberSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo)
{
    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<char>(m_rewards[(int)type][slot].m_flag08) == cardNo)
            return true;
    }
    return false;
}

// ============================================================================
// ORIG 0x082853b8（报告 _ForceSelectCard.md）：type>1 直接返回；
// 对两槽：m_flag08==0xff（未选）时遍历 members，凡 !isMemberSelectCard(type,*it)
// 者写入 m_flag08（低字节）。持续遍历不提前退出（ORIG 形态）。
// ============================================================================
void CTournamentDungeonReward::_ForceSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if ((int)type > 1)
        return;

    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<unsigned char>(m_rewards[(int)type][slot].m_flag08) == 0xff)
        {
            for (std::vector<int>::const_iterator it = members.begin();
                 it != members.end(); ++it)
            {
                if (!isMemberSelectCard(type, *it))
                    m_rewards[(int)type][slot].m_flag08 = static_cast<char>(*it);
            }
        }
    }
}

// ============================================================================
// ORIG 0x0828523c（报告 ForceSelectCard.md）：_ForceSelectCard(0)；victory 时
// 再 _ForceSelectCard(1)。
// ============================================================================
void CTournamentDungeonReward::ForceSelectCard(std::vector<int>& members, bool victory)
{
    _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, members);
    if (victory)
        _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, members);
}

// ============================================================================
// ORIG 0x08284d08（报告 isCardSelectComplete.md）：members 空 → 0。
// type==2（双卡型）：2x2 全部槽的 m_flag08（char 引用，std::find<char> 实证
// _ZSt4findI...cET_S8_S8_RKT0_）都必须出现在 members 中；否则查该行两槽。
// 全命中 → 1。
// ============================================================================
bool CTournamentDungeonReward::isCardSelectComplete(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if (members.empty())
        return false;

    if ((int)type == 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                if (std::find(members.begin(), members.end(),
                              m_rewards[i][j].m_flag08) == members.end())
                    return false;
            }
        }
    }
    else
    {
        for (int j = 0; j < 2; ++j)
        {
            if (std::find(members.begin(), members.end(),
                          m_rewards[(int)type][j].m_flag08) == members.end())
                return false;
        }
    }
    return true;
}

// ============================================================================
// ORIG 0x08284fa4（报告 MakeSelectPacket.md）：两轮 put_byte(2)；内轮对两槽
// put_byte((char)m_flag08 符号扩展)。
// ============================================================================
void CTournamentDungeonReward::MakeSelectPacket(PacketGuard* packet)
{
    for (int type = 0; type < 2; ++type)
    {
        packet->put_byte(2);
        for (int slot = 0; slot < 2; ++slot)
        {
            packet->put_byte(static_cast<char>(m_rewards[type][slot].m_flag08));
        }
    }
}

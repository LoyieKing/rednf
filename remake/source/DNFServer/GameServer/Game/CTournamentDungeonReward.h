#ifndef DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_
#define DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

#include <map>
#include <vector>

class TournamentRewardRate;              // CDungeon.h 权威定义
class TournamentRewardItemRate;
struct TournamentRewardItem;
class PacketGuard;
namespace WongWork
{
struct stGenerateResult_t;
}

// ORIG：多态类（ctor 0x8284770 写虚表 0x8c0cbf0），尺寸 0x38
// （CBattle_Field 成员 @ +0xc98..+0xcd0）。
class Inven_Item;

// ORIG mangled 32 字符类型名（GetItemFromCard 参数；值域实证 0/1）
enum ENUM_TOURNAMENT_REWARD_CARD_TYPE
{
    ENUM_TOURNAMENT_REWARD_CARD_TYPE_0 = 0,
    ENUM_TOURNAMENT_REWARD_CARD_TYPE_1 = 1,
    ENUM_TOURNAMENT_REWARD_CARD_TYPE_2 = 2   // isCardSelectComplete cmp $2 实证（双卡型）
};

class TournamentRewardType
{
public:
    // ORIG C1/C2 W 0x08285546：仅 call Clear。
    TournamentRewardType() { Clear(); }

    // ORIG T 0x08284750 (size 0x1f)：Clear 为**外联强符号**（非头内 inline）⇒
    // 定义在 CTournamentDungeonReward.cpp 且位于全部 caller 之前（P210②），
    // 使 4.4 精确继承其小 outgoing 帧（ResetReward 0x28→0x14）。
    void Clear();

    int m_id;        // +0x00
    int m_field04;   // +0x04
    char m_flag08;   // +0x08
    char m_pad09[3];
};

class CTournamentDungeonReward
{
public:
    CTournamentDungeonReward();      // ORIG 0x8284770
    virtual ~CTournamentDungeonReward();  // ORIG D1 0x82847dc / D0 0x8284802
    void ResetReward();              // ORIG 0x8284820
    void MakeReward(const std::map<int, TournamentRewardRate>* rates,
                    const TournamentRewardItemRate* itemRate, int round,
                    bool victory);   // ORIG 0x8284890
    void SetRewardGold(WongWork::stGenerateResult_t& result,
                       float goldRate);   // ORIG 0x8284b8e（非 const 引用，mangled ERN8）
    void MakeRewardPacket(PacketGuard* packet, bool flag);   // ORIG 0x8284caa
private:
    void DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE card,
                            TournamentRewardRate& rate,
                            const TournamentRewardItemRate* itemRate);   // ORIG 0x8284a24（非 const 引用，mangled R20）
    void _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                           PacketGuard* packet);   // ORIG 0x082852e4（mangled P11PacketGuard）
    // ORIG T 0x08285284（isCardSelectComplete/_ForceSelectCard 消费）
    bool isMemberSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo);
    // ORIG T 0x082853b8（ForceSelectCard 逐卡型包装）
    void _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                          const std::vector<int>& members);

public:
    int GetItemFromCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo,
                        Inven_Item* items, unsigned int maxItems);   // ORIG 0x828502e
    // ORIG T 0x08284d08（_ZN24...20isCardSelectCompleteE32ENUM_..RKSt6vectorIiSaIiEE，
    // 非 const 成员）。
    bool isCardSelectComplete(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                              const std::vector<int>& members);
    // ORIG T 0x08284ee4（_ZN24CTournamentDungeonReward10SelectCardEicc，
    // 卡选落槽：槽位 slotNo 写入 m_rewards[cardA][cardB] 未选槽）。
    bool SelectCard(int slotNo, char cardA, char cardB);
    // ORIG T 0x0828523c（参数为非 const vector&）。
    void ForceSelectCard(std::vector<int>& members, bool victory);
    // ORIG T 0x08284fa4。
    void MakeSelectPacket(PacketGuard* packet);

private:
    int m_field04;                       // +0x04（ResetReward 清零）
    TournamentRewardType m_rewards[2][2]; // +0x08，行步长 0x18、列步长 0xc
};

#endif  // DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

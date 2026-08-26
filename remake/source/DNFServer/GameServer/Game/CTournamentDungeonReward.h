#ifndef DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_
#define DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

#include <map>

class TournamentRewardRate;              // CDungeon.h 权威定义
class TournamentRewardItemRate;
struct TournamentRewardItem;
class PacketGuard;
class InterfacePacketBuf;
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
    ENUM_TOURNAMENT_REWARD_CARD_TYPE_1 = 1
};

class TournamentRewardType
{
public:
    void Clear()
    {
        m_id = -1;
        m_field04 = 0;
        m_flag08 = static_cast<char>(0xff);
    }

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
    void SetRewardGold(const WongWork::stGenerateResult_t& result,
                       float goldRate);   // ORIG 0x8284b8e
    void MakeRewardPacket(PacketGuard* packet, bool flag);   // ORIG 0x8284caa
private:
    void DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE card,
                            const TournamentRewardRate& rate,
                            const TournamentRewardItemRate* itemRate);   // ORIG 0x8284a24
    void _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                           InterfacePacketBuf* packet);   // ORIG 0x82852e4

public:
    int GetItemFromCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo,
                        Inven_Item* items, unsigned int maxItems);   // ORIG 0x828502e

private:
    int m_field04;                       // +0x04（ResetReward 清零）
    TournamentRewardType m_rewards[2][2]; // +0x08，行步长 0x18、列步长 0xc
};

#endif  // DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

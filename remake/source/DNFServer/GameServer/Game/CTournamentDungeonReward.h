#ifndef DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_
#define DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

// G2 占位（TODO：真实实现属后续批次）。
// ORIG：多态类（ctor 0x8284770 写虚表 0x8c0cbf0），尺寸 0x38
// （CBattle_Field 成员 @ +0xc98..+0xcd0）。

// ORIG Clear 0x8284750：int+0 = -1、int+4 = 0、byte+8 = 0xff（sizeof 0xc）。
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

private:
    int m_field04;                       // +0x04（ResetReward 清零）
    TournamentRewardType m_rewards[2][2]; // +0x08，行步长 0x18、列步长 0xc
};

#endif  // DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

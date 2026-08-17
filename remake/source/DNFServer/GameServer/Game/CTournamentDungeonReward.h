#ifndef DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_
#define DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

// G2 占位（TODO：真实实现属后续批次）。
// ORIG：多态类（ctor 0x8284770 写虚表 0x8c0cbf0），尺寸 0x38
// （CBattle_Field 成员 @ +0xc98..+0xcd0）。

class CTournamentDungeonReward
{
public:
    CTournamentDungeonReward();      // ORIG 0x8284770
    virtual ~CTournamentDungeonReward();  // ORIG D1 0x82847dc / D0 0x8284802
    void ResetReward();              // ORIG 0x8284820

private:
    char m_pad[0x34];  // 虚表指针之后（总尺寸 0x38）
};

#endif  // DNF_GAME_CTOURNAMENTDUNGEONREWARD_H_

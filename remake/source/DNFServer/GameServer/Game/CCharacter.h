// df_game_r 还原 —— CCharacter 类声明（独立 TU，batch：从 GameStubs.cpp 迁移）。
// 权威符号（ORIG df_game_r）：
//   CCharacter::CCharacter()                         T @0x0834828c
//   CCharacter::~CCharacter()                        T @0x083483f4(析构)
//   CCharacter::get_give_skill(int,int,std::vector<std::pair<int,int>>&,uint) const  T @0x08348798
#ifndef DNF_GAME_CCHARACTER_H_
#define DNF_GAME_CCHARACTER_H_

#include <utility>
#include <vector>

class CCharacter
{
public:
    CCharacter();
    ~CCharacter();

    char get_give_skill(int firstGrow, int secondGrow,
                        std::vector<std::pair<int, int> >& out,
                        unsigned int flag) const;

private:
    std::vector<std::pair<int, int> > m_giveSkillList;
};

#endif  // DNF_GAME_CCHARACTER_H_
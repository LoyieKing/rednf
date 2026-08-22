// ============================================================================
// df_game_r 还原 —— CCharacter（依据 CCharacList 完整构造/析构报告）
// ============================================================================
#include "CCharacter.h"

CCharacter::CCharacter()
{
}

CCharacter::~CCharacter()
{
}

char CCharacter::get_give_skill(int firstGrow, int secondGrow,
                                std::vector<std::pair<int, int> >& out,
                                unsigned int flag) const
{
    if (firstGrow < 0 || firstGrow > 5)
        return 0;
    if (flag == 1) {
        out.insert(out.end(), m_giveSkillList.begin(), m_giveSkillList.end());
        return 1;
    }
    if (flag == 2) {
        if (secondGrow >= 1 && secondGrow <= 2)
            out.insert(out.end(), m_giveSkillList.begin(), m_giveSkillList.end());
        return 1;
    }
    if (flag == 0) {
        out.insert(out.end(), m_giveSkillList.begin(), m_giveSkillList.end());
        return 1;
    }
    return 0;
}

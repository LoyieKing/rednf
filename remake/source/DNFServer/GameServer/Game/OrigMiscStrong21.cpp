// ============================================================================
// OrigMiscStrong21.cpp — 杂项强补齐（独占 TU，任务 OrigMiscStrong21）
//
// 覆盖符号（ORIG df_game_r nm 核实）：
//   stNPCCommonData_t::getIllustIndex(int)             T 0x089faeee
//   stNPCCommonData_t::stOneBuff_t::~stOneBuff_t()     W 0x08a04b54（D1/D2 同址，空）
// ============================================================================

#include "CDataManager.h"

int stNPCCommonData_t::getIllustIndex(int npcIdx)
{
    for (unsigned int i = 0; i < m_illustrationVariableNpcList.size(); ++i)
    {
        if (m_illustrationVariableNpcList.at(i) == npcIdx)
            return i;
    }
    return -1;
}

stNPCCommonData_t::stOneBuff_t::~stOneBuff_t()
{
}

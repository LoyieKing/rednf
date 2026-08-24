// ============================================================================
// df_game_r 还原 —— Secu_DungeonAverageCheck（G2 安全域）
// 声明点：Secu_HackLogCheckByParty.h（唯一声明点；Secu_HackLogCheckByParty :
// Secu_DungeonAverageCheck 为 CParty 子对象 +0x1abc）。
// 语义依据 ORIG setKillMonsterInfo 0x82794d6（逐指令转写，含 ROUND=截断浮点、
// 系数 0.1/0.5/0.05 来自 .rodata 0x8c02770/74/78）。
// ============================================================================
#include "Secu_HackLogCheckByParty.h"

#include <string.h>

#include "CParty.h"
#include "CUser.h"
#include "WarField.h"          // map_monster（m_roleType +0x08 / m_mobIndex +0x0c）
#include "CGameManager.h"      // WongWork::CHackAnalyzer::addServerHackCnt 声明

Secu_DungeonAverageCheck::Secu_DungeonAverageCheck()
    : m_party(0), m_killInfos()
{
}

Secu_DungeonAverageCheck::~Secu_DungeonAverageCheck()
{
}

void Secu_DungeonAverageCheck::SetPartyPtr(CParty* party)
{
    m_party = party;
}

void Secu_DungeonAverageCheck::startDungeon()
{
    m_killInfos.clear();
}

std::vector<SecKillMonsterInfo>::iterator
Secu_DungeonAverageCheck::findKillMonsterInfo(unsigned int monsterId)
{
    // ORIG 0x827993c：线性查找 m_monsterId 匹配项，未命中返回 end()。
    std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
    for (; it != m_killInfos.end(); ++it)
    {
        if ((unsigned int)it->m_monsterId == monsterId)
            break;
    }
    return it;
}

void Secu_DungeonAverageCheck::setKillMonsterInfo(const MSG_MONSTER_DIE* info,
                                                  map_monster* monster)
{
    // ORIG 0x82794d6。ROUND 为控制字截断（round toward zero），用 (int) 强转等价。
    if (m_party == 0)
        return;
    if (monster->isNamedMonster() || monster->m_roleType != 0)
        return;

    unsigned int monsterId = (unsigned int)monster->m_mobIndex;
    std::vector<SecKillMonsterInfo>::iterator it = findKillMonsterInfo(monsterId);
    if (it != m_killInfos.end())
    {
        SecKillMonsterInfo& rec = *it;
        if (4 < (unsigned int)rec.m_count)
        {
            float avg1 = (float)(unsigned int)rec.m_sumValue1 /
                         (float)(unsigned int)rec.m_count;
            float avg2 = (float)(unsigned int)rec.m_sumValue2 /
                         (float)(unsigned int)rec.m_count;

            float adj1;
            if ((int)avg1 < 10)
                adj1 = 1.0f;
            else
                adj1 = 0.1f * avg1;

            unsigned int upper1 = (unsigned int)(int)(0.5f + avg1 + adj1);
            unsigned int lower1 = (unsigned int)(int)(avg1 - adj1);
            float adj2 = 0.05f * avg2;
            unsigned int upper2 = (unsigned int)(int)(0.5f + avg2 + adj2);
            unsigned int lower2 = (unsigned int)(int)(avg2 - adj2);

            if (*(int*)((char*)m_party + 0x1874) > 0 &&
                lower1 <= info->m_killValue1 && info->m_killValue1 <= upper1)
            {
                rec.m_hackCntE6 += 1;
                for (int i = 0; i < 4; ++i)
                {
                    if (!m_party->checkValidUser(i))
                        continue;
                    CUser* user = m_party->get_user(i);
                    CUser* userArg = m_party->get_user(i);
                    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
                    analyzer->addServerHackCnt(userArg, (WongWork::ENUM_HACKTYPE)0xe6, 1, 0, 0);
                }
            }
            if (*(int*)((char*)m_party + 0x1874) > 0 &&
                lower2 <= info->m_killValue2 && info->m_killValue2 <= upper2)
            {
                rec.m_hackCntE7 += 1;
                for (int i = 0; i < 4; ++i)
                {
                    if (!m_party->checkValidUser(i))
                        continue;
                    CUser* user = m_party->get_user(i);
                    CUser* userArg = m_party->get_user(i);
                    WongWork::CHackAnalyzer* analyzer = user->getHackAnalyzer();
                    analyzer->addServerHackCnt(userArg, (WongWork::ENUM_HACKTYPE)0xe7, 1, 0, 0);
                }
            }
        }
        rec.m_count += 1;
        rec.m_sumValue1 += (int)info->m_killValue1;
        rec.m_sumValue2 += (int)info->m_killValue2;
    }
    else
    {
        m_killInfos.push_back(SecKillMonsterInfo(
            monsterId, 1,
            (unsigned int)info->m_killValue1,
            (unsigned int)info->m_killValue2));
    }
}

void Secu_DungeonAverageCheck::dungeonClear()
{
    // ORIG 0x82799d6 —— 校验平均等级（clear）；报告缺失深水区，保持占位。
}

void Secu_DungeonAverageCheck::dungeonFail()
{
    // ORIG 0x82799f4 —— 校验平均等级（fail）；报告缺失深水区，保持占位。
}

void Secu_DungeonAverageCheck::reset()
{
    m_killInfos.clear();
}

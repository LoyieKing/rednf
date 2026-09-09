// ============================================================================
// df_game_r 还原 —— Secu_DungeonAverageCheck（G2 安全域）
// 声明点：Secu_HackLogCheckByParty.h（唯一声明点；Secu_HackLogCheckByParty :
// Secu_DungeonAverageCheck 为 CParty 子对象 +0x1abc）。
// 语义依据 ORIG setKillMonsterInfo 0x82794d6（逐指令转写，含 ROUND=截断浮点、
// 系数 0.1/0.5/0.05 来自 .rodata 0x8c02770/74/78）。
// ============================================================================
#include "Secu_HackLogCheckByParty.h"

#include <string.h>
#include <algorithm>   // std::max（ORIG 0x08279a12 调 _ZSt3maxIiERKT_S2_S2_ ×2）

#include "CParty.h"
#include "CUser.h"
#include "WarField.h"          // map_monster（m_roleType +0x08 / m_mobIndex +0x0c）
#include "CGameManager.h"      // WongWork::CHackAnalyzer::addServerHackCnt 声明

void Secu_DungeonAverageCheck::reset()
{
    m_killInfos.clear();
}

Secu_DungeonAverageCheck::Secu_DungeonAverageCheck()
{
    m_party = 0;   // m_killInfos(+4) 自动构造先于 m_party(+0)=0，故置 body(ORIG 序)
}

Secu_DungeonAverageCheck::~Secu_DungeonAverageCheck()
{
}

void Secu_DungeonAverageCheck::SetPartyPtr(CParty* party)
{
    m_party = party;
    reset();
}

void Secu_DungeonAverageCheck::startDungeon()
{
    // ORIG 0x8279490：调用 reset()（reset 内部即 clear）
    reset();
}

// [b28d-24] DIFF 残20: P167 MISSING vector::end×1；ORIG 用 sret 临时 -0x10 + 返回拷贝，我方具名 it 直构（P28/P50 槽阶梯）。ORIG 0x0827993c
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
        if (4 < (unsigned int)it->m_count)
        {
            float avg1 = (float)(unsigned int)it->m_sumValue1 /
                         (float)(unsigned int)it->m_count;
            float avg2 = (float)(unsigned int)it->m_sumValue2 /
                         (float)(unsigned int)it->m_count;

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

            if (m_party->m_mapHitCount > 0 &&
                lower1 <= info->m_killValue1 && info->m_killValue1 <= upper1)
            {
                it->m_hackCntE6 += 1;
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
            if (m_party->m_mapHitCount > 0 &&
                lower2 <= info->m_killValue2 && info->m_killValue2 <= upper2)
            {
                it->m_hackCntE7 += 1;
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
        it->m_count += 1;
        it->m_sumValue1 += (int)info->m_killValue1;
        it->m_sumValue2 += (int)info->m_killValue2;
    }
    else
    {
        m_killInfos.push_back(SecKillMonsterInfo(
            monsterId, 1,
            (unsigned int)info->m_killValue1,
            (unsigned int)info->m_killValue2));
    }
}

// ORIG T 0x08279a12：遍历 m_killInfos；每项按
//   limit = ROUND(0.5 + 0.7*m_count) - 4（rodata 0x8c02788=0.5 / 0x8c02780=0.7）
//   计算超限值 m_hackCntE6/0xE7 - max(limit,0)；任一 >0 时对 4 人队伍逐成员
//   上报 SetETC(0xe8/0xe9, job*100+grow1*10+grow2) 与
//   CHackAnalyzer::addServerHackCnt(user, 0xe8/0xe9, 超限值, SetETC 返回值, 0)。
void Secu_DungeonAverageCheck::CheckHackTypeEndDungeon()
{
    if (m_party == 0)
        return;
    for (std::vector<SecKillMonsterInfo>::iterator it = m_killInfos.begin();
         it != m_killInfos.end(); ++it)
    {
        const int limit =
            (int)(0.5 + 0.7 * (double)(unsigned int)it->m_count) - 4;
        // ORIG P167：调 std::max<int>(limit,0) 两次（三目形丢 2 个调用点）。
        const int overTime = it->m_hackCntE6 - std::max(limit, 0);
        const int overExp = it->m_hackCntE7 - std::max(limit, 0);
        if (overTime > 0 || overExp > 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                if (m_party->checkValidUser(i))
                {
                    CUser* user = m_party->get_user(i);
                    const int code =
                        user->get_charac_job() * 100 +
                        (int)user->getCurCharFirstGrowType() * 10 +
                        (int)user->getCurCharSecondGrowType();
                    if (overTime > 0)
                    {
                        const int r = user->SetETC(0xe8, code);
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe8,
                            (unsigned int)overTime, (unsigned int)r, 0);
                    }
                    if (overExp > 0)
                    {
                        const int r = user->SetETC(0xe9, code);
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACKTYPE)0xe9,
                            (unsigned int)overExp, (unsigned int)r, 0);
                    }
                }
            }
        }
    }
}

void Secu_DungeonAverageCheck::dungeonClear()
{
    // ORIG 0x82799d6：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}

void Secu_DungeonAverageCheck::dungeonFail()
{
    // ORIG 0x82799f4：CheckHackTypeEndDungeon() + reset()。
    CheckHackTypeEndDungeon();
    reset();
}

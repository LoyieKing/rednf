// df_game_r Game/ APSystem::CActionPointManager 实现（批3：从 GameStubs.cpp 迁移）。
// 类声明位于 CUser.h（APSystem::CActionPointManager）。本 TU 是这些符号的唯一权威定义。
// ORIG：GM_Set @0x8121b3e（T）、Reset @0x8122af4（W）、GetActionPoint @0x8694bfc（W）。
#include "CUser.h"
#include <string.h>

namespace APSystem
{

// _ActionGroupIndex / CUserProc 权威声明见 CUserProc.h（本 TU 本地镜像已删）。

// ORIG 0x8694bfc（W，内联）：mov 0x8(%ebp),%eax; add $0x8,%eax; ret。
// 返回嵌入的 _SIG_LOAD_ACTION_POINT 首地址；具体返回类型仍按现有 ABI
// 以字节指针暴露，避免把 packed 包误当成普通对齐对象。
unsigned char* CActionPointManager::GetActionPoint() const
{
    return reinterpret_cast<unsigned char*>(
        const_cast<_SIG_LOAD_ACTION_POINT*>(&m_sig));
}

// ORIG 0x8121b3e：m_flag00=1；m_sig.m_apSum=0；m_prevApSum=0；
// 先把所有已领条目的五个日期槽取负标记；再按行/槽扫描，在行动点总数
// 未达目标 value 前逐项恢复、累加并发包通知。_ActionPoint 已按 ORIG
// 0xb 字节 packed，具名成员访问仍保持 +0x00/+0x02/+0x06 的真实布局。
void CActionPointManager::GM_Set(CUser& user, unsigned int value)
{
    m_flag00 = 1;
    m_sig.m_apSum = 0;
    m_prevApSum = 0;
    for (int row = 0; row <= 299; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0; slot <= 4; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry > 0)
            {
                rec.m_days[slot] = static_cast<unsigned char>(-entry);
            }
        }
    }
    for (int row = 0;
         row <= 299 && m_sig.m_apSum < value; ++row)
    {
        _ActionPoint& rec = m_sig.m_actionPoints[row];
        for (int slot = 0;
             slot <= 4 && m_sig.m_apSum < value; ++slot)
        {
            const signed char entry =
                static_cast<signed char>(rec.m_days[slot]);
            if (entry < 0)
            {
                m_prevApSum = m_sig.m_apSum;
                const unsigned char restored =
                    static_cast<unsigned char>(-entry);
                rec.m_days[slot] = restored;
                m_sig.m_apSum += static_cast<signed char>(restored);
                const _ActionGroupIndex group =
                    static_cast<_ActionGroupIndex>(rec.m_groupIndex);
                CUserProc::SendActionClear(user, rec.m_actionNo, group, true,
                                           ENUM_PACKETCLASS_0);
                CUserProc::CheckAndSendToMessageAllUser(
                    m_prevApSum, m_sig.m_apSum, user.getCurCharacName());
            }
        }
    }
}

// ORIG 0x08122af4（W）：m_flag00=0；m_prevApSum=0；
// memset(&m_sig, 0, sizeof(_SIG_LOAD_ACTION_POINT))。无负标记扫描循环。
void CActionPointManager::Reset()
{
    m_flag00 = 0;
    m_prevApSum = 0;
    memset(&m_sig, 0, sizeof(m_sig));
}

} // namespace APSystem

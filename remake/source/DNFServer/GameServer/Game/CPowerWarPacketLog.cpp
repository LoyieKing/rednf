// df_game_r 还原 —— CPowerWarPacketLog 实现。
// 报告：docs/class_func_reports/CPowerWarPacketLog/（CPowerWarPacketLog /
// InsertPacketLog / PrintPowerWarPacketLogReport / Reset / ~CPowerWarPacketLog）。
#include <algorithm>
#include <vector>

#include "CPowerWarPacketLog.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0848085a：map 成员默认构造 + Reset（无 vptr 写入）。
CPowerWarPacketLog::CPowerWarPacketLog()
{
    Reset();
}

// ORIG D1 0x0848089c：Reset() + map 成员析构（Reset 已 clear，析构为空操作）。
CPowerWarPacketLog::~CPowerWarPacketLog()
{
    Reset();
}

// ORIG Reset 0x084808de：遍历 map，delete 每个 STPacketLog（析构+释放），
// 最后 map::clear()。
void CPowerWarPacketLog::Reset()
{
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        STPacketLog* log = it->second;
        if (log != 0)
            delete log;
    }
    m_map.clear();
}

// ORIG InsertPacketLog 0x08480970：以 (unsigned int)t 为 key find；
// 命中 → ++(元素->m_count)；未命中 → new STPacketLog(0x14) 初始化
// （m_count=1、m_field8=t、m_sendType=type、m_field4=i、m_name.assign(name)）
// 后 make_pair(t, log) insert。
void CPowerWarPacketLog::InsertPacketLog(int i, unsigned short t,
                                         eSendType type, char const* name)
{
    std::map<int, STPacketLog*>::iterator it = m_map.find(t);
    if (it != m_map.end())
    {
        STPacketLog* found = it->second;
        found->m_count = found->m_count + 1;
    }
    else
    {
        STPacketLog* log = new STPacketLog;
        log->m_count = 1;
        log->m_field8 = t;
        log->m_sendType = type;
        log->m_field4 = i;
        log->m_name.assign(name);
        m_map.insert(std::make_pair(t, log));
    }
}

// ORIG PrintPowerWarPacketLogReport 0x08480ac0：
//   cMyTrace 三段头 → 收集 vector<STPacketLog*> → sort(Compare) →
//   逐条 cMyTrace 打印 → 尾分隔线。
void CPowerWarPacketLog::PrintPowerWarPacketLogReport()
{
    cMyTrace trace1(__PRETTY_FUNCTION__, 0x3a1, 0);
    trace1("-------------------------------------------------------------------------------------------------------------");
    cMyTrace trace2(__PRETTY_FUNCTION__, 0x3a2, 0);
    trace2("|                                   POWAR WAR PACKET RESULT REPORT                                          |");
    cMyTrace trace3(__PRETTY_FUNCTION__, 0x3a3, 0);
    trace3("-------------------------------------------------------------------------------------------------------------");

    std::vector<STPacketLog*> vec;
    for (std::map<int, STPacketLog*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        vec.push_back(it->second);
    }
    std::sort(vec.begin(), vec.end(), STPacketLog::Compare);

    for (std::vector<STPacketLog*>::iterator it = vec.begin();
         it != vec.end(); ++it)
    {
        STPacketLog* log = *it;
        cMyTrace trace4(__PRETTY_FUNCTION__, 0x3b1, 0);
        trace4("- PACKET CLASS:%d NO:%5d, COUNT:%8d, NAME:%s",
               log->m_field4, log->m_field8, log->m_count, log->m_name.c_str());
    }

    cMyTrace trace5(__PRETTY_FUNCTION__, 0x3b6, 0);
    trace5("-------------------------------------------------------------------------------------------------------------");
}

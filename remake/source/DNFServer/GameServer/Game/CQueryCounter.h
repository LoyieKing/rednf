#ifndef DNF_GAME_CQUERYCOUNTER_H_
#define DNF_GAME_CQUERYCOUNTER_H_

// ============================================================================
// df_game_r 还原 —— CQueryCounter（SQL 查询计数器，D4 批次）
// 参照 docs/class_func_reports/CQueryCounter.md + df_game_r 反汇编。
// 布局（sizeof = 0x3c）：
//   +0x00 bool m_bLogDB（setLogDB；true 时经 MsgQueueMgr DB_Q 交 DBMW 落库）
//   +0x04 map<int, QUERY_INFO> m_queryInfoMap
//   +0x1c map<string, map<int,QUERY_INFO>::iterator> m_queryIDMap
//   +0x34 map<int,QUERY_INFO>::iterator m_current
//   +0x38 int m_totalQueryCount
// QUERY_INFO 12 字节：count(+0) / responseTime(+4, double)。
// 查询字符串含 "log_query_ref" 或 "from db_connect" 时不计数。
// ============================================================================

#include <map>
#include <string>

#include "DBMgr.h"

class StreamPool;
class MsgQueueMgr;

class CQueryCounter
{
public:
    struct QUERY_INFO
    {
        QUERY_INFO()
        {
            count = 0;
            responseTime = 0.0;
        }

        int count;            // +0
        double responseTime;  // +4
    };

    CQueryCounter();          // 0x085ecaec
    ~CQueryCounter();         // 0x085ecb8a
    void Reset();             // 0x085ecbd6
    void IncreaseQueryCount(const std::string& query);  // 0x085ecc5e
    void UpdateQueryCounter();  // 0x085ecee6
    void SetResponseTime(double responseTime);  // 0x085ed108
    bool LoadQueryTable();    // 0x085ed18a
    bool SaveQuery(const char* query, int& qid);  // 0x085ed3a2
    bool RegisterQuery(const char* query, int qid, int type);  // 0x085ed5ae

    void setLogDB(bool bLogDB);  // 0x0844d63a（弱）
    std::map<int, QUERY_INFO>& GetQueryCountTable();  // 0x0844d652（弱）

    bool m_bLogDB;            // +0x00
    std::map<int, QUERY_INFO> m_queryInfoMap;  // +0x04
    std::map<std::string, std::map<int, QUERY_INFO>::iterator> m_queryIDMap;  // +0x1c
    std::map<int, QUERY_INFO>::iterator m_current;  // +0x34
    int m_totalQueryCount;    // +0x38
};

inline void CQueryCounter::setLogDB(bool bLogDB)
{
    m_bLogDB = bLogDB;
}

inline std::map<int, CQueryCounter::QUERY_INFO>& CQueryCounter::GetQueryCountTable()
{
    return m_queryInfoMap;
}

#endif  // DNF_GAME_CQUERYCOUNTER_H_

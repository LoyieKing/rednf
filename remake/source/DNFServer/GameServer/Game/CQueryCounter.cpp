// df_game_r 还原 —— CQueryCounter（D4 批次）
// 参照 docs/class_func_reports/CQueryCounter.md；源文件 QueryCounter.cpp。
// [G5 2026-09-10 · L12 round2 组5 复核] 5 件全核（判据⑩/⑪ + p167 + 归一化 asm 对比）：
//  · **真缺口修复 1**：IncreaseQueryCount —— p167 显示我方多 1×
//    `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`
//    （const operator->）；`(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv），
//    改 `m_current->second.count++`（后置自增，P38 反向）后 calls 30→29、
//    **CALLSITE CLEAN**（真缺口清零）。
//  · LoadQueryTable/SaveQuery/RegisterQuery 的 7 处具名 cMyTrace 改全表达式临时链（P227）：
//    LoadQueryTable 实测四数不变（中性，保留）。
//  · 残余 N 族：SaveQuery/LoadQueryTable 的 `memset(buf,0,N)` 常量经 %edx 中转
//    （ORIG 0x085ed3a2/0x085ed18a `mov $N,%edx; mov %edx,0x8(%esp)`）—— 本波探针
//    /tmp/probe_memset.cpp f1..f4（普通/sizeof/unsigned/cast）四形恒直写
//    `movl $N,0x8(%esp)` ⇒ N 封顶（P238 实参变体，无源形）；UpdateQueryCounter 的
//    nop/尾岛；RegisterQuery 槽分区（N95/N82，槽集合两侧全等）；IncreaseQueryCount
//    残余槽差 -0x1c↔-0x18（CStreamGuard 对象区）。
#include "CQueryCounter.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GameBase.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- ORIG 全局工具函数（属 DB 工具 TU，0x083fbc66）----
int GetIdentityFromDB(MySQL* mysql);

CQueryCounter::CQueryCounter()
{
    m_current = m_queryInfoMap.end();
    m_totalQueryCount = 0;
}

CQueryCounter::~CQueryCounter()
{
}

void CQueryCounter::Reset()
{
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); ++it)
    {
        it->second.count = 0;
        it->second.responseTime = 0.0;
    }
    m_totalQueryCount = 0;
}

// [b28d-62] 修：P220 极性翻转（`it == end` 直落未命中分支）+ m_bLogDB 正极性。
//   ORIG 0x085ecc5e
void CQueryCounter::IncreaseQueryCount(const std::string& query)
{
    if (query.find("log_query_ref", 0) != std::string::npos)
    {
        return;
    }
    if (query.find("from db_connect", 0) != std::string::npos)
    {
        return;
    }
    std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator it =
        m_queryIDMap.find(query);
    // [b28d-62] P220 极性翻转：ORIG 0x085eccfc 调 operator==（`it == end` 为真直落
    //   未命中分支），我方 `!=` 生相反块序；m_bLogDB 正极性（`test %al; je else`，
    //   ORIG 0x085ecd0f 实证）⇒ 未命中分支内先 stream 后 SaveQuery。
    if (it == m_queryIDMap.end())
    {
        if (m_bLogDB)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1d2), true);
            **guard << 0xbe;
            **guard << -1;
            **guard << (int)query.size();
            guard->put_binary((void*)query.c_str(), (int)query.size());
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            int qid = 0;
            if (SaveQuery(query.c_str(), qid))
            {
                RegisterQuery(query.c_str(), qid, 0);
            }
        }
    }
    else
    {
        m_current = it->second;
        // [G5 2026-09-10 P167] ORIG 0x085ecc5e 调用差集：我方多 1×
        // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`。
        // 实测 `(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv×2）⇒ 维持
        // `->` 形；试后置自增（P38 反向：`= x+1` 单语句 SAVE_EXPR 合并成单次 ptEv，
        // 拆/`++` 形可出第二处访问）。
        if (m_current->second.count == 0)
        {
            m_totalQueryCount = m_totalQueryCount + 1;
        }
        m_current->second.count++;
    }
}

// [b28d-62] 修：迭代器增量改后置 `it++`（P28 增量侧：ORIG 调 operator++(int)）。
//   ORIG 0x085ecee6
void CQueryCounter::UpdateQueryCounter()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1f1), true);
    **guard << 0x7d;
    **guard << -1;
    int curTime = time(0);
    **guard << curTime;
    **guard << m_totalQueryCount;
    // [b28d-62] P28 增量侧：ORIG 增量段调 operator++(int)（后置，带独立 sret
    //   临时槽）⇒ 源形必写 `it++`（`++it` 走 ppEv 免临时槽）。
    for (std::map<int, QUERY_INFO>::iterator it = m_queryInfoMap.begin();
         it != m_queryInfoMap.end(); it++)
    {
        int qid = it->first;
        int count = it->second.count;
        int response = (int)(it->second.responseTime * 1000.0);
        if (count > 0)
        {
            **guard << qid;
            **guard << count;
            **guard << response;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    Reset();
}

void CQueryCounter::SetResponseTime(double responseTime)
{
    if (m_current != m_queryInfoMap.end())
    {
        m_current->second.responseTime += responseTime;
        m_current = m_queryInfoMap.end();
    }
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed18a
bool CQueryCounter::LoadQueryTable()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    int ret = 0;
    char query[0x6001];
    mysql->set_query("seLect q_id,query from log_query_ref");
    if (!mysql->exec(true))
    {
        return false;
    }
    int nRows = mysql->get_n_rows();
    int i = 0;    // P191 ORIG -0xc 双 init 形
    for (i = 0; i < nRows; i = i + 1)
    {
        int qid = 0;
        memset(query, 0, 0x6001);
        if (!mysql->fetch())
        {
            // [G5 2026-09-10 P227 试验] ORIG 0x085ed18a 段为 cMyTrace ctor 后直接
            // operator()（varargs 在 ctor 前右起求值并保活 callee-saved）⇒ 改全表达式
            // 临时链；具名两语句形使 4.4 把 ctor 提前、帧 0x…。
            cMyTrace(__PRETTY_FUNCTION__, 0x22f, 5)("LoadQueryTable, fatch ERROR");
            return false;
        }
        if (!mysql->get_int(0, qid))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x235, 5)("LoadQueryTable, get_int(0) ERROR");
            return false;
        }
        if (!mysql->get_str(1, query, 0x6000))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x23b, 5)("LoadQueryTable, get_str(1) ERROR");
            return false;
        }
        if (!RegisterQuery(query, qid, 0))
        {
            return false;
        }
    }
    return true;
}

// [b28d-62] N82：p167 CLEAR。ORIG 0x085ed3a2
bool CQueryCounter::SaveQuery(const char* query, int& qid)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    char escaped[0xc001];
    memset(escaped, 0, 0xc001);
    mysql->escape_string(escaped, query);
    mysql->set_query("inSert into log_query_ref(query, query_hash) values('%s',password('%s'))",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        qid = GetIdentityFromDB(mysql);
        return true;
    }
    mysql->set_query("seLect q_id from log_query_ref where query_hash=password('%s')",
                     escaped, escaped);
    if (mysql->exec(true))
    {
        if (mysql->get_n_rows() == 0)
        {
            return false;
        }
        if (mysql->fetch())
        {
            if (mysql->get_int(0, qid))
            {
                return true;
            }
            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 5);
            trace("SaveQuery, get_int(0) ERROR");
            return false;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x271, 5);
        trace("SaveQuery, fetch() ERROR");
        return false;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x265, 5);
    trace("SaveQuery, exec() ERROR");
    return false;
}

// [b28d-62] N95/N82：槽基整体差 0x10（4.4 槽基自 ebp-8 起，N95）；p167 CLEAR。
//   ORIG 0x085ed5ae
bool CQueryCounter::RegisterQuery(const char* query, int qid, int type)
{
    QUERY_INFO info;
    info.count = type;
    std::pair<std::map<int, QUERY_INFO>::iterator, bool> ins =
        m_queryInfoMap.insert(std::make_pair(qid, info));
    if (!ins.second)
    {
        return false;
    }
    std::pair<std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator, bool> ins2 =
        m_queryIDMap.insert(std::make_pair(query, ins.first));
    if (!ins2.second)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x291, 0);
        trace("LoadQueryTable,  query_id_map_.insert ERROR");
        return false;
    }
    return true;
}

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff）。
#include <iostream>

#include "LogManager.h"
// df_game_r 还原 —— MySQL（D4 批次）
// 参照 docs/class_func_reports/MySQL.md；源文件 DBMgr.cpp（ORIG 日志文件名）。
#include "MySQL.h"

#include <cstdarg>
#include <stdio.h>
#include <string.h>

#include "CQueryCounter.h"
#include "CUnixTimer.h"
#include "GameBase.h"

// ---- cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）----
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};
// [T1-GI · P327] MySQL ctor 定义前移为本 TU 首个「强符号 · 外部链接 · 本 TU 定义」
// 实体 ⇒ GI 键名 `_GLOBAL__I__ZN5MySQLC2Ev`（ORIG 0x83f52ff；4.4 外联 ctor 发 C1+C2，
// C2 先入 assemble 队列）。改前键名 = `_GLOBAL__I__ZN9CDnFTimer11SetLastTimeEv`。
MySQL::MySQL() throw()
{
    m_dbHandle = 0;
    m_pQueryCounter = 0;
    m_bQueryCounterEnable = false;
}

// ---- CDnFTimer / CDnFTimerFactory（G7 计时族，ORIG 弱符号；算法正确即可）----
void CDnFTimer::SetLastTime() {}
double CDnFTimer::GetTimeInterval() { return 0.0; }
CDnFTimer* CDnFTimerFactory::CreateDnFTimer() { return new CUnixTimer; }
void CDnFTimerFactory::DestroyDnFTimer(CDnFTimer* timer) { delete timer; }


MySQL::~MySQL() throw()
{
    close();
}

bool MySQL::set_compress_option()
{
    if (mysql_options(m_dbHandle, MYSQL_OPT_COMPRESS, 0) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x40, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x40);
        return false;
    }
    return true;
}

bool MySQL::set_read_default_grp_option()
{
    if (mysql_options(m_dbHandle, MYSQL_READ_DEFAULT_GROUP, "UseSQL") != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4f, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x4f);
        return false;
    }
    return true;
}

bool MySQL::set_charset_name_option()
{
    return true;
}

bool MySQL::set_reconnect_option()
{
    return true;
}

bool MySQL::init_db_handle()
{
    if (m_dbHandle != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x81, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x81);
        return false;
    }
    m_dbHandle = mysql_init(0);
    if (m_dbHandle == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8c, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x8c);
        return false;
    }
    return true;
}

bool MySQL::init()
{
    bool ret = init_db_handle();
    if (ret != true)
    {
        return false;
    }
    ret = set_compress_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_read_default_grp_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_charset_name_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_reconnect_option();
    if (ret != true)
    {
        return false;
    }
    memset(m_query, 0, 0x6001);
    m_queryLen = 0;
    m_result = 0;
    m_lengths = 0;
    m_nRows = 0;
    m_nFields = 0;
    memset(m_host, 0, 0x10);
    memset(m_user, 0, 0x14);
    memset(m_db, 0, 0x1e);
    memset(m_passwd, 0, 0x14);
    return true;
}

void MySQL::close()
{
    if (m_dbHandle != 0)
    {
        mysql_close(m_dbHandle);
        m_dbHandle = 0;
    }
}

bool MySQL::open(char* host, char* db, char* user, char* passwd)
{
    strcpy(m_db, db);
    strcpy(m_host, host);
    strcpy(m_user, user);
    strcpy(m_passwd, passwd);
    if (mysql_real_connect(m_dbHandle, host, user, passwd, db, 0xcea, 0, 0x400) == 0)
    {
        register int err = mysql_errno(m_dbHandle);  // P87：ORIG ebx 跨 C1 调用缓存
        cMyTrace(__PRETTY_FUNCTION__, 0x10a, 9, true, true)(
            "\n\t- mysql_real_connect(ip:%s, port:%d, name:%s, id:%s error='%d') fail",
            host, 0xcea, db, user, err);
        cMyTrace(__PRETTY_FUNCTION__, 0x10e, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x10e);
        return false;
    }
    return true;
}

bool MySQL::open(char* host, unsigned int port, char* db, char* user, char* passwd)
{
    if (port == 0)
    {
        port = 0xcea;
    }
    strcpy(m_db, db);
    strcpy(m_host, host);
    m_port = port;
    strcpy(m_user, user);
    strcpy(m_passwd, passwd);
    if (mysql_real_connect(m_dbHandle, host, user, passwd, db, port, 0, 0) == 0)
    {
        register int err = mysql_errno(m_dbHandle);  // P87
        cMyTrace(__PRETTY_FUNCTION__, 0x138, 9, true, true)(
            "\n\t- mysql_real_connect(ip:%s, port:%d, name:%s, id:%s error='%d') fail",
            host, port, db, user, err);
        cMyTrace(__PRETTY_FUNCTION__, 0x13c, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x13c);
        return false;
    }
    return true;
}

bool MySQL::set_query(const char* fmt, ...)
{
    if (m_bQueryCounterEnable)
    {
        if (m_pQueryCounter == 0)
        {
            return false;
        }
        std::allocator<char> alloc;
        std::string query(fmt, alloc);
        m_pQueryCounter->IncreaseQueryCount(query);
    }
    // [b28d-lane11] ORIG 槽序 ap(-0x28 最深)→query(-0x24)→alloc(-0x1d)；
    // 4.4 对块内局部优先占深槽，故把 ap 段包进独立块使 ap 落深槽。
    {
        va_list ap;
        va_start(ap, fmt);
        vsprintf(m_query, fmt, ap);
        va_end(ap);
    }
    int len = strlen(m_query);
    if (len > 0x5fff)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x169, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x169);
        return false;
    }
    m_queryLen = len;
    return true;
}

unsigned long long MySQL::getAffectedRowCount()
{
    return mysql_affected_rows(m_dbHandle);
}

bool MySQL::exec(bool bLog)
{
    // b28d-lane11 判词 N7（本 TU 447 路由）：残余仅 3 处间接 call 目标寄存器
    // 差——ORIG 0x083f433f/0x083f437c/0x083f43da `mov (%eax),%edx … call *%edx`，
    // 4.4.7 恒落 %eax（P230 补：该形唯 4.4.6 后端可翻，本 TU 非 446h46 路由）。
    // m_nFields int→unsigned 后 +0x18 store 已回 LHS-first（P248 档③）。
    int ret;
    CDnFTimer* timer = CDnFTimerFactory::CreateDnFTimer();
    timer->SetLastTime();
    for (int retry = 0; retry < 5; retry = retry + 1)
    {
        ret = exec_query();
        if (ret == 1)
        {
            if (m_bQueryCounterEnable)
            {
                m_pQueryCounter->SetResponseTime(timer->GetTimeInterval());
            }
            CDnFTimerFactory::DestroyDnFTimer(timer);
            return false;
        }
        if (ret == 0)
        {
            break;
        }
    }
    if (m_bQueryCounterEnable)
    {
        m_pQueryCounter->SetResponseTime(timer->GetTimeInterval());
        CDnFTimerFactory::DestroyDnFTimer(timer);
    }
    if (ret == 0)
    {
        m_result = mysql_store_result(m_dbHandle);
        if (m_result != 0)
        {
            m_nRows = (int)mysql_num_rows(m_result);
            m_nFields = (int)mysql_num_fields(m_result);
        }
        else
        {
            m_nRows = 0;
            m_nFields = 0;
        }
        return true;
    }
    if (bLog)
    {
        LogManager::logFormat(1, "DBMgr.cpp", __PRETTY_FUNCTION__, 0x244,
                              "Database query error. The last query('%s') has been lost.(%d)",
                              m_query, m_dbError);
    }
    return false;
}

bool MySQL::fetch()
{
    if (m_result == 0)
    {
        return false;
    }
    m_row = mysql_fetch_row(m_result);
    if (m_row != 0)
    {
        m_lengths = mysql_fetch_lengths(m_result);
        return true;
    }
    return false;
}

int MySQL::ping()
{
    return mysql_ping(m_dbHandle);
}

char* MySQL::blob_to_str(int idx, void* data, int len)
{
    if (idx < 0 || idx > 9 || data == 0 || len > 0x5fff)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x267, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x267);
    }
    m_blob[idx][0] = 0;
    if (len > 0)
    {
        char* p = m_blob[idx];
        p = p + mysql_real_escape_string(m_dbHandle, p, (const char*)data, len);
        *p++ = 0;
    }
    return m_blob[idx];
}

const char* MySQL::getServerVersion()
{
    return mysql_get_server_info(m_dbHandle);
}

const char* MySQL::getClientVersion()
{
    return mysql_get_client_info();
}

int MySQL::select()
{
    if (!set_query())
    {
        return 3;
    }
    if (!exec(true))
    {
        return 1;
    }
    return 0;
}

int MySQL::select(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    return select();
}

int MySQL::insert(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    if (!set_query())
    {
        return 3;
    }
    if (!exec(true))
    {
        return 5;
    }
    return 0;
}

int MySQL::update(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    if (!set_query())
    {
        return 3;
    }
    if (!exec(true))
    {
        return 5;
    }
    return 0;
}

bool MySQL::set_query()
{
    if (m_bQueryCounterEnable)
    {
        if (m_pQueryCounter == 0)
        {
            return false;
        }
        std::allocator<char> alloc;
        std::string query(m_query, alloc);
        m_pQueryCounter->IncreaseQueryCount(query);
    }
    int len = strlen(m_query);
    if (len > 0x5fff)
    {
        LogManager::logFormat(1, "DBMgr.cpp", __PRETTY_FUNCTION__, 0x2d1,
                              "[%s][%d]", __PRETTY_FUNCTION__, 0x2d1);
        return false;
    }
    m_queryLen = len;
    return true;
}

unsigned long MySQL::escape_string(char* dst, const char* src)
{
    return mysql_real_escape_string(m_dbHandle, dst, src, strlen(src));
}

void MySQL::clear_result_set()
{
    if (m_result != 0)
    {
        mysql_free_result(m_result);
    }
    m_result = 0;
    m_row = 0;
}

int MySQL::exec_query()
{
    clear_result_set();
    int ret = mysql_real_query(m_dbHandle, m_query, m_queryLen);
    if (ret != 0)
    {
        m_dbError = mysql_errno(m_dbHandle);
        if (m_dbError == 0x7d5 || m_dbError == 0x7dd || m_dbError == 0x7d6)
        {
            // b28d-lane11：P221 全表达式临时链（翻互斥块对象槽方向）。
            cMyTraceNoop()("DB connection lost, reconnecting...");
            int ping = mysql_ping(m_dbHandle);
            if (ping != 0)
            {
                if (mysql_errno(m_dbHandle) == 0x7d6)
                {
                    // b28d-lane11：P0 内联（ORIG 0x083f54c4 `call mysql_real_connect;
                    // test %eax,%eax; sete %al; test %al,%al; je` 无 conn 槽往返）。
                    if (mysql_real_connect(m_dbHandle, m_host, m_user, m_passwd,
                                           m_db, m_port, 0, 0) == 0)
                    {
                        LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x19a,
                                              "DB reconnection fail. err_no(%d)",
                                              mysql_errno(m_dbHandle));
                    }
                    else
                    {
                        LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x19e,
                                              "DB Reconnect By Server Gone Error");
                    }
                    LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x1a1,
                                          "DB reconnection fail. %d (dbname : %s, ip : %s, port : %d, id : %s)",
                                          ping, m_db, m_host, m_port, m_user);
                }
            }
            return 2;
        }
        if (m_dbError != 0x426)
        {
            // b28d-lane11 真语义修正：ORIG 0x083f5573-0x083f558b 此处为 fresh
            // `call mysql_errno(m_dbHandle)`（`lea 0x1c(%eax),%ebx` 先缓存 m_query，
            // call 后 %eax 压 0x14(%esp)），非读缓存字段 m_dbError（字段读仅用于
            // 紧随其后的 ==0x7d6 判定，ORIG 0x083f558f）。
            LogManager::logFormat(1, "DBMgr.cpp", "bool MySQL::exec_query()", 0x1af,
                                  "DB error occured (%d) Query('%s')",
                                  mysql_errno(m_dbHandle), m_query);
            if (m_dbError == 0x7d6)
            {
                cMyTrace("bool MySQL::exec_query()", 0x1b2, 5)(
                    "DB error occured Check Connection First, Must Be Not Connected!");
            }
        }
        return 1;
    }
    return 0;
}

// ============================================================================
// GetIdentityFromDB（ORIG 0x83fbc66 T）
// 包装 WongWork::DBCommon::GetIdentity（0x83f9ad4）：查询 @@identity（上次
// INSERT 的自增 ID），供 CQueryCounter::insertCounter 取 q_id 使用。
// ORIG：call WongWork::DBCommon::GetIdentity → leave/ret。
// ============================================================================
namespace WongWork {

class DBCommon
{
public:
    static unsigned int GetIdentity(MySQL* db);  // ORIG T 0x083f9ad4
};

}  // namespace WongWork

int GetIdentityFromDB(MySQL* mysql)
{
    return (int)WongWork::DBCommon::GetIdentity(mysql);
}

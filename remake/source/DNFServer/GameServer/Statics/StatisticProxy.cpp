// df_statics_r — StatisticProxy（ORIG StatisticProxy.o 拆分）
#include <stdio.h>
#include <string.h>

#include "StatisticProxy.h"
#include "PacketHeader.h"

statistc_proxy::StatisticProxy* getStatisticProxy()
{
    static statistc_proxy::StatisticProxy statisticProxy;
    return &statisticProxy;
}
PacketInsertUpdate::PacketInsertUpdate()
    : PacketHeader(0x2721, 0x1018)
{
    m_handleIdx = 0;
    m_updateQueryId = 0;
    m_insertQueryId = 0;
    int i = 0;
    for (i = 0; i <= 0x800; i++)
    {
        m_updateSql[i] = 0;
    }
    for (i = 0; i <= 0x800; i++)
    {
        m_insertSql[i] = 0;
    }
}
namespace statistc_proxy
{
void (*StatisticProxy::sendPacketFunctionPointer)(char*) = 0;
static std::string g_updateCondition;

Field::Field()
    : m_fields()
{
    reset();
}

Field::Field(const Field& other)
    : m_fields(other.m_fields)
{
}

Field::~Field()
{
}

void Field::reset()
{
    m_fields.clear();
}

void Field::add(const char* name, unsigned int value)
{
    std::map<std::string, unsigned long long>::iterator it = m_fields.find(name);
    if (it != m_fields.end())
    {
        it->second += value;
    }
    else
    {
        m_fields.insert(std::make_pair(name, value));
    }
}

char* Field::getFieldName()
{
    static std::string queryPiece;
    // [2026-09-14 修复] ORIG df_statics_r 里这三个函数级静态 `tmp` 的大小都是
    //   **0x400**（由 global_size_audit.py 扫出：ORIG 0x400 vs 本地 0x20）。
    //   本地只按 `%llu` 最长 20 字符的需要开了 32 字节；功能上够用，但对象
    //   大小与 ORIG 不一致 ⇒ 按 ORIG 对齐成 0x400。
    static char tmp[0x400];
    queryPiece.clear();
    for (std::map<std::string, unsigned long long>::iterator it = m_fields.begin();
         it != m_fields.end(); ++it)
    {
        if (queryPiece.length() != 0)
        {
            queryPiece += ",";
        }
        queryPiece += it->first.c_str();
    }
    return (char*)queryPiece.c_str();
}

char* Field::getFieldValue()
{
    static std::string queryPiece;
    // [2026-09-14 修复] ORIG df_statics_r 里这三个函数级静态 `tmp` 的大小都是
    //   **0x400**（由 global_size_audit.py 扫出：ORIG 0x400 vs 本地 0x20）。
    //   本地只按 `%llu` 最长 20 字符的需要开了 32 字节；功能上够用，但对象
    //   大小与 ORIG 不一致 ⇒ 按 ORIG 对齐成 0x400。
    static char tmp[0x400];
    queryPiece.clear();
    for (std::map<std::string, unsigned long long>::iterator it = m_fields.begin();
         it != m_fields.end(); ++it)
    {
        if (queryPiece.length() != 0)
        {
            queryPiece += ",";
        }
        sprintf(tmp, "%llu", it->second);
        queryPiece += tmp;
    }
    return (char*)queryPiece.c_str();
}

char* Field::getUpdateValue()
{
    static std::string queryPiece;
    // [2026-09-14 修复] ORIG df_statics_r 里这三个函数级静态 `tmp` 的大小都是
    //   **0x400**（由 global_size_audit.py 扫出：ORIG 0x400 vs 本地 0x20）。
    //   本地只按 `%llu` 最长 20 字符的需要开了 32 字节；功能上够用，但对象
    //   大小与 ORIG 不一致 ⇒ 按 ORIG 对齐成 0x400。
    static char tmp[0x400];
    queryPiece.clear();
    for (std::map<std::string, unsigned long long>::iterator it = m_fields.begin();
         it != m_fields.end(); ++it)
    {
        if (queryPiece.length() != 0)
        {
            queryPiece += ",";
        }
        queryPiece += it->first.c_str();
        queryPiece += "=";
        queryPiece += it->first.c_str();
        queryPiece += "+";
        sprintf(tmp, "%llu", it->second);
        queryPiece += tmp;
    }
    return (char*)queryPiece.c_str();
}

bool Field::MakeInsertQuery(char* out, const char* table, const std::string& key,
                            const std::string& cond)
{
    snprintf(out, 0x800, "inSert into %s(%s,%s) values(%s,%s)", table, cond.c_str(),
             getFieldName(), key.c_str(), getFieldValue());
    // [2026-09-15] ORIG 为分支物化形（cmp $0x7ff/jbe → mov $1/jmp → mov $0），
    // 非 setbe；真分支落在后方 ⇒ 条件写负极性早返（P56/P347）。
    if (strlen(out) > 0x7ff)
    {
        return false;
    }
    return true;
}

bool Field::MakeUpdateQuery(char* out, const char* table, const std::string& key,
                            const std::string& cond)
{
    // [2026-09-15] ORIG 调用点实参序为 (cond, key)：反汇编实证
    //   0x804cf93 mov 0x14(%ebp)->0x8(%esp) / 0x804cf9a mov 0x18(%ebp)->0x4(%esp)
    //   ⇒ 第一实参 = cond（getUpdateCondition 内第一形参承担 substr 角色）。
    //   返回 char*（原为 void + 全局缓冲），改函数级 static 以复现 ORIG 的
    //   _ZZN..getUpdateCondition..E10queryPiece / _ZGVZ.. guard 组。
    snprintf(out, 0x800, "update %s set %s where %s", table, getUpdateValue(),
             getUpdateCondition(cond, key));
    if (strlen(out) > 0x7ff)
    {
        return false;
    }
    return true;
}

const char* Field::getUpdateCondition(const std::string& key, const std::string& cond)
{
    static std::string queryPiece;
    queryPiece.clear();
    // [2026-09-15] ORIG 声明/首次使用序为 prev1→prev2→pos1→pos2（反汇编实证：
    //   0x804cae0 movl $0,-0x28 / 0x804cae7 movl $0,-0x24 先于两次 find_first_of），
    //   4.4 与 4.1 均按"先声明更深"分配槽 ⇒ 顺序须照抄。
    size_t prev1 = 0;
    size_t prev2 = 0;
    size_t pos1 = key.find_first_of(",", 0);
    size_t pos2 = cond.find_first_of(",", 0);
    while (pos1 != std::string::npos && pos2 != std::string::npos)
    {
        if (queryPiece.length() != 0)
        {
            queryPiece += " and ";
        }
        queryPiece += key.substr(prev1, pos1 - prev1);
        queryPiece += "=";
        queryPiece += cond.substr(prev2, pos2 - prev2);
        prev1 = pos1 + 1;
        prev2 = pos2 + 1;
        pos2 = cond.find_first_of(",", pos2 + 1);
        pos1 = key.find_first_of(",", pos1 + 1);
    }
    if (queryPiece.length() != 0)
    {
        queryPiece += " and ";
    }
    queryPiece += key.substr(prev1);
    queryPiece += "=";
    queryPiece += cond.substr(prev2);
    return queryPiece.c_str();
}

int Field::updateDatabase(const char* table, const std::string& key, const std::string& cond)
{
    PacketInsertUpdate p;
    p.m_handleIdx = 4;
    p.m_updateQueryId = 0x4f2b;
    p.m_insertQueryId = 0x4f2c;
    MakeInsertQuery(p.m_updateSql, table, key, cond);
    MakeUpdateQuery(p.m_insertSql, table, key, cond);
    (*StatisticProxy::sendPacketFunctionPointer)((char*)&p);
    return 1;
}

Table::Table()
    : m_fields(), m_key()
{
    reset();
}

Table::Table(const Table& other)
    : m_fields(other.m_fields), m_nValue(other.m_nValue), m_bFlag(other.m_bFlag),
      m_key(other.m_key)
{
}

Table::~Table()
{
}

void Table::setKey(const char* key)
{
    m_key = key;
}

void Table::add(const char* name, unsigned int value, const char* key)
{
    std::map<std::string, Field>::iterator it = m_fields.find(key);
    if (it != m_fields.end())
    {
        it->second.add(name, value);
    }
    else
    {
        Field f;
        f.add(name, value);
        m_fields.insert(std::make_pair(key, f));
    }
}

void Table::resetValue()
{
    for (std::map<std::string, Field>::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
    {
        it->second.reset();
    }
}

int Table::updateDatabase(const char* table)
{
    for (std::map<std::string, Field>::iterator it = m_fields.begin(); it != m_fields.end(); ++it)
    {
        it->second.updateDatabase(table, it->first, m_key);
    }
    return 1;
}

void Table::reset()
{
    m_fields.clear();
    m_nValue = 0;
    m_bFlag = 0;
}

StatisticProxy::StatisticProxy()
    : m_tables()
{
}

StatisticProxy::~StatisticProxy()
{
}

bool StatisticProxy::registTable(const char* name, Table& table)
{
    return m_tables.insert(std::make_pair(name, table)).second;
}

void StatisticProxy::add(const char* name, unsigned int value, const char* key,
                         const char* table)
{
    std::map<std::string, Table>::iterator it = m_tables.find(table);
    if (it != m_tables.end())
    {
        it->second.add(name, value, key);
    }
}

int StatisticProxy::updateDatabase()
{
    for (std::map<std::string, Table>::iterator it = m_tables.begin(); it != m_tables.end(); ++it)
    {
        it->second.updateDatabase(it->first.c_str());
    }
    return 1;
}

void StatisticProxy::resetTable()
{
    for (std::map<std::string, Table>::iterator it = m_tables.begin(); it != m_tables.end(); ++it)
    {
        it->second.reset();
    }
}

int initialize()
{
    Table t;
    t.setKey("level,occ_date");
    getStatisticProxy()->registTable("log_random_option", t);
    t.reset();
    return 1;
}

void addStatisticProxy(StatisticsPacket* packet)
{
    // 与发送端 statistc_proxy::add(table, field, value, fmt, ...) 对应：
    // name=+0x2b（列名）、value=+0x85、key=+0x4c（条件值）、table=+0xa（表名）
    getStatisticProxy()->add(packet->m_fieldName, packet->m_value, packet->m_key,
                             packet->m_tableName);
}

void sendDBStatisticProxy()
{
    getStatisticProxy()->updateDatabase();
}

void resetStatisticProxy()
{
    getStatisticProxy()->resetTable();
}
}

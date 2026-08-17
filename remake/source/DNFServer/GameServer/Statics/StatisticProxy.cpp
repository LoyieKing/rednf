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
    static char tmp[32];
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
    static char tmp[32];
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
    static char tmp[32];
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
    return strlen(out) < 0x800;
}

bool Field::MakeUpdateQuery(char* out, const char* table, const std::string& key,
                            const std::string& cond)
{
    getUpdateCondition(key, cond);
    snprintf(out, 0x800, "update %s set %s where %s", table, getUpdateValue(),
             g_updateCondition.c_str());
    return strlen(out) < 0x800;
}

void Field::getUpdateCondition(const std::string& key, const std::string& cond)
{
    g_updateCondition.clear();
    size_t pos1 = key.find_first_of(",", 0);
    size_t pos2 = cond.find_first_of(",", 0);
    size_t prev1 = 0;
    size_t prev2 = 0;
    while (pos1 != std::string::npos && pos2 != std::string::npos)
    {
        if (g_updateCondition.length() != 0)
        {
            g_updateCondition += " and ";
        }
        g_updateCondition += key.substr(prev1, pos1 - prev1);
        g_updateCondition += "=";
        g_updateCondition += cond.substr(prev2, pos2 - prev2);
        prev1 = pos1 + 1;
        prev2 = pos2 + 1;
        pos2 = cond.find_first_of(",", pos2 + 1);
        pos1 = key.find_first_of(",", pos1 + 1);
    }
    if (g_updateCondition.length() != 0)
    {
        g_updateCondition += " and ";
    }
    g_updateCondition += key.substr(prev1);
    g_updateCondition += "=";
    g_updateCondition += cond.substr(prev2);
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

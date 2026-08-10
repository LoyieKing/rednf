#ifndef STATISTIC_PROXY_H_
#define STATISTIC_PROXY_H_

#include <string>
#include <map>

#include "PacketHeader.h"

class StatisticsPacket;

// ---- PacketInsertUpdate（statistc_proxy::Field 组装 SQL 上报用）----
#pragma pack(push, 1)
class PacketInsertUpdate : public PacketHeader
{
public:
    PacketInsertUpdate();
    int m_handleIdx;          // +0xa
    int m_updateQueryId;      // +0xe
    int m_insertQueryId;      // +0x12
    char m_updateSql[0x801];  // +0x16
    char m_insertSql[0x801];  // +0x817
} __attribute__((packed));
#pragma pack(pop)

// statistc_proxy（统计代理，statics 独有）
namespace statistc_proxy
{
class Field
{
public:
    Field();
    Field(const Field& other);
    ~Field();
    char* getFieldName();
    char* getFieldValue();
    char* getUpdateValue();
    int updateDatabase(const char* table, const std::string& key, const std::string& cond);
    bool MakeInsertQuery(char* out, const char* table, const std::string& key,
                         const std::string& cond);
    bool MakeUpdateQuery(char* out, const char* table, const std::string& key,
                         const std::string& cond);
    void getUpdateCondition(const std::string& key, const std::string& cond);
    void add(const char* name, unsigned int value);
    void reset();
    std::map<std::string, unsigned long long> m_fields;  // +0
};

class Table
{
public:
    Table();
    Table(const Table& other);
    ~Table();
    void resetValue();
    int updateDatabase(const char* table);
    void add(const char* name, unsigned int value, const char* key);
    void reset();
    void setKey(const char* key);
    std::map<std::string, Field> m_fields;  // +0
    int m_nValue;                           // +0x18
    char m_bFlag;                           // +0x1c
    std::string m_key;                      // +0x20
};

class StatisticProxy
{
public:
    StatisticProxy();
    ~StatisticProxy();
    void resetTable();
    bool registTable(const char* name, Table& table);
    int updateDatabase();
    void add(const char* table, unsigned int value, const char* key, const char* cond);
    std::map<std::string, Table> m_tables;  // +0
    static void (*sendPacketFunctionPointer)(char*);
};

int initialize();
void addStatisticProxy(StatisticsPacket* packet);
void resetStatisticProxy();
void sendDBStatisticProxy();
}

statistc_proxy::StatisticProxy* getStatisticProxy();

#endif // STATISTIC_PROXY_H_

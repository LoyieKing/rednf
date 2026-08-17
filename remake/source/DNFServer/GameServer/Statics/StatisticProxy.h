#ifndef STATISTIC_PROXY_H_
#define STATISTIC_PROXY_H_

#include <string>
#include <map>
#include <string.h>

#include "PacketHeader.h"

// df_game_r: StatisticsPacket::StatisticsPacket() @ 0x8610132（0x2720 / 0x89）
// 双向证据：发送端 statistc_proxy::add(CUser*, table, field, value, fmt, ...) @ 0x860fdd2
// （strcpy 入 +0xa/+0x2b、vsprintf 入 +0x4c、param_4 入 +0x85）；
// 接收端 addStatisticProxy() -> StatisticProxy::add(name, value, key, table)。
class StatisticsPacket : public PacketHeader {
public:
    char m_tableName[0x21];  // +0xa..+0x2a：SQL 表名（如 "log_random_option"），发送端 strlen<0x21
    char m_fieldName[0x21];  // +0x2b..+0x4b：SQL 字段/列名（如 "change_cnt"、"unseal_gold"），strlen<0x21
    char m_key[0x39];        // +0x4c..+0x84：SQL key（WHERE 条件值，逗号分隔，如 "%d,'%s'" -> level,occ_date），strlen<0x39
    unsigned int m_value;    // +0x85：statistic value（增量），发送端 param_4 为 uint，接收端按 uint 读

    StatisticsPacket() : PacketHeader(0x2720, 0x89)
    {
        memset(m_tableName, 0, sizeof(m_tableName));
        memset(m_fieldName, 0, sizeof(m_fieldName));
        memset(m_key, 0, sizeof(m_key));
        m_value = 0;
    }
} __attribute__((packed));

TEST_CLASS_SIZE(StatisticsPacket, 0x89);

// df_game_r: StatisticsGmCmdPacket::StatisticsGmCmdPacket() @ 0x822c4a2（0x2722 / 0xe）
// 发送端 oldGmRoutine（GM 命令 "statisticlog"/"sl"）：local_34c = getCurCharacNo 正好落在 +0xa
// 接收端 OnStatisticGmCmd 只触发 sendDBStatisticProxy()+resetStatisticProxy()，字段本身不消费
class StatisticsGmCmdPacket : public PacketHeader {
public:
    int m_characNo;  // +0xa：触发统计刷库的玩家角色编号

    StatisticsGmCmdPacket() : PacketHeader(0x2722, 0xe),
        m_characNo(0) {};
} __attribute__((packed));

TEST_CLASS_SIZE(StatisticsGmCmdPacket, 0xe);

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

#ifndef DNF_GAME_DBMGR_H_
#define DNF_GAME_DBMGR_H_

// ============================================================================
// df_game_r 还原 —— DBMgr（MySQL 连接池，D4 批次）
// 参照 docs/class_func_reports/DBMgr.md + df_game_r 反汇编。
// 布局（sizeof = 0x2c + hash_map(0x14) + map(0x30)）：
//   +0x00 MySQL* m_dbHandle（new MySQL[0x11]，cookie 在 [-4]）
//   +0x04 int    m_userIdx（setUserIdx；-1 = 未分库）
//   +0x08 bool   m_bActiveGlobalDB（activateGlobalDB 置 1）
//   +0x0c hash_map<int, stUserDBInfo_t> m_userDBInfoMap
//   +0x20 map<ENUM_SERVER_GROUP, map<ENUM_DB_HANDLE_IDX, MySQL*> > m_dbHandleMap
// stUserDBInfo_t 12 字节：userIdx / 未用 / serverGroup（GetServerType 返回末字段）。
// 连接错误码定义：0x7d5=2005 CR_CONN_HOST_ERROR, 0x7d6=2006 CR_SERVER_GONE_ERROR,
// 0x7dd=2013 CR_SERVER_LOST, 0x426=1062 ER_DUP_ENTRY。
// ============================================================================

#include <ext/hash_map>
#include <map>

#include "CEnvironment.h"
#include "GlobalData.h"
#include "MySQL.h"

class CQueryCounter;

class DBMgr
{
public:
    struct stUserDBInfo_t
    {
        int m_userDBIdx;      // +0
        int m_userDBType;     // +4
        int m_serverGroup;    // +8
    };

    DBMgr();                          // 0x083f48de
    ~DBMgr();                         // 0x083f4a1c

    bool Open(ENUM_DB_HANDLE_IDX idx, char* host, char* db, char* user,
              char* passwd, ENUM_SERVER_GROUP group);   // 0x083f4bc4
    bool Open(ENUM_DB_HANDLE_IDX idx, char* host, unsigned int port,
              char* db, char* user, char* passwd,
              ENUM_SERVER_GROUP group);                 // 0x083f4d2e
    void Close();                     // 0x083f4ea6
    void Close(ENUM_DB_HANDLE_IDX idx);                 // 0x083f4ee0
    int GetServerType(int userIdx);   // 0x083f4f10
    void addUserDBInfo(int userIdx, const stUserDBInfo_t& info);  // 0x083f4f9e
    void removeUserDBInfo(int userIdx);                 // 0x083f5038
    MySQL* GetDBHandleServerGroup(ENUM_DB_HANDLE_IDX idx,
                                  ENUM_SERVER_GROUP group);  // 0x083f506c
    void SetQueryCounterPointer(CQueryCounter* pCounter);  // 0x083f5150
    MySQL* GetDBHandle(ENUM_DB_HANDLE_IDX idx,
                       ENUM_SERVER_GROUP group);       // 0x083f523e

    void setUserIdx(int userIdx);     // 0x0818fb1a（弱）
    void activateGlobalDB();          // 0x082a420c（弱）
    bool isActiveGlobalDB();          // 0x083f533c（弱）

    MySQL* m_dbHandle;                // +0x00
    int m_userIdx;                    // +0x04
    bool m_bActiveGlobalDB;           // +0x08
    __gnu_cxx::hash_map<int, stUserDBInfo_t> m_userDBInfoMap;  // +0x0c
    std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> > m_dbHandleMap;  // +0x20
};

inline void DBMgr::setUserIdx(int userIdx)
{
    m_userIdx = userIdx;
}

inline void DBMgr::activateGlobalDB()
{
    m_bActiveGlobalDB = true;
}

inline bool DBMgr::isActiveGlobalDB()
{
    return m_bActiveGlobalDB;
}

#endif  // DNF_GAME_DBMGR_H_

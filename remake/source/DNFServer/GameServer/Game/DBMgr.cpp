// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DBMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DBMgr（D4 批次）
// 参照 docs/class_func_reports/DBMgr.md；源文件 DBMgr.cpp。
#include "DBMgr.h"

#include <new>

#include "CQueryCounter.h"
#include "GameBase.h"

#include "DBMgr/DBMgr.inc"
{
    m_dbHandle = new MySQL[0x11];
    if (m_dbHandle == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x301, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x301);
    }
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].init();
    }
    m_bActiveGlobalDB = false;
    m_userIdx = -1;
}

#include "DBMgr/dtor_DBMgr.inc"
{
    Close();
    delete[] m_dbHandle;
    for (std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> >::iterator outer =
             m_dbHandleMap.begin();
         outer != m_dbHandleMap.end(); ++outer)
    {
        for (std::map<ENUM_DB_HANDLE_IDX, MySQL*>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            MySQL* p = inner->second;
            if (p != 0)
            {
                delete p;
            }
        }
    }
}

#include "DBMgr/Open.inc"
                 char* passwd, ENUM_SERVER_GROUP group)
{
    if (group != 0)
    {
        if (m_dbHandleMap[group][idx] != 0)
        {
            return false;
        }
        m_dbHandleMap[group][idx] = new MySQL;
        if (!m_dbHandleMap[group][idx]->init())
        {
            return false;
        }
        if (!m_dbHandleMap[group][idx]->open(host, db, user, passwd))
        {
            return false;
        }
        return true;
    }
    bool ret = m_dbHandle[idx].open(host, db, user, passwd);
    return ret;
}

#include "DBMgr/Open_1.inc"
                 char* db, char* user, char* passwd, ENUM_SERVER_GROUP group)
{
    if (group != 0)
    {
        if (m_dbHandleMap[group][idx] != 0)
        {
            return false;
        }
        m_dbHandleMap[group][idx] = new MySQL;
        if (!m_dbHandleMap[group][idx]->init())
        {
            return false;
        }
        if (!m_dbHandleMap[group][idx]->open(host, port, db, user, passwd))
        {
            return false;
        }
        return true;
    }
    bool ret = m_dbHandle[idx].open(host, port, db, user, passwd);
    return ret;
}

#include "DBMgr/Close.inc"
{
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].close();
    }
}

#include "DBMgr/Close_1.inc"
{
    if (idx >= 0 && idx < 0x11)
    {
        m_dbHandle[idx].close();
    }
}

#include "DBMgr/GetServerType.inc"
{
    __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
        m_userDBInfoMap.find(userIdx);
    if (it != m_userDBInfoMap.end())
    {
        return it->second.m_serverGroup;
    }
    return 0;
}

#include "DBMgr/addUserDBInfo.inc"
{
    std::pair<__gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator, bool> ins =
        m_userDBInfoMap.insert(std::make_pair(userIdx, info));
    if (!ins.second)
    {
        m_userDBInfoMap[userIdx] = info;
    }
}

#include "DBMgr/removeUserDBInfo.inc"
{
    if (GlobalData::s_db_mgr->isActiveGlobalDB() != true)
    {
        return;
    }
    m_userDBInfoMap.erase(userIdx);
}

#include "DBMgr/GetDBHandleServerGroup.inc"
{
    if (group == 0)
    {
        __gnu_cxx::hash_map<int, stUserDBInfo_t>::const_iterator it =
            m_userDBInfoMap.find(m_userIdx);
        if (it == m_userDBInfoMap.end())
        {
            if (m_userIdx == -1)
            {
                return &m_dbHandle[idx];
            }
            return 0;
        }
        group = (ENUM_SERVER_GROUP)it->second.m_serverGroup;
    }
    return m_dbHandleMap[group][idx];
}

#include "DBMgr/SetQueryCounterPointer.inc"
{
    for (std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, MySQL*> >::iterator outer =
             m_dbHandleMap.begin();
         outer != m_dbHandleMap.end(); ++outer)
    {
        for (std::map<ENUM_DB_HANDLE_IDX, MySQL*>::iterator inner = outer->second.begin();
             inner != outer->second.end(); ++inner)
        {
            MySQL* p = inner->second;
            if (p != 0)
            {
                p->SetQueryCounter(pCounter);
            }
        }
    }
}

#include "DBMgr/GetDBHandle.inc"
{
    MySQL* base = m_dbHandle;
    if (!m_bActiveGlobalDB)
    {
        return &m_dbHandle[idx];
    }
    int fallbackIdx = 4;
    if (idx == 2 || idx == 3)
    {
        MySQL* p = GetDBHandleServerGroup(idx, group);
        if (p == 0)
        {
            return &base[4];
        }
        return p;
    }
    return &base[idx];
}

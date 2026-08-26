// df_game_r Game/ CEventManager 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "GameWorld.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "GlobalData.h"
#include "CGlobalEffectManager.h"
#include "CEnvironment.h"
#include "InterfacePacketBuf.h"
#include "CGuildServerProxy.h"
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();
};
#include <cstring>
#include <cstdio>

#include <new>
#include <algorithm>
#include <map>
#include <vector>
#include "RDARScriptStringManager.h"   // g_scriptStringManager_/findString
#include "CEventTable.h"              // CEventTable 权威声明（唯一）

CDelivery::CDelivery()
    : m_eventMap()
{
}

CDelivery::~CDelivery()
{
    m_eventMap.clear();
}

bool CDelivery::checkAddress(int type)
{
    return type >= 1 && type <= 2;
}

void CDelivery::attach(int type, CEventBase* event)
{
    if (!checkAddress(type))
        return;
    std::list<CEventBase*>& events = m_eventMap[type];
    if (std::find(events.begin(), events.end(), event) == events.end())
        events.push_back(event);
}

void CDelivery::detach(int type, CEventBase* event)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
        it->second.remove(event);
}

// ORIG 0x08116682（notify.md）：checkAddress 校验 → map.find →
// 遍历该 type 的 list，对非空 CEventBase 经 vtable+0x30（= delivery 虚槽 12）
// 调用 delivery(msg)。此处用真实虚调用，生成 call vtable[+0x30]，与 ORIG 一致。
void CDelivery::notify(int type, DeliveryMsg& msg)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it == m_eventMap.end())
        return;
    for (std::list<CEventBase*>::iterator event = it->second.begin();
         event != it->second.end(); ++event)
    {
        if (*event != 0)
            (*event)->delivery(msg);
    }
}

namespace
{
    CDelivery* getDelivery(CEventManager* manager)
    {
        return *reinterpret_cast<CDelivery**>(
            reinterpret_cast<char*>(manager) + 0x298);
    }
}


CEventManager::CEventManager()
{
    std::memset(m_pad, 0, sizeof(m_pad));
    // ORIG 0x08114ce4：memset 0x298，operator new(0x18) 后调 CDelivery::CDelivery()
    // （0x08116386），存 CDelivery* 于 +0x298。此处用真实 CDelivery 构造。
    CDelivery* delivery = new CDelivery;
    *reinterpret_cast<CDelivery**>(m_pad + 0x298) = delivery;
}



CEventManager::~CEventManager()
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        CEventBase* event = reinterpret_cast<CEventBase**>(m_pad)[i];
        if (event != 0)
            delete event;
    }
    CDelivery* delivery = getDelivery(this);
    if (delivery != 0)
    {
        delete delivery;
    }
}

int CEventManager::AddEvent(int type, CEventBase* event)
{
    reinterpret_cast<CEventBase**>(m_pad)[type] = event;
    return 1;
}

CEventBase* CEventManager::GetRepeatEvent(int type)
{
    if (type < 0 || type > 0xa5)
        return 0;
    return reinterpret_cast<CEventBase**>(m_pad)[type];
}

// ORIG T 0x08116212 _ZN13CEventManager30IsExistOverlappedExpBonusEventEv
// 事件 0x66（经验加成类）进行中，或全局效果管理器效果 2 生效 → 视为存在
// 重叠经验加成。
bool CEventManager::IsExistOverlappedExpBonusEvent()
{
    bool eventExp = GlobalData::s_event_manager->GetRepeatEvent(0x66)->IsEventing(0);
    bool effect2 = GlobalData::s_globalEffectManager_->isAffectedEffect(2);
    if (eventExp || effect2)
        return true;
    return false;
}

void CEventManager::dailyresetData()
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        CEventBase* event = reinterpret_cast<CEventBase**>(m_pad)[i];
        if (event != 0 && event->IsEventing(0))
            event->dailyresetData();
    }
}

int filterEvent(int type)
{
    if (type == 2)
        return 0;
    GameWorld* world = G_GameWorld();
    unsigned int channel = world != 0 ? (unsigned int)world->GetChannelType() : 0;
    if (channel >= 0x11)
        return 0;
    unsigned int bit = 1U << channel;
    if ((bit & 0x1a100U) == 0)
    {
        if ((bit & 0x4000U) == 0)
            return ((bit & 0x600U) != 0 && type != 9) ? 1 : 0;
        if (type == 0x39) return 0;
        if (type < 0x3a)
            return type == 0xb ? 0 : 1;
        return (type - 0x3dU > 1) ? 1 : 0;
    }
    if (type < 0x3a)
        return (type < 0x36 && type != 0xb) ? 1 : 0;
    if (type < 0x3d)
        return 1;
    return (type > 0x3e && type != 0x5c) ? 1 : 0;
}
struct EventAddInfo
{
    unsigned short eventId;
    unsigned short info0;
    unsigned short info1;
};

void CEventManager::MakeNotiEventInfo(PacketGuard& packet, CUser* user)
{
    std::vector<EventAddInfo> extra;
    packet.clear();
    packet.put_header(0, 0x6c);
    int countIndex = packet.get_index();
    int activeCount = 0;
    packet.put_short(0);
    for (int id = 0; id <= 0xa5; ++id)
    {
        CEventBase* event = GetRepeatEvent(id);
        if (event == 0 || !event->IsEventing(user))
            continue;
        packet.put_short(id);
        ++activeCount;
        int packed = event->GetAddInfo();
        unsigned short info0 = (unsigned short)(packed & 0xffff);
        if ((short)info0 >= 0)
        {
            EventAddInfo item;
            item.eventId = (unsigned short)id;
            item.info0 = info0;
            item.info1 = (unsigned short)((unsigned int)packed >> 16);
            extra.push_back(item);
        }
    }
    packet.put_short(countIndex, activeCount);
    for (int id = 0; id <= 0xa5; ++id)
    {
        CEventBase* event = GetRepeatEvent(id);
        if (event != 0 && event->IsEventing(user))
        {
            event->AppendInfo(packet);
        }
    }
    packet.put_byte((int)extra.size());
    for (size_t i = 0; i < extra.size(); ++i)
    {
        packet.put_short(extra[i].eventId);
        packet.put_short(extra[i].info0);
        packet.put_short(extra[i].info1);
    }
    packet.finalize(true);
}

void CEventManager::BroadcastEventInfo()
{
    GameWorld* world = G_GameWorld();
    if (world == 0)
        return;
    for (std::map<unsigned short, CUser*>::iterator it =
             world->m_UserInWorld.begin(); it != world->m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && user->get_state() > 2)
        {
            PacketGuard packet;
            MakeNotiEventInfo(packet, user);
            user->Send(packet);
        }
    }
}

int CEventManager::TriggerEventStart(int idx, Word_Param param)
{
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event == 0)
        return 0;
    event->StartEvent(param);
    BroadcastEventInfo();
    event->deliveryStart(getDelivery(this));
    event->StartAction();
    return 1;
}

int CEventManager::TriggerEventEnd(int idx)
{
    if (filterEvent(idx))
        return 1;
    CEventBase* event = GetRepeatEvent(idx);
    if (event == 0)
        return 0;
    event->deliveryEnd(getDelivery(this));
    BroadcastEventInfo();
    event->EndEvent();
    return 1;
}

bool CEventManager::InitEventManager()
{
    // The original initializes the event table before loading persisted rows.
    // Event constructors are not available in this TU, so an empty table must
    // not be reported as successfully initialized.
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (reinterpret_cast<CEventBase**>(m_pad)[i] != 0)
            return LoadEventFromDB();
    }
    return false;
}

bool CEventManager::LoadEventFromDB()
{
    if (GlobalData::s_db_mgr == 0)
        return false;
    CEnvironment* env = G_CEnvironment();
    if (env == 0)
        return false;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    if (db == 0 || !db->set_query(
            "SELECT event_type, parameter1, parameter2 FROM dnf_event_log "
            "WHERE end_time=0 AND server_id IN (0,%d) AND unix_timestamp()>=start_time "
            "ORDER BY start_time", *reinterpret_cast<const int*>(reinterpret_cast<const char*>(env) + 0x378)) || !db->exec(true))
        return false;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        if (!db->fetch())
            return false;
        int type = 0;
        unsigned short p1 = 0;
        unsigned short p2 = 0;
        if (!db->get_int(0, type) || !db->get_ushort(1, p1) ||
            !db->get_ushort(2, p2))
            return false;
        Word_Param param;
        param.m_duration = p1;
        param.m_field2 = p2;
        if (!TriggerEventStart(type, param))
            return false;
    }
    return true;
}

void CEventManager::eventDeliveryNotify(int type, DeliveryMsg& msg)
{
    CDelivery* delivery = getDelivery(this);
    if (delivery != 0)
        delivery->notify(type, msg);
}


namespace
{
// ORIG 每项 0x14 字节（_FindEventTable 0x811a15d / InsertEventTable 0x811a4d4 证据）：
//   +0x00 event_id / +0x04 apply_type / +0x08 CEventBase* / +0x0c name(char*) /
//   +0x10 explain(char*)。
struct EventTableEntry
{
    int id;              // +0x00
    int apply_type;      // +0x04
    CEventBase* base;    // +0x08
    const char* name;    // +0x0c（event_name）
    const char* explain; // +0x10（event_explain key/string）
};
EventTableEntry g_event_table[0x4f];  // ORIG 0x936fa60（BSS 零初始化）
}  // namespace

CEventTable::CEventTable() {}

int CEventTable::GetEventTableCount()
{
    return 0x4f;
}

CEventBase* CEventTable::GetEventBase(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return g_event_table[idx].base;
}

int CEventTable::GetEventId(int idx)
{
    if (idx < 0 || GetEventTableCount() <= idx)
        return 0;
    return g_event_table[idx].id;
}

// ORIG 0x811a15d（_Z15_FindEventTableiPKcRb，131 字节）：在全局事件表中按
// event_id 查找条目；命中后以 strncmp(entry.name, name, strlen(name)) 比名。
// 名字匹配则直接返回索引；不匹配则置 *dup=true 仍返回该索引；未命中返回 -1。
int _FindEventTable(int id, const char* name, bool* dup)
{
    for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
    {
        if (g_event_table[i].id == id)
        {
            const char* tblName = g_event_table[i].name;
            const std::size_t n = name ? strlen(name) : 0;
            if (tblName != 0 && name != 0 && strncmp(tblName, name, n) == 0)
                return i;
            if (dup)
                *dup = true;
            return i;
        }
    }
    return -1;
}

// ORIG 0x811a1e0（0x2f4 字节）：经 DBMgr::GetDBHandle 查询
// dnf_event_info(event_id, event_name)，逐行 _FindEventTable 校验；把校验通过
// 的事件表索引记入集合，最后将「未在 DB 中」的事件表索引追加到 out，返回是否成功。
bool CEventTable::VerifyEventTable(std::vector<int>& out)
{
    out.clear();
    MySQL* db = GlobalData::s_db_mgr ?
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE) : 0;
    if (db == 0)
        return false;

    if (!db->set_query("seLect event_id, event_name from dnf_event_info"))
        return false;
    if (!db->exec(true))
        return false;

    std::map<int, int> found;
    const int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        if (!db->fetch())
            return false;
        int id = 0;
        if (!db->get_int(0, id))
            return false;
        char name[0xff];
        if (!db->get_str(1, name, 0xff))
            return false;
        bool dup = false;
        int idx = _FindEventTable(id, name, &dup);
        if (idx < 0)
            continue;
        if (dup)
            return false;
        found[idx] = 1;
    }

    for (int i = 0; i < GetEventTableCount(); ++i)
    {
        if (found.find(i) == found.end())
            out.push_back(i);
    }
    return true;
}

// ORIG 0x811a4d4（0x2d0 字节）：为空则直接返回 1；否则遍历 ids 中各事件表索引，
// 用事件表 name/explain 字段构造 "(%d,%d,'%s','%s')" 插入串（explain 经
// g_scriptStringManager_.findString(4, ...) 解析），一次性 INSERT 到
// dnf_event_info，exec 成功返回 1，出错码 0x426（诱骗重复）或失败返回 0。
bool CEventTable::InsertEventTable(std::vector<int>& ids)
{
    if (ids.empty())
        return true;

    std::string sql;
    std::string temp;
    for (std::vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it)
    {
        const int idx = *it;
        if (idx < 0 || GetEventTableCount() <= idx)
            continue;
        const EventTableEntry& e = g_event_table[idx];
        const char* explain = e.explain ? e.explain : "";
        const char* resolved = g_scriptStringManager_.findString(4, explain, 0);
        char buf[0x100];
        std::snprintf(buf, sizeof(buf), "(%d,%d,'%s','%s')",
                      e.id, e.apply_type, e.name ? e.name : "", resolved ? resolved : "");
        sql += buf;
        if (it + 1 != ids.end())
            sql += ",";
    }

    MySQL* db = GlobalData::s_db_mgr ?
        GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE) : 0;
    if (db == 0)
        return false;

    std::string query = "inSert into dnf_event_info(event_id, apply_type, event_name, event_explain) values";
    query += sql;
    if (!db->set_query("%s", query.c_str()))
        return false;
    if (!db->exec(true))
    {
        (void)db->getDBError();
        return false;
    }
    return true;
}

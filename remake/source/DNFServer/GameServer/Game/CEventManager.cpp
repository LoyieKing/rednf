// df_game_r Game/ CEventManager 实现（独立 TU，从 GameStubs.cpp 拆出）。
#include "GameWorld.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "GlobalData.h"
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


#include <new>
#include <algorithm>

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

// CEventTable belongs to the event manager translation unit.
class CEventTable
{
public:
    int GetEventTableCount();
};

int CEventTable::GetEventTableCount()
{
    return 0x4f;
}

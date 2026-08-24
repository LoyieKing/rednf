// df_game_r Game/ EventClassify::CEventScriptMng 实现。
#include "CEventScriptMng.h"

#include "CUser.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "PacketGuard.h"
#include "DB_SelectLevelUpReward.h"   // process_level_up_reward 的 DB 确认请求
#include <ctime>
#include <cstring>

#include "SyncScriptDeps.h"          // toTString

// InGameEventInfo / InGameEventInfoList 为全局作用域类型（见 CEventScriptMng.h），
// 故其方法定义位于任何命名空间之外。

// ============ InGameEventInfo ============

InGameEventInfo::InGameEventInfo()
    : m_eventId(0), m_field4(0), m_name(), m_startTime(0),
      m_endTime(0), m_isEventing(0)
{
}

InGameEventInfo::~InGameEventInfo()
{
}

InGameEventInfo::InGameEventInfo(const InGameEventInfo& o)
    : m_eventId(o.m_eventId), m_field4(o.m_field4), m_name(o.m_name),
      m_startTime(o.m_startTime), m_endTime(o.m_endTime), m_isEventing(o.m_isEventing)
{
}

InGameEventInfo& InGameEventInfo::operator=(const InGameEventInfo& o)
{
    if (this != &o)
    {
        m_eventId = o.m_eventId;
        m_field4 = o.m_field4;
        m_name = o.m_name;
        m_startTime = o.m_startTime;
        m_endTime = o.m_endTime;
        m_isEventing = o.m_isEventing;
    }
    return *this;
}

// ============ InGameEventInfoList ============

InGameEventInfoList::InGameEventInfoList() : m_info() {}
InGameEventInfoList::~InGameEventInfoList() { m_info.clear(); }

// ORIG 0x0897eb30：m_info[id] = info。
void InGameEventInfoList::insertEventInfo(int id, InGameEventInfo& info)
{
    m_info[id] = info;
}

// ============ InGameEventScriptLoader ============


InGameEventScriptLoader::InGameEventScriptLoader() : m_scripts() {}
InGameEventScriptLoader::~InGameEventScriptLoader() { m_scripts.clear(); }

// ORIG 0x08982f10：非空则 m_scripts[name] = script；返回 script。
InGameEventScript* InGameEventScriptLoader::insertEventScript(
    const std::string& name, InGameEventScript* script)
{
    if (script != 0)
        m_scripts[name] = script;
    return script;
}

// ORIG 0x08982f38（_ZN23InGameEventScriptLoader14getEventScriptEiRKSs）：
// m_scripts.find(name)；命中返回脚本指针；未命中先 import 再入表并返回。
InGameEventScript* InGameEventScriptLoader::getEventScript(int id,
                                                           const std::string& name)
{
    // ORIG：find 与 end 各存一个栈槽，!= 判定后命中走 operator->，
    // 未命中把 importInGameEventScript 结果直接作为 insertEventScript 实参。
    // （本实现与 ORIG 仅迭代器栈槽分配对调，语义/调用序一致。）
    std::map<std::string, InGameEventScript*>::iterator it =
        m_scripts.find(name);
    std::map<std::string, InGameEventScript*>::iterator endIt =
        m_scripts.end();
    if (it != endIt)
        return it->second;

    return insertEventScript(name, importInGameEventScript(id, name));
}

namespace EventClassify
{

// ============ CEventEntity ============

// ORIG 0x810af40：设 vptr，+0x04 字节=0，构造 m_dbTableName(std::string)，
// +0x0c/0x10/0x14/0x18 均清 0。
CEventEntity::CEventEntity()
    : m_isEventing(0), m_dbTableName(), m_startTime(0),
      m_endTime(0), m_script(0), m_recordType(0)
{
}

CEventEntity::~CEventEntity()
{
    // ORIG D1 0x810afc4：恢复 vptr 后析构 m_dbTableName。
}

bool CEventEntity::is_eventing()
{
    // ORIG 0x810b016：movzbl 0x4(%eax)。
    return m_isEventing != 0;
}

void CEventEntity::set_event_flag(bool flag)
{
    // ORIG 0x810b022：写 +0x04 字节。
    m_isEventing = flag ? 1 : 0;
}

void CEventEntity::set_event_data(int key, InGameEventScript* script)
{
    // ORIG 0x810b03a：+0x18=key，+0x14=script。
    m_recordType = key;
    m_script = script;
}

int CEventEntity::get_event_record_type()
{
    // ORIG 0x810b052：return +0x18。
    return m_recordType;
}

void CEventEntity::set_event_record_type(int type)
{
    // ORIG 0x810b05e：写 +0x18。
    m_recordType = type;
}

// ORIG 0x810b0ac（32 字节）：基类实现为纯 return 0 —— 参数仅存栈不使用。
bool CEventEntity::send_init_data(CUser*, PacketGuard&, short,
                                  unsigned char, bool)
{
    return false;
}

char* CEventEntity::get_db_table_name()
{
    // ORIG 0x810b06c：return (char*)(this+0x08) = &m_dbTableName。
    return reinterpret_cast<char*>(&m_dbTableName);
}

int CEventEntity::get_event_start_time()
{
    // ORIG 0x810b094：return *(int*)(this+0x0c)。
    return m_startTime;
}

void CEventEntity::set_event_start_time(int t)
{
    m_startTime = t;
}

int CEventEntity::get_event_end_time()
{
    // ORIG 0x810b0a0：return *(int*)(this+0x10)。
    return m_endTime;
}

void CEventEntity::set_event_end_time(int t)
{
    m_endTime = t;
}

// ============ CEventScriptMng ============

CEventScriptMng::CEventScriptMng()
    : InGameEventInfoList(), InGameEventScriptLoader(),
      m_entities(), m_field48(0), m_flag4c(0), m_vec50()
{
}

CEventScriptMng::~CEventScriptMng()
{
}

// ORIG 0x810ba36：key ∈ [0,0x19c] 时取 map[key] 并调用其虚 is_eventing。
bool CEventScriptMng::is_eventing(int key)
{
    if (key < 0 || 0x19c < key)
        return false;
    CEventEntity* entity = m_entities[key];
    return entity != 0 ? entity->is_eventing() : false;
}

// ORIG T 0x0810bea8（报告 EventClassify__CEventScriptMng/
// process_add_item_count_db.md）：user 有效且事件 0x193 实体存在、
// CAddItemCountDB::is_event_item(itemId) 命中时，给当前角色累加
// 事件物品计数（count 按 unsigned short 零扩展）。
void EventClassify::CEventScriptMng::process_add_item_count_db(CUser* user,
                                                               int itemId,
                                                               int count)
{
    if (user == 0)
        return;
    CEventEntity* entity = get_event_entity(0x193);
    if (entity == 0)
        return;
    if (static_cast<CAddItemCountDB*>(entity)->is_event_item(itemId) != 0)
        user->IncCurCharacEventAddItemCount((unsigned short)count);
}

// ORIG 0x810c40e：包 0x176 —— put_header(0,0x176)、put_int(m_field48)、
// 占位 byte 后遍历实体：is_eventing 的实体调用虚 send_init_data(user,packet,
// key,m_flag4c,flag)，成功则计数+1；回填计数、finalize 并 CUser::Send。
void CEventScriptMng::send_event_init_data(CUser* user, bool flag)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.put_header(0, 0x176);
    packet.put_int(m_field48);
    int countIdx = packet.get_index();
    packet.put_byte(0);
    unsigned char count = 0;
    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin();
         it != m_entities.end(); ++it)
    {
        if (is_eventing(it->first))
        {
            if (it->second->send_init_data(user, packet,
                                           static_cast<short>(it->first),
                                           m_flag4c, flag))
                ++count;
        }
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG 0x810b0cc：0x191→0 / 0x192→1 / 0x19c→2，其它 -1。
int CEventScriptMng::get_convert_to_user_confirmflag(int key)
{
    int v = -1;
    if (key == 0x192)
        v = 1;
    else if (key == 0x19c)
        v = 2;
    else if (key == 0x191)
        v = 0;
    return v;
}

// ORIG 0x810b96a：map find，命中返回实体指针，未命中返回 0。
CEventEntity* CEventScriptMng::get_event_entity(int key)
{
    std::map<int, CEventEntity*>::iterator it = m_entities.find(key);
    if (it == m_entities.end())
        return 0;
    return it->second;
}

// ORIG 0x810b9d4（const）：map find != end。
bool CEventScriptMng::is_event_entity(int key) const
{
    return m_entities.find(key) != m_entities.end();
}

// ORIG 0x810c7b8：实体存在则调用虚 get_event_record_type（vtable+0x14）。
int CEventScriptMng::get_record_type(int key)
{
    CEventEntity* entity = get_event_entity(key);
    if (entity == 0)
        return 0;
    return entity->get_event_record_type();
}

// ORIG 0x810c6d6：return *(int*)(this+0x48)。
int CEventScriptMng::get_today_time()
{
    return m_field48;
}

// ORIG 0x810c6ca：return (byte)(this+0x4c)。
unsigned char CEventScriptMng::get_today_time_day()
{
    return m_flag4c;
}

// ORIG 0x810c75c：InGameEventInfoList map<int,InGameEventInfo> find != end。
bool CEventScriptMng::is_current_ingame_event_record_type(int key)
{
    return m_info.find(key) != m_info.end();
}

// ORIG 0x810c6e2：clear out 后把 InGameEventInfoList map 的全部 key 写入 out。
void CEventScriptMng::get_ingame_event_record_type_list(std::vector<int>& out)
{
    out.clear();
    for (std::map<int, InGameEventInfo>::iterator it = m_info.begin();
         it != m_info.end(); ++it)
        out.push_back(it->first);
}

// ORIG 0x810c974：两个 YYYYMMDD 整数相差的天数；任一 <=0 或前>后返回 -1。
int CEventScriptMng::get_pass_day(int yyyymmdd1, int yyyymmdd2)
{
    if (yyyymmdd1 < 1 && yyyymmdd2 < 1)
        return -1;

    struct tm t1, t2;
    std::memset(&t1, 0, sizeof(t1));
    t1.tm_year = yyyymmdd1 / 10000 - 1900;
    t1.tm_mon  = (yyyymmdd1 % 10000) / 100 - 1;
    t1.tm_mday = yyyymmdd1 % 100;
    t1.tm_hour = 0;
    time_t v1 = std::mktime(&t1);

    std::memset(&t2, 0, sizeof(t2));
    t2.tm_year = yyyymmdd2 / 10000 - 1900;
    t2.tm_mon  = (yyyymmdd2 % 10000) / 100 - 1;
    t2.tm_mday = yyyymmdd2 % 100;
    t2.tm_hour = 0;
    time_t v2 = std::mktime(&t2);

    if (v2 < v1)
        return -1;
    return static_cast<int>((v2 - v1) / 0x15180);
}

// ORIG 0x810c5d2：按当前系统时刻与「换日小时」刷新 m_field48(YYYYMMDD)/m_flag4c(日)。
void CEventScriptMng::update_time_values(int hour)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    time_t prev = now - 0x15180;   // 前一日
    struct tm localT, prevT;
    localtime_r(&now, &localT);
    localtime_r(&prev, &prevT);

    if (localT.tm_hour < 0 || hour <= localT.tm_hour)
    {
        m_flag4c = static_cast<unsigned char>(localT.tm_mday);
        m_field48 = localT.tm_year * 10000 + (localT.tm_mon + 1) * 100
                    + 19000000 + static_cast<unsigned char>(m_flag4c);
    }
    else
    {
        m_flag4c = static_cast<unsigned char>(prevT.tm_mday);
        m_field48 = prevT.tm_year * 10000 + (prevT.tm_mon + 1) * 100
                    + 19000000 + static_cast<unsigned char>(m_flag4c);
    }
}

// ORIG 0x810cdb0：清空并填充 m_vec50 的事件记录类型号集合。
void CEventScriptMng::init_event_nums()
{
    m_vec50.clear();

    m_vec50.push_back(0x56);
    m_vec50.push_back(0x191);
    m_vec50.push_back(0x192);
    m_vec50.push_back(0x193);
    m_vec50.push_back(0x194);
    m_vec50.push_back(0x196);
    m_vec50.push_back(0x197);
    m_vec50.push_back(0x199);
    m_vec50.push_back(0x19a);
    m_vec50.push_back(0x19b);
    m_vec50.push_back(0x5c);
    m_vec50.push_back(0x65);
    m_vec50.push_back(0x19c);
}

// ORIG 0x810bd14（_ZN13EventClassify15CEventScriptMng17init_evnet_entityEv）：
// init_event_nums() 后遍历 m_vec50（this+0x50 vector<int>）；对尚无实体的记录
// 类型，m_entities[type] = create_event_entity(type, true)（基类实体）。
void CEventScriptMng::init_evnet_entity()
{
    init_event_nums();

    // ORIG：循环下标按有符号 setg 与 size() 比较；map 槽位由复合赋值直接
    // 写入（无具名中间变量），与 ORIG 寄存器编排一致。
    for (int i = 0; (int)m_vec50.size() > i; ++i)
    {
        if (get_event_entity(m_vec50[i]) == 0)
            m_entities[m_vec50[i]] = create_event_entity(m_vec50[i], true);
    }
}

void CEventScriptMng::process_level_up_reward(CUser* user, bool flag)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* reward =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    const int recvCheck = reward->is_recv_check_item();
    if (recvCheck != 0)
    {
        const int ok = reward->check_recv_level(user->get_charac_level(),
                                                user->get_charac_job(),
                                                user->getCurCharFirstGrowType(),
                                                flag);
        if (ok != 0)
        {
            DB_SelectLevelUpReward::makeRequest(
                static_cast<int>(user->GetUID()),
                user->getCurCharacNo(),
                user->get_charac_level());
        }
    }
    else
    {
        const int set = reward->set_reward_item(user->get_charac_level(),
                                                user->get_charac_job(),
                                                user->getCurCharFirstGrowType());
        if (set != 0)
        {
            reward->do_action(user);
        }
    }
}

// ORIG 0x810ba7a（_ZN13EventClassify15CEventScriptMng14initScriptlistER19InGameEventInfoList）：
// 遍历 list.m_info；m_isEventing 非空的项：toTString(m_name) 构造临时串 →
// 基类 getEventScript(id, name) 取脚本（失败 return 0）→ m_entities[id] =
// create_event_entity(id,false) → 虚 set_event_flag(flag!=0) → 虚
// set_event_data(mapKey, script) → set_event_start_time/set_event_end_time。
// 循环耗尽后调 init_evnet_entity() 并 return 1。
bool CEventScriptMng::initScriptlist(InGameEventInfoList& list)
{
    for (std::map<int, InGameEventInfo>::iterator it = list.m_info.begin();
         it != list.m_info.end(); ++it)
    {
        InGameEventInfo& info = it->second;
        if (info.m_isEventing == 0)
            continue;

        // ORIG 以 toTString 结果立即构造临时 std::string。
        std::string name(toTString(info.m_name));

        InGameEventScript* script =
            InGameEventScriptLoader::getEventScript(info.m_eventId, name);
        if (script == 0)
            return false;

        CEventEntity*& slot = m_entities[info.m_eventId];
        slot = create_event_entity(info.m_eventId, false);
        slot->set_event_flag(info.m_isEventing != 0);
        slot->set_event_data(it->first, script);
        slot->set_event_start_time(info.m_startTime);
        slot->set_event_end_time(info.m_endTime);
    }

    init_evnet_entity();
    return true;
}

}  // namespace EventClassify

// df_game_r Game/ EventClassify::CEventScriptMng 实现。
#include "CEventScriptMng.h"

#include "CUser.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "PacketGuard.h"
#include "DB_SelectLevelUpReward.h"   // process_level_up_reward 的 DB 确认请求
#include "GameWorld.h"
#include "CDataManager.h"
#include "DB_UpdateInGameEventHistory.h"
#include "InGameEventScripts.h"
#include <ctime>
#include <cstring>

#include "SyncScriptDeps.h"          // toTString

// InGameEventInfo / InGameEventInfoList 为全局作用域类型（见 CEventScriptMng.h），
// 故其方法定义位于任何命名空间之外。

// ============ InGameEventInfo ============

// ORIG 0x08986044：ctor 仅构造 m_name（+0x08），m_eventId/m_field4 及后续
// 字段保持未初始化，由后续 insertEventInfo 赋值（ORIG 汇编无任何 int 清零）。
InGameEventInfo::InGameEventInfo()
    : m_name()
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

// ORIG _ZN15InGameEventInfoaSERKS_：无自赋值检查，逐字段复制。
InGameEventInfo& InGameEventInfo::operator=(const InGameEventInfo& o)
{
    m_eventId = o.m_eventId;
    m_field4 = o.m_field4;
    m_name = o.m_name;
    m_startTime = o.m_startTime;
    m_endTime = o.m_endTime;
    m_isEventing = o.m_isEventing;
    return *this;
}

// ============ InGameEventInfoList ============

// ORIG _ZN19InGameEventInfoListD1Ev：仅 map 析构（隐式），无 clear（P12）。
InGameEventInfoList::InGameEventInfoList() : m_info() {}

// ORIG 0x0897eb30：m_info[id] = info。
void InGameEventInfoList::insertEventInfo(int id, InGameEventInfo& info)
{
    m_info[id] = info;
}

// ============ InGameEventScriptLoader ============


// ORIG _ZN23InGameEventScriptLoaderD1Ev：仅 map 析构（隐式），无 clear（P12）。
InGameEventScriptLoader::InGameEventScriptLoader() : m_scripts() {}

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
// [b28c-31] N93：ORIG 0x08982f5x find 目标槽 -0x10 / end 槽 -0xc（4.1 先声明占
// 深槽），4.4 恒反向（先声明 -0xc）；声明序翻转无效，且"先声明后赋值"会引入
// ORIG 不存在的 _Rb_tree_iterator::C1Ev（nm 实证）⇒ 封顶。
// [b28d-27] NEAR N93（复核）：ORIG 0x08982f38；ORIG find 槽 -0x10 / end 槽 -0xc
// （调用序 find 先、end 后），4.4 恒反向且交换声明序会改调用序 ⇒ 封顶。
// p167 CLEAR（6/6），语义闭合。
InGameEventScript* InGameEventScriptLoader::getEventScript(int id,
                                                           const std::string& name)
{
    // [b28c-31] ORIG 0x08982f5x：find 目标槽 -0x10 / end 目标槽 -0xc（4.1 先声明
    // 占深槽）⇒ 4.4 先声明占浅槽（N93），试 it 先声明 + endIt 后声明。
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
    // ORIG 0x810af8e：构造后调 m_dbTableName.clear()（空串冗余但存在，
    // 产生异常清理路径含 _ZNSsD1Ev——调用集合对齐）。
    m_dbTableName.clear();
}

CEventEntity::~CEventEntity()
{
    // ORIG D1 0x810afc4：恢复 vptr 后析构 m_dbTableName。
}

bool CEventEntity::is_eventing()
{
    // ORIG 0x810b016：movzbl 0x4(%eax)（直读 bool）。
    return m_isEventing;
}

void CEventEntity::set_event_flag(bool flag)
{
    // ORIG 0x810b022：movzbl 参数 + mov %dl,0x4(%eax)（bool→uchar 直赋）。
    m_isEventing = flag;
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

std::string& CEventEntity::get_db_table_name()
{
    // ORIG 0x810b06c：return &m_dbTableName（+0x08）。
    return m_dbTableName;
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
      m_entities(), m_todayTime(0), m_todayTimeDay(0), m_eventRecordTypes()
{
    m_entities.clear();
    m_info.clear();
    m_scripts.clear();
}

// [b28d-27] DIFF（D1/D2 别名）：ORIG 0x0810b218；p167 显示 ORIG 调成员 D1Ev、
// 我方调 D2Ev（同址别名，P93 类伪差集），语义等价。
CEventScriptMng::~CEventScriptMng()
{
    for (std::map<std::string, InGameEventScript*>::iterator it =
             m_scripts.begin();
         it != m_scripts.end(); ++it)
    {
        if (it->second != 0)
            delete it->second;
        it->second = 0;
    }

    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin();
         it != m_entities.end(); ++it)
    {
        if (it->second != 0)
            delete it->second;
        it->second = 0;
    }
}

// ORIG 0x810ba36：key ∈ [0,0x19c] 时取 map[key] 并调用其虚 is_eventing。
bool CEventScriptMng::is_eventing(int key)
{
    // ORIG 0x810ba36：正条件包裹（js/jg 均跳尾部 return 0），
    // operator[] 后直虚调用（P140：ORIG 无判空，0x810ba36 段
    // call operator[] → mov (%eax),%eax → vptr+0x8 直 call *%edx）。
    if (key >= 0 && key <= 0x19c)
    {
        return m_entities[key]->is_eventing();
    }
    return false;
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
    CAddItemCountDB* entity =
        static_cast<CAddItemCountDB*>(get_event_entity(0x193));
    if (entity != 0)
    {
        if (entity->is_event_item(itemId))
            user->IncCurCharacEventAddItemCount((unsigned short)count);
    }
}

// ORIG 0x810b110：按事件类型取实体，并转发到实体的表名访问器。
std::string& CEventScriptMng::get_db_table_name(int key)
{
    return get_event_entity(key)->get_db_table_name();
}

// ORIG 0x810bde0：事件开启且用户尚未确认时，调用账号首次登录实体动作。
// [b28d-27] DIFF N82：ORIG 0x0810bde0；p167 CLEAR（6/6），残差为帧/槽与分支
// 块排布，语义闭合。
void CEventScriptMng::process_account_first_login(
    int key, CUser* user, std::bitset<32u> flags)
{
    if (user == 0 || !is_eventing(key))
        return;

    const int confirmFlag = get_convert_to_user_confirmflag(key);
    if (!flags[confirmFlag])
    {
        CEventEntity* entity = get_event_entity(key);
        if (entity != 0)
            static_cast<CAccountFirstLogin*>(entity)->do_action(
                key, user, flags);
    }
}

// ORIG 0x810c110：活动 0x56 开启时更新用户服务器疲劳值。
void CEventScriptMng::process_add_fatigue_count(CUser* user, int count)
{
    if (user == 0)
        return;
    if (!is_eventing(0x56))
        return;
    user->set_server_fatigue_count((short)count);
}

// ORIG 0x810bf00：活动 0x193 的任务计数命中时累加一次物品计数。
// [B24D判词 N] 语义正确：user!=0 → get_event_entity(0x193)→is_event_quest→
//   IncCurCharacEventAddItemCount(1)。diff=ORIG 句尾 `jmp;nop`（4.1 合并跳转+对齐）
//   我方直接 leave;ret，27/25 纯 lowering/对齐表示差异，语义等价。
void CEventScriptMng::process_add_quest_count_db(CUser* user, int questId)
{
    if (user == 0)
        return;
    CAddItemCountDB* entity =
        static_cast<CAddItemCountDB*>(get_event_entity(0x193));
    if (entity != 0)
    {
        if (entity->is_event_quest(questId))
            user->IncCurCharacEventAddItemCount(1);
    }
}

// ORIG 0x810c0a6：升级奖励按指定等级/成长类型发放。
void CEventScriptMng::process_level_up_reward_recv_level(
    CUser* user, int level, int growType)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* entity =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (entity != 0)
        entity->send_reward_from_recv_level(user, level, growType);
}

// ORIG 0x810c152：按疲劳事件用户类型累加已使用疲劳量。
void CEventScriptMng::inc_user_fatigue(CUser* user,
                                       unsigned int fatigue)
{
    // ORIG 顺序：先 is_eventing(0x199)（bool 经 xor $1 取反，jne 早退），
    // 再 get_event_entity(0x199)；userType 用 unsigned char（movzbl 零扩展
    // 后 int 级 test/cmp 分派），case 0 直接累加 / case 1 需 GetPCRoomNo()。
    if (!is_eventing(0x199))
        return;
    CFatigueQuantity* entity =
        static_cast<CFatigueQuantity*>(get_event_entity(0x199));
    if (entity == 0)
        return;

    // movzbl（零扩展）后 int 级 test/cmp 分派（switch 布局：case0 je/case1 je/
    // 缺省 jmp 出口，userType 留在寄存器不入栈）。case 1 需 GetPCRoomNo()。
    switch ((unsigned char)entity->get_user_type())
    {
    case 0:
        user->IncCurCharacUsedFatigueQuantity(fatigue);
        break;
    case 1:
        if (user->GetPCRoomNo() != 0)
            user->IncCurCharacUsedFatigueQuantity(fatigue);
        break;
    }
}

// ORIG 0x810c1ea：礼物疲劳事件验证通过后发送奖励。
void CEventScriptMng::send_gift_fatigue_item(
    CUser* user, unsigned int fatigue)
{
    // ORIG 形：三段独立守卫（P133；ORIG 每段 jne/je 直跳统一尾部出口，
    // 无 bool 汇聚）；verifyApplyUser bool 直测（test %al）；尾部无返回值。
    if (!is_eventing(0x65))
        return;
    CUsedFatigueGiveItem* entity =
        static_cast<CUsedFatigueGiveItem*>(get_event_entity(0x65));
    if (entity == 0)
        return;
    if (entity->verifyApplyUser(user))
    {
        user->IncCurCharacUsedGiftFatigueQuantity(fatigue);
        entity->sendEventItemFromFatigue(user, fatigue);
    }
}

// ORIG 0x810c270：商城奖励活动开启时更新目标商品计数。
// [B24D判词 N] 语义正确：is_eventing(0x19a)→get_event_entity(0x19a)→
//   updateTargetGoodsCount(user,goodsId,request)。diff=ORIG 句尾 `jmp;nop;jmp`（4.1
//   合并跳转+对齐）我方单 jmp，32/30 纯 lowering/对齐表示差异，语义等价。
void CEventScriptMng::update_purchase_target_item_count(
    CUser* user, unsigned int goodsId, stCeraShopIPGRequest_t* request)
{
    if (!is_eventing(0x19a))
        return;

    CPurchaseCashItemBonus* entity =
        static_cast<CPurchaseCashItemBonus*>(get_event_entity(0x19a));
    if (entity != 0)
        entity->updateTargetGoodsCount(user, goodsId, request);
    return;
}

// ORIG 0x810c2d4：透传商城消费奖励邮件结果。
bool CEventScriptMng::send_purchase_bonus_mail(
    CUser* user, unsigned int goodsId)
{
    if (!is_eventing(0x19a))
        return false;

    CPurchaseCashItemBonus* entity =
        static_cast<CPurchaseCashItemBonus*>(get_event_entity(0x19a));
    if (entity == 0)
        return false;

    return entity->sendPurchaseBonusReward(user, (int)goodsId);
}

// ORIG 0x810c33a：养成事件开启时发送疲劳奖励。
bool CEventScriptMng::send_bring_up_item_fatigue(
    CUser* user, int fatigue)
{
    if (!is_eventing(0x19b))
        return false;

    CObjectBringUp* entity =
        static_cast<CObjectBringUp*>(get_event_entity(0x19b));
    if (entity == 0)
        return false;

    entity->sendEventItemFromFatigue(user, (unsigned int)fatigue);
    return true;
}

// ORIG 0x810c3a4：网吧事件初始化通知。
void CEventScriptMng::send_pc_room_play_init_time(CUser* user)
{
    // ORIG 形：三段独立守卫（P133）+ verify int 返回 `== 0` 直比
    // （probe pr6.cpp h1 实证 4.4 出 test %eax; sete %al; test %al 同形）。
    if (!is_eventing(0x5c))
        return;
    CPcRoomPlayTime* entity =
        static_cast<CPcRoomPlayTime*>(get_event_entity(0x5c));
    if (entity == 0)
        return;
    if (entity->verifyUserPcRoomPlayEvent(user) == 0)
        user->sendNotiPcRoomPlayTime();
}

// ORIG 0x810c40e：包 0x176 —— put_header(0,0x176)、put_int(m_todayTime)、
// 占位 byte 后遍历实体：is_eventing 的实体调用虚 send_init_data(user,packet,
// key,m_todayTimeDay,flag)，成功则计数+1；回填计数、finalize 并 CUser::Send。
// [b28d-27] DIFF N82：ORIG 0x0810c40e；p167 CLEAR（19/19），残差为帧/槽分配，
// 语义闭合。
void CEventScriptMng::send_event_init_data(CUser* user, bool flag)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.put_header(0, 0x176);
    packet.put_int(m_todayTime);
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
                                           m_todayTimeDay, flag))
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
    // P46：稀疏 switch 叶块序=case 书写序（0,1,2 与 ORIG 0x810b0cc 块序
    // 一致）；4.4 决策树根取中值 0x192（与 ORIG 首比较同）。
    // ORIG 有 v 槽（-0x4）：初值 -1 + 各 case 槽写 + 尾 mov -0x4,%eax 回读。
    int v = -1;
    switch (key)
    {
    case 0x191:
        v = 0;
        break;
    case 0x192:
        v = 1;
        break;
    case 0x19c:
        v = 2;
        break;
    }
    return v;
}

// ORIG 0x810b96a：map find，命中返回实体指针，未命中返回 0。
CEventEntity* CEventScriptMng::get_event_entity(int key)
{
    // ORIG 0x810b96a：find != end 先行（ne），命中返回 second。
    std::map<int, CEventEntity*>::iterator it = m_entities.find(key);
    if (it != m_entities.end())
        return it->second;
    return 0;
}

// ORIG 0x810b9d4（const）：map find != end。
bool CEventScriptMng::is_event_entity(int key) const
{
    if (m_entities.find(key) != m_entities.end())
        return true;
    return false;
}

// ORIG 0x810c7b8：实体存在则调用虚 get_event_record_type（vtable+0x14）。
int CEventScriptMng::get_record_type(int key)
{
    // ORIG 0x810c7b8：独立 map find（不调 get_event_entity），
    // 未命中返回 0，命中虚调 get_event_record_type（vtable+0x14）。
    std::map<int, CEventEntity*>::iterator it = m_entities.find(key);
    if (it != m_entities.end())
        return it->second->get_event_record_type();
    return 0;
}

int CEventScriptMng::get_today_time()
{
    return m_todayTime;
}

unsigned char CEventScriptMng::get_today_time_day()
{
    return m_todayTimeDay;
}

bool CEventScriptMng::is_current_ingame_event_record_type(int key)
{
    std::map<int, InGameEventInfo>::iterator it = m_info.find(key);
    if (it != m_info.end())
        return true;
    return false;
}

void CEventScriptMng::get_ingame_event_record_type_list(std::vector<int>& out)
{
    out.clear();
    for (std::map<int, InGameEventInfo>::iterator it = m_info.begin(),
             endIt = m_info.end();
         it != endIt; ++it)
        out.push_back(it->first);
}

int CEventScriptMng::get_pass_day(int yyyymmdd1, int yyyymmdd2)
{
    if (yyyymmdd1 < 1 && yyyymmdd2 < 1)
        return -1;

    // ORIG 0x810c974：{0} 初始化产生 rep stos 内联（P4），非 call memset。
    struct tm t1 = {0};
    t1.tm_year = yyyymmdd1 / 10000 - 1900;
    t1.tm_mon  = (yyyymmdd1 % 10000) / 100 - 1;
    t1.tm_mday = yyyymmdd1 % 100;
    t1.tm_hour = 0;
    time_t v1 = std::mktime(&t1);

    struct tm t2 = {0};
    t2.tm_year = yyyymmdd2 / 10000 - 1900;
    t2.tm_mon  = (yyyymmdd2 % 10000) / 100 - 1;
    t2.tm_mday = yyyymmdd2 % 100;
    t2.tm_hour = 0;
    time_t v2 = std::mktime(&t2);

    if (v2 >= v1)
        return static_cast<int>((v2 - v1) / 0x15180);
    return -1;
}

// [b28d-27] DIFF N82：ORIG 0x0810c5d2；p167 CLEAR（4/4），残差为帧/槽与 time_t
// 装载序，语义闭合。
void CEventScriptMng::update_time_values(int hour)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // ORIG 对 now 与 prev 各独立调一次 getCurSec()（两次 call），
    // prev = getCurSec() - 0x15180，不复用 now 结果。
    time_t prev = GlobalData::s_systemTime_.getCurSec() - 0x15180;
    struct tm localT, prevT;
    // ORIG 保存 localtime_r 返回的指针局部（-0x14=&localT / -0x10=&prevT），
    // 后续经指针成员访问（==&localT/+0x8 tm_hour 等），非每次 lea 重求址。
    struct tm* localTp = localtime_r(&now, &localT);
    struct tm* prevTp = localtime_r(&prev, &prevT);

    if (localTp->tm_hour < 0 || hour <= localTp->tm_hour)
    {
        m_todayTimeDay = static_cast<unsigned char>(localTp->tm_mday);
        m_todayTime = localTp->tm_year * 10000 + (localTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
    else
    {
        m_todayTimeDay = static_cast<unsigned char>(prevTp->tm_mday);
        m_todayTime = prevTp->tm_year * 10000 + (prevTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
}

void CEventScriptMng::init_event_nums()
{
    m_eventRecordTypes.clear();
    m_eventRecordTypes.push_back(0x56);
    m_eventRecordTypes.push_back(0x191);
    m_eventRecordTypes.push_back(0x192);
    m_eventRecordTypes.push_back(0x193);
    m_eventRecordTypes.push_back(0x194);
    m_eventRecordTypes.push_back(0x196);
    m_eventRecordTypes.push_back(0x197);
    m_eventRecordTypes.push_back(0x199);
    m_eventRecordTypes.push_back(0x19a);
    m_eventRecordTypes.push_back(0x19b);
    m_eventRecordTypes.push_back(0x5c);
    m_eventRecordTypes.push_back(0x65);
    m_eventRecordTypes.push_back(0x19c);
}

void CEventScriptMng::init_evnet_entity()
{
    init_event_nums();
    for (int i = 0; (int)m_eventRecordTypes.size() > i; ++i)
    {
        if (get_event_entity(m_eventRecordTypes[i]) == 0)
            m_entities[m_eventRecordTypes[i]] =
                create_event_entity(m_eventRecordTypes[i], true);
    }
}

bool CEventScriptMng::initScriptlist(InGameEventInfoList& list)
{
    // [b28d-27] ORIG 0x0810ba7a（p167：ptEv×11 / map::operator[]×4）：ORIG 每个
    // 字段访问都重发 `it->second.xxx` 与 `m_entities[...]`（P157/P183 禁缓存
    // 引用/指针）；旧实现用 `InGameEventInfo& info` + `CEventEntity*& slot`
    // 缓存 ⇒ 少 11 条 ptEv + 3 条 operator[]。已修：ndiff 302→113；残差为
    // 帧/槽分配（N82），p167 差集已清零。
    for (std::map<int, InGameEventInfo>::iterator it = list.m_info.begin();
         it != list.m_info.end(); ++it)
    {
        if (it->second.m_isEventing == 0)
            continue;

        std::string name(toTString(it->second.m_name));
        InGameEventScript* script =
            InGameEventScriptLoader::getEventScript(it->second.m_eventId, name);
        if (script == 0)
            return false;

        m_entities[it->second.m_eventId] =
            create_event_entity(it->second.m_eventId, false);
        m_entities[it->second.m_eventId]->set_event_flag(
            it->second.m_isEventing != 0);
        m_entities[it->second.m_eventId]->set_event_data(it->first, script);
        m_entities[it->second.m_eventId]->set_event_start_time(
            it->second.m_startTime);
        m_entities[it->second.m_eventId]->set_event_end_time(
            it->second.m_endTime);
    }

    init_evnet_entity();
    return true;
}


// [b28d-27] DIFF N82：ORIG 0x0810bf56；p167 CLEAR（16/16），残差为帧/槽与
// 分支块排布，语义闭合。
void CEventScriptMng::process_level_up_reward(CUser* user, bool flag)
{
    if (user == 0 || !is_eventing(0x194))
        return;

    CLevelUpReward* reward =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (reward->is_recv_check_item() != 0)
    {
        const int ok = reward->check_recv_level(user->get_charac_level(),
                                                user->get_charac_job(),
                                                user->getCurCharFirstGrowType(),
                                                flag);
        if (ok != 0)
            DB_SelectLevelUpReward::makeRequest(
                static_cast<int>(user->GetUID()),
                user->getCurCharacNo(),
                user->get_charac_level());
    }
    else if (reward->set_reward_item(user->get_charac_level(),
                                     user->get_charac_job(),
                                     user->getCurCharFirstGrowType()) != 0)
    {
        reward->do_action(user);
    }
}

}  // namespace EventClassify

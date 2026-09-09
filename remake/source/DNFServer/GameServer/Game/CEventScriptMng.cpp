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

// [b28d-solo-CEventScriptMng] getEventScript：NEAR — N93 封顶。
// ORIG 0x08982f38：find 目标槽 -0x10 / end 目标槽 -0xc（4.1 先声明占深槽且调用序
// find→end）；4.4.7 按首次使用序分配，恒反向（find→-0xc / end→-0x10）。
// 实测三形：①本形（it 先声明）find 槽 -0xc；②交换声明序（endIt 先）调用序变
// end→find，指令序整体错位；③先声明后赋值引入 ORIG 不存在的
// _Rb_tree_iterator::C1Ev（nm 实证）⇒ 槽方向与调用序不可同时满足，封顶。
// p167 CLEAR（6/6），语义闭合。
InGameEventScript* InGameEventScriptLoader::getEventScript(int id,
                                                           const std::string& name)
{
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

// [b28d-solo-CEventScriptMng] ~CEventScriptMng：AE。
// ORIG 0x0810b218：两个 map 循环各自 begin/end 缓存（P13），修复前每轮重调 end()
// 多占 sret 临时槽并顶偏全帧 ⇒ 缓存后槽位逐条对齐、compare AE。
// p167 余 2×D1Ev↔2×D2Ev 为同址别名（ORIG 0x08110f24/0x08110f60 与我们的 .o 均
// 同址 W 双名，P93 伪差集），compare 判 AE，语义闭合。
CEventScriptMng::~CEventScriptMng()
{
    // ORIG 0x0810b218：end() 在循环初始化区求值一次并缓存（P13）——
    // begin 槽 -0xc / end 槽 -0x10（脚本 map），begin 槽 -0x14 / end 槽 -0x18
    // （实体 map）；`it != m.end()` 每轮重调 end() 会多出 end/临时槽并使全帧漂移。
    for (std::map<std::string, InGameEventScript*>::iterator it =
             m_scripts.begin(), endIt = m_scripts.end();
         it != endIt; ++it)
    {
        if (it->second != 0)
            delete it->second;
        it->second = 0;
    }

    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin(),
             endIt = m_entities.end();
         it != endIt; ++it)
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

// [b28d-solo-CEventScriptMng] process_account_first_login：NEAR — N7 封顶。
// ORIG 0x0810bde0：两条独立守卫（P133）+ 匿名 bitset reference 临时（槽 -0x14）；
// 修复前合并 `||` 触发 bool 汇聚（mov$1/jmp/mov$0/test）⇒ 拆两条 if 后守卫岛
// （nop;jmp + nop）与全帧槽位逐条对齐。
// 残差唯一：尾部虚调用目标寄存器 ORIG %edx vs 4.4.7 恒 %eax（N7；本 TU 未走
// 4.4.6 路由，P230 分诊=后端敏感族）。p167 CLEAR（6/6），语义闭合。
void CEventScriptMng::process_account_first_login(
    int key, CUser* user, std::bitset<32u> flags)
{
    // P133 独立守卫；confirmFlag 无栈槽（P0：值经 %edx 直传 operator[] 实参）。
    if (user == 0)
        return;
    if (!is_eventing(key))
        return;

    if (!flags[get_convert_to_user_confirmflag(key)])
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

// [b28d-solo-CEventScriptMng] send_event_init_data：DIFF — N82附 封顶。
// ORIG 0x0810c40e：包 0x176；end() 循环头缓存（P13）后全帧槽位逐条对齐
// （PacketGuard -0x28 / countIdx -0x2c / it -0x30 / endIt -0x34 / vfn -0x40 /
// count -0x19 与 ORIG 全等）。
// 残差唯一：多实参虚调用三活值（vfn/flag/m_todayTimeDay）的 callee-saved 角色
// 对调——ORIG vfn→槽 -0x40、flag→%edi、day→%esi；4.4.7 vfn→%esi、flag→槽
// -0x40、day→%edi（多 1 条 `mov -0x40(%ebp),%edx` 回读，135 vs 136 条）。
// 实测两形：①原实参形（本形）；②P226 语句表达式 `({register bool f=flag;f;})`
// 反劣（多 movzbl 中转，且循环旋转）。flag 的裸 register 三条件中「4.4 不自行
// 升格」不成立 ⇒ 无源码解，封 N82附（多实参 call 首值 callee-saved 对调）。
// p167 CLEAR（19/19），语义闭合。
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
    // ORIG 0x0810c487：end() 在循环初始化区求值一次（P13），it 槽 -0x30 /
    // endIt 槽 -0x34；每轮重调 end() 会多占 sret 临时槽并顶偏 PacketGuard 槽。
    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin(),
             endIt = m_entities.end();
         it != endIt; ++it)
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

// [b28d-solo-CEventScriptMng] update_time_values：AE。
// ORIG 0x0810c5d2：两处修复——① 0x0810c628 `movl $0x0,-0xc(%ebp)` 死局部
// （P232 死槽占帧，令 now/prev/localT/prevT/指针组整体下移 4，全帧槽位与 ORIG
// 全等）；② 0x0810c62f 的 `||` 条件两条守卫（js/jge）同落**出线**的 localTp 块、
// prevTp 块 fall-through（4.1 布局）⇒ 用德摩根正向包裹（P222/P272）条件取反、
// 两块互换复现。p167 CLEAR（4/4），语义闭合。
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

    // ORIG 0x0810c628：`movl $0x0,-0xc(%ebp)` 死局部（此后从不被读；P232 死槽
    // 占帧，使 now/prev/localT/prevT/指针组整体下移 4，槽位与 ORIG 全等）。
    int deadFlag = 0;

    // ORIG 0x0810c62f：两条守卫（js/jge）同落**出线**的 localTp 块，prevTp 块为
    // fall-through（4.1 的 `||` 条件 then 块外置）⇒ 4.4 复现形为德摩根正向包裹
    // （P222/P272）：条件取反、两块互换，语义与 ORIG 完全一致。
    if (localTp->tm_hour >= 0 && localTp->tm_hour < hour)
    {
        m_todayTimeDay = static_cast<unsigned char>(prevTp->tm_mday);
        m_todayTime = prevTp->tm_year * 10000 + (prevTp->tm_mon + 1) * 100
                      + 19000000 + static_cast<unsigned char>(m_todayTimeDay);
    }
    else
    {
        m_todayTimeDay = static_cast<unsigned char>(localTp->tm_mday);
        m_todayTime = localTp->tm_year * 10000 + (localTp->tm_mon + 1) * 100
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
    // [b28d-solo-CEventScriptMng] initScriptlist：AE。
    // ORIG 0x0810ba7a：每个字段访问重发 `it->second.xxx` / `m_entities[...]`
    // （P157/P183 禁缓存引用/指针）。
    // ORIG 结构：end() 循环头缓存一次（P13，it 槽 -0x18 / endIt 槽 -0x1c）；
    // m_isEventing 正极性包裹（P214①：ORIG `setne;test;je <自增>`，负写
    // `==0 + continue` 反产 sete/jne）；name 为 getEventScript 实参内的**匿名
    // string 临时**（P0：ORIG 在 null 检查前即 ~string/~allocator，具名局部
    // 会延后到语句尾并多一组 EH 记账）。三处修毕 compare AE；
    // p167 CLEAR（35/35）。
    for (std::map<int, InGameEventInfo>::iterator it = list.m_info.begin(),
             endIt = list.m_info.end();
         it != endIt; ++it)
    {
        if (it->second.m_isEventing != 0)
        {
            InGameEventScript* script =
                InGameEventScriptLoader::getEventScript(
                    it->second.m_eventId, toTString(it->second.m_name));
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
    }

    init_evnet_entity();
    return true;
}


// [b28d-solo-CEventScriptMng] process_level_up_reward：AE。
// ORIG 0x0810bf56：三条独立守卫（user==0 / !is_eventing / reward!=0 正向包裹，
// P133 + P214①；尾部两岛 nop;jmp + nop 与 ORIG 全等）；check_recv_level 结果
// 无具名局部（P0：call 结果寄存器直测）；flag 形参入口 spill 到 -0x2c（bool
// 形参一次性截断，P244）。
// 三个 callee 返回类型链：ORIG 0x0810bfb8/0x0810c005/0x0810c000/0x0810c07c 均
// `call; test %al,%al; je`；4.4.7 探针对 char/int 返回恒产 4 条（调用点
// cast/!=0/!! 四形全败），仅 bool 返回产 2 条 ⇒ 由 Main 代落
// CEventScriptMng.h is_recv_check_item/check_recv_level/set_reward_item → bool
// （定义端 EventLevelupA2/CEventScriptMng_entities 同步；P248 档①/P315）后
// 6 条残差归零。p167 CLEAR（16/16），语义闭合。
void CEventScriptMng::process_level_up_reward(CUser* user, bool flag)
{
    if (user == 0)
        return;
    if (!is_eventing(0x194))
        return;

    CLevelUpReward* reward =
        static_cast<CLevelUpReward*>(get_event_entity(0x194));
    if (reward != 0)
    {
        if (reward->is_recv_check_item() != 0)
        {
            if (reward->check_recv_level(user->get_charac_level(),
                                         user->get_charac_job(),
                                         user->getCurCharFirstGrowType(),
                                         flag) != 0)
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
}

}  // namespace EventClassify

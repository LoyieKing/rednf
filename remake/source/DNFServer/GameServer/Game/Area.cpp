#include "LogManager.h"
#include "GameRand.h"
// ============================================================================
// df_game_r Area（sizeof 0x98）G2-5 批次还原
//
// 逐函数对照 docs/class_func_reports/Area.md（汇编为准，Ghidra C 语义参考）
// 与 ORIG 反汇编实现；布局依据 ORIG 构造/析构（0x86d214c / 0x86d2270）与
// set_area（0x86c1d14）/ take_fish（0x86c21e2）字段访问推导：
//   +0x00 int  m_areaIndex            （get_area_index / set_area）
//   +0x04 int  m_villageIndex         （set_area 写入，发包 put_byte）
//   +0x08 int  m_worldMapIndex        （set_area 类型 4 取 script[0x18][0]）
//   +0x0c Mutex（0x18；ORIG 内嵌 pthread_mutex_t）
//   +0x24 LotteryInfo（0x20：defaultIdx/items/defaultCount/counts）
//   +0x44 std::map<unsigned short, CUser*> m_UsersInArea
//   +0x5c std::vector<int> m_npcIdList（hasNPC）
//   +0x68 int  m_areaType（1=村庄 2=钓鱼/竞技场 4=特殊）
//   +0x6c uchar m_field6c（STMapScript 限制交易标记，默认 1）
//   +0x70 std::vector<std::pair<int,int> > m_npcPositions（AssignNpc 坐标）
//   +0x7c std::vector<MapArea> m_mapAreas
//   +0x88 std::vector<std::pair<int,int> > m_moveAbleArea（IsMovalbeArea）
//   +0x94 uchar m_bExistItemShopNPC（IsExistItemShopNPC / set_area 0x7f NPC）
// sizeof = 0x98（0x95..0x97 对齐填充）。
//
// 跨类（CUser/PacketGuard/CBelong/管理器等）以最小声明 + extern "C" asm 标签
// 指向 ORIG 真实符号；本 TU 自包含、只编译不链接（check_tu_game_orig.sh 单 TU
// 验证）。异常处理器（insert_user 的 map::insert bad_alloc 捕获）按语义简化。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

// ---------------------------------------------------------------------------
// 跨类最小声明
// ---------------------------------------------------------------------------
class CUser;
class PacketGuard;
class CBelong;

enum ch_state
{
    ch_state_0 = 0,
    ch_state_3 = 3,
    ch_state_4 = 4
};

enum ENUM_NPC_ROLE
{
    ENUM_NPC_ROLE_NONE = 0
};

// ---- InterfacePacketBuf / PacketGuard（ORIG 858e294 / 858dd4c / 858de80）----
class InterfacePacketBuf
{
public:
    void put_header(int type, int size);
    void put_byte(int b);
    void put_short(int s);
    void put_short(int& index, int count);
    void finalize(bool encryptRequired);
    void clear();
    int get_index();
};

class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();
};

// ---- CUser / CUserCharacInfo（以 CUser 指针为 this 的真实符号）----
extern "C" unsigned short sub_CUser_get_unique_id(const void* self)
    asm("_ZNK5CUser13get_unique_idEv");
extern "C" int sub_CUser_get_state(void* self)
    asm("_ZN5CUser9get_stateEv");
extern "C" void sub_CUser_set_state(void* self, ch_state st)
    asm("_ZN5CUser9set_stateE8ch_state");
extern "C" int sub_CUser_get_area(void* self, bool b)
    asm("_ZN5CUser8get_areaEb");
extern "C" unsigned short sub_CUser_get_posX(void* self)
    asm("_ZN5CUser8get_posXEv");
extern "C" unsigned short sub_CUser_get_posY(void* self)
    asm("_ZN5CUser8get_posYEv");
extern "C" char sub_CUser_get_direction(void* self)
    asm("_ZN5CUser13get_directionEv");
extern "C" void sub_CUser_Send(void* self, PacketGuard& packet)
    asm("_ZN5CUser4SendER11PacketGuard");

extern "C" char sub_CUserCharacInfo_getCurCharacVill(const void* self)
    asm("_ZNK15CUserCharacInfo16getCurCharacVillEv");
extern "C" char sub_CUserCharacInfo_get_charac_visible_values(const void* self)
    asm("_ZN15CUserCharacInfo25get_charac_visible_valuesEv");
extern "C" char sub_CUserCharacInfo_IsCurCharacVisible(const void* self)
    asm("_ZN15CUserCharacInfo18IsCurCharacVisibleEv");
extern "C" unsigned short sub_CUserCharacInfo_getPowerWarHP(const void* self)
    asm("_ZN15CUserCharacInfo13getPowerWarHPEv");

// ---- pvp_assault ----
namespace pvp_assault
{
class CAssaultMgr;
}
extern "C" void* sub_GetInstanceAssaultMgr()
    asm("_ZN11pvp_assault21GetInstanceAssaultMgrEv");
extern "C" void sub_CAssaultMgr_OnMoveArea(void* mgr, void* user)
    asm("_ZN11pvp_assault11CAssaultMgr10OnMoveAreaEP5CUser");

// ---- village_object ----
namespace village_object
{
class IObject;
class CVillageObjectMgr;
struct Zone
{
    Zone(unsigned char a, unsigned char b);
    unsigned short m_field0;   // +0x00（2 字节）
};
}
extern "C" void sub_village_object_on_move_area(void* mgr, void* user)
    asm("_ZN14village_object17CVillageObjectMgr12on_move_areaEP5CUser");
extern "C" void sub_village_object_register_object(
    void* mgr, village_object::Zone zone, void* obj)
    asm("_ZN14village_object17CVillageObjectMgr15register_objectENS_4ZoneEPNS_7IObjectE");

// ---- village_attacked ----
namespace village_attacked
{
class CVillageMonsterMgr;
}
extern "C" void sub_village_monster_OnMoveArea(void* mgr, void* user)
    asm("_ZN16village_attacked18CVillageMonsterMgr10OnMoveAreaEP5CUser");
extern "C" void sub_village_monster_InsertVillageMonster(
    void* mgr, int a, int b, int c, int d,
    std::vector<struct STAttackedMonster>* monsters,
    std::vector<struct MapArea>* areas)
    asm("_ZN16village_attacked18CVillageMonsterMgr20InsertVillageMonsterEiiiiPSt6vectorI17STAttackedMonsterSaIS2_EEPS1_I7MapAreaSaIS6_EE");

// ---- CTimeGate ----
extern "C" void sub_CTimeGate_SetTimeGate(void* self, int a, int b, int c, int d, int e)
    asm("_ZN9CTimeGate11SetTimeGateEiiiii");

// ---- CDataManager / CNPCScriptList / CNPCScript ----
extern "C" void* sub_G_CDataManager() asm("_Z14G_CDataManagerv");
extern "C" void* sub_CNPCScriptList_find(const void* self, unsigned int id)
    asm("_ZNK14CNPCScriptList4findEj");
extern "C" char sub_CNPCScript_isExistRole(const void* self, ENUM_NPC_ROLE role)
    asm("_ZNK10CNPCScript11isExistRoleE13ENUM_NPC_ROLE");

extern "C" void sub_createLotteryInfo(void* lottery, std::vector<int>* counts)
    asm("_Z17createLotteryInfoP11LotteryInfoPSt6vectorIiSaIiEE");

// ---- GlobalData 单例指针（ORIG BSS：0941f774/0941f77c/0941f7f4）----
extern "C" void* GlobalData_s_villageObjectMgr
    asm("_ZN10GlobalData18s_villageObjectMgrE");
extern "C" void* GlobalData_s_villageMonsterMgr
    asm("_ZN10GlobalData19s_villageMonsterMgrE");
extern "C" void* GlobalData_s_timeGate_
    asm("_ZN10GlobalData11s_timeGate_E");

// ---------------------------------------------------------------------------
// Area 依赖结构（字段布局按 ORIG 反汇编偏移）
// ---------------------------------------------------------------------------
struct MapArea
{
    int m_x;    // +0x00
    int m_y;    // +0x04
    int m_w;    // +0x08
    int m_h;    // +0x0c
};

struct STAttackedMonster
{
    char m_pad[0x10];
};

// LotteryInfo：0x20 字节（Area 内嵌于 +0x24；ORIG C1 083706d4）
struct LotteryInfo
{
    LotteryInfo();
    ~LotteryInfo();

    int m_defaultIdx;                          // +0x00
    std::vector<std::pair<int, int> > m_items; // +0x04（take_fish）
    int m_defaultCount;                        // +0x10
    std::vector<int> m_counts;                 // +0x14
};

// Mutex：0x18 字节（Area 内嵌于 +0x0c；ORIG C1 080cb412）
class Mutex
{
public:
    Mutex();
    ~Mutex();

private:
    char m_pad[0x18];
};

// set_area 输入结构
struct AssignNpc
{
    int m_npcId;    // +0x00
    int m_pad4;     // +0x04
    int m_pad8;     // +0x08
    int m_pad0c;    // +0x0c
    int m_x;        // +0x10
    int m_y;        // +0x14
};

struct TownPath
{
    char m_pad0[0x1c];  // +0x00
    int m_field1c;      // +0x1c
    int m_field20;      // +0x20
};

struct stAttackedMapInfo_t
{
    int m_vill;     // +0x00
    int m_area;     // +0x04
    int m_field8;   // +0x08
    int m_field0c;  // +0x0c
};

struct TownAreaScript
{
    int m_field0;                  // +0x00（区域索引）
    int m_pad4;                    // +0x04
    int m_field8;                  // +0x08（区域类型）
    char m_pad0c[0x18 - 0x0c];     // +0x0c
    std::vector<int> m_field18;    // +0x18
    unsigned char m_field24;       // +0x24
};

struct STMapScript
{
    char m_pad0[0x60];                                // +0x00
    std::vector<AssignNpc> m_assignNpcs;              // +0x60
    char m_pad6c[0xdc - 0x6c];                        // +0x6c
    std::vector<TownPath> m_townPaths;                // +0xdc
    std::vector<MapArea> m_mapAreas;                  // +0xe8
    char m_padf4[0x318 - 0xf4];                       // +0xf4
    std::vector<STAttackedMonster> m_attackedMonsters;  // +0x318
};

// ---------------------------------------------------------------------------
// Area（sizeof 0x98）
// ---------------------------------------------------------------------------
class Area
{
public:
    Area();                                        // ORIG 086d214c（弱）
    ~Area();                                       // ORIG 086d2270（弱）

    int GetUserCount() const;                      // ORIG 086c31f2
    bool IsMovalbeArea(int vill, int area) const;  // ORIG 086c3162
    void delete_user(CUser* user);                 // ORIG 086c2a38
    void erase_user(CUser* user);                  // ORIG 086c2be4
    void get_user_id_list(std::vector<unsigned short>& out); // ORIG 086c305e
    bool hasNPC(unsigned int npcId) const;         // ORIG 086c214c
    void insert_user(CUser* user);                 // ORIG 086c25a6
    void make_power_war_hp_of_vill(PacketGuard& packet, int& count); // ORIG 086c2c1e
    void send_area_users(CUser* user);             // ORIG 086c22f2
    void send_group(PacketGuard& packet, CBelong* belong); // ORIG 086c2e1e
    void send_to_all(PacketGuard& packet);         // ORIG 086c2d2a
    void send_to_all(PacketGuard& packet,
                     std::vector<CUser*>& users);  // ORIG 086c2f2c
    void set_area(int vill, TownAreaScript& script,
                  STMapScript& mapScript);         // ORIG 086c1d14
    int take_fish();                               // ORIG 086c21e2
    int get_area_index();                          // ORIG 086d238e（弱）
    int get_world_map_index();                     // ORIG 086d2398（弱）
    bool IsExistItemShopNPC();                     // ORIG 082343aa（弱）

private:
    int m_areaIndex;             // +0x00
    int m_villageIndex;          // +0x04
    int m_worldMapIndex;         // +0x08
    Mutex m_mutex;               // +0x0c（0x18）
    LotteryInfo m_lottery;       // +0x24（0x20）
    std::map<unsigned short, CUser*> m_UsersInArea;  // +0x44（0x18）
    std::vector<int> m_npcIdList;                    // +0x5c
    int m_areaType;              // +0x68
    unsigned char m_field6c;     // +0x6c
    char m_pad6d[0x70 - 0x6d];   // +0x6d
    std::vector<std::pair<int, int> > m_npcPositions;  // +0x70
    std::vector<MapArea> m_mapAreas;                   // +0x7c
    std::vector<std::pair<int, int> > m_moveAbleArea;  // +0x88
    unsigned char m_bExistItemShopNPC;                 // +0x94
    char m_pad95[0x98 - 0x95];   // +0x95
};

// ---------------------------------------------------------------------------
// 构造 / 析构（成员按声明序构造、逆序析构，与 ORIG 086d214c/086d2270 一致）
// ---------------------------------------------------------------------------
Area::Area()
{
    m_areaIndex = -1;
    m_bExistItemShopNPC = 0;
    m_field6c = 1;
}

Area::~Area()
{
    m_areaIndex = -1;
}

// ---------------------------------------------------------------------------
// GetUserCount：统计 m_UsersInArea 中 state > 2 的在线用户
// ---------------------------------------------------------------------------
int Area::GetUserCount() const
{
    int count = 0;
    for (std::map<unsigned short, CUser*>::const_iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && sub_CUser_get_state(user) > 2)
            ++count;
    }
    return count;
}

// ---------------------------------------------------------------------------
// IsMovalbeArea：m_moveAbleArea（TownPath 起点对集合）是否包含 (vill, area)
// ---------------------------------------------------------------------------
bool Area::IsMovalbeArea(int vill, int area) const
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_moveAbleArea.begin();
         it != m_moveAbleArea.end(); ++it)
    {
        if (it->first == vill && it->second == area)
            return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// delete_user：非村庄区域从地图移除用户并广播 0x17 离开包
// ---------------------------------------------------------------------------
void Area::delete_user(CUser* user)
{
    if (m_areaType == 1)
        return;

    m_UsersInArea.erase(sub_CUser_get_unique_id(user));

    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short(sub_CUser_get_unique_id(user));
    packet.put_byte(sub_CUserCharacInfo_getCurCharacVill(user));
    packet.put_byte(sub_CUser_get_area(user, true));
    packet.put_short(sub_CUser_get_posX(user));
    packet.put_short(sub_CUser_get_posY(user));
    packet.put_byte(sub_CUser_get_direction(user));
    packet.put_byte(sub_CUserCharacInfo_get_charac_visible_values(user));
    packet.finalize(true);

    if (sub_CUserCharacInfo_IsCurCharacVisible(user))
        send_to_all(packet);
    else
        sub_CUser_Send(user, packet);
}

// ---------------------------------------------------------------------------
// erase_user：非村庄区域仅从地图移除用户（不发包）
// ---------------------------------------------------------------------------
void Area::erase_user(CUser* user)
{
    if (m_areaType != 1)
        m_UsersInArea.erase(sub_CUser_get_unique_id(user));
}

// ---------------------------------------------------------------------------
// get_user_id_list：输出 state > 2 的在线用户 unique_id
// ---------------------------------------------------------------------------
void Area::get_user_id_list(std::vector<unsigned short>& out)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && sub_CUser_get_state(user) > 2)
        {
            unsigned short id = sub_CUser_get_unique_id(user);
            out.push_back(std::move(id));
        }
    }
}

// ---------------------------------------------------------------------------
// hasNPC：m_npcIdList 是否包含指定 NPC
// ---------------------------------------------------------------------------
bool Area::hasNPC(unsigned int npcId) const
{
    return std::find(m_npcIdList.begin(), m_npcIdList.end(), npcId) !=
           m_npcIdList.end();
}

// ---------------------------------------------------------------------------
// insert_user：进图（村庄广播 0x18 / 普通广播 0x17 + 各管理器 OnMoveArea）
// ---------------------------------------------------------------------------
void Area::insert_user(CUser* user)
{
    PacketGuard packet;

    if (m_areaType == 1)
    {
        sub_CUser_set_state(user, ch_state_3);
        packet.put_header(0, 0x18);
        packet.put_byte(m_villageIndex);
        packet.put_byte(sub_CUser_get_area(user, true));
        packet.put_short(1);
        packet.put_short(sub_CUser_get_unique_id(user));
        packet.put_short(sub_CUser_get_posX(user));
        packet.put_short(sub_CUser_get_posY(user));
        packet.put_byte(sub_CUser_get_direction(user));
        packet.put_byte(sub_CUserCharacInfo_get_charac_visible_values(user));
        packet.finalize(true);
        sub_CUser_Send(user, packet);
        return;
    }

    if (m_areaType == 2)
        sub_CUser_set_state(user, ch_state_4);
    else
        sub_CUser_set_state(user, ch_state_3);

    unsigned short id = sub_CUser_get_unique_id(user);
    if (m_UsersInArea.find(id) != m_UsersInArea.end())
    {
        LogManager::logFormat(1, "world.cpp",
                                 "void Area::insert_user(CUser*)", 0x1a7,
                                 "m_UsersInArea.find(%d)", (int)id);
        return;
    }

    m_UsersInArea.insert(std::pair<unsigned short, CUser*>(id, user));

    packet.put_header(0, 0x17);
    packet.put_short(id);
    packet.put_byte(m_villageIndex);
    packet.put_byte(sub_CUser_get_area(user, true));
    packet.put_short(sub_CUser_get_posX(user));
    packet.put_short(sub_CUser_get_posY(user));
    packet.put_byte(sub_CUser_get_direction(user));
    packet.put_byte(sub_CUserCharacInfo_get_charac_visible_values(user));
    packet.finalize(true);

    if (sub_CUserCharacInfo_IsCurCharacVisible(user))
        send_to_all(packet);
    else
        sub_CUser_Send(user, packet);

    send_area_users(user);
    sub_CAssaultMgr_OnMoveArea(sub_GetInstanceAssaultMgr(), user);
    sub_village_object_on_move_area(GlobalData_s_villageObjectMgr, user);
    sub_village_monster_OnMoveArea(GlobalData_s_villageMonsterMgr, user);
}

// ---------------------------------------------------------------------------
// make_power_war_hp_of_vill：收集在线用户 PowerWar HP 到包并计数
// ---------------------------------------------------------------------------
void Area::make_power_war_hp_of_vill(PacketGuard& packet, int& count)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        if (user == 0 || sub_CUser_get_state(user) <= 2)
            continue;
        packet.put_short(sub_CUserCharacInfo_getPowerWarHP(user));
        ++count;
    }
}

// ---------------------------------------------------------------------------
// send_area_users：向新进图用户发送同区可见用户列表（0x18，人数回填）
// ---------------------------------------------------------------------------
void Area::send_area_users(CUser* user)
{
    if (sub_CUserCharacInfo_getCurCharacVill(user) != m_villageIndex ||
        sub_CUser_get_area(user, false) != m_areaIndex)
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x18);
    packet.put_byte(m_villageIndex);
    packet.put_byte(sub_CUser_get_area(user, true));

    int index = packet.get_index();
    int count = 0;
    packet.put_short((int)m_UsersInArea.size());

    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* u = it->second;
        if (sub_CUserCharacInfo_IsCurCharacVisible(u) || u == user)
        {
            packet.put_short(sub_CUser_get_unique_id(u));
            packet.put_short(sub_CUser_get_posX(u));
            packet.put_short(sub_CUser_get_posY(u));
            packet.put_byte(sub_CUser_get_direction(u));
            packet.put_byte(sub_CUserCharacInfo_get_charac_visible_values(u));
            ++count;
        }
    }

    packet.put_short(index, count);
    packet.finalize(true);
    sub_CUser_Send(user, packet);
}

// ---------------------------------------------------------------------------
// send_group：按 CBelong 虚谓词（vtable slot 0）过滤后发送
// ---------------------------------------------------------------------------
class CBelong
{
public:
    virtual char IsBelong(CUser* user) = 0;
};

void Area::send_group(PacketGuard& packet, CBelong* belong)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        if (user == 0 || sub_CUser_get_state(user) <= 2)
            continue;
        if (belong->IsBelong(user))
            sub_CUser_Send(user, packet);
    }
}

// ---------------------------------------------------------------------------
// send_to_all：向 state > 2 的在线用户广播包
// ---------------------------------------------------------------------------
void Area::send_to_all(PacketGuard& packet)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && sub_CUser_get_state(user) > 2)
            sub_CUser_Send(user, packet);
    }
}

void Area::send_to_all(PacketGuard& packet, std::vector<CUser*>& users)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
         it != m_UsersInArea.end(); ++it)
    {
        CUser* user = it->second;
        // ORIG 086c2f2c：内层对 users 逐一比较，结果未使用（死代码），保留遍历。
        for (std::vector<CUser*>::size_type i = 0; i < users.size(); ++i)
        {
            if (users[i] == user)
            {
            }
        }
        if (user != 0 && sub_CUser_get_state(user) > 2)
            sub_CUser_Send(user, packet);
    }
}

// ---------------------------------------------------------------------------
// set_area：由 TownAreaScript/STMapScript 初始化区域（NPC/传送/怪物注册）
// ---------------------------------------------------------------------------
void Area::set_area(int vill, TownAreaScript& script, STMapScript& mapScript)
{
    m_npcIdList.clear();
    m_villageIndex = vill;
    m_areaIndex = script.m_field0;
    m_areaType = script.m_field8;
    m_field6c = script.m_field24;

    if (m_areaType == 2)
        sub_createLotteryInfo(&m_lottery, &script.m_field18);

    if (m_areaType == 4)
        m_worldMapIndex = script.m_field18[0];
    else
        m_worldMapIndex = -1;

    if (!mapScript.m_assignNpcs.empty())
    {
        for (std::vector<AssignNpc>::iterator it = mapScript.m_assignNpcs.begin();
             it != mapScript.m_assignNpcs.end(); ++it)
        {
            AssignNpc& npc = *it;
            std::pair<int, int> pos(npc.m_x, npc.m_y);
            m_npcPositions.push_back(pos);
            m_npcIdList.push_back(npc.m_npcId);

            if (m_bExistItemShopNPC != 1)
            {
                void* dataMgr = sub_G_CDataManager();
                void* npcList = *(void**)((char*)dataMgr + 0xa8d0);
                void* npcScript = sub_CNPCScriptList_find(
                    npcList, (unsigned int)npc.m_npcId);
                if (npcScript != 0)
                {
                    m_bExistItemShopNPC = (unsigned char)
                        sub_CNPCScript_isExistRole(npcScript, ENUM_NPC_ROLE_NONE);
                }
            }

            if (npc.m_npcId == 0x7f)
            {
                sub_CTimeGate_SetTimeGate(GlobalData_s_timeGate_, vill,
                                          m_areaIndex, npc.m_npcId,
                                          0x118a, 0x11bc);
                village_object::Zone zone((unsigned char)vill,
                                          (unsigned char)m_areaIndex);
                sub_village_object_register_object(GlobalData_s_villageObjectMgr,
                                                   zone, GlobalData_s_timeGate_);
            }
        }
    }

    m_mapAreas = mapScript.m_mapAreas;
    for (unsigned int i = 0; i < mapScript.m_townPaths.size(); ++i)
    {
        TownPath& tp = mapScript.m_townPaths[i];
        m_moveAbleArea.push_back(std::make_pair(tp.m_field1c, tp.m_field20));
    }

    stAttackedMapInfo_t* attackedInfo = 0;
    std::vector<stAttackedMapInfo_t>* infoList =
        (std::vector<stAttackedMapInfo_t>*)((char*)sub_G_CDataManager() + 0x61a0);
    for (std::vector<stAttackedMapInfo_t>::iterator it = infoList->begin();
         it != infoList->end(); ++it)
    {
        if (it->m_vill == m_villageIndex && it->m_area == m_areaIndex)
            attackedInfo = &*it;
    }

    if (!mapScript.m_attackedMonsters.empty() && attackedInfo != 0)
    {
        sub_village_monster_InsertVillageMonster(
            GlobalData_s_villageMonsterMgr, m_villageIndex, m_areaIndex,
            attackedInfo->m_field8, attackedInfo->m_field0c,
            &mapScript.m_attackedMonsters, &m_mapAreas);
    }
}

// ---------------------------------------------------------------------------
// take_fish：按权重随机取鱼（类型 2；权重表 m_lottery.m_items，兜底 defaultIdx）
// ---------------------------------------------------------------------------
int Area::take_fish()
{
    if (m_areaType != 2)
        return -1;

    int result = 0;
    int randVal = get_rand_int(100000);
    int sum = 0;
    std::pair<int, int> cur;

    for (std::vector<std::pair<int, int> >::iterator it = m_lottery.m_items.begin();
         it != m_lottery.m_items.end(); ++it)
    {
        cur = *it;
        sum += cur.second;
        if (sum > randVal)
        {
            result = cur.first;
            break;
        }
    }

    if (result == 0)
        result = m_lottery.m_defaultIdx;
    return result;
}

// ---------------------------------------------------------------------------
// 内联 getter（ORIG 弱符号 086d238e / 086d2398 / 082343aa）
// ---------------------------------------------------------------------------
int Area::get_area_index()
{
    return m_areaIndex;
}

int Area::get_world_map_index()
{
    return m_worldMapIndex;
}

bool Area::IsExistItemShopNPC()
{
    return m_bExistItemShopNPC;
}

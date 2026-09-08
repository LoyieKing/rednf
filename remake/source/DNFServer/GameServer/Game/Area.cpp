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
// 跨类调用一律 include 真实头（CUser.h / CAssaultMgr.h / CVillageObjectMgr.h /
// CVillageMonsterMgr.h / CTimeGate.h / CDataManager.h / NPCScript.h /
// GlobalData.h / PacketGuard.h / TownAreaScript.h），不再使用 extern asm 桥。
// 异常处理器（insert_user 的 map::insert bad_alloc 捕获）按语义简化。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <utility>
#include <vector>

#include "CUser.h"
#include "CAssaultMgr.h"
#include "CVillageObjectMgr.h"
#include "CVillageMonsterMgr.h"
#include "CTimeGate.h"
#include "CDataManager.h"
#include "NPCScript.h"
#include "GlobalData.h"
#include "PacketGuard.h"
#include "TownAreaScript.h"

// ---------------------------------------------------------------------------
// 跨类最小声明（无权威头的残留）
// ---------------------------------------------------------------------------
class CBelong;

// createLotteryInfo（ORIG 0x89babc5，自由函数；定义于 CStackableItem.cpp，
// 无权威头——CStackableItem.cpp 同样局部声明）
void createLotteryInfo(LotteryInfo* lottery, std::vector<int>* counts);

// ---------------------------------------------------------------------------
// Area 依赖结构（字段布局按 ORIG 反汇编偏移）
// ---------------------------------------------------------------------------
// MapArea / STAttackedMonster：全局作用域定义见 CVillageMonsterMgr.h
// （InsertVillageMonster 的 mangled 参数需非限定类型名）。
// TownAreaScript：共享定义见 TownAreaScript.h（0x30）。

// LotteryInfo：0x20 字节（Area 内嵌于 +0x24；ORIG C1 083706d4）
// LotteryInfo（0x20）：权威定义见 CStackableItem.h（经 CUser.h 链引入；
// Area 内嵌于 +0x24；ORIG C1 083706d4）。
// Mutex（0x18）：权威定义见 GameTypes.h（Area 内嵌于 +0x0c；内联
// pthread_mutex_init/destroy，ORIG 亦为内联弱符号族）。

// set_area 输入结构
struct AssignNpc
{
    int m_npcId;    // +0x00
    int m_pad4;     // +0x04
    int m_pad8;     // +0x08
    int m_pad0c;    // +0x0c
    int m_x;        // +0x10
    int m_y;        // +0x14
    int m_z;        // +0x18（权威 CMap.h 定义：sizeof 0x1c）
};

struct TownPath
{
    char m_pad0[0x1c];  // +0x00
    int m_field1c;      // +0x1c
    int m_field20;      // +0x20
    std::vector<int> m_conds;   // +0x24（[quest condition] 列表；权威 CMap.h 定义）
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
    unsigned char IsExistItemShopNPC();            // ORIG 082343aa（弱，movzbl 直返）

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
    // [b28d-48] ORIG 086c31f2 切片：user 预清(-0x10) → it 默认构造(-0x20) →
    // begin sret(-0x2c) 赋值 → count(-0xc)=0 → do 形循环（end sret -0x1c、
    // operator++ 两分支各一 sret -0x18/-0x14）⇒ 源为 while(true)+体内 return
    // + 具名 user 预清（P198①/P50 反例复活）。
    // [b28d-48 收口] DIFF 65 → NEAR 4（长度 68=68、助记符序列全同）。组合修法：
    // P50 先声明后赋值迭代器（默认构造槽）+ P283 goto 尾条件（begin 后 jmp 底部）
    // + P28 后置 `it++`×2（ORIG 两处均 ppEi 带独立 sret）+ P198① user 预清。
    // 残余 4 = 四个 sret 临时槽号互换（ORIG it++/-0x18、end/-0x1c、it++/-0x14
    // vs 4.4 -0x1c/-0x14/-0x18）——N93 块级临时槽方向跨版本相反，不可翻。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::const_iterator it;
    it = m_UsersInArea.begin();
    int count = 0;
    // P283：ORIG 0x086c322f `jmp 086c32a8`（begin 后直跳底部条件块），
    // 条件块尾 jne 回循环体、假路 mov count 直落——4.4 对 while(true) 恒
    // 条件置顶，故用 goto 尾标签复现。
    goto check;
loop_body:
    user = it->second;
    if (user == 0 || user->get_state() <= 2)
    {
        it++;                                        // ORIG 0x086c3268 ppEi sret -0x18
        goto check;
    }
    ++count;
    it++;                                            // ORIG 0x086c3299 ppEi sret -0x14
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
    return count;
}

// ---------------------------------------------------------------------------
// IsMovalbeArea：m_moveAbleArea（TownPath 起点对集合）是否包含 (vill, area)
// ---------------------------------------------------------------------------
bool Area::IsMovalbeArea(int vill, int area) const
{
    std::vector<std::pair<int, int> >::const_iterator it = m_moveAbleArea.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = m_moveAbleArea.end();
    for (; it != itEnd; ++it)
    {
        const std::pair<int, int>& p = *it;   // ORIG 解引用一次（绑定引用）
        if (p.first == vill && p.second == area)
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

    m_UsersInArea.erase(user->get_unique_id());

    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short(user->get_unique_id());
    packet.put_byte(user->getCurCharacVill());
    packet.put_byte(user->get_area(true));
    packet.put_short(user->get_posX());
    packet.put_short(user->get_posY());
    packet.put_byte(user->get_direction());
    packet.put_byte(user->get_charac_visible_values());
    packet.finalize(true);

    if (user->IsCurCharacVisible())
        send_to_all(packet);
    else
        user->Send(packet);
}

// ---------------------------------------------------------------------------
// erase_user：非村庄区域仅从地图移除用户（不发包）
// ---------------------------------------------------------------------------
void Area::erase_user(CUser* user)
{
    // [b28c-21] ORIG 尾部 jmp+nop 中继岛（P223）：goto 尾空标签复现。
    if (m_areaType == 1)
        goto END;
    m_UsersInArea.erase(user->get_unique_id());
END:;
}

// ---------------------------------------------------------------------------
// get_user_id_list：输出 state > 2 的在线用户 unique_id
// ---------------------------------------------------------------------------
void Area::get_user_id_list(std::vector<unsigned short>& out)
{
    // [b28d-48] 同 GetUserCount 族：ORIG 086c305e 的 user 预清(-0xc) + P50 先声明
    // 后赋值 + P283 goto 尾条件 + P28 后置 it++（ppEi sret -0x18）+ id 具名槽(-0x12)。
    CUser* user = 0;
    unsigned short id;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    goto check;
loop_body:
    user = it->second;
    if (user == 0 || user->get_state() <= 2)
    {
        it++;
        goto check;
    }
    id = user->get_unique_id();
    out.push_back(id);
    it++;
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
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
        user->set_state(ch_state_3);
        packet.put_header(0, 0x18);
        packet.put_byte(m_villageIndex);
        packet.put_byte(user->get_area(true));
        packet.put_short(1);
        packet.put_short(user->get_unique_id());
        packet.put_short(user->get_posX());
        packet.put_short(user->get_posY());
        packet.put_byte(user->get_direction());
        packet.put_byte(user->get_charac_visible_values());
        packet.finalize(true);
        user->Send(packet);
        return;
    }

    if (m_areaType == 2)
        user->set_state(ch_state_4);
    else
        user->set_state(ch_state_3);

    unsigned short id = user->get_unique_id();
    if (m_UsersInArea.find(id) != m_UsersInArea.end())
    {
        LogManager::logFormat(1, "world.cpp",
                                 __PRETTY_FUNCTION__, 0x1a7,
                                 "m_UsersInArea.find(%d)", (int)id);
        return;
    }

    m_UsersInArea.insert(std::pair<unsigned short, CUser*>(id, user));

    packet.put_header(0, 0x17);
    packet.put_short(id);
    packet.put_byte(m_villageIndex);
    packet.put_byte(user->get_area(true));
    packet.put_short(user->get_posX());
    packet.put_short(user->get_posY());
    packet.put_byte(user->get_direction());
    packet.put_byte(user->get_charac_visible_values());
    packet.finalize(true);

    if (user->IsCurCharacVisible())
        send_to_all(packet);
    else
        user->Send(packet);

    send_area_users(user);
    pvp_assault::GetInstanceAssaultMgr()->OnMoveArea(user);
    GlobalData::s_villageObjectMgr->on_move_area(user);
    GlobalData::s_villageMonsterMgr->OnMoveArea(user);
}

// ---------------------------------------------------------------------------
// make_power_war_hp_of_vill：收集在线用户 PowerWar HP 到包并计数
// ---------------------------------------------------------------------------
void Area::make_power_war_hp_of_vill(PacketGuard& packet, int& count)
{
    // [b28d-48] 同 GetUserCount 族（ORIG 086c2c1e）：user 预清 + P50 先声明后赋值
    // + P283 goto 尾条件 + P28 后置 it++。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    goto check;
loop_body:
    user = it->second;
    if (user == 0 || user->get_state() <= 2)
    {
        it++;
        goto check;
    }
    packet.put_short(user->getPowerWarHP());
    ++count;
    it++;
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
}

// ---------------------------------------------------------------------------
// send_area_users：向新进图用户发送同区可见用户列表（0x18，人数回填）
// ---------------------------------------------------------------------------
void Area::send_area_users(CUser* user)
{
    if (user->getCurCharacVill() != m_villageIndex ||
        user->get_area(false) != m_areaIndex)
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x18);
    packet.put_byte(m_villageIndex);
    packet.put_byte(user->get_area(true));

    int index = packet.get_index();
    int count;
    count = 0;    // P197③ 双段式（ORIG movl $0x0,-0xc 预清独立行）
    packet.put_short((int)m_UsersInArea.size());

    // [b28c-21] ORIG begin 调用之后才发 `movl $0x0,-0xc(%ebp)`（u 空预置）
    // ⇒ 迭代器初始化拆到循环外、u 声明置于其后（P0：u 有槽往返＝具名局部）。
    std::map<unsigned short, CUser*>::iterator it = m_UsersInArea.begin();
    CUser* u = 0;
    for (; it != m_UsersInArea.end(); it++)
    {
        u = it->second;
        // [b28c-21] ORIG 086c2411 段：可见性谓词两次独立调用（P48：O0 无 CSE），
        // 第二臂为 `!vis2 && u == user` 短路形（xor$1;test;je 后 cmp）。
        if (u->IsCurCharacVisible() ||
            (!u->IsCurCharacVisible() && u == user))
        {
            packet.put_short(u->get_unique_id());
            packet.put_short(u->get_posX());
            packet.put_short(u->get_posY());
            packet.put_byte(u->get_direction());
            packet.put_byte(u->get_charac_visible_values());
            ++count;
        }
    }

    packet.put_short(index, count);
    packet.finalize(true);
    user->Send(packet);
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
    // [b28d-48] 同 GetUserCount 族（ORIG 086c2e1e）。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    goto check;
loop_body:
    user = it->second;
    if (user == 0 || user->get_state() <= 2)
    {
        it++;
        goto check;
    }
    if (belong->IsBelong(user))
        user->Send(packet);
    it++;
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
}

// ---------------------------------------------------------------------------
// send_to_all：向 state > 2 的在线用户广播包
// ---------------------------------------------------------------------------
void Area::send_to_all(PacketGuard& packet)
{
    // [b28d-48] 同 GetUserCount 族（ORIG 086c2d2a）：user 预清 + P50 先声明后赋值
    // + P283 goto 尾条件 + P28 后置 it++。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    goto check;
loop_body:
    user = it->second;
    if (user == 0 || user->get_state() <= 2)
    {
        it++;
        goto check;
    }
    user->Send(packet);
    it++;
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
}

void Area::send_to_all(PacketGuard& packet, std::vector<CUser*>& users)
{
    // [b28d-48] 同 GetUserCount 族（ORIG 086c2f2c）。
    CUser* user = 0;
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UsersInArea.begin();
    goto check;
loop_body:
    user = it->second;
    // ORIG 086c2f2c：内层对 users 逐一比较，结果未使用（死代码），保留遍历。
    for (std::vector<CUser*>::size_type i = 0; i < users.size(); ++i)
    {
        if (users[i] == user)
        {
        }
    }
    if (user == 0 || user->get_state() <= 2)
    {
        it++;
        goto check;
    }
    user->Send(packet);
    it++;
check:
    if (it != m_UsersInArea.end())
        goto loop_body;
}

// ---------------------------------------------------------------------------
// set_area：由 TownAreaScript/STMapScript 初始化区域（NPC/传送/怪物注册）
// ---------------------------------------------------------------------------
void Area::set_area(int vill, TownAreaScript& script, STMapScript& mapScript)
{
    m_npcIdList.clear();
    m_villageIndex = vill;
    m_areaIndex = script.m_areaIndex;
    m_areaType = script.m_areaType;
    m_field6c = script.m_field24;

    if (m_areaType == 2)
        createLotteryInfo(&m_lottery, &script.m_field18);

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
                CDataManager* dataMgr = G_CDataManager();
                CNPCScriptList* npcList = dataMgr->m_npcScriptList;
                CNPCScript* npcScript =
                    (CNPCScript*)npcList->find((unsigned int)npc.m_npcId);
                if (npcScript != 0)
                {
                    m_bExistItemShopNPC = (unsigned char)
                        npcScript->isExistRole(ENUM_NPC_ROLE_0);
                }
            }

            if (npc.m_npcId == 0x7f)
            {
                GlobalData::s_timeGate_->SetTimeGate(vill,
                                          m_areaIndex, npc.m_npcId,
                                          0x118a, 0x11bc);
                village_object::Zone zone((unsigned char)vill,
                                          (unsigned char)m_areaIndex);
                GlobalData::s_villageObjectMgr->register_object(
                    zone, (village_object::IObject*)GlobalData::s_timeGate_);
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
        &G_CDataManager()->m_villageAttacked.m_mapList;
    for (std::vector<stAttackedMapInfo_t>::iterator it = infoList->begin();
         it != infoList->end(); ++it)
    {
        if (it->m_vill == m_villageIndex && it->m_area == m_areaIndex)
            attackedInfo = &*it;
    }

    if (!mapScript.m_attackedMonsters.empty() && attackedInfo != 0)
    {
        GlobalData::s_villageMonsterMgr->InsertVillageMonster(
            m_villageIndex, m_areaIndex,
            attackedInfo->m_field8, attackedInfo->m_fieldC,
            &mapScript.m_attackedMonsters, &m_mapAreas);
    }
}

// ---------------------------------------------------------------------------
// take_fish：按权重随机取鱼（类型 2；权重表 m_lottery.m_items，兜底 defaultIdx）
// ---------------------------------------------------------------------------
int Area::take_fish()
{
    // [b28d-48] P59 正向包裹：ORIG 0x086c2xxx 为 `jne <尾部 return -1>`、
    // 主流程直落（areaType==2 时走主流程），原 guard 写法 4.4 产 `je <主流程>`
    // + fail 块前置（反向布局）。
    if (m_areaType == 2)
    {
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
    return -1;
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

unsigned char Area::IsExistItemShopNPC()
{
    return m_bExistItemShopNPC;
}

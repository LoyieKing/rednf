// df_game_r 还原 —— village_attacked::CVillageMonsterMgr
// 独立 TU。OnKillVillageMonster(int,int,unsigned short,bool)/OnChangeState/
// SendVillageMonsterFightResult/MakeVillageMonsterListPacket/SendVillageMonsterList
// /GetRewardType 由 VillageMonster_helpers.cpp 提供（本 TU 不重复定义）。
// 逐函数对照 ORIG df_game_r 反汇编（证据见各函数注释）。

#include "CVillageMonsterMgr.h"

#include <cstdlib>

#include "CGameManager.h"   // CUser / CParty / pvp_assault::Point2d 完整类型
#include "PacketGuard.h"
#include "GameWorld.h"      // G_GameWorld / send_all / send_to_area
#include "TimerQueue.h"     // G_TimerQueue / InsertTimer
#include "GameRand.h"       // get_rand_int

// ============================================================================
// CParty 村战状态区局部视图（ORIG：CBattle_Field(+0xb24) 嵌入子区 BF+0x128）。
// CParty.h 尚未具名该子区；本 TU 以局部视图按 ORIG 偏移具名访问（不跨 TU）。
// 字段对应：OnFightVillageMonster 0x86b4672 写 / OnKillVillageMonster 0x86b4866 读。
// ============================================================================
namespace
{

struct CPartyVillageStateView
{
    unsigned char m_pad0[0xc4c];
    unsigned short m_villageMonsterZone;   // +0xc4c（BF+0x128；0 = 未在村战）
    int m_villageVill;                     // +0xc50（战斗对象 vill）
    int m_villageArea;                     // +0xc54（战斗对象 area）
    int m_villageMonsterField24;           // +0xc58（怪物 m_field24）
    unsigned char m_pad1[0xcb8 - 0xc5c];
    int m_villageMonsterField20;           // +0xcb8（怪物 m_field20）
};

}  // anonymous namespace

// ============================================================================
// MapArea::include(x,y,outX,outY,margin) —— ORIG T 0x82f02e6
//   把 (x,y) 按 margin 裁剪到本矩形内（结果写 outX/outY）；
//   未发生移动（裁剪后仍等于原坐标）返回 1，否则返回 0。
// ============================================================================
int MapArea::include(int x, int y, int& outX, int& outY, int margin)
{
    if (m_x + margin > x)
        outX = m_x + margin;
    else if (m_x + m_w - margin < x)
        outX = m_x + m_w - margin;
    else
        outX = x;
    if (m_y + margin > y)
        outY = m_y + margin;
    else if (m_y + m_h - margin < y)
        outY = m_y + m_h - margin;
    else
        outY = y;
    if (outX == x && outY == y)
        return 1;
    return 0;
}

// GetDistance 前置声明（定义见下）。
int GetDistance(pvp_assault::Point2d a, pvp_assault::Point2d b);

// ============================================================================
// pvp_assault::IsIncludeVirtualMoveAreas —— ORIG T 0x82e5a0b（村战 OnRun 依赖）。
//   遍历虚拟移动区：点落在某区 margin 内 → 返回 1（outX/outY 维持原坐标）；
//   否则取各区域近边缘中距原点的最小值（margin!=0 写绝对坐标，margin==0 写
//   增量），全程未命中返回 0。
// ============================================================================
namespace pvp_assault
{

bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y,
                              int& outX, int& outY, int margin)
{
    int minDist = 0x7fffffff;
    outX = x;
    outY = y;
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        // ORIG 0x82e5a0b 用 at()（越界抛异常路径与 ORIG 一致）。
        MapArea& a = areas->at(i);
        int tx = 0;
        int ty = 0;
        if (a.include(x, y, tx, ty, margin))
            return 1;
        int dist = GetDistance(Point2d(tx, ty), Point2d(x, y));
        if (dist < minDist)
        {
            if (margin != 0)
            {
                outX = tx;
                outY = ty;
            }
            else
            {
                outX = tx - x;
                outY = ty - y;
            }
            minDist = dist;
        }
    }
    return 0;
}

}  // namespace pvp_assault

// ============================================================================
// GetDistance(Point2d, Point2d) —— ORIG T 0x82e59f1（= Point2d::operator-
// 的曼哈顿距离）。ORIG 经 operator- 计算；本实现等价（同名符号）。
// ============================================================================
int GetDistance(pvp_assault::Point2d a, pvp_assault::Point2d b)
{
    // ORIG 0x82e59f1 尾调 Point2d::operator-（= |dx|+|dy| 曼哈顿距离）。
    // 语义等价（ORIG 传参形态差异属编译器形式；operator- 实现一致）。
    return a - b;   // ORIG 尾调 Point2d::operator-（曼哈顿距离 10 insns，我们内联 29）
}

// ---- ORIG 域常量（nm：_ZN16village_attackedL…E，r 段；值自 ORIG rodata 读取）----
namespace village_attacked
{
// gcc 4.4：namespace 内 static const 未使用也发射（内部链接，名含 L）。
static const int MONSTER_REGEN = 1;                // 0x08bc43c0
static const int MONSTER_MOVE_PIXEL = 300;         // 0x08bc43b0 (0x12c)
static const int MONSTER_UPDATE_RATE = 1;          // 0x08bc43b8
static const int MAX_START_TIME_COUNT = 3;         // 0x08bc4394
static const int start_time_count_next[3] = {5, 4, 1};     // 0x08bc43a4 (0xc 字节)
static const int MIN_MONSTER_MOVE_PIXEL = 50;      // 0x08bc43b4 (0x32)
static const int start_time_count_remain[3] = {10, 5, 1};  // 0x08bc4398 (0xc 字节)
static const int MONSTER_MOVE_RANDOM_CONST = 3;    // 0x08bc43bc

// ============================================================================
// CVillageMonster area 构造 —— ORIG 0x86b3752
// ============================================================================

// ============================================================================
// CVillageMonster area 构造 —— ORIG 0x86b3752
// ============================================================================
CVillageMonsterArea::CVillageMonsterArea(int a, int b, int c, int d,
                                         std::vector<STAttackedMonster>* monsters,
                                         std::vector<MapArea>* areas)
{
    // ORIG 0x86b3752：map/vector 默认构造后按偏移赋值。
    m_field28 = a;                       // +0x28
    m_field2c = b;                       // +0x2c
    m_monsterList = *monsters;           // +0x1c vector 拷贝
    m_areaList = areas;                  // +0x18
    m_field34 = c;                       // +0x34
    m_field38 = d;                       // +0x38
    m_field30 = 0;                       // +0x30
}

void CVillageMonsterArea::OnMoveArea(CUser* user)
{
    if (!m_monsters.empty())
        SendVillageMonsterList(user);    // ORIG 0x86b3abe（列表发送）
}

void CVillageMonsterMgr::OnMoveArea(CUser* user)
{
    // ORIG 0x86b45bc：Zone(curVill, curArea) 查找，命中则转交区域处理。
    unsigned char area = (unsigned char)user->get_area(false);
    unsigned char vill = user->getCurCharacVill();
    Zone zone(vill, area);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it != m_areas.end())
        it->second.OnMoveArea(user);
}

bool CVillageMonsterMgr::OnEvent()
{
    // ORIG 0x82f0f44：返回 bool(m_field04 != 0)
    return m_field04 != 0;
}

void CVillageMonsterMgr::InsertVillageMonster(int a, int b, int c, int d,
                                              std::vector<STAttackedMonster>* monsters,
                                              std::vector<MapArea>* areas)
{
    // ORIG 0x86b4144：构造区域后以 Zone(a,b) 为键插入 m_areas。
    CVillageMonsterArea area(a, b, c, d, monsters, areas);
    Zone zone((unsigned char)a, (unsigned char)b);
    m_areas.insert(std::make_pair(zone, area));
}

// ============================================================================
// CVillageMonster 构造/析构/GetUID —— ORIG 0x86b3004 / 0x86b3098 / 0x86b3106 /
// 0x86b4b64。7 参 ctor：m_area=area；m_id=uid_seed 自增分配（清零则回 1）；
// m_zone=id；m_field04=x；m_field08=y；m_field10=0；m_field0c=slot；
// m_partyIndex=-1；m_state=0；m_field20/m_field24 直接落位。
// ============================================================================
CVillageMonster::CVillageMonster(unsigned short id, int x, int y, int slot,
                                 CVillageMonsterArea* area, int field20, int field24)
{
    static int uid_seed = 0;   // ORIG d 0x093ace54（函数内静态）
    m_area = area;
    m_id = (unsigned short)uid_seed++;
    if (uid_seed == 0)
        uid_seed = 1;
    m_zone = id;
    m_field04 = x;
    m_field08 = y;
    m_field10 = 0;
    m_field0c = slot;
    m_partyIndex = -1;
    m_state = 0;
    m_field20 = field20;
    m_field24 = field24;
}

CVillageMonster::CVillageMonster()
{
    m_area = 0;
    m_id = 0;
    m_zone = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field10 = 0;
    m_field0c = 0;
    m_partyIndex = -1;
    m_state = 0;
    m_field20 = 0;
    m_field24 = 0;
}

CVillageMonster::~CVillageMonster()
{
}

unsigned short CVillageMonster::GetUID() const
{
    // ORIG W 0x86b4b64：return m_id
    return m_id;
}

// ============================================================================
// CVillageMonster::OnRunVillageMonster —— ORIG T 0x86b310c
//   m_field10 清零；m_state!=0 或 idx%3!=m_field0c → 不动；否则随机 ±150 像素
//   试探新位置，用 IsIncludeVirtualMoveAreas 裁剪到虚拟移动区内；若距旧位置
//   的曼哈顿距离 > 0x31 保留新位置并置 m_field10=1，否则回退旧位置。
// ============================================================================
void CVillageMonster::OnRunVillageMonster(std::vector<MapArea>* areas, int idx)
{
    m_field10 = 0;
    if (m_state != 0)
        return;
    if (idx % 3 != m_field0c)
        return;
    int oldX = m_field04;
    int oldY = m_field08;
    int newX = m_field04 + get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    int newY = m_field08 + get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    pvp_assault::IsIncludeVirtualMoveAreas(areas, newX, newY,
                                           m_field04, m_field08, 0x14);
    if (abs(m_field04 - oldX) + abs(m_field08 - oldY) > 0x31)
        m_field10 = 1;
    else
    {
        m_field04 = oldX;
        m_field08 = oldY;
    }
}

// ============================================================================
// CVillageMonster::OnFightVillageMonster —— ORIG T 0x86b3240
//   m_state!=0 → 0x13；距怪物 > 0x12c（x 或 y）→ 0x63；否则状态置 1、
//   记录 partyIndex，回填 m_field20/m_field24 到 out1/out2，返回 0。
// ============================================================================
int CVillageMonster::OnFightVillageMonster(CUser* user, int& out1, int& out2)
{
    if (m_state != 0)
        return 0x13;
    if (abs((int)user->get_posX() - m_field04) > MONSTER_MOVE_PIXEL)
        return 0x63;
    if (abs((int)user->get_posY() - m_field08) > MONSTER_MOVE_PIXEL)
        return 0x63;
    OnChangeState(ENUM_VILLAGE_MONSTER_STATE_1);
    m_partyIndex = (int)user->GetPartyIndex();
    out1 = m_field20;
    out2 = m_field24;
    return 0;
}

// ============================================================================
// CVillageMonster::MakeVillageMonsterPositionPacket —— ORIG T 0x86b36e6
//   仅 m_field10==1（本轮移动过）时：put_short(m_id/m_field04/m_field08)，
//   清零 m_field10，返回 true；否则返回 false。
// ============================================================================
bool CVillageMonster::MakeVillageMonsterPositionPacket(PacketGuard& packet)
{
    if (m_field10 != 1)
        return false;
    packet.put_short(m_id);
    packet.put_short(m_field04);
    packet.put_short(m_field08);
    m_field10 = 0;
    return true;
}

// ============================================================================
// CVillageMonsterArea::OnRunVillageMonster —— ORIG T 0x86b3f06
//   区域无人 → 直接返回；否则逐怪物转 OnRunVillageMonster(m_areaList, m_field30)，
//   然后 SendVillageMonsterPosition 并 ++m_field30（轮转槽）。
// ============================================================================
void CVillageMonsterArea::OnRunVillageMonster()
{
    if (G_GameWorld()->GetUserCount(m_field28, m_field2c) == 0)
        return;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        it->second.OnRunVillageMonster((std::vector<MapArea>*)m_areaList,
                                       m_field30);
    }
    SendVillageMonsterPosition();
    ++m_field30;
}

// ============================================================================
// CVillageMonsterArea::SendVillageMonsterPosition —— ORIG T 0x86b3962
//   0xef 位置包：put_header(0,0xef)；预留 count 字节；遍历怪物调用
//   MakeVillageMonsterPositionPacket（成功则 count++）；回填 count；
//   finalize(true)；count!=0 时 SendPacket。
// ============================================================================
void CVillageMonsterArea::SendVillageMonsterPosition()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xef);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(0);
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        if (it->second.MakeVillageMonsterPositionPacket(packet))
            ++count;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    if (count != 0)
        SendPacket(packet);
}

// ============================================================================
// CVillageMonsterArea::GetAttackedMonster —— ORIG T 0x86b3aea
//   返回首个 m_attackWeight >= idx 的怪物；全不满足返回最后一个；空表返回 0。
// ============================================================================
STAttackedMonster* CVillageMonsterArea::GetAttackedMonster(int idx)
{
    if (m_monsterList.empty())
        return 0;
    std::vector<STAttackedMonster>::iterator it = m_monsterList.begin();
    for (; it != m_monsterList.end(); ++it)
    {
        if (it->m_attackWeight >= idx)
            return &*it;
    }
    --it;
    return &*it;
}

// ============================================================================
// CVillageMonsterArea::OnCreateVillageMonster —— ORIG T 0x86b3bd4
//   m_field34 <= 当前怪物数 → 返回 0；随机 10000 取表项，构造怪物（slot =
//   size%3，m_field20/m_field24 取表 +0x14/+0x18），insert 后发 0xf6 新怪物包，
//   注册 0x86 生成定时器（param：m_field38/m_field28/m_field2c），返回 1。
// ============================================================================
int CVillageMonsterArea::OnCreateVillageMonster()
{
    if ((unsigned int)m_field34 <= (unsigned int)m_monsters.size())
        return 0;
    STAttackedMonster* attacked = GetAttackedMonster(get_rand_int(0x2710));
    if (attacked == 0)
        return 0;
    CVillageMonster monster((unsigned short)attacked->m_id,
                            attacked->m_field04, attacked->m_field08,
                            (int)(m_monsters.size() % 3), this,
                            attacked->m_field14, attacked->m_field18);
    unsigned short uid = monster.GetUID();
    m_monsters.insert(std::make_pair(uid, monster));
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0xf6);
        packet.put_short(monster.GetUID());
        packet.put_short(attacked->m_id);
        packet.put_byte(0);
        packet.put_short(attacked->m_field04);
        packet.put_short(attacked->m_field08);
        packet.finalize(true);
        SendPacket(packet);
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x86,
                                m_field38, m_field28, m_field2c);
    return 1;
}

// ============================================================================
// CVillageMonsterArea::OnDestroyVillageMonster —— ORIG T 0x86b3ee0
//   清空怪物表并广播空列表包。
// ============================================================================
void CVillageMonsterArea::OnDestroyVillageMonster()
{
    m_monsters.clear();
    SendVillageMonsterList(0);
}

// ============================================================================
// CVillageMonsterArea::OnFightVillageMonster —— ORIG T 0x86b3fe2
//   zone 未命中 → 0x15；否则转交怪物（out1/out2 引用透传）。
// ============================================================================
int CVillageMonsterArea::OnFightVillageMonster(CUser* user, unsigned short zone,
                                               int& out1, int& out2)
{
    std::map<unsigned short, CVillageMonster>::iterator it =
        m_monsters.find(zone);
    if (it == m_monsters.end())
        return 0x15;
    return it->second.OnFightVillageMonster(user, out1, out2);
}

// ============================================================================
// CVillageMonsterMgr 构造/析构 —— ORIG 0x86b4a4c / 0x86b4a80
// ============================================================================
CVillageMonsterMgr::CVillageMonsterMgr()
{
    m_field04 = 0;
    m_field00 = 0;
    m_field20 = 0;
}

CVillageMonsterMgr::~CVillageMonsterMgr()
{
}

// ============================================================================
// CVillageMonsterMgr::OnStartVillageAttacked —— ORIG 0x86b4a20
//   未开战（m_field04 != 1）时创建怪物并置 m_field04=1。
// ============================================================================
void CVillageMonsterMgr::OnStartVillageAttacked()
{
    if (m_field04 != 1)
    {
        OnCreateVillageMonster();
        m_field04 = 1;
    }
}

// ============================================================================
// CVillageMonsterMgr::OnCreateVillageMonster —— ORIG 0x86b4280
//   各区域生成怪物；m_field04=2；注册 0x85 轮转定时器（1 秒）。
// ============================================================================
void CVillageMonsterMgr::OnCreateVillageMonster()
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnCreateVillageMonster();
    }
    m_field04 = 2;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x85,
                                1, 0, 0);
}

// ============================================================================
// CVillageMonsterMgr::OnDestroyVillageMonster —— ORIG 0x86b43d4
//   各区域清空；m_field04=0、m_field00=0、m_field20=reward；广播 0xf2 结束包。
// ============================================================================
void CVillageMonsterMgr::OnDestroyVillageMonster(ENUM_VILLAGE_ATTACKED_REWARD reward)
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnDestroyVillageMonster();
    }
    m_field04 = 0;
    m_field00 = 0;
    m_field20 = (int)reward;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf2);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
}

// ============================================================================
// CVillageMonsterMgr::OnEndRewardVillageMonster —— ORIG 0x86b44ec
//   m_field20 = 0。
// ============================================================================
void CVillageMonsterMgr::OnEndRewardVillageMonster()
{
    m_field20 = 0;
}

// ============================================================================
// CVillageMonsterMgr::OnRunVillageMonster —— ORIG 0x86b44fc
//   未开战（m_field04==0）返回；各区域轮转；注册下一个 0x85 轮转定时器。
// ============================================================================
void CVillageMonsterMgr::OnRunVillageMonster()
{
    if (m_field04 == 0)
        return;
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnRunVillageMonster();
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x85,
                                1, 0, 0);
}

// ============================================================================
// CVillageMonsterMgr::OnRegenVillageMonster —— ORIG 0x86b4338
//   未开战返回；Zone(a,b) 命中区域则补生成一只。
// ============================================================================
void CVillageMonsterMgr::OnRegenVillageMonster(int a, int b)
{
    if (m_field04 == 0)
        return;
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    if (it == m_areas.end())
        return;
    it->second.OnCreateVillageMonster();
}

// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(CUser*,bool) —— ORIG 0x86b4866
//   从队伍村战状态区取 zone（+0xc4c；0 → 0x15）与 vill/area（+0xc50/+0xc54），
//   Zone 命中区域转交 Area::OnKillVillageMonster(user, zone, flag)。
// ============================================================================
int CVillageMonsterMgr::OnKillVillageMonster(CUser* user, bool flag)
{
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    CPartyVillageStateView* view =
        reinterpret_cast<CPartyVillageStateView*>(party);
    unsigned short zone = view->m_villageMonsterZone;
    if (zone == 0)
        return 0x15;
    Zone z((unsigned char)view->m_villageVill,
           (unsigned char)view->m_villageArea);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(z);
    if (it == m_areas.end())
        return 0x15;
    return it->second.OnKillVillageMonster(user, zone, flag);
}

// ============================================================================
// CVillageMonsterMgr::OnFightVillageMonster(CUser*,unsigned short) —— ORIG 0x86b4672
//   szZone 未命中或队伍缺失 → 0x15；区域 OnFight 失败原样返回；成功则把
//   怪物 m_field20/m_field24 写入队伍村战状态区（+0xcb8/+0xc58）、zon/ vill/area
//   落位（+0xc4c/+0xc50/+0xc54），并向队伍广播 0xf3，返回 0。
// ============================================================================
int CVillageMonsterMgr::OnFightVillageMonster(CUser* user, unsigned short zone)
{
    unsigned short z = zone;
    unsigned char area = (unsigned char)user->get_area(false);
    unsigned char vill = user->getCurCharacVill();
    Zone findZone(vill, area);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(findZone);
    if (it == m_areas.end())
        return 0x15;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    int out1 = 0;   // → monster.m_field20 → party+0xcb8
    int out2 = 0;   // → monster.m_field24 → party+0xc58
    int ret = areaObj->OnFightVillageMonster(user, z, out1, out2);
    if (ret != 0)
        return ret;
    CPartyVillageStateView* view =
        reinterpret_cast<CPartyVillageStateView*>(party);
    view->m_villageMonsterZone = z;
    view->m_villageVill = (int)(signed char)vill;
    view->m_villageArea = user->get_area(false);
    view->m_villageMonsterField24 = out2;
    view->m_villageMonsterField20 = out1;
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf3);
    packet.finalize(true);
    party->send_to_party(packet);
    return 0;
}

}  // namespace village_attacked

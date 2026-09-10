// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CVillageMonsterMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN16village_attacked15CVillageMonsterC2EtiiiPNS_19CVillageMonsterAreaEii`（P327）。
// 全限定定义可写在命名空间块之外 ⇒ 发射序提前。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG 0x86b3004：7 参 ctor，m_id=uid_seed 自增分配（清零则回 1）。
#include "CVillageMonsterMgr/CVillageMonster.inc"
    unsigned short id, int x, int y, int slot,
    village_attacked::CVillageMonsterArea* area, int field20, int field24)
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
    m_state = ENUM_VILLAGE_MONSTER_STATE_0;   // b28d：m_state 已 enum 化（CVillageMonsterMgr.h）
    m_field20 = field20;
    m_field24 = field24;
}

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
#include "CVillageMonsterMgr/include.inc"
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
#include "CVillageMonsterMgr/GetDistance.inc"

// ============================================================================
// pvp_assault::IsIncludeVirtualMoveAreas —— ORIG T 0x82e5a0b（村战 OnRun 依赖）。
//   遍历虚拟移动区：点落在某区 margin 内 → 返回 1（outX/outY 维持原坐标）；
//   否则取各区域近边缘中距原点的最小值（margin!=0 写绝对坐标，margin==0 写
//   增量），全程未命中返回 0。
// ============================================================================
namespace pvp_assault
{

// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x082e5a0b）：去 MapArea& 引用缓存
// （P157/P183 逐点 areas->at(i)）；tx/ty/dist 提函数头（P212 全头部声明）+
// GetDistance 实参序换为 (Point2d(x,y), Point2d(tx,ty)) + 比较写 minDist>dist
// 复现 4.1 折痕；MapArea::include 返回 int→bool（P292 调用点 test %al）。
// p167 CLEAR。
#include "CVillageMonsterMgr/IsIncludeVirtualMoveAreas.inc"
                              int& outX, int& outY, int margin)
{
    int minDist = 0x7fffffff;
    int dist = 0;
    int tx = 0;
    int ty = 0;
    outX = x;
    outY = y;
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        // ORIG 0x82e5a0b 用 at()（越界抛异常路径与 ORIG 一致），不缓存引用。
        if (areas->at(i).include(x, y, tx, ty, margin))
            return 1;
        dist = GetDistance(Point2d(x, y), Point2d(tx, ty));
        if (minDist > dist)
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
#include "CVillageMonsterMgr/GetDistance_1.inc"
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
#include "CVillageMonsterMgr/CVillageMonsterArea.inc"
                                         std::vector<STAttackedMonster>* monsters,
                                         std::vector<MapArea>* areas)
{
    // ORIG 0x86b3752：map/vector 默认构造后按偏移赋值。
    m_field28 = a;                       // +0x28
    m_field2c = b;                       // +0x2c
    m_monsterList = *monsters;           // +0x1c vector 拷贝
    m_areaList = areas;                  // +0x18
    // N76 封顶（ndiff1）：ORIG 0x86b3752 此句先备值(c→edx)后载 this；4.4 对 int 参数
    // mem→mem store 恒 LHS-first，探针直成员/视图/链式/中间量全不可翻（enum 参数才值先）。
    m_field34 = c;                       // +0x34
    m_field38 = d;                       // +0x38
    m_field30 = ENUM_VILLAGE_MONSTER_ROTATE_0;   // +0x30
}

#include "CVillageMonsterMgr/OnMoveArea.inc"
{
    if (m_monsters.empty())              // ORIG 0x86b3aXX test al;jne 尾（空跳，Send fall）
        goto T_END;
    SendVillageMonsterList(user);        // ORIG 0x86b3abe（列表发送）
T_END:
    ;
}

#include "CVillageMonsterMgr/OnMoveArea_1.inc"
{
    // ORIG 0x86b45bc：Zone(curVill, curArea) 查找，命中则转交区域处理。
    // P36/P52：ORIG 0x86b45bc get_area 值经 %ebx 跨 getCurCharacVill 调用保活
    // （无栈槽 ⇒ 无具名局部），vill 为内联右值直进 eax。
    // [b28-L11 清违例] 原 register asm("ebx") 绑定违反 P0/N96 全域禁令；改
    // P52 单语句内联实参形（右起求值，get_area 结果由编译器升格 callee-saved）。
    Zone zone(user->getCurCharacVill(), (unsigned char)user->get_area(false));
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    // ORIG 0x86b45bc 实为 eq + jne 早退形，且 pt 后 +0x4 经具名槽 -0xc（P0 指针局部）。
    if (it == m_areas.end())
        return;
    CVillageMonsterArea* areaObj = &it->second;
    areaObj->OnMoveArea(user);
}

#include "CVillageMonsterMgr/OnEvent.inc"
{
    // ORIG 0x82f0f44：返回 bool(m_field04 != 0)
    return m_field04 != 0;
}

#include "CVillageMonsterMgr/InsertVillageMonster.inc"
                                              std::vector<STAttackedMonster>* monsters,
                                              std::vector<MapArea>* areas)
{
    // ORIG 0x86b4144：构造区域后以 Zone(a,b) 为键插入 m_areas。
    // P210（本件实证）：全临时内联形使 4.4 栈槽排布复现 ORIG（area -0x48 / zone
    // -0xa / mkpair -0x88 / pair_cc -0xc8 / insert 返回 -0xd0）；具名局部形 4.4 将
    // 具名对象压帧底（-0xd0/-0x92），逐条错位。
    m_areas.insert(std::make_pair(Zone((unsigned char)a, (unsigned char)b),
                                  CVillageMonsterArea(a, b, c, d, monsters, areas)));
}

// ============================================================================
// CVillageMonster 构造/析构/GetUID —— ORIG 0x86b3004 / 0x86b3098 / 0x86b3106 /
// 0x86b4b64。7 参 ctor：m_area=area；m_id=uid_seed 自增分配（清零则回 1）；
// m_zone=id；m_field04=x；m_field08=y；m_field10=0；m_field0c=slot；
// m_partyIndex=-1；m_state=0；m_field20/m_field24 直接落位。
// ============================================================================
#include "CVillageMonsterMgr/CVillageMonster_1.inc"
{
    m_area = 0;
    m_id = 0;
    m_zone = 0;
    m_field04 = 0;
    m_field08 = 0;
    m_field10 = 0;
    m_field0c = 0;
    m_partyIndex = -1;
    m_state = ENUM_VILLAGE_MONSTER_STATE_0;   // b28d：m_state 已 enum 化（CVillageMonsterMgr.h）
    m_field20 = 0;
    m_field24 = 0;
}

#include "CVillageMonsterMgr/dtor_CVillageMonster.inc"
{
}

#include "CVillageMonsterMgr/GetUID.inc"
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
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b310c）：去 newX/newY 局部，
// 改 `m_field04 = m_field04 + offset;` 两句复用同一 offset 槽（复现 ORIG
// call→sub→存槽→读 LHS 序）；IsInclude 实参直传字段；if/else 反写
// （`<=0x31` 为 then）复现 ORIG `jg` 块序（P47/P222）。p167 CLEAR。
#include "CVillageMonsterMgr/OnRunVillageMonster.inc"
{
    m_field10 = 0;
    if (m_state != 0)
        return;
    if (idx % 3 != m_field0c)
        return;
    int oldX = m_field04;
    int oldY = m_field08;
    int offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field04 = m_field04 + offset;
    offset = get_rand_int(MONSTER_MOVE_PIXEL) - 0x96;
    m_field08 = m_field08 + offset;
    pvp_assault::IsIncludeVirtualMoveAreas(areas, m_field04, m_field08,
                                           m_field04, m_field08, 0x14);
    if (abs(m_field04 - oldX) + abs(m_field08 - oldY) <= 0x31)
    {
        m_field04 = oldX;
        m_field08 = oldY;
    }
    else
        m_field10 = 1;
}

// ============================================================================
// CVillageMonster::OnFightVillageMonster —— ORIG T 0x86b3240
//   m_state!=0 → 0x13；距怪物 > 0x12c（x 或 y）→ 0x63；否则状态置 1、
//   记录 partyIndex，回填 m_field20/m_field24 到 out1/out2，返回 0。
// ============================================================================
#include "CVillageMonsterMgr/OnFightVillageMonster.inc"
{
    if (m_state != 0)
        return 0x13;
    // [b28-L11 清违例] 原 register bool asm("eax") 违反 P0/N96；还原 P133
    // 内联短路判定形（探针 dl17p/p2.cpp g4：内联短路 4.4 复现直分支）。
    if (abs((int)user->get_posX() - m_field04) > MONSTER_MOVE_PIXEL ||
        abs((int)user->get_posY() - m_field08) > MONSTER_MOVE_PIXEL)
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
#include "CVillageMonsterMgr/MakeVillageMonsterPositionPacket.inc"
{
    // [b28-L14] ORIG T 0x86b36e6 = `movzbl 0x10(%eax),%eax; xor $0x1,%eax;
    // test %al,%al; je body` ⇒ 字段为 bool 且源码写 `if (!m_field10) return false;`
    // （P168a 裸形；旧 L11 判词「裸 != 1 反演」实为字段类型 char 所致，m_field10
    // char→bool 后 4.4 直产 xor$1 物化形）。
    if (!m_field10)
        return false;
    // 以下保持原序：三次 put_short 后清零 m_field10。
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
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b3f06）：加具名 monster 指针
// 局部（复现 ORIG -0xc 槽 + ptE 先于实参求值）；m_field30 经
// `(ENUM_VILLAGE_MONSTER_ROTATE)m_field30` 实参 cast 复现两段装载（P265），
// 字段保持 int 以保 `++` 的 lea 形。p167 CLEAR。
#include "CVillageMonsterMgr/OnRunVillageMonster_1.inc"
{
    if (G_GameWorld()->GetUserCount(m_field28, m_field2c) == 0)
        return;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        monster->OnRunVillageMonster((std::vector<MapArea>*)m_areaList,
                                     m_field30);
    }
    SendVillageMonsterPosition();
    m_field30 = (ENUM_VILLAGE_MONSTER_ROTATE)(m_field30 + 1u);
}

// ============================================================================
// CVillageMonsterArea::SendVillageMonsterPosition —— ORIG T 0x86b3962
//   0xef 位置包：put_header(0,0xef)；预留 count 字节；遍历怪物调用
//   MakeVillageMonsterPositionPacket（成功则 count++）；回填 count；
//   finalize(true)；count!=0 时 SendPacket。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] N 封顶（ORIG 0x086b3962，N82/N112 族）：
// float framePad 已对齐帧 0x40 与深槽（begin -0x2c/countIdx -0x28/packet -0x24），
// 残余 = end/count 槽浅 4（-0x14/-0x10 vs ORIG -0x18/-0x14）+ ORIG goto 岛 nop×1；
// 单 double pad 深 8、双 double 帧 0x50 均反劣。p167 CLEAR（17/17）。
#include "CVillageMonsterMgr/SendVillageMonsterPosition.inc"
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xef);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(count);
    int deadPad = 0;   // ORIG -0x10 死槽（P232：写后未读）
    (void)deadPad;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        if (!monster->MakeVillageMonsterPositionPacket(packet))
            goto SKIP_COUNT;
        ++count;
    SKIP_COUNT:
        ;
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
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b3aea）：end() 提升到循环外
// 具名 endIt（P13 缓存 end，复现 ORIG 单次 call end）；`(*it).m_attackWeight`
// 替 `it->m` 复现 deEv（P167 it-> vs *it 判据）。p167 CLEAR。
#include "CVillageMonsterMgr/GetAttackedMonster.inc"
{
    if (m_monsterList.empty())
        return 0;
    std::vector<STAttackedMonster>::iterator it = m_monsterList.begin();
    std::vector<STAttackedMonster>::iterator endIt = m_monsterList.end();
    for (; it != endIt; ++it)
    {
        if ((*it).m_attackWeight >= idx)
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
// [M5E round6 · 旧判词 N82/N95/N104 全部推翻，213/214，仅余 .h 宽度轴]
//  ORIG 0x086b3bd4 三处直证 + 对应修正：
//  ① +0x03c `mov %eax,-0x20(%ebp)` / +0x03f `mov -0x20(%ebp),%eax`：ORIG 有具名
//     randIdx 局部（-0x20），我方旧码把 `get_rand_int(0x2710)` 直插实参缺 2 条；
//     补上后**全部槽位与 ORIG 逐个对齐**（attacked -0x1c、uid 形参 -0x22、monster
//     -0xb8、packet -0x90、pair -0x50/-0x7c/-0x84）——旧判「randIdx 具名反致帧
//     0xcc→0xdc」是当年**同时保留 uid 局部+cast 临时**时的读数，非 randIdx 之罪。
//  ② +0x0e3 `mov %ax,-0x22(%ebp)`（GetUID 结果直接落 make_pair 首参槽）⇒ ORIG 无
//     具名 uid、无 `(unsigned short)` cast 临时；旧码 `unsigned short uid` +
//     `make_pair((unsigned short)uid, monster)` 多 2 条（-0x1a 存 + 载）。改直传
//     `std::make_pair(monster.GetUID(), monster)` 后 p167 仍 CLEAR（make_pairIt
//     实例未失：符号集 delta 为空）。
//  ③ +0x2a6 `mov $0x1,%ebx`（return 1 求值）→ +0x2b4 ~PacketGuard → +0x2fa
//     ~CVillageMonster（正常路径尾块在函数末）⇒ ORIG 的 packet **不在内层块**，
//     其 dtor 晚于 InsertTimer；旧码内层 `{ }` 使正常路径多了 1 条 `jmp` 跳过
//     landing pad。去掉块后 215→214。
//  残余（唯一，.h 待父泳道裁决）：+0x0a3 / +0x1dc 两处 ORIG 为 `mov (%eax),%eax`
//     4 字节装载（ctor 实参处再 `movzwl %ax,%eax` 降位、put_short(int) 处不降位）
//     ⇒ ORIG 的 STAttackedMonster 偏移 0 字段**是 4 字节**；我方 .h 为
//     `unsigned short m_id; char m_pad02[2];` ⇒ 2×`movzwl (%eax),%eax` + 1 条多余
//     `movzwl %ax,%eax`（+1 条）。改 `unsigned int m_id;`（删 pad02，偏移
//     0x04/0x08/0x14/0x18 与 sizeof 0x20 全不变）⇒ 本件 213/213 逐条相同。
#include "CVillageMonsterMgr/OnCreateVillageMonster.inc"
{
    if ((unsigned int)m_field34 <= (unsigned int)m_monsters.size())
        return 0;
    int randIdx = get_rand_int(0x2710);
    STAttackedMonster* attacked = GetAttackedMonster(randIdx);
    if (attacked == 0)
        return 0;
    CVillageMonster monster(attacked->m_id,
                            attacked->m_field04, attacked->m_field08,
                            (int)(m_monsters.size() % 3), this,
                            attacked->m_field14, attacked->m_field18);
    m_monsters.insert(std::make_pair(monster.GetUID(), monster));
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
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x86,
                                m_field38, m_field28, m_field2c);
    return 1;
}

// ============================================================================
// CVillageMonsterArea::OnDestroyVillageMonster —— ORIG T 0x86b3ee0
//   清空怪物表并广播空列表包。
// ============================================================================
#include "CVillageMonsterMgr/OnDestroyVillageMonster.inc"
{
    m_monsters.clear();
    SendVillageMonsterList(0);
}

// ============================================================================
// CVillageMonsterArea::OnFightVillageMonster —— ORIG T 0x86b3fe2
//   zone 未命中 → 0x15；否则转交怪物（out1/out2 引用透传）。
// ============================================================================
#include "CVillageMonsterMgr/OnFightVillageMonster_1.inc"
                                               int& out1, int& out2)
{
    std::map<unsigned short, CVillageMonster>::iterator it =
        m_monsters.find(zone);
    // P0：ORIG 0x86b3fe2 有 -0xc 槽 ⇒ 原码有具名 monster 指针局部；
    // ne 正形 + 尾部 0x15 块（je +0x87）。
    if (it != m_monsters.end())
    {
        CVillageMonster* monster = &it->second;
        return monster->OnFightVillageMonster(user, out1, out2);
    }
    return 0x15;
}

// ============================================================================
// CVillageMonsterMgr 构造/析构 —— ORIG 0x86b4a4c / 0x86b4a80
// ============================================================================
#include "CVillageMonsterMgr/CVillageMonsterMgr.inc"
{
    m_field04 = 0;
    m_field00 = 0;
    m_field20 = 0;
}

#include "CVillageMonsterMgr/dtor_CVillageMonsterMgr.inc"
{
}

// ============================================================================
// CVillageMonsterMgr::OnStartVillageAttacked —— ORIG 0x86b4a20
//   未开战（m_field04 != 1）时创建怪物并置 m_field04=1。
// ============================================================================
#include "CVillageMonsterMgr/OnStartVillageAttacked.inc"
{
    // [b28-L14] ORIG 0x86b4a20：`cmp $1; je <nop 岛>; body; jmp <leave>` ⇒
    // P223 形 `if (c) goto DONE; body; DONE:;`（旧 L11 N 判词为负例，本轮翻案）。
    if (m_field04 == 1)
        goto DONE;
    OnCreateVillageMonster();
    m_field04 = 1;
DONE:
    ;
}

// ============================================================================
// CVillageMonsterMgr::OnCreateVillageMonster —— ORIG 0x86b4280
//   各区域生成怪物；m_field04=2；注册 0x85 轮转定时器（1 秒）。
// ============================================================================
#include "CVillageMonsterMgr/OnCreateVillageMonster_1.inc"
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
#include "CVillageMonsterMgr/OnDestroyVillageMonster_1.inc"
{
    for (std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.begin();
         it != m_areas.end(); ++it)
    {
        it->second.OnDestroyVillageMonster();
    }
    m_field04 = 0;
    m_field00 = 0;
    // P95：ORIG 0x86b43d4 末段 store 先载 this 后载 reward；具名视图复现。
    struct CVMgrRewardView
    {
        char m_head[0x20];
        int  m_field20;
    };
    ((CVMgrRewardView*)this)->m_field20 = (int)reward;
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
#include "CVillageMonsterMgr/OnEndRewardVillageMonster.inc"
{
    m_field20 = 0;
}

// ============================================================================
// CVillageMonsterMgr::OnRunVillageMonster —— ORIG 0x86b44fc
//   未开战（m_field04==0）返回；各区域轮转；注册下一个 0x85 轮转定时器。
// ============================================================================
#include "CVillageMonsterMgr/OnRunVillageMonster_2.inc"
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
#include "CVillageMonsterMgr/OnRegenVillageMonster.inc"
{
    if (m_field04 == 0)
        return;
    Zone zone((unsigned char)a, (unsigned char)b);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(zone);
    // P133：ORIG 0x86b4338 用 operator!=（je 出口正形）非早退 eq。
    if (it != m_areas.end())
    {
        it->second.OnCreateVillageMonster();
    }
}

// ============================================================================
// CVillageMonsterMgr::OnKillVillageMonster(CUser*,bool) —— ORIG 0x86b4866
//   从队伍村战状态区取 zone（+0xc4c；0 → 0x15）与 vill/area（+0xc50/+0xc54），
//   Zone 命中区域转交 Area::OnKillVillageMonster(user, zone, flag)。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b4866）：去 view 具名指针
// （P0 每使用点完整 cast 表达式）+ 加 areaObj 具名（ORIG -0xc 槽），
// 消除 esi/ebx push 与帧差。p167 CLEAR。
#include "CVillageMonsterMgr/OnKillVillageMonster.inc"
{
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
        return 0x15;
    unsigned short zone =
        ((CPartyVillageStateView*)party)->m_villageMonsterZone;
    if (zone == 0)
        return 0x15;
    Zone z((unsigned char)((CPartyVillageStateView*)party)->m_villageVill,
           (unsigned char)((CPartyVillageStateView*)party)->m_villageArea);
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(z);
    if (it == m_areas.end())
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    return areaObj->OnKillVillageMonster(user, zone, flag);
}

// ============================================================================
// CVillageMonsterMgr::OnFightVillageMonster(CUser*,unsigned short) —— ORIG 0x86b4672
//   szZone 未命中或队伍缺失 → 0x15；区域 OnFight 失败原样返回；成功则把
//   怪物 m_field20/m_field24 写入队伍村战状态区（+0xcb8/+0xc58）、zon/ vill/area
//   落位（+0xc4c/+0xc50/+0xc54），并向队伍广播 0xf3，返回 0。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] N 封顶（ORIG 0x086b4672，N7/N82 族）：
// 去 z/area/vill 具名局部（P0 右值直传）+ 去 view 指针后槽集与 ORIG 全等、
// 帧 0x60=0x60；残余 = 两个 `return 0x15` 被 4.4 汇聚到 %ebx（ORIG 各自
// `mov $0x15,%eax`）+ party 二次使用被缓存 %ebx（ORIG 每次重载 -0x14），
// 共 4 条寄存器分配差。p167 CLEAR（19/19）。
// [M5E round6 判据 ③ 已改 · 旧判词 N7/N82 被推翻] 4 条「寄存器分配差」的真因是
//   **LHS 基址表达式的 tree 形**：逐句 `((View*)party)->m = rhs` 的 NOP_EXPR 会被
//   4.4 在 RHS 之前物化（旧码 +0x127/+0x146 `mov -0x14(%ebp),%ebx` 在 call 前），
//   而 ORIG 的基址是**普通具名局部**（+0x12b/+0x147/+0x153/+0x15f 四处
//   `mov -0x14(%ebp),%eax/%edx` 全在 call **之后**）⇒ ORIG 的 `party` 本身即视图
//   类型（cast 只在 GetParty 赋值处做一次）。探针实证：/tmp/m5e_lhs_probe.cpp
//   形 B（具名 `V* v = (V*)p;` 后 `v->m = call();`）= `call; mov -0xc(%ebp),%eax;
//   mov %edx,0xc50(%eax)`（RHS-first，与 ORIG 同形）；形 A（逐句 cast）= `call` 前
//   `mov 0x8(%ebp),%ebx`（旧码形）。改后槽集/帧不变（仍是 -0x14 一个 4B 局部）。
// [P6S5 2026-09-11 本波负结论] 末句 `((CParty*)party)->send_to_party(packet)` 的
//   4 行残差 = 「实参求值序」（ORIG 先 `lea -0x34(%ebp),%eax; mov %eax,0x4(%esp)`
//   求第 2 参，再 `mov -0x14(%ebp),%eax` 装 this；我方相反）。本波探针：
//   加具名 `CParty* partyObj = (CParty*)party; partyObj->send_to_party(packet);`
//   ⇒ 读数**劣化** 136→138（gap +0→−2）且槽漂移（-0x1a→-0x1e）⇒ 已回滚。
//   4.1 先实参 / 4.4 先 this 属寄存器分配族，源形不可造。
#include "CVillageMonsterMgr/OnFightVillageMonster_2.inc"
{
    Zone findZone(user->getCurCharacVill(),
                  (unsigned char)user->get_area(false));
    std::map<Zone, CVillageMonsterArea>::iterator it = m_areas.find(findZone);
    if (it == m_areas.end())
        return 0x15;
    CPartyVillageStateView* party =
        (CPartyVillageStateView*)user->GetParty();
    if (party == 0)
        return 0x15;
    CVillageMonsterArea* areaObj = &it->second;
    int out1 = 0;   // → monster.m_field20 → party+0xcb8
    int out2 = 0;   // → monster.m_field24 → party+0xc58
    int ret = areaObj->OnFightVillageMonster(user, zone, out1, out2);
    if (ret == 0)
    {
        party->m_villageMonsterZone = zone;
        party->m_villageVill = (int)(signed char)user->getCurCharacVill();
        party->m_villageArea = user->get_area(false);
        party->m_villageMonsterField24 = out2;
        party->m_villageMonsterField20 = out1;
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0xf3);
        packet.finalize(true);
        ((CParty*)party)->send_to_party(packet);
    }
    return ret;
}

}  // namespace village_attacked

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CVillageMonsterMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

// GetDistance 前置声明（定义见下）。
#include "CVillageMonsterMgr/GetDistance.inc"
namespace pvp_assault
{

// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x082e5a0b）：去 MapArea& 引用缓存
// （P157/P183 逐点 areas->at(i)）；tx/ty/dist 提函数头（P212 全头部声明）+
// GetDistance 实参序换为 (Point2d(x,y), Point2d(tx,ty)) + 比较写 minDist>dist
// 复现 4.1 折痕；MapArea::include 返回 int→bool（P292 调用点 test %al）。
// p167 CLEAR。
#include "CVillageMonsterMgr/IsIncludeVirtualMoveAreas.inc"
}

}  // namespace pvp_assault

// ============================================================================
// GetDistance(Point2d, Point2d) —— ORIG T 0x82e59f1（= Point2d::operator-
// 的曼哈顿距离）。ORIG 经 operator- 计算；本实现等价（同名符号）。
// ============================================================================
#include "CVillageMonsterMgr/GetDistance_1.inc"
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
}

#include "CVillageMonsterMgr/OnMoveArea.inc"
}

#include "CVillageMonsterMgr/OnMoveArea_1.inc"
}

#include "CVillageMonsterMgr/OnEvent.inc"
}

#include "CVillageMonsterMgr/InsertVillageMonster.inc"
}

// ============================================================================
// CVillageMonster 构造/析构/GetUID —— ORIG 0x86b3004 / 0x86b3098 / 0x86b3106 /
// 0x86b4b64。7 参 ctor：m_area=area；m_id=uid_seed 自增分配（清零则回 1）；
// m_zone=id；m_field04=x；m_field08=y；m_field10=0；m_field0c=slot；
// m_partyIndex=-1；m_state=0；m_field20/m_field24 直接落位。
// ============================================================================
#include "CVillageMonsterMgr/CVillageMonster_1.inc"
}

#include "CVillageMonsterMgr/dtor_CVillageMonster.inc"
}

#include "CVillageMonsterMgr/GetUID.inc"
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
}

// ============================================================================
// CVillageMonster::OnFightVillageMonster —— ORIG T 0x86b3240
//   m_state!=0 → 0x13；距怪物 > 0x12c（x 或 y）→ 0x63；否则状态置 1、
//   记录 partyIndex，回填 m_field20/m_field24 到 out1/out2，返回 0。
// ============================================================================
#include "CVillageMonsterMgr/OnFightVillageMonster.inc"
}

// ============================================================================
// CVillageMonster::MakeVillageMonsterPositionPacket —— ORIG T 0x86b36e6
//   仅 m_field10==1（本轮移动过）时：put_short(m_id/m_field04/m_field08)，
//   清零 m_field10，返回 true；否则返回 false。
// ============================================================================
#include "CVillageMonsterMgr/MakeVillageMonsterPositionPacket.inc"
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
}

// ============================================================================
// CVillageMonsterArea::GetAttackedMonster —— ORIG T 0x86b3aea
//   返回首个 m_attackWeight >= idx 的怪物；全不满足返回最后一个；空表返回 0。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] AE（ORIG 0x086b3aea）：end() 提升到循环外
// 具名 endIt（P13 缓存 end，复现 ORIG 单次 call end）；`(*it).m_attackWeight`
// 替 `it->m` 复现 deEv（P167 it-> vs *it 判据）。p167 CLEAR。
#include "CVillageMonsterMgr/GetAttackedMonster.inc"
}

// ============================================================================
// CVillageMonsterArea::OnCreateVillageMonster —— ORIG T 0x86b3bd4
//   m_field34 <= 当前怪物数 → 返回 0；随机 10000 取表项，构造怪物（slot =
//   size%3，m_field20/m_field24 取表 +0x14/+0x18），insert 后发 0xf6 新怪物包，
//   注册 0x86 生成定时器（param：m_field38/m_field28/m_field2c），返回 1。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] N 封顶（ORIG 0x086b3bd4，N82/N95/N104 族）：
// make_pair 第一参 cast 到右值（`(unsigned short)uid`）已把 p167 MISSING
// `make_pairIt` 清零、attacked 指针槽差 4（-0x20 vs -0x1c）+ ORIG 正常路径尾块
// 在函数末（N104 块序）；randIdx 具名反致帧 0xcc→0xdc。p167 CLEAR（31/31）。
#include "CVillageMonsterMgr/OnCreateVillageMonster.inc"
}

// ============================================================================
// CVillageMonsterArea::OnDestroyVillageMonster —— ORIG T 0x86b3ee0
//   清空怪物表并广播空列表包。
// ============================================================================
#include "CVillageMonsterMgr/OnDestroyVillageMonster.inc"
}

// ============================================================================
// CVillageMonsterArea::OnFightVillageMonster —— ORIG T 0x86b3fe2
//   zone 未命中 → 0x15；否则转交怪物（out1/out2 引用透传）。
// ============================================================================
#include "CVillageMonsterMgr/OnFightVillageMonster_1.inc"
}

// ============================================================================
// CVillageMonsterMgr 构造/析构 —— ORIG 0x86b4a4c / 0x86b4a80
// ============================================================================
#include "CVillageMonsterMgr/CVillageMonsterMgr.inc"
}

#include "CVillageMonsterMgr/dtor_CVillageMonsterMgr.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnStartVillageAttacked —— ORIG 0x86b4a20
//   未开战（m_field04 != 1）时创建怪物并置 m_field04=1。
// ============================================================================
#include "CVillageMonsterMgr/OnStartVillageAttacked.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnCreateVillageMonster —— ORIG 0x86b4280
//   各区域生成怪物；m_field04=2；注册 0x85 轮转定时器（1 秒）。
// ============================================================================
#include "CVillageMonsterMgr/OnCreateVillageMonster_1.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnDestroyVillageMonster —— ORIG 0x86b43d4
//   各区域清空；m_field04=0、m_field00=0、m_field20=reward；广播 0xf2 结束包。
// ============================================================================
#include "CVillageMonsterMgr/OnDestroyVillageMonster_1.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnEndRewardVillageMonster —— ORIG 0x86b44ec
//   m_field20 = 0。
// ============================================================================
#include "CVillageMonsterMgr/OnEndRewardVillageMonster.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnRunVillageMonster —— ORIG 0x86b44fc
//   未开战（m_field04==0）返回；各区域轮转；注册下一个 0x85 轮转定时器。
// ============================================================================
#include "CVillageMonsterMgr/OnRunVillageMonster_2.inc"
}

// ============================================================================
// CVillageMonsterMgr::OnRegenVillageMonster —— ORIG 0x86b4338
//   未开战返回；Zone(a,b) 命中区域则补生成一只。
// ============================================================================
#include "CVillageMonsterMgr/OnRegenVillageMonster.inc"
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
#include "CVillageMonsterMgr/OnFightVillageMonster_2.inc"
}

}  // namespace village_attacked

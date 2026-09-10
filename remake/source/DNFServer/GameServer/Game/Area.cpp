// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Area/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
//   +0x68 unsigned int m_areaType（1=村庄 2=钓鱼/竞技场 4=特殊；P211/P274 异型化）
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
#include <cerrno>
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
#include "CSchoolMgr.h"   // CBelong 权威声明点（IsRight 纯虚，返回 bool）
#include "NPCScript.h"
#include "GlobalData.h"
#include "PacketGuard.h"
#include "TownAreaScript.h"

// createLotteryInfo（ORIG 0x89babc5，自由函数；定义于 CStackableItem.cpp，
// 无权威头——CStackableItem.cpp 同样局部声明）
#include "Area/createLotteryInfo.inc"
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
    unsigned int m_areaType;     // +0x68（P211/P274：ORIG store 对 int RHS 出 eax→edx 中转）
    // [b28d-solo-Area] P248 两端冲突判例：本 TU 的 Area 局部定义保持 unsigned char
    // —— set_area 写点 `m_field6c = script.m_field24`（RHS uchar）需同型才与 ORIG
    // 086c1d4f..1d59 `movzbl 0x24; mov %dl,0x6c` 同形；bool 化实测 set_area 回 DIFF。
    // Area.h（GameWorld.cpp 读点 0x086c4609 需 bool）另取 bool——两份定义同宽同偏移，
    // 跨 TU 无 ABI 影响。
    unsigned char m_field6c;     // +0x6c
    char m_pad6d[0x70 - 0x6d];   // +0x6d
    std::vector<std::pair<int, int> > m_npcPositions;  // +0x70
    std::vector<MapArea> m_mapAreas;                   // +0x7c
    std::vector<std::pair<int, int> > m_moveAbleArea;  // +0x88
    bool m_bExistItemShopNPC;                          // +0x94（P292/P248 档②）
    char m_pad95[0x98 - 0x95];   // +0x95
};

// ---------------------------------------------------------------------------
// 构造 / 析构（成员按声明序构造、逆序析构，与 ORIG 086d214c/086d2270 一致）
// ---------------------------------------------------------------------------
#include "Area/Area.inc"
}

#include "Area/dtor_Area.inc"
}

// ---------------------------------------------------------------------------
// GetUserCount：统计 m_UsersInArea 中 state > 2 的在线用户
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 8=8；70=70）。修法：P50 先声明后赋值 + P198①
// user 预清 + **while+continue 真循环形**（取代历轮 goto 尾条件形）——4.4 对互斥分支各自
// 的 `it++` 独立发射 ppEi 并复现 4.1 的匿名 sret 槽方向（推翻旧 N93/N98「不可翻」判词）。
#include "Area/GetUserCount.inc"
}

// ---------------------------------------------------------------------------
// IsMovalbeArea：m_moveAbleArea（TownPath 起点对集合）是否包含 (vill, area)
// ---------------------------------------------------------------------------
#include "Area/IsMovalbeArea.inc"
}

// ---------------------------------------------------------------------------
// delete_user：非村庄区域从地图移除用户并广播 0x17 离开包
// ---------------------------------------------------------------------------
#include "Area/delete_user.inc"
}

// ---------------------------------------------------------------------------
// erase_user：非村庄区域仅从地图移除用户（不发包）
// ---------------------------------------------------------------------------
#include "Area/erase_user.inc"
}

// ---------------------------------------------------------------------------
// get_user_id_list：输出 state > 2 的在线用户 unique_id
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 10=10）。修法：push_back 实参用
// get_unique_id() 临时右值 ⇒ 选中 ORIG 的 push_back(EOt)（P234）；while+continue 真循环形
// 复现 4.1 匿名 sret 槽方向（同 GetUserCount 族）。
#include "Area/get_user_id_list.inc"
}

// ---------------------------------------------------------------------------
// hasNPC：m_npcIdList 是否包含指定 NPC
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 8=8；46=46）。修法：具名 `const_iterator it =
// std::find(...)` 后再 `it == end()`——P293 求值序旋钮：ORIG 086c214c eq(find(-0x18),
// end(-0xc)) 要求 find 先求值；单表达式 `find()==end()` 被 4.4 反序（先求右 end()），
// `end()==find()` 则 eq 实参序反。具名形两侧逐条全等。
#include "Area/hasNPC.inc"
}

// ---------------------------------------------------------------------------
// insert_user：进图（村庄广播 0x18 / 普通广播 0x17 + 各管理器 OnMoveArea）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 63=63；313=313）。修法：①try/catch 补全
// （086c275f..086c2821，__cxa_begin_catch×1/end_catch×2 + errno/strerror），**catch 末尾
// return** 复现 ORIG 086c289d `end_catch; jmp 086c2a22`（原实现 fall-through 继续发包 =
// 真语义 bug：异常路径本应直接返回）；②`unsigned short id;` **声明前置于 PacketGuard**
// 使 4.4 槽位集合与 4.1 全等（PacketGuard -0x30 / it -0x34 / id -0x22；旧形 id 落 -0x32）；
// ③insert 实参 `pair<const unsigned short, CUser*>(...)`（C1ItRS2_EEOT_OT0_，P234/P315）；
// ④logFormat/put_short 按 P92 重新调 get_unique_id；iterator 先声明后赋值（P50）。
#include "Area/insert_user.inc"
}

// ---------------------------------------------------------------------------
// make_power_war_hp_of_vill：收集在线用户 PowerWar HP 到包并计数
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 10=10）。修法：while+continue 真循环形 +
// 主流程尾序照 ORIG put_short → it++ → ++count（086c2ccc ppEi 先于 086c2ce9 ++count）。
#include "Area/make_power_war_hp_of_vill.inc"
}

// ---------------------------------------------------------------------------
// send_area_users：向新进图用户发送同区可见用户列表（0x18，人数回填）
// ---------------------------------------------------------------------------
#include "Area/send_area_users.inc"
}

// ---------------------------------------------------------------------------
// send_group：按 CBelong 虚谓词（vtable slot 0）过滤后发送
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9）。修法（真修复）：删本文件重复定义的
// 局部 `class CBelong`（含发明符号 IsBelong，违 §4.4/N114），include 权威头 CSchoolMgr.h，
// 调用改 `belong->IsRight(user)`（ORIG 086c2eaf vptr+0 即 CBelong 唯一虚函数 IsRight；
// 返回 bool ⇒ test %al; je 无 setne，P292）+ while+continue 真循环形。
#include "Area/send_group.inc"
}

// ---------------------------------------------------------------------------
// send_to_all：向 state > 2 的在线用户广播包
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9；70=70）。修法：P50+P28 后置 it++ +
// **while+continue 真循环形**——一条形翻正全部 5 个同族循环的匿名 sret 槽方向（旧判
// N93/N98 作废）。
#include "Area/send_to_all.inc"
}

// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9）。修法：ORIG 086c2f95 死比较
// `cmp; sete %al; test %al,%al`（无分支）用 `while (users[i] == user) break;` 复现
// （P225 dead 变体；`(void)(a==b)`/三目均被 4.4 DCE）+ while+continue 真循环形。
#include "Area/send_to_all_1.inc"
}

// ---------------------------------------------------------------------------
// set_area：由 TownAreaScript/STMapScript 初始化区域（NPC/传送/怪物注册）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 38=38；313=313）。修法：pair 默认构造+逐字段
// 赋值、it/itEnd 具名提升、`(*it).m_x` 与 `it->m_npcId` 混用（deEv×2+ptEv×4）、
// G_CDataManager()->m_npcScriptList 内联（去 dataMgr/npcList 局部）、`(*it).m_vill` 具名
// 指针、m_bExistItemShopNPC bool 化 + 裸 `if(!field)`（P292/P248 档②）。本轮新增：
// ①**Zone 临时内联进 register_object 实参** ⇒ 4.4 把 s_timeGate_ 在 Zone ctor 前升格
//   %ebx 跨调用保活（P52 callee-saved 独占形，ORIG 086c1f16/1b 逐条同形）；
// ②**m_areaType int→unsigned int 异型化** ⇒ 复现 ORIG 086c1d44/47/4c 的
//   `mov 0x8(%eax),%eax; mov %eax,%edx; mov %edx,0x68` store 中转（P211/P274）。
#include "Area/set_area.inc"
}

// ---------------------------------------------------------------------------
// take_fish：按权重随机取鱼（类型 2；权重表 m_lottery.m_items，兜底 defaultIdx）
// ---------------------------------------------------------------------------
// [b28d-solo-Area] verdict=AE（P167 CLEAR 9=9）。修法：`cur = std::pair<int,int>(*it)`
// 选 ORIG 的 `aSEOS0_` 右值赋值（P135）；it 声明在 cur 之前（构造序照 ORIG 086c2214/1f）；
// **while+break 真循环形**（取代 goto 尾标签）复现 ORIG 的匿名 pair 临时/ppEi/end 槽方向。
#include "Area/take_fish.inc"
}

// ---------------------------------------------------------------------------
// 内联 getter（ORIG 弱符号 086d238e / 086d2398 / 082343aa）
// ---------------------------------------------------------------------------
#include "Area/get_area_index.inc"
}

#include "Area/get_world_map_index.inc"
}

#include "Area/IsExistItemShopNPC.inc"
}

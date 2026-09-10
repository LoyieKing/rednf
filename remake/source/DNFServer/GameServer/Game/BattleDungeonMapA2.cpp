// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleDungeonMapA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— 地图/战场依赖 TU（BattleDungeonMapA2 批次）。
// 9 个 ORIG 符号，全部逐指令对照 docs/class_func_reports/ 完整报告转写
// （## 汇编 + ## 反编译 C 已全文核对；nm 仅用于符号核验）：
//
//   CBattle_Field::MakePacket                        0x083090a4
//   CBattle_Field::SelectDungeon                     0x082ff486
//   CBattle_Field::check_random_appear_hell_dungeon  0x0830a862
//   CBattle_Field::drop_item                         0x0830db84
//   CBattle_Field::get_field_item                    0x0830d9c0
//   CBattle_Field::is_last_field_item                0x0830da6c
//   CDungeon::InsertMap                              0x0834b988
//   CDungeon::insertStart                            0x0834be5a
//   CDungeon::IsOpenScheduleDungeon                  0x0834d2d4
//
// 字段化依据：CBattle_Field.h / CDungeon.h / CParty.h / WarField.h（MapInfo、
// map_item、map_monster）/ CMap.h 权威布局。
// ============================================================================

#include <map>
#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CWorldMap.h"
#include "GlobalData.h"
#include "DataManagerInitDepends.h"  // TOD_UserState/TOD_Layer（MakePacket 0x1c 当日层链）
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"

// ============================ CBattle_Field ============================

// ---- drop_item(map_item) —— ORIG 0x0830db84（drop_item.md 全文） ----
// GetCurrentMapInfo()->m_dropItemCnt(+0x40) 作 uid 分配器：取旧值、+1 回写，
// 再 MapInfo::Add_Item(map_item)（ORIG 将按值参数复制到栈上传入）。
#include "BattleDungeonMapA2/drop_item.inc"
}

// ---- get_field_item(int, map_item&) —— ORIG 0x0830d9c0（get_field_item.md） ----
// 当前地图 m_itemMap(+0x24) 查找 uid；命中则 21 dword 复制到出参并返回 1。
// ORIG 形态同 is_last_field_item：迭代器先默认构造（C1Ev @+0x0f）、find 经
// sret 临时槽（-0x2c）赋值（P28），op-> 一次 + rep movsl 21 dword 整体复制
// （P83/N78 4.1 内联形态）。
#include "BattleDungeonMapA2/get_field_item.inc"
}

// ---- is_last_field_item(int) —— ORIG 0x0830da6c（is_last_field_item.md） ----
// 与 get_field_item 同构，但作用于 GetLastMapInfo()；返回 uid 是否仍在场上。
// ORIG 形态（报告 0x830da72-0x830da6b）：迭代器先默认构造（C1Ev @+0x09）、
// find 经 sret 临时槽（-0x1c）再 4 字节赋值到 -0x10（P28「先声明后赋值」）、
// == end 后走 test/je→mov$1 分支岛（P56，非 sete 直折）。
#include "BattleDungeonMapA2/is_last_field_item.inc"
}

// ---- check_random_appear_hell_dungeon(CDungeon const*, ENUM_DUNGEON_MODE,
//      ENUM_DUNGEON_TYPE, bool&) —— ORIG 0x0830a862（报告全文） ----
// 外层守卫：m_party->m_hellWorldMap(+0x1868) 非零。地狱开关+所在地狱位面时，
// 统计 4 席有效成员的等级 min/max（无角色者按 1 级），依据与副本标准等级的
// 差值决定 *flag；等级离散(>8)且 ≥2 人、或两个旗标未置且类型 1 时清除缓存。
// mode==1（普通模式）强制清除并置 flag=false。
// [b28d-solo-BattleDungeonMapA2] IDENTICAL/AE（147=147，本波 DIFF→AE）：
// ① IsEnableHellDungeon 改裸 bool 判（P292；`> 0` 形 4.4 产 movzbl/test/setg）；
// ② min/max 三目臂序翻转 `(min>level)?min:level`（复现 ORIG jle 直落+then 岛）；
// ③ validCount 块 P222 反向包裹 `if(validCount>1){…}else{flag=1}`；
// ④ CParty::m_field1864/m_sealDoorFlag bool 化（P248 档②，ORIG
// `movzbl;xor $1;test;je`；CParty.h 已 R2 广播，消费点 CParty.cpp/
// CPartyDepsA2.cpp 回执零回归）。
#include "BattleDungeonMapA2/check_random_appear_hell_dungeon.inc"
}

// ---- SelectDungeon(...) —— ORIG 0x082ff486（SelectDungeon.md 全文） ----
// 记录副本/难度/类型/世界图参数 → 地狱派对判定（type==1 强制，或随机条件）
// → m_89d 非 1 时清 hell 标记 → 由 dungeon->m_6e0[diff] 取迷宫指针 →
// GetAppropriateMaze/SetGridPath/清除条件 → 起始格定位起始地图 →
// 构造 MapInfo{mapIndex,startMap} 插入 m_mapInfo 并 ConsistMap。
// [b28d-solo-BattleDungeonMapA2] IDENTICAL/AE（313=313，本波 DIFF→AE）：
// ① logFormat 第 3 参改 __PRETTY_FUNCTION__（P118/P139；ORIG 具
// _ZZN13CBattle_Field13SelectDungeon…E19__PRETTY_FUNCTION__ 局部静态，
// 手写字符串只产匿名 .rodata）；② IsEnableHellDungeon 裸 bool 判（P292）；
// ③ m_hellPartyMode 经权威头匿名 union 的 enum 视角赋值复现 LHS-first
// （P211 档③；探针 f1/f2/f3 证 int 字段+(int)cast 恒 RHS-first）；
// ④ m_hellPartyRandom bool 化（同型 store）；⑤ CDungeon::m_89d bool 化 +
// `!dungeon->m_89d`（P248 档②）；⑥ startMap 声明=0 + 独立赋值（P198③）。
// CBattle_Field.h/CDungeon.h 两处 .h 改动均已 R2 广播。
#include "BattleDungeonMapA2/SelectDungeon.inc"
}

// ---- MakePacket(int, void*) —— ORIG 0x083090a4（MakePacket.md 全文） ----
// [b28d-solo-BattleDungeonMapA2] §6.2 超大函数三件套验收（fresh .o）：
// ① gap +5（our 1058 / orig 1053）；② P167 调用点差集 CLEAR（182=182；
// 本波补回真语义缺口——ORIG @0xd8/0xea 是两条独立 put_byte 调用点，
// 旧实现写成三目合并致 MISSING put_byte×1）；③ put_byte 实参直方图 64=64
// 逐类全等（reg 44 / 0×14 / 0xff×2 / 0xb×2 / 0x1 / 0x3）、equal 885/1053、
// 帧 0x46c vs 0x45c。残余全为跨版本形态族：N82（帧/槽平移）+ N7/N18a
// （4.1 值经 %edx 中转 vs 4.4 %eax 直传）+ P194/setcc（bool 物化）⇒ 语义完成、
// byte 层 N 封顶。本波另修：0x182 的 setg 有符号比较、find_map 结果落具名槽
// （void* mapData）、`m_party==0` 两处 P222 反向 if/else。
// 六个包分支（switch 决策树 cmp 0xd3 根）：0x1d（进图全量：坐标/怪物/掉落）、
// 0x1c（房间基础+骑乘物+当日层）、0xd3/0x183（小地图信息，两份独立展开，
// find_map 失败走日志且不 finalize）、0xd4（无头的精简房间信息）、
// 0x182（锦标赛轮次表）。MapInfo 字段访问按 ORIG 逐点重取
// GetCurrentMapInfo()（P183 禁缓存；0x182 分支例外——ORIG 自身缓存 map ptr）。
#include "BattleDungeonMapA2/MakePacket.inc"
}
// ============================== CDungeon ==============================

// ---- InsertMap(int, int, CMap*) —— ORIG 0x0834b988（InsertMap.md 全文） ----
// 按 CMap::get_map_type() 选择容器：0 → m_map1a4[idx]；2 → m_dummyMap4a4[idx]
// 并置 m_624=1；其余 → m_dummyMap324[idx]。direction 位 1/2/4/8 各插入一条
// pair(方向键, CMap*)。三段结构按 ORIG 原样展开。
// [b28d-solo-BattleDungeonMapA2] NEAR 封顶（N82）：347=347 助记符序列全等、
// P167 调用点差集 CLEAR；本波把 12 组 pair 构造实参改为 PATH_DIRECTION 枚举
// （ORIG mangled `C1I14PATH_DIRECTIONRS2_EEOT_OT0_`，权威头 CDungeon.h 新增
// 全局 enum PATH_DIRECTION 定义）后调用点符号全等。残余 102 行全为 4.1
// 不复用槽（各分支独立 key@-0xbc/-0xac/…）vs 4.4 生命周期复用
// （-0x68/-0x60/…）+ call/jmp 目标显示差；槽策略跨版本不可翻。
#include "BattleDungeonMapA2/InsertMap.inc"
}

// ---- insertStart(int, CMap*) —— ORIG 0x0834be5a（insertStart.md 全文） ----
// 以 CMap::get_index() 为键把起始地图插入 m_maplist[idx]（hash_map @+0x64）。
// [b28d-solo-BattleDungeonMapA2] NEAR 封顶（N86/N82）：31=31 助记符序列全等、
// P167 CLEAR，pair 构造符号与 ORIG 全等（`C1IiRS2_EEOT_OT0_`）；仅三临时槽
// 分配位互换（ORIG int 临时@-0xc/pair@-0x14/insert-sret@-0x20 vs 4.4
// sret@-0x14/临时@-0x18/pair@-0x20）。本波实测：具名 key 形改符号为
// `C1IRiRS2_E`（左值）不可用；static_cast<int&&> 形劣化为 DIFF；4.4 的
// 临时槽分配序由前端固定，源形不可择。
#include "BattleDungeonMapA2/insertStart.inc"
}

// ---- IsOpenScheduleDungeon() const —— ORIG 0x0834d2d4（报告全文） ----
// GlobalData::s_DailyScheduleManager.GetScheduleState(type=0, m_index) ∈ {1,2}。
#include "BattleDungeonMapA2/IsOpenScheduleDungeon.inc"
}

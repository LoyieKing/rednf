// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleDepsB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// BattleDepsB2.cpp — CBattle_Field / CRidable 剩余链接方法（独占 TU，宿主登记 CMake）。
// ----------------------------------------------------------------------------
// [b28d-E03 2026-09-10 本波] GetHellPartyEventType **13=13 归零**（DIFF→AE）：
//   `CDataManager::get_hellparty_script_values()` 返回类型 void*→stHellParty_t*
//   （P211 callee 返回类型旋钮，返回类型不入 mangled ⇒ 零 ABI；探针
//   /tmp/probe_hp2.cpp：void* 形 fA=14 条含 `mov %eax,%edx`，stHellParty_t* 形
//   fB=13 条 = ORIG 0x082ffb06 逐条形）。其余三件封顶证据见各函数头注释；
//   SetGridPath 另附 F2a C05② 复测（7 处 `cmp mem,reg` 全为 mem 直比、jl↔jg 属
//   折向非符号性翻转、idx/prevIdx 已均 int ⇒ 无旋钮，N82/N98 维持）。
//
// 恢复符号（ORIG nm 全部实证，df_game_r）：
//   _ZN13CBattle_Field23CheckHellPartyConditionEv        T 0x082ff988
//   _ZN13CBattle_Field10ConsistMapEv                     T 0x083031d2
//   _ZNK13CBattle_Field18GetAppropriateMazeE23ENUM_DUNGEON_DIFFICULTY T 0x082ff128
//   _ZN13CBattle_Field22GetHellPartyDifficultyE17ENUM_DUNGEON_TYPE    T 0x082ffa2e
//   _ZN13CBattle_Field21GetHellPartyEventTypeEc          T 0x082ffb06
//   _ZN13CBattle_Field7MoveMapEiiRb                      T 0x08308ae6
//   _ZN13CBattle_Field11SetGridPathEP10MazeScripti       T 0x082ffb30
//   _ZN13CBattle_Field15checkLayeredMapEii               T 0x0830e32e
//   _ZN13CBattle_Field9onUseItemEP5CUserRK10Inven_Item   T 0x0830dbde
//   _ZN13CBattle_Field16setQuestMazeInfoEv               T 0x0830e170
//
// 报告依据（编辑前已完整读 ##汇编 + ##反编译 C）：
//   docs/class_func_reports/CBattle_Field/{CheckHellPartyCondition,ConsistMap,
//     GetAppropriateMaze,GetHellPartyDifficulty,GetHellPartyEventType,MoveMap,
//     SetGridPath,checkLayeredMap,onUseItem,setQuestMazeInfo}.md
//   CRidable::GetObjectCnt 无函数报告 → 已先读同类报告（CRidable/README、
//     SetRidableObjectState）与调用方（BattleDungeonMapA2.cpp
//     MakePacket 段）后按规则 objdump 0x830ea0a 转写（= m_objects.size()）。
//
// 布局核对：CBattle_Field(0xcd0)/CRidable/CParty/BattleData(+0x7e0)/CMap/
//   MapInfo/MazeScript/GridScript/stHellParty_t 均用权威头具名成员；
//   仅 CDataManager+0x49d0 一处 ORIG 未建模字段保持直接读取（见 ConsistMap 注释）。
// ============================================================================
// [b28d-solo-BattleDepsB2] 本 TU 判词表（verdict = check_tu_routed.sh，fresh .o）：
//   CheckHellPartyCondition 0x082ff988 AE：修法 = 正向包裹 NULL 检查 +
//     `&&` 短路条件 + 尾部 return false（探针 probe_cp2 f_fwd_and 50 条 +
//     ORIG 特有 G_CDataManager 二次调用 2 条 = ORIG 52 条逐条同形）。
//   ConsistMap 0x083031d2             AE：修法 = 删 dm/dropLimit 具名槽
//     （三目右值直传，P0/P183）+ seq/uid 声明序互换 + 否定守卫早返。
//   MoveMap 0x08308ae6                AE（**真语义 bug**：原 `if(A||B){拒绝}`
//     分支整体反置；ORIG 0x082ffb2c/+0x03e 两处 `xor $1;test;je <主流程+0x63>`
//     ⇒ A||B 走主流程。修法 = 嵌套否定守卫把拒绝块放最内层）+ P198① err 预清
//     + P225 while 物化 4 处 + GetCurrentMapInfo 第三调用点（p167 32/32）。
//   setQuestMazeInfo 0x0830e170       NEAR(117/117)：结构全等；残差 = 2 处
//     receiver/实参装载序（ORIG 先 receiver，4.4 恒先算实参——probe_ord2
//     f1..f10 十形全同；4.4.6 后端实测读数完全一致，非路由敏感）⇒ N-P75 封顶。
//   GetAppropriateMaze 0x082ff128     N(217/212)：正向复合条件嵌套 + size()
//     三分支直接 return 已对齐（p167 CLEAR 25/25）；残差 = 4.1/4.4 槽序
//     （N82/N98）+ `get_member_count()>1` setg 物化（P197④ 4.4 恒直跳）⇒ 封顶。
//   SetGridPath 0x082ffb30            N(1101/1121，超大件 §6.2)：p167 差集仅剩
//     EXTRA stEventMonsterStatus_t::~D1Ev ×4（ORIG D1 为头内 W 空体、4.1 前端
//     省略调用；4.4 对 out-of-line 空 dtor 恒发射）。消除需 CBattle_Field.h
//     头内化 + CBattle_Field.cpp 删定义（非本泳道 .cpp，已上报 Main）。
//     其余为 m_gridMaze 成员装载/槽基差（N82/N95）。
//   GetHellPartyEventType 0x082ffb06  N(13/14)：ORIG 留 call 结果 %eax 作基址、
//     索引 movsbl→%edx；4.4 ISel 恒 index→%eax、base 挪 %edx（+1 mov）。唯一旋钮
//     = getter 返回类型 void*→stHellParty_t*（探针 probe3 h1 14 条 / probe4 a1
//     13 条 = ORIG 形，4.4.7/4.4.6 双后端一致）；涉 CDataManager.h + 7 消费 .cpp，
//     Main 裁决本波不落（收益 1 件 < 风险）⇒ 封顶。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <list>
#include <utility>
#include <vector>

#include "BattleData.h"           // BattleData（CParty+0x32c 子对象，m_hellPartyValueTotal）
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h        // CBattle_Field + CMap/CDungeon/CParty/WarField 权威类型
#include "CGuildServerProxy.h"    // CEventBase::IsEventing / CEventManager::GetRepeatEvent
#include "DataManagerInitDepends.h"  // stHellParty_t（get_hellparty_script_values 返回值）
#include "Quest.h"               // Quest 完整类型（m_field120）
#include "GlobalData.h"           // GlobalData::s_systemTime_ / s_event_manager

// ---- MONSTER_POS（ORIG 全局类型：W operator== 0x830e6e9 双字节比较；
//      SetGridPath 局部 std::list<MONSTER_POS> 去重表。全源码此前无定义点。）----
struct MONSTER_POS
{
    char m_x;   // +0x00
    char m_y;   // +0x01
};

inline bool operator==(const MONSTER_POS& a, const MONSTER_POS& b)
{
    return a.m_x == b.m_x && a.m_y == b.m_y;
}

// ============================ CRidable ============================

// ORIG 0x0830ea0a：直接返回 m_objects.size()（call _ZNKSt3mapIi20DungeonRidableObject...4sizeEv）。
#include "BattleDepsB2/GetObjectCnt.inc"
}

// ============================ CBattle_Field ============================

// ORIG 0x082ff988 CheckHellPartyCondition：
//   m_party 为空 → false；rate=(int)party->BattleData.m_hellPartyValueTotal(+0xb0c)；
//   rate >= get_rand_int(100) 且 rate >= stHellParty_t.m_guideLineValue(+0x90) → true。
// b28d-55（P0/P183）：ORIG 无 bd/vals 指针局部、无 ok 槽——party+0xb0c 两次
//   独立读取均单条 `flds 0xb0c(%eax)` 折叠，bool 汇聚在 %eax（g1 形探针
//   /tmp/b28d_l55/probe_bool.cpp）。故去掉中间指针缓存与具名 bool，改嵌套正向
//   if + return（P214①/P169）。残差：ORIG 尾部多一层 bool 归一岛（4.1 寄存器化
//   bool，4.4 具名局部必占槽）——P0 判读为无具名局部，落 N（跨版本 bool 物化）。
#include "BattleDepsB2/CheckHellPartyCondition.inc"
}

// ORIG 0x083031d2 ConsistMap：当前图装配怪物/物品链（逐调用与报告一致）。
#include "BattleDepsB2/ConsistMap.inc"
}

// ORIG 0x082ff128 GetAppropriateMaze(diff)：
//   按 questType 0/1 取任务迷宫向量；过滤血战态/hell 标记/任务连接/单人格挡；
//   questType==0 额外要求 maze->m_54 <= diff；候选 1 个直取、多个 rand()%n，
//   两轮无候选则回退 CDungeon::GetRandMazebyPartyCount(成员数)。
// b28d-solo-BattleDepsB2 定案（N 封顶 217/212）：已对齐项 = 正向复合条件嵌套（血战/hell/
//   连接/任务三过滤）、候选 size 三分支直接 return（无 picked/found 具名槽）、
//   push_back 右值重载、size() 三次独立调用；p167 CLEAR(25/25)。残差 = 4.1/4.4
//   槽序（maze 等局部 N82/N98）+ `get_member_count()>1` 的 setg 物化
//   （ORIG cmp/setg/test vs 4.4 直跳，P197④ 族）⇒ 封顶。
#include "BattleDepsB2/GetAppropriateMaze.inc"
}

// ORIG 0x082ffa2e GetHellPartyDifficulty(type)：
//   六字节难度比例取自 stHellParty_t（type==1 → m_field84[6]，否则 m_field7e[6]
//   且非空队伍经 increase_hell_party_ratio 提升），rnd=get_rand_int(100) 落区间
//   [i]=1..5 返回 i，否则 0（下标 0 槽不参与）。
#include "BattleDepsB2/GetHellPartyDifficulty.inc"
}

// ORIG 0x082ffb06 GetHellPartyEventType(diff)：查 stHellParty_t.m_field8a[diff]。
// b28d-solo-BattleDepsB2 定案（N 封顶 13/14）：残差为 base/index 寄存器放置——ORIG 留 call
//   结果于 %eax、索引 `movsbl -0xc(%ebp),%edx`；4.4 ISel 恒 index→%eax、base 挪
//   %edx（多 1 条 `mov %eax,%edx`）。探针定案：唯一旋钮 = getter 返回类型
//   void*→stHellParty_t*（/tmp/b28d_bdb2/probe3.cpp h1 = 14 条、probe4.cpp a1
//   = 13 条 = ORIG 形；4.4.7 与 4.4.6 后端读数一致，非路由敏感）。该改动涉
//   CDataManager.h + 7 个消费 .cpp 的 static_cast<const float*>，Main 裁决本波
//   不落（收益 1 件 < 跨 TU 风险）⇒ 封顶。
#include "BattleDepsB2/GetHellPartyEventType.inc"
}

// ORIG 0x08308ae6 MoveMap(x, y, firstVisit)：换图主流程（错误码 0/1/0x12/0x14/0x15 或检查函数码）。
#include "BattleDepsB2/MoveMap.inc"
}

// ORIG 0x082ffb30 SetGridPath(maze, dummyAppearCnt)：
//   记录迷宫宽高与起点/boss 槽位链、dummy 图序列、hell 三字段、ridable 对象；
//   bossRoom90c==1 时装配进 boss 房条件组；事件怪两轮生成（0x24 重复事件判活 +
//   MONSTER_POS 去重 / 第二事件不去重）。
// b28d-solo-BattleDepsB2（超大件 §6.2，N 封顶 1101/1121）修法与残差：
//   * 全程 `m_gridMaze->` 成员访问（ORIG 0x74(%eax) 装载实证，非形参 maze）；
//   * 三个 pair C1Ev 默认构造 + 赋值（无 dummyPos 具名局部，临时槽直传
//     push_back）；`size()==0` 判空（非 empty()）；
//   * getEventMonsters/getSecondEventMonsters 各两次独立调用（begin/end 内联
//     receiver，P157）+ const 版 begin/end（编译期 const 指针转换）；
//   * MONSTER_POS 去重：`_List_iterator` 先声明后赋值（P50 复活）+ operator==
//     （非 operator!=）；mapCandidates 循环后显式 `cit == citEnd` 判定（无 ok bool）。
//   p167 差集仅剩 EXTRA stEventMonsterStatus_t::~D1Ev ×4：ORIG 的 D1 为头内 W
//   空体（08316adc push/pop/ret），4.1 前端省略该调用；4.4 对 out-of-line 空 dtor
//   恒发射。消除需 CBattle_Field.h 头内化 + CBattle_Field.cpp 删定义（非本泳道
//   .cpp，已上报 Main）。其余残差为槽基/帧差（N82/N95）。
#include "BattleDepsB2/SetGridPath.inc"
}

// ORIG 0x0830e32e checkLayeredMap(x, y)：迷宫格带层叠表则推进层号并置当前图层标记。
#include "BattleDepsB2/checkLayeredMap.inc"
}

// ORIG 0x0830dbde onUseItem(user, item)：技能材料计时 + 用物计数。
#include "BattleDepsB2/onUseItem.inc"
}

// ORIG 0x0830e170 setQuestMazeInfo()：扫描两类任务迷宫，符合条件者置任务迷宫标记。
// b28d-solo-BattleDepsB2 定案（NEAR 117/117，N-P75 封顶）：已对齐 = for(i<=1) 尾测 setle、
//   `mazes!=NULL && !empty()` 正向包裹、while(it!=itEnd) 三槽阶梯、continue 中继
//   nop 岛、全部失败路径汇入循环尾 ++it（ORIG +0x154）。残差仅 2 处 receiver/
//   实参装载序（ORIG 先 receiver 后实参；4.4 恒先算实参，probe_ord2 f1..f10
//   十形全同，4.4.6 后端读数逐字节一致）⇒ 跨版本装载时刻，封顶。
#include "BattleDepsB2/setQuestMazeInfo.inc"
}

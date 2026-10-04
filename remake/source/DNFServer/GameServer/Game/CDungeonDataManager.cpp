// ===== [SplitC batch9 · A 手法合并] holder=CDungeonDataManager.cpp ← T=InitEnvHelpers.cpp（T 内容追加末尾；原 T 已从 CMake 移除）=====
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CDungeonDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [SweepCDungeonDataManager 判词] 本 TU 8 件最终状态：
//  InsertDungeonKey 0x084856ac NEAR：残差 17 行全为 lea 槽号（ORIG it -0x1e0/end -0x1dc/
//   node -0x98；我方 node -0x1e0/it -0x150）。4.4 聚合对象优先占深槽 vs 4.1 按声明序
//   （先声明更深）⇒ N93/N82附 封顶；node 提前声明会改 ctor 发射位置（ORIG ctor 在
//   find/if 之后 0x0848571d），不可取。p167 余 C1/C2、D1/D2 同址别名（P93 伪差集）。
//  UpdateSkill 0x084859d8 / UpdateMonster 0x08485a5a / UpdateCharacter 0x08485956：
//   N107 翻案 —— 残差根因不是「成员常量偏移取址」而是**建模**：ORIG `lea 0x4(%eax),%ecx`
//   是把 SIG +0x04 处当作 12 字节 DungeonKey 子对象取址（find(DungeonKey&)）。旧源码
//   `(DungeonKey&)stat->m_dungeonIndex` 是 int 字段 cast 形 ⇒ 4.4 恒 add+mov（N107 六形
//   探针结论仅对「int 字段 cast」成立）；改用完整类型具名视图成员
//   `((SigDungeonStatisticKeyView*)stat)->m_key` 后 4.4 发单条 lea（探针 /tmp/probe_ddm2.cpp
//   probeC 与 ORIG 逐条同形），三件 DIFF→AE。
//  InsertDungeonKey：node 改用**表达式临时** CDungeonNode()（[N5E 2026-09-11]）——ORIG
//   0x0848573c 实例为 <DungeonKey&, CDungeonNode>（值传/右值形）；具名 `CDungeonNode node;`
//   形令 4.4 把具名对象放深区（node=-0x1e0，其余 5 槽整体前移，NEAR），表达式临时形则
//   node=-0x98/make_pair=-0x134/pair<const>=-0x1d0/insert.ret=-0x1d8/end=-0x1dc/it=-0x1e0
//   一次归位（ORIG 0x084856ac 逐槽实证）⇒ **NEAR→AE（95/96，gap −1 为尾 nop 豁免）**。
//   p167 修后仅剩 C1/C2、D1/D2 同址别名。
//  其余 4 件（InsertMonsterKey/InsertSkillKey/UpdateDungeonDataStatistic/GetInstance）
//  及 C1/D1 与值类 dtor 均 AE 或 IDENTICAL；无 `call *%reg` 间接调用（ORIG
//  区间 0x084849b4..0x08485b79 与 .o 全文件均 0 处）⇒ 无虚槽核对项。
//  四数（check_tu_routed fresh）：141/211/1/4/128 → 142/216/1/1/125 → 142/217/0/1/125
//  （InsertDungeonKey NEAR→AE：near 1→0、ae 216→217，零劣化）。
// CDungeonDataManager 批次实现（仅 ORIG 符号，无桥）。
// 证据：docs/class_func_reports/CDungeonDataManager/ 全部 7 份报告
// （InsertDungeonKey/InsertMonsterKey/InsertSkillKey/UpdateCharacter/
//  UpdateMonster/UpdateSkill/UpdateDungeonDataStatistic）。
// 单例 GetInstanceDungeonDataManager T 0x084849b4：P20 函数局部 static。
#include "CDungeonDataManager.h"

#include "SigTypes_B.h"  // SIG_DUNGEON_DATA_STATISTIC 完整定义（键 DungeonKey @ +0x04）

// SIG_DUNGEON_DATA_STATISTIC +0x04 处是 12 字节 DungeonKey 子对象：
// ORIG 0x08485956/0x08485a5a/0x084859d8 均以 `lea 0x4(%eax),%ecx` 把 param_1+0x04
// 作 DungeonKey& 传给 map::find；DungeonKey::operator< 读 +4 diff / +0 索引，与 SIG 的
// m_dungeonDiff(+0x08)/m_dungeonIndex(+0x04) 逐字段重合（CDungeonNode.h:26 布局）。
// 视图只做指针类型双关、从不实例化（零符号发射），sizeof/偏移不变（P95/P235）。
namespace
{
#pragma pack(push, 1)
#pragma pack(pop)
}

CDungeonDataManager& GetInstanceDungeonDataManager()
{
    static CDungeonDataManager instance;
    return instance;
}

#include "CDungeonDataManager/InsertDungeonKey.inc"


#include "CDungeonDataManager/InsertMonsterKey.inc"


#include "CDungeonDataManager/InsertSkillKey.inc"


#include "CDungeonDataManager/UpdateCharacter.inc"


#include "CDungeonDataManager/UpdateMonster.inc"


#include "CDungeonDataManager/UpdateSkill.inc"


#include "CDungeonDataManager/UpdateDungeonDataStatistic.inc"




// ===== [merged from InitEnvHelpers.cpp] =====
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitEnvHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <vector>

#include "InitEnvHelpers/SetCollectDungeonStatistic_decl_placeholder.inc"

#include "InitEnvHelpers/SetCollectDungeonStatistic.inc"
#include "InitEnvHelpers/prepareServerDelayCheckDamage.inc"
// ---------------------------------------------------------------------------
// ORIG importCreatureCommonScript / importEtcParameterScriptForServer
// （真实符号，定义由脚本装载批次提供）
// ---------------------------------------------------------------------------
class CreatureCommonScript;

#include "InitEnvHelpers/importCreatureCommonScript.inc"
                                std::vector<int>* damageTable);
    // _Z26importCreatureCommonScriptP20CreatureCommonScriptPKcPSt6vectorIiSaIiEE

#include "InitEnvHelpers/importEtcParameterScriptForServer.inc"
    // _Z33importEtcParameterScriptForServerv

// ---------------------------------------------------------------------------
// CreatureCommonScript（ORIG C1 0x08096ab4 / clear 0x08096a2c / D1 0x08096b22）
//   clear：+0x00..+0x14 六个 int 清零 → vector<int>@+0x18/.clear()
//          → vector<int>@+0x24/.clear() → +0x30=1 → +0x34=1 → +0x38=100(0x64)
//          → +0x3c=0
//   C1：vector@+0x18 构造 → vector@+0x24 构造 → clear()
//   D1：~vector@+0x24 → ~vector@+0x18（成员逆序析构）
//   尺寸按 ORIG 调用方栈对象 0x58（Ghidra local [88]）补尾垫。
// ---------------------------------------------------------------------------
class CreatureCommonScript
{
public:

    CreatureCommonScript() { clear(); }   // ORIG C1：vector 成员默认构造 + clear()
    ~CreatureCommonScript();

    void clear();
    // [b28d-62] P238：ORIG clear 0x08096a35 对 +0x00 发 `mov $0x0,%edx;
    //   mov %edx,(%eax)`（常量 store 经 edx 中转）⇒ 字段真型 float（`= 0.0f`
    //   复现中转形；宽度/偏移/sizeof 不变）。
    float m_field00;                // +0x00（clear 清零）
    int m_field04;                  // +0x04
    int m_field08;                  // +0x08
    int m_field0c;                  // +0x0c
    int m_field10;                  // +0x10
    int m_field14;                  // +0x14
    std::vector<int> m_vec18;       // +0x18（clear 清空）
    std::vector<int> m_vec24;       // +0x24（clear 清空）
    int m_field30;                  // +0x30（clear 置 1）
    int m_field34;                  // +0x34（clear 置 1）
    int m_field38;                  // +0x38（clear 置 0x64）
    int m_field3c;                  // +0x3c（clear 置 0）
    char m_pad40[0x18];             // +0x40..+0x57（至 ORIG 栈对象尺寸 0x58）
};

#include "InitEnvHelpers/dtor_CreatureCommonScript.inc"

#include "InitEnvHelpers/clear.inc"


// ORIG g_creature_damage_vector_ B 0x093f5d50 (0xc)：生物伤害表装载目标。
std::vector<int> g_creature_damage_vector_;

#include "InitEnvHelpers/prepareServerDelayCheckDamage_1.inc"


// ===========================================================================
// 地下城统计采集开关（ORIG DungeonData 域：0x8484a47/51/67 三连；
// Set 由 CDataManager::init @0x835a4bd 调用，Is 由 CParty 域 @0x85b5xxx 消费）
// ===========================================================================

bool* GetCollectDungeonStatistic(void)
{
    static bool collect_dungeon_statistic;   // ORIG _ZZ26GetCollectDungeonStatisticvE25collect_dungeon_statistic
    return &collect_dungeon_statistic;    // ORIG mov $0x943d520,%eax
}

#include "InitEnvHelpers/IsCollectDungeonStatistic.inc"


#include "InitEnvHelpers/SetCollectDungeonStatistic_1.inc"


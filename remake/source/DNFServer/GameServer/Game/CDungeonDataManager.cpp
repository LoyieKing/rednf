// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDungeonDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
//  InsertDungeonKey 的 make_pair 第二参改 static_cast<CDungeonNode&&>(node)：ORIG 0x0848573c
//   实例为 <DungeonKey&, CDungeonNode>（值传/右值形），旧具名左值形实例化 <.., CDungeonNode&>
//   （p167 EXTRA/MISSING）。修后 p167 仅剩 C1/C2、D1/D2 同址别名。
//  其余 4 件（InsertMonsterKey/InsertSkillKey/UpdateDungeonDataStatistic/GetInstance）
//  及 C1/D1 与值类 dtor 均 AE 或 IDENTICAL；无 `call *%reg` 间接调用（ORIG
//  区间 0x084849b4..0x08485b79 与 .o 全文件均 0 处）⇒ 无虚槽核对项。
//  四数（check_tu_routed fresh）：141/211/1/4/128 → 142/216/1/1/125。
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
struct SigDungeonStatisticKeyView
{
    int m_type;        // +0x00
    DungeonKey m_key;  // +0x04（sizeof 0xc，覆盖 SIG +0x04..+0x0f）
};
#pragma pack(pop)
}

CDungeonDataManager& GetInstanceDungeonDataManager()
{
    static CDungeonDataManager instance;
    return instance;
}

#include "CDungeonDataManager/InsertDungeonKey.inc"
}

#include "CDungeonDataManager/InsertMonsterKey.inc"
}

#include "CDungeonDataManager/InsertSkillKey.inc"
}

#include "CDungeonDataManager/UpdateCharacter.inc"
}

#include "CDungeonDataManager/UpdateMonster.inc"
}

#include "CDungeonDataManager/UpdateSkill.inc"
}

#include "CDungeonDataManager/UpdateDungeonDataStatistic.inc"
}


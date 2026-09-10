// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BattleData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r BattleData 还原（G2-5 批次）。
// CParty +0x32c，总尺寸 0x7f8。符号（nm -C，W/T 全实现）：
//   C1 0x082a4cea / D1 0x082a54d6 / Reset 0x082a4e8a /
//   GetHellPartyValueTotal 0x0822d7c4 / SetHellPartyValueTotal 0x0822d7b2 /
//   GetTotalKilledMonsterCount 0x085bf456 /
//   IncEventDungeonDestoryObjectPoint 0x085be6e2 /
//   ResetEventDungeonClearPoint 0x085be6ba /
//   ClearEventDungeonRoom 0x085be7aa / IsClearEventDungeonRoom 0x085be862
// 布局（由 Reset/构造/析构反汇编推导）：
//   +0x00 int[10] 字段区、+0x28 char[4]、+0x2c int[9]（含 3 个击杀计数）、
//   +0x50 char[4]x5、+0x64 int[4]、
//   +0x74 Inven_Item[4][2]（0x3d 每项）、+0x25c Inven_Item[4][2]、
//   +0x444 Inven_Item[4][2]、+0x62c stEventClearData[12]（0x8 每项）、
//   +0x68c int、+0x690 GameResultType[4]（0x50 每项）、+0x7d0 int[4]、
//   +0x7e0 float m_hellPartyValueTotal、+0x7e4 char[4]、+0x7e8 int[4]。
// Inven_Item/GameResultType 跨类方法以 extern asm 标签引用（ORIG 真实符号）。

#include <string.h>

#include "BattleData.h"
#include "Inven_Item.h"
#include "GameResultType.h"

// ============================================================================
// 构造 / 析构 / Reset
// ============================================================================

// ORIG 0x082a4cea：ctor 体内只有 Reset()；三组 Inven_Item[4][2] 与
// GameResultType[4] 的构造全部由编译器隐式生成（含 EH 反向析构 landing pad），
// 不存在 placement-new（ORIG 无 _ZnwjPv 调用点）。
#include "BattleData/BattleData.inc"
}

// ORIG 0x082a54d6：空体，仅隐式析构 GameResultType[4]（反向 0x140 → 0）。
#include "BattleData/dtor_BattleData.inc"
}

// ORIG 0x82a4e90 头部清零为 4.1 dest 缓存形：每对字段一次 this 载入+双 store
//（0x0..0x24 五对、0x2c..0x48 四对、0x4c 单、0x28..0x2b 字节逐条、0x68c 单）。
// 4.4 函数体逐条重载 this 不可造（探针 /tmp/l0_probe2.cpp 逐句/链式/逗号三形
// 实测全不复现）——经 8 字节清零视图折叠复现（探针 /tmp/l0_probe4.cpp ResetG
// 与 /tmp/l0_probe3.cpp ResetF 同形实证）。视图仅表达布局，字段语义仍以头内
// m_fieldN/m_killedMonsterN 为权威。
namespace {
struct BattleDataZeroView {
    long long a[5];   // +0x00 m_field0/4 .. m_field20/24
    char pad28[4];    // +0x28 m_flag28[0..3]（字节域另逐字节清）
    long long b[4];   // +0x2c m_field2c/30 .. m_killedMonster2/m_field48
    int c;            // +0x4c m_field4c
};
} // namespace

#include "BattleData/Reset.inc"
}

#include "BattleData/GetHellPartyValueTotal.inc"
}

#include "BattleData/SetHellPartyValueTotal.inc"
}

#include "BattleData/GetTotalKilledMonsterCount.inc"
}

// b28d-40: 真 bug 已修（比较方向 >=）+ P183 去缓存 + goto fail 共享出口；
// 残差 = ORIG ebx 缓存 roomIndex + 单累加器 add 链 vs 4.4 lea（N4/P30 封顶）。
// ORIG 0x085be7aa。
#include "BattleData/IncEventDungeonDestoryObjectPoint.inc"
}

#include "BattleData/ResetEventDungeonClearPoint.inc"
}

// b28d-40: 真 bug 已修（st.m_monsterIndex >= param_2，原写 <= 反向；ORIG 0x085be82b
// `cmp 0x10(%ebp),%eax; jl fail`）+ P183 去 idx 缓存 + 共享 fail 出口复现；
// 本文档轮（R25DungeonPvpBattleData）实测：goto 形生 3 条出口中继岛（nop+jmp，我方 64 条）
// ⇒ 改「正极性早返 + 嵌套 if 正向包裹」（P214①/P222）：`jbe 主体; mov $0,%eax; jmp epi`
// 早返岛与三个 `jne fail` 共享 fail 块（mov $0,%eax 落尾部）逐块与 ORIG 同构，60 条 vs 61。
// 残差 = ①ORIG 用 %ebx 缓存 roomIndex（push/pop %ebx 各 1，callee-saved 角色 N7/N125）
//        ②3*p1 强度归约 ORIG `add %eax,%eax; add %edx,%eax` vs 4.4 `add; lea (%eax,%edx,1)`
//        ⇒ N86/N125 族，源形不可择（P321-T5：装载序/lea-vs-add 不得用类型旋钮赌）。
// ORIG 0x085be7aa。
#include "BattleData/ClearEventDungeonRoom.inc"
}

#include "BattleData/IsClearEventDungeonRoom.inc"
}

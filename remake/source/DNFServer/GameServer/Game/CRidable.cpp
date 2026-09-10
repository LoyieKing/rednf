// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CRidable/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CRidable 专属 TU（从 GameStubs.cpp 拆出）。
// 语义依据 ORIG CheckAppearRidableObject 0x82fe9c8 / SetRidableObjectState
// 0x82feb5e / SendSpawnRidableObject 0x82fece2 / Clear 0x82fe6bc。
#include "CRidable.h"
#include "PacketBuf.h"
#include "PacketGuard.h"

// ORIG ctor 0x82a4406：默认构造 map<int,DungeonRidableObject>@+0x00；
// dtor 0x82a441a：析构该 map。
// ORIG 0x82a4406：仅 map C1（m_flag18 未初始化，字节一致优先）。
#include "CRidable/CRidable.inc"
#include "CRidable/dtor_CRidable.inc"
#include "CRidable/Clear.inc"
}

#include "CRidable/SendSpawnRidableObject.inc"
}

#include "CRidable/SetRidableObjectState.inc"
}
// ORIG 0x082fec2c：遍历对象表，依次 push (m_x, m_y)。
// ORIG 汇编为 pair<int,int>::pair<int&,int&> 模板构造（左值引用实例，非
// make_pair 值传递），故此处用局部左值 + 直接 pair 构造对齐符号集。
#include "CRidable/GetObjectPos.inc"
}
// ORIG 0x82fe9c8：遍历 m_objects；坐标 (m_x,m_y)==(a,b) 且 state==4 或
// (state==3 && m_flag18) 的对象置 state=1、计数并发送生成包；循环后
// p->put_byte(idx, 计数) 收尾。party 形参 ORIG 未使用（保留签名）。
// 成员偏移按 CRidable.h：pair.second 起 +4 → m_x@+4/m_y@+8/m_field0c@+0xc/
// m_field10@+0x10/m_field14@+0x14/m_field18@+0x18/m_state@+0x1c。
// [A28 round5 定稿] 本轮修：①`begin()` 提到 counter 之前（ORIG 0x082fe9d1..de
//   先 begin → counter(-0x1c) → idx(-0x28) → put_byte）；②`p->put_byte(counter)`
//   （ORIG `mov -0x1c(%ebp),%edx` 读槽变量，非立即数 0）。
//   残余 1 条：ORIG `mov 0x18(%eax),%eax; mov %eax,%edi` vs 4.4 折单段
//   `mov 0x18(%eax),%edi`（%edi 直落）⇒ N18a（4.4 单段直落不可造；P254 enum 化
//   需改 CDungeon.h 的 RidableObject（多 TU 共享）且无 ORIG enum 证据，不改）。
//   [A28 round5 · C07 命中形实测 2026-09-10 19:48] 按 Main 转发的「字段读升格
//   callee-saved ⇒ 写具名 int 局部」试一次旋钮：**反劣**（帧 0x3c→0x40、丢
//   `push %edi`、迭代器槽 -0x24→-0x18 整体平移）⇒ 已回滚，N18a 维持（一次旋钮
//   纪律：无改善即回滚封顶）。
//   与 A14 `CPrivateStore::BuyItem` 反例同构：本件的字段读消费者是**被调方的按值
//   实参**（SendSpawnRidableObject 第 5 参），非「call 后仍被本函数复用」⇒ 具名
//   局部必然落槽 +2，符合 C07 命中形的否定边界。
#include "CRidable/CheckAppearRidableObject.inc"
}

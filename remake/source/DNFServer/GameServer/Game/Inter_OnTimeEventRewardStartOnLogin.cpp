// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Inter_OnTimeEventRewardStartOnLogin/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Inter_OnTimeEventRewardStartOnLogin.h"
#include "CUser.h"
#include "PacketGuard.h"

// data 包缓冲的组件视图（P1：组件视图 → 单指令 disp 寻址，匹配 ORIG
// mov 0x12(%eax) / mov 0xa(%eax)；裸指针加偏移会生成 add 两步）
#pragma pack(push, 1)
struct OnTimeEventRewardStartOnLoginView
{
    char m_pad[0xa];   // +0x00
    int m_characNo;    // +0x0a（回发确认包下发角色号）
    char m_pad2[0x4];  // +0x0e
    int m_rewardNo;    // +0x12（与当前角色号比对）
};
#pragma pack(pop)

#include "Inter_OnTimeEventRewardStartOnLogin/dispatch_sig.inc"
}

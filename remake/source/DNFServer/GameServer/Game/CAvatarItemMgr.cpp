// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAvatarItemMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [b28d-solo-CAvatarItemMgr 判词] 本 TU 最终状态（单文件泳道 4 件 + 既有件）：
//  _AssignTempKey 0x082f9e02 NEAR（N95/N82 槽基差：ORIG counter@-0xc vs 4.4 恒 -0x4；13 形探针）
//  UnRegistItem 0x082f9d.. AE（单表达式 find()!=end() 翻求值序 + erase(key)）
//  EraseAvatarItem AE（同上）
//  SaveEndurance 0x082f9f9e AE（迭代器声明序 it 先于 endIt）
//  GetEmblemReduceRate 0x082f9f.. AE（非 const iterator P96/P270 + P222 正极性包裹）
//  UpdateEndurance AE（P222 否定条件块序 + pair 直构 + endIt 具名）
//  RegistItem(iiiPKc…) 0x082f901c AE（早返形 + _FindItem/Acquire 独立变量）
//  RegistNotAssignedAvatarItem(iiPKcb…) 0x082f9xxx AE（SetData 三分支物理内联 P165）
//  AddEmblemEndurance 0x082f9e4c DIFF→N18a 封顶（p167 CLEAR 12/12；本轮 P291 具名数组成员
//   视图消 lea+mov（104 vs 105）、P223 取反 goto 尾标签复现空 then 占位岛；残余 1 条
//   ORIG `mov mem,%eax; mov %eax,%ebx` 中转 = N18a「4.4 恒折单步不可造」+ 装载序 NEAR）
//  Reset 0x082f9ae4 NEAR（p167 CLEAR 25/25；const_iterator 双端转换+去缓存/null 检查；N93 槽向）
//  GetItem 0x082f925c AE（p167 CLEAR 16/16；P13 endIt 具名缓存 + P191 i 双初始化 +
//   P0 去 jewelData 槽内联表达式；槽位 i@-0xc/it@-0x10/end@-0x14 与 ORIG 全等）
// df_game_r Game/ WongWork::CAvatarItemMgr 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 语义字段：m_registeredItems/m_waitAssignItems/m_tempKeyCounter/m_emblemEnduranceMap
//          /m_enduranceUpdateMap。
#include "CAvatarItemMgr.h"
#include "AvatarRechargeServer.h"
#include "GameTypes.h"
#include "CDataManager.h"
#include <cstring>
namespace WongWork
{

// [b28d-38] jewel socket 槽视图：步长 6（+0 short pad、+2 int itemId）。
// ORIG AddEmblemEndurance `mov 0x2(%eax,%ecx,1),%eax`（i*6 与 base 各占寄存器、
// +2 折 disp）实证；裸偏移 `*(int*)(jd + i*6 + 2)` 使 4.4 把 +2 并入索引（多 2 条）。
namespace
{
// [sweep-CAvatarItemMgr] P254 档③：ORIG 第二读为两段 `mov 0x2(...),%eax; mov %eax,%ebx`
// （4.1 对 enum 字段保留转换中转），int 字段下 4.4 恒折单段 `mov ...,%ebx` ⇒ 字段 enum 化。
// enum 底层 4 字节、packed 布局不变（sizeof(JewelSocketSlot) 仍 6）。
enum JewelSocketItemId
{
    JEWEL_SOCKET_ITEM_ID_NONE = 0
};

struct JewelSocketSlot
{
    short m_pad;
    JewelSocketItemId m_itemId;
} __attribute__((packed));

// [b28d-solo-CAvatarItemMgr] P291：具名数组成员视图使 4.4 把 base+i*6+2 折叠为
// 单条 `mov 0x2(%eax,%ecx,1),%eax`（指针下标形恒折 lea+mov，多 1 条）。
struct JewelSocketData
{
    JewelSocketSlot m_slots[5];
} __attribute__((packed));
}

#include "CAvatarItemMgr/m_AvatarItemPool.inc"

// ORIG 布局（nm/报告实证）：m_registeredItems(+0x00)/m_waitAssignItems(+0x18)
// /m_tempKeyCounter(+0x30)/m_emblemInfo(+0x34)/m_emblemEnduranceMap(+0x54)
// /m_enduranceUpdateMap(+0x6c)，总 0x84（CInventory.h:783 权威注释）。
static_assert(sizeof(CAvatarItemMgr) == 0x84, "CAvatarItemMgr ABI size changed");

#include "CAvatarItemMgr/dtor_CAvatarItemMgr.inc"


#include "CAvatarItemMgr/Reset.inc"


#include "CAvatarItemMgr/IsTempKey.inc"

#include "CAvatarItemMgr/_MakeTempKey.inc"

#include "CAvatarItemMgr/_AssignTempKey.inc"


#include "CAvatarItemMgr/_FindItem.inc"


#include "CAvatarItemMgr/_FindWaitAssignItem.inc"


#include "CAvatarItemMgr/_FindItemAll.inc"


#include "CAvatarItemMgr/AddAvatarItem.inc"


#include "CAvatarItemMgr/AddEmblemEndurance.inc"


#include "CAvatarItemMgr/AssignAvatarItemInfo.inc"


#include "CAvatarItemMgr/AssignAvatarItemKey.inc"


#include "CAvatarItemMgr/RegistNotAssignedAvatarItem.inc"


#include "CAvatarItemMgr/RegistNotAssignedAvatarItem_1.inc"


#include "CAvatarItemMgr/RegistItem.inc"


#include "CAvatarItemMgr/RegistItem_1.inc"


#include "CAvatarItemMgr/GetExpireDate.inc"


#include "CAvatarItemMgr/GetRegistTime.inc"


#include "CAvatarItemMgr/GetIPGAgencyNo.inc"


#include "CAvatarItemMgr/GetRemainDate.inc"


#include "CAvatarItemMgr/GetEmblemReduceRate.inc"


#include "CAvatarItemMgr/getJewelSocketData.inc"


#include "CAvatarItemMgr/GetExpansionInfo.inc"


#include "CAvatarItemMgr/SetAvatarColor.inc"


#include "CAvatarItemMgr/GetAvatarInfo.inc"


#include "CAvatarItemMgr/GetItem.inc"


#include "CAvatarItemMgr/UnRegistItem.inc"


#include "CAvatarItemMgr/EraseAvatarItem.inc"


#include "CAvatarItemMgr/isSocketAvatar.inc"


#include "CAvatarItemMgr/setEmblemSocketData.inc"


#include "CAvatarItemMgr/UpdateEndurance.inc"


#include "CAvatarItemMgr/SaveEndurance.inc"


#include "CAvatarItemMgr/updateEmblemSocket.inc"


}  // namespace WongWork

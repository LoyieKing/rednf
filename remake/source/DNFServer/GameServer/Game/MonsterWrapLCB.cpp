// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MonsterWrapLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "MonsterIndependentDrop.h"

#include "CItemList.h"   // CItemList::create_item / ENUM_ITEM_CREATE_TYPE
#include "CMonster.h"   // CMonster::getMonsterScript / STMonsterScript 完整类型

#include "MonsterWrapLCB/generateDropAvatarItem.inc"
                            unsigned char rarity, unsigned char grade,
                            std::vector<Inven_Item>& out);   // ORIG 0x08349da0
#include "MonsterWrapLCB/generateCatchMonsterItem.inc"
                              std::vector<Inven_Item>& out);  // ORIG 0x08349f4b

#include "MonsterWrapLCB/IsHellMonster.inc"
{
    // [b28c-35] ORIG 0x08539de8 为单条 `movzbl 0x70(%eax),%eax`；裸偏移算术链
    // （reinterpret_cast + 0x70）4.4 发 `add $0x70,%eax; movzbl (%eax)`（+1 条）。
    // 改具名尾部视图成员访问（P82）即折叠单条 disp；视图只读该字节，不改类布局。
    struct Tail { char m_pad[0x70]; bool m_flag; };
    return reinterpret_cast<const Tail*>(this)->m_flag;
}

#include "MonsterWrapLCB/generateDropAvatarItem_1.inc"


#include "MonsterWrapLCB/generateDropAvatarItem_2.inc"


#include "MonsterWrapLCB/generateCatchMonsterItem_1.inc"


#include "MonsterWrapLCB/generateDropAvatarItem_3.inc"


#include "MonsterWrapLCB/generateCatchMonsterItem_2.inc"


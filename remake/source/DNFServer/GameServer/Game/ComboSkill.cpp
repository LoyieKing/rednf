// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ComboSkill/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN10ComboSkill9resetTreeE20ENUM_SKILL_TREE_KIND`（ORIG 0x80ea3c7）。
#include <iostream>


#include <utility>

#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"



// G_CDataManager() 返回对象 +0x14 处为 char*（组合技列表基址），成员访问形态
// 与 ORIG 一致（mov 0x14(%eax),%eax；而非 add+mov）。
struct CDataManagerComboLayout
{
    char m_pad[0x14];
    char* m_pComboList;  // +0x14
};

#include "ComboSkill/resetTree.inc"


#include "ComboSkill/ComboSkill.inc"


#include "ComboSkill/ComboSkill_1.inc"


#include "ComboSkill/dtor_ComboSkill.inc"


ComboSkill& ComboSkill::operator=(const ComboSkill& other)
{
    m_comboSkillMap = other.m_comboSkillMap;
    m_comboSkillSfpMap = other.m_comboSkillSfpMap;
    return *this;
}

#include "ComboSkill/convertComboSkillMap.inc"


#include "ComboSkill/convertComboSkillArray.inc"


#include "ComboSkill/loadData.inc"


#include "ComboSkill/saveData.inc"


#include "ComboSkill/getComboSkillSendList.inc"


#include "ComboSkill/getComboSkillSendForm.inc"


#include "ComboSkill/setComboSkill.inc"


#include "ComboSkill/eraseDuplicationComboSKill.inc"


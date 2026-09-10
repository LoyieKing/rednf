// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PremiumServiceData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "PremiumServiceData.h"

// 0x0891bb5e：pair/string/vector/map 按声明序默认构造；函数体赋默认值
//   （+0x00=0、pair{-1,-1}、+0x0c=-1、+0x10=-1、string=""、vector.clear()）。
#include "PremiumServiceData/PremiumServiceData.inc"
{
    m_type = 0;
    m_mainImageIndex.first = -1;
    m_mainImageIndex.second = -1;
    m_additionalImageIndex = -1;
    m_iconIndex = -1;
    m_iconExplain = "";
    m_addEquipmentList.clear();
}

// 0x0891bc4a：与 C1 相同的默认值，另清空 map。
#include "PremiumServiceData/clear.inc"
{
    m_type = 0;
    m_mainImageIndex.first = -1;
    m_mainImageIndex.second = -1;
    m_additionalImageIndex = -1;
    m_iconIndex = -1;
    m_iconExplain = "";
    m_addEquipmentList.clear();
    m_selectAbleEquipmentList.clear();
}

// 0x0891ca96：析构体为空，成员按逆声明序（map→vector→string）由编译器生成。
#include "PremiumServiceData/dtor_PremiumServiceData.inc"
{
}

// 0x089317a0：POD 成员（int/pair）直接拷贝，string/vector/map 用拷贝构造。
#include "PremiumServiceData/PremiumServiceData_1.inc"
    : m_type(rhs.m_type),
      m_mainImageIndex(rhs.m_mainImageIndex),
      m_additionalImageIndex(rhs.m_additionalImageIndex),
      m_iconIndex(rhs.m_iconIndex),
      m_iconExplain(rhs.m_iconExplain),
      m_addEquipmentList(rhs.m_addEquipmentList),
      m_selectAbleEquipmentList(rhs.m_selectAbleEquipmentList)
{
}

// 0x089318e8：逐成员赋值，string/vector/map 用 operator=，返回 *this。
PremiumServiceData& PremiumServiceData::operator=(PremiumServiceData const& rhs)
{
    m_type = rhs.m_type;
    m_mainImageIndex = rhs.m_mainImageIndex;
    m_additionalImageIndex = rhs.m_additionalImageIndex;
    m_iconIndex = rhs.m_iconIndex;
    m_iconExplain = rhs.m_iconExplain;
    m_addEquipmentList = rhs.m_addEquipmentList;
    m_selectAbleEquipmentList = rhs.m_selectAbleEquipmentList;
    return *this;
}

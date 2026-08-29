// ============================================================================
// df_game_r 还原 —— PremiumServiceData（OrigPremiumData 批独占）
// 5 个用户定义成员体（ORIG 均 W 弱符号，定义于 RDARScriptEtcParameter 域 TU）：
//   C1/C2 0x0891bb5e、C1RKS/C2RKS 0x089317a0、D1/D2 0x0891ca96、
//   aS 0x089318e8、clear 0x0891bc4a。
// 转写依据 objdump（报告缺失，§3.2 兜底），字段布局见 PremiumServiceData.h。
// 说明：C1/clear 中 +0x14 字符串赋值为空串（ORIG 传 0x8e0f3c0，首字节 0x00）。
// ============================================================================

#include "PremiumServiceData.h"

// 0x0891bb5e：pair/string/vector/map 按声明序默认构造；函数体赋默认值
//   （+0x00=0、pair{-1,-1}、+0x0c=-1、+0x10=-1、string=""、vector.clear()）。
PremiumServiceData::PremiumServiceData()
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
void PremiumServiceData::clear()
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
PremiumServiceData::~PremiumServiceData()
{
}

// 0x089317a0：POD 成员（int/pair）直接拷贝，string/vector/map 用拷贝构造。
PremiumServiceData::PremiumServiceData(PremiumServiceData const& rhs)
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

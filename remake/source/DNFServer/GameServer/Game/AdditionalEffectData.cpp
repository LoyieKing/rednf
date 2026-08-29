// AdditionalEffectData 实现 —— ORIG W 符号转写（报告缺失，objdump 定址实证）。
// 布局与字段见 AdditionalEffectData.h；均为非虚成员（无 vptr 操作）。
#include "AdditionalEffectData.h"

// ORIG C1 0x0891b05e：vector 默认构造 → m_id=-1 → vector clear →
// m_mode=2 → m_count=0 → m_max=-1 → m_min=-1 → m_valueA/B/C=0 → m_flag=0。
AdditionalEffectData::AdditionalEffectData()
    : m_strings()
{
    m_id = -1;
    m_strings.clear();
    m_mode = 2;
    m_count = 0;
    m_max = -1;
    m_min = -1;
    m_valueA = 0;
    m_valueB = 0;
    m_valueC = 0;
    m_flag = 0;
}

// ORIG C1ERKSs 0x08929074：初始化列表 m_id 先行、vector 拷贝构造次之
// （ORIG 汇编 m_id@+0 拷贝在 vector C1ERKS1 之前），其余标量函数体逐句赋值。
AdditionalEffectData::AdditionalEffectData(AdditionalEffectData const& other)
    : m_id(other.m_id), m_strings(other.m_strings)
{
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
}

// ORIG D1 0x0891c73e：非虚用户析构（无 vptr 重置），仅析构 vector 成员。
AdditionalEffectData::~AdditionalEffectData()
{
}

// ORIG 0x0891c754：逐字段赋值，返回 *this（ORIG 尾部 mov this,%eax）。
AdditionalEffectData& AdditionalEffectData::operator=(AdditionalEffectData const& other)
{
    m_id = other.m_id;
    m_strings = other.m_strings;
    m_mode = other.m_mode;
    m_count = other.m_count;
    m_min = other.m_min;
    m_max = other.m_max;
    m_valueA = other.m_valueA;
    m_valueB = other.m_valueB;
    m_valueC = other.m_valueC;
    m_flag = other.m_flag;
    return *this;
}

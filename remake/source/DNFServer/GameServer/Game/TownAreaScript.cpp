// ============================================================================
// TownAreaScript（G2 Village/区域批次）——按 ORIG 反汇编真实实现。
//   C1Ev 0x86d1d9a：构造 m_mapName/m_rects/m_field18 后调 clear()。
//   D1Ev 0x86d28f6：逆序析构 m_field18/m_rects/m_mapName。
//   C1ERKS_ 0x86d296a：逐成员拷贝。
//   aSERKS_ 0x86d2a1a：逐成员赋值。
//   clear 0x86d1d46：m_areaIndex=-1、m_mapName=""、m_areaType=0、
//                    两 vector 清空、m_field24=1。
// Village.cpp / Area.cpp 共用 TownAreaScript.h（唯一声明点，sizeof 0x30）。
// ============================================================================

#include "TownAreaScript.h"

TownAreaScript::TownAreaScript()
    : m_areaIndex(0), m_mapName(), m_areaType(0), m_rects(), m_field18(),
      m_field24(0)
{
    clear();
}

TownAreaScript::TownAreaScript(const TownAreaScript& other)
    : m_areaIndex(other.m_areaIndex),
      m_mapName(other.m_mapName),
      m_areaType(other.m_areaType),
      m_rects(other.m_rects),
      m_field18(other.m_field18),
      m_field24(other.m_field24)
{
    for (int i = 0; i < (int)sizeof(m_pad25); ++i)
        m_pad25[i] = other.m_pad25[i];
}

TownAreaScript& TownAreaScript::operator=(const TownAreaScript& other)
{
    if (this != &other)
    {
        m_areaIndex = other.m_areaIndex;
        m_mapName = other.m_mapName;
        m_areaType = other.m_areaType;
        m_rects = other.m_rects;
        m_field18 = other.m_field18;
        m_field24 = other.m_field24;
        for (int i = 0; i < (int)sizeof(m_pad25); ++i)
            m_pad25[i] = other.m_pad25[i];
    }
    return *this;
}

TownAreaScript::~TownAreaScript()
{
}

void TownAreaScript::clear()
{
    m_areaIndex = -1;
    m_mapName = "";
    m_areaType = 0;
    m_rects.clear();
    m_field18.clear();
    m_field24 = 1;
}

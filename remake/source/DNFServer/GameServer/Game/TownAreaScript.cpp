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

// [b28-L14] ORIG C1Ev 0x86d1d9a 只构造 m_mapName/m_rects/m_field18 后调 clear()
// （无标量清零）；C1ERKS_/aSERKS_ 为 W 隐式合成（P74），用户定义已删。
TownAreaScript::TownAreaScript()
{
    clear();
}

// [b28-L14] P74/P26：ORIG C1ERKS_ 0x86d296a / aSERKS_ 0x86d2a1a 是编译器隐式合成
// （逐成员、跳过尾部对齐空洞），`= default` 外联定义既保持 W 符号发射又不再搬 pad。
TownAreaScript::TownAreaScript(const TownAreaScript& other) = default;
TownAreaScript& TownAreaScript::operator=(const TownAreaScript& other) = default;

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

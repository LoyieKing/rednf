// df_game_r Game/ CRidable 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 CRidable.h：m_objects 为 map<int,DungeonRidableObject> 占位。
// 这两个方法 ORIG 逻辑依赖完整 DungeonRidableObject 结构（+4 x/+8 y/+0x1c state）
// 与 SendSpawnRidableObject 发包，当前头为占位类型，按任务约定保留为
// 签名匹配的空体桩（符号 T），后续批次补全语义。
#include "CRidable.h"

// ORIG ctor 0x82a4406：默认构造 map<int,DungeonRidableObject>@+0x00；
// dtor 0x82a441a：析构该 map。
CRidable::CRidable() : m_objects() {}
CRidable::~CRidable() {}

// ORIG Clear 0x82fe6bc：直接调 m_objects.clear()。
void CRidable::Clear()
{
    m_objects.clear();
}

void CRidable::CheckAppearRidableObject(CParty* /*party*/, int /*a*/, int /*b*/,
                                        PacketGuard* /*p*/)
{
}

void CRidable::SetRidableObjectState(int /*idx*/, eRidableObjectState /*state*/)
{
}
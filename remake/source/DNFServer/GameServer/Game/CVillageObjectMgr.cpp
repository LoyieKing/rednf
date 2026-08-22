// CVillageObjectMgr.cpp
#include "CVillageObjectMgr.h"

namespace village_object
{
Zone::Zone(unsigned char a, unsigned char b)
    : m_a(a)
    , m_b(b)
{
}

void CVillageObjectMgr::on_move_area(class CUser*)
{
}

void CVillageObjectMgr::register_object(village_object::Zone, village_object::IObject*)
{
}

}  // namespace village_object
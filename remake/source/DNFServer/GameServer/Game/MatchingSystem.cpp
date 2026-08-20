#include <cstring>

#include "PvP_deps.h"

CMatchingSystem::CMatchingSystem()
{
    std::memset(m_pad, 0, sizeof(m_pad));
}

CMatchingSystem::~CMatchingSystem() {}

IMatchingSystem* CMatchingSystem::GetMatchingSystem(ENUM_PVP_MATCHING_TYPE)
{
    return 0;
}

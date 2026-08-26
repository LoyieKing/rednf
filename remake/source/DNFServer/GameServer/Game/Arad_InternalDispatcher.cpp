// df_game_r Game/ ARAD::DISPATCHER 内部流派发器体系实现。
// 依据 docs/class_func_reports/ARAD__DISPATCHER__*/ 各报告 + ORIG nm/objdump。
#include "Arad_InternalDispatcher.h"
#include "CStreamGuard.h"
#include <algorithm>

namespace ARAD
{

namespace DISPATCHER
{

// ============================================================================
// make_internal_stream_jpn —— ORIG 0x081977b6 T（ARAD__DISPATCHER/make_internal_stream_jpn.md）
// guard->0x2dc(魔数) → param → code；写入序按报告 Ghidra C 还原。
// ============================================================================
void make_internal_stream_jpn(CStreamGuard& guard, ENUM_INTERNALPACKET_JPN code, int param)
{
    guard.operator*()->operator<<((int)0x2dc);
    guard.operator*()->operator<<((int)param);
    guard.operator*()->operator<<((int)code);
}

// ============================================================================
// Arad_IInterDispatcher —— ORIG ctor 0x0819a36a / D1 0x0819a382 / get_code 0x819a3c6
// ============================================================================
Arad_IInterDispatcher::Arad_IInterDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    m_code = code;
}

Arad_IInterDispatcher::~Arad_IInterDispatcher()
{
}

// ============================================================================
// Arad_IDatabaseDispatcher —— ORIG ctor 0x081867a4 / D1 0x081867bc / get_code 0x8186800
// ============================================================================
Arad_IDatabaseDispatcher::Arad_IDatabaseDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    m_code = code;
}

Arad_IDatabaseDispatcher::~Arad_IDatabaseDispatcher()
{
}

// ============================================================================
// Arad_DatabaseDispatcher —— 容器（ARAD__DISPATCHER__Arad_DatabaseDispatcher 各报告）
// ============================================================================

// ORIG ctor 0x08184c7c：map C1 + map::clear（无显式逻辑）。
Arad_DatabaseDispatcher::Arad_DatabaseDispatcher() {}

// ORIG ~D1 0x08184cbe：for_each(begin,end,DeleteDispatcher 删除各派发器) 后析构 map。
Arad_DatabaseDispatcher::~Arad_DatabaseDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
}

// ORIG 0x081850e0：find(code)；!=(end) 返回 second，否则 NULL。
Arad_IDatabaseDispatcher* Arad_DatabaseDispatcher::GetDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>::iterator it = m_dispatchers.find(code);
    if (it != m_dispatchers.end())
        return it->second;
    return 0;
}

// ORIG 0x08185144：GetDispatcher 为空则 map[code]=d；返回"之前不存在"。
bool Arad_DatabaseDispatcher::_registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IDatabaseDispatcher* d)
{
    // ORIG 0x08185144：已存在则立即 false 早退；否则注册并 true。
    if (GetDispatcher(code) != 0)
        return false;
    m_dispatchers[code] = d;
    return true;
}


// ORIG 0x0818518a：不存在则直接返回；命中则 erase。
// ORIG 0x0818518a 命中路径：erase(find(code))。
void Arad_DatabaseDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    if (GetDispatcher(code) == 0)
        return;
    m_dispatchers.erase(code);
}

// ORIG 0x0818680c W：delete pair.second。
void Arad_DatabaseDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>& p)
{
    delete p.second;
}

// ============================================================================
// Arad_InternalDispatcher —— 容器（与 Database 同构；ORIG 0x081984d4 起）
// ============================================================================
Arad_DatabaseDispatcher::Arad_DatabaseDispatcher()
{
    // ORIG 0x08184c7c：map C1 后跟一次显式 map::clear。
    m_dispatchers.clear();
}

Arad_InternalDispatcher::~Arad_InternalDispatcher()
{
    std::for_each(m_dispatchers.begin(), m_dispatchers.end(), DeleteDispatcher());
}

Arad_IInterDispatcher* Arad_InternalDispatcher::GetDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>::iterator it = m_dispatchers.find(code);
    if (it != m_dispatchers.end())
        return it->second;
    return 0;
}

bool Arad_InternalDispatcher::_registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IInterDispatcher* d)
{
    // ORIG 0x08198a02：已存在则立即 false 早退；否则注册并 true。
    if (GetDispatcher(code) != 0)
        return false;
    m_dispatchers[code] = d;
    return true;
}

void Arad_InternalDispatcher::_unregistDispatcher(ENUM_INTERNALPACKET_JPN code)
{
    // ORIG 0x08198a48：不存在则直接返回；命中则 erase。
        m_dispatchers.erase(m_dispatchers.find(code));
}

void Arad_InternalDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>& p)
{
    delete p.second;
}

}  // namespace DISPATCHER
}  // namespace ARAD

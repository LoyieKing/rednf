// ============================================================================
// df_game_r 还原 —— STNameList（字符串名 -> ID 映射表，见 STNameList.md）
//   STNameList ctor 0x088baef2（调 m_map ctor + Clear）
//   Clear         0x088baf34（m_map.clear()）
//   GetID         0x088baf48（m_map.get(name)，未命中 0xffffffff）
// ============================================================================

#include <cstring>
#include "STNameList.h"

STNameList::STNameList()
{
    Clear();
}

void STNameList::Clear()
{
    m_map.clear();
}

int STNameList::GetID(const char* name) const
{
    // ORIG const GetID 仍调非 const 的 StringUnorderedMap::get(char const*)；
    // 若直接 m_map.get(name) 会选 const 重载（_ZNK...），callee 不符，故去 const。
    const int* p =
        const_cast<StringUnorderedMap<CharStringHashTrait, int>&>(m_map).get(name);
    if (p == 0)
        return 0xffffffff;
    return *p;
}

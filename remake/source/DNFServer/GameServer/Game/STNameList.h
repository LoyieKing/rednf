#ifndef GAME_STNAMELIST_H_
#define GAME_STNAMELIST_H_

// ============================================================================
// df_game_r 还原 —— STNameList（字符串名 -> ID 的映射表）
// 布局依据 ORIG objdump（STNameList.md）：
//   ctor 0x088baef2：构造 StringUnorderedMap<CharStringHashTrait,int> 后调 Clear；
//   Clear 0x088baf34：直接调 m_map.clear()；
//   GetID 0x088baf48（const）：m_map.get(name)，命中取 *ret，未命中返回 0xffffffff。
// 成员：+0x00 StringUnorderedMap<CharStringHashTrait,int>。
// StringUnorderedMap<CharStringHashTrait,int> 显式实例化见 StringTypes.cpp。
// ============================================================================

#include "HashMap.h"

struct CharStringHashTrait;

class STNameList
{
public:
    STNameList();

    void Clear();
    // 返回 name 对应 ID；未命中返回 0xffffffff。
    int GetID(const char* name) const;

private:
    StringUnorderedMap<CharStringHashTrait, int> m_map;  // +0x00
};

#endif  // GAME_STNAMELIST_H_
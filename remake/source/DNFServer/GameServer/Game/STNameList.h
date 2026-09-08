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


class STNameList
{
public:
    STNameList();

    void Clear();
    // 返回 name 对应 ID；未命中返回 0xffffffff。
    int GetID(const char* name) const;
    // loadRDARNameFile（0x88bc068）逐条插入 name->id。
    void Insert(const char* name, int id) { m_map.insert(name, id); }
    // 是否为空（ORIG 尾部以 begin()==end() 判断）。
    bool IsEmpty() { return m_map.begin() == m_map.end(); }

    // [b28d-lane16] m_map 公开：ORIG loadRDARNameFile(0x088bc068) 直接调
    // StringUnorderedMap::insert(PKc,RKi) 并用其返回值判重复（ORIG nm 无
    // STNameList::Insert 符号）。sizeof/偏移/mangled 不变，仅访问控制。
    StringUnorderedMap<CharStringHashTrait, int> m_map;  // +0x00
};

#endif  // GAME_STNAMELIST_H_
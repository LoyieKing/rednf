// CSuccessJobNode：CDungeonSuccessNode::m_jobMap 值类型（0x58 字节）。
// 布局证据（objdump + 报告 docs/class_func_reports/CSuccessJobNode/）：
//   +0x00 map<int,JobParty> m_jobPartyMap（0x18；ctor 0x08484c30 预插 job 2..4）
//   +0x18 Character m_charac（0x40；ORIG ctor 0x08484c30 调 Character::Character，
//     dtor 0x08486c5a 调 Character::~Character（_ZN9CharacterD1Ev），
//     copy ctor 0x0848757e 复制 0x18..0x54 恰 0x40 字节。
//     CUser.h Character 大小已修为 0x40（与 PlayInfo 同布局），故本成员用 Character。）
// 方法：ctor T 0x08484c30 / copy W 0x0848757e / dtor W 0x08486c5a（非虚，无 vtable，P39）/
// InsertDB T 0x08486234 / UpdateCharacter T 0x08484d4c。
#ifndef DNF_GAME_CSUCCESSJOBNODE_H_
#define DNF_GAME_CSUCCESSJOBNODE_H_

#include <map>

#include "IData.h"
#include "JobParty.h"
#include "JobKey.h"
#include "CUser.h"   // PlayInfo（只读，冻结头）

class DungeonKey;
class SIG_DUNGEON_DATA_STATISTIC;

class CSuccessJobNode
{
public:
    CSuccessJobNode();                                     // T 0x08484c30
    CSuccessJobNode(const CSuccessJobNode& other);         // W 0x0848757e
    ~CSuccessJobNode();                                    // W 0x08486c5a（非虚）
    int UpdateCharacter(SIG_DUNGEON_DATA_STATISTIC*);      // T 0x08484d4c
    int InsertDB(const DungeonKey& key, bool flag, const JobKey& jobKey);  // T 0x08486234

    std::map<int, JobParty> m_jobPartyMap;   // +0x00（0x18）
    Character m_charac;                      // +0x18（0x40）
};
// sizeof 0x58

inline CSuccessJobNode::CSuccessJobNode(const CSuccessJobNode& other)
    : m_jobPartyMap(other.m_jobPartyMap), m_charac(other.m_charac)
{
}

inline CSuccessJobNode::~CSuccessJobNode()
{
}

#endif // DNF_GAME_CSUCCESSJOBNODE_H_

#ifndef GAME_JOBPARTY_H_
#define GAME_JOBPARTY_H_

// JobParty：CSuccessJobNode 的 map<int,JobParty> 值类型（4 字节，IData EBO）。
// 布局证据：ctor W 0x08486a32 = IData::C1 + Reset；Reset W 0x08486a24（清 +0 单 int）；
// 无 dtor 符号（trivial）。CSuccessJobNode::UpdateCharacter 0x08484d4c：+0 自增
// （party_user_count）。
#include "IData.h"

class JobParty : public IData
{
public:
    JobParty() : IData()   // ORIG W 0x08486a32
    {
        Reset();
    }

    void Reset()           // ORIG W 0x08486a24
    {
        m_count = 0;
    }

    int m_count;           // +0x00
};
// sizeof 0x04

#endif  // GAME_JOBPARTY_H_

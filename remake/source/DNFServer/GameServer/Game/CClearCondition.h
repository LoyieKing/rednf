#ifndef DNF_GAME_CCLEARCONDITION_H_
#define DNF_GAME_CCLEARCONDITION_H_

// G2 占位（TODO：真实实现属后续批次）。
// 布局依据 ORIG：ctor 0x82a4342（vector<int> @ +8 + Clear），
// Clear 0x82a438a（+0=0、+4=0、vec.clear、+0x14=0）。
// 尺寸 0x18：CBattle_Field 成员 @ +0xf0..+0x108（+0x108 为 CMap* m_pMap）。

#include <vector>

enum ENUM_DUNGEON_CLEAR_CONDITION_TYPE
{
    ENUM_DUNGEON_CLEAR_CONDITION_TYPE_0 = 0
};

// 真布局属 CMap 批（脚本条件表项）；本批只传递 vector 引用，无需精确尺寸。
struct DungeonClearCondition
{
    char m_pad[0x28];
};

class CClearCondition
{
public:
    CClearCondition();                       // ORIG 0x82a4342
    ~CClearCondition();                      // ORIG 0x82a442e

    void Clear();                            // ORIG 0x82a438a
    bool Empty() const;                      // ORIG 0x822cba8
    bool IsClearCondition() const;           // ORIG 0x822cb92
    bool ClearCondition(ENUM_DUNGEON_CLEAR_CONDITION_TYPE type, int idx);  // ORIG 0x82fefce
    void SetDungeonClearConditionVector(
        std::vector<DungeonClearCondition>& vec);  // ORIG 0x82fef12

private:
    int m_flag;                // +0x00
    int m_count;               // +0x04
    std::vector<int> m_vec;    // +0x08
    int m_unk14;               // +0x14
};

#endif  // DNF_GAME_CCLEARCONDITION_H_

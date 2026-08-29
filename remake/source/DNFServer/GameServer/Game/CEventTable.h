#ifndef GAME_CEVENTTABLE_H_
#define GAME_CEVENTTABLE_H_

#include <vector>

class CEventBase;

// ---------------------------------------------------------------------------
// df_game_r —— CEventTable 权威声明（唯一）。
// 方法全部 static（ORIG 0x811a0d4 起仅压一个参数）；成员函数定义在
// CEventManager.cpp。原 CEventManager.cpp / GameMain.cpp 的两份逐字相同
// 局部声明已合并至此。
// ---------------------------------------------------------------------------
class CEventTable
{
public:
    CEventTable();                                       // 0x0811a0c8（空）
    ~CEventTable();                                      // 0x0811a0ce（空）
    static int GetEventTableCount();                     // 0x0811a0d4
    static CEventBase* GetEventBase(int idx);            // 0x0811a0de
    static int GetEventId(int idx);                      // 0x0811a11e
    static bool VerifyEventTable(std::vector<int>& out); // 0x0811a1e0
    static bool InsertEventTable(std::vector<int>& ids); // 0x0811a4d4
};

#endif  // GAME_CEVENTTABLE_H_

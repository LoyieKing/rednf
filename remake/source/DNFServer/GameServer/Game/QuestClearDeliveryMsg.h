#ifndef DNF_GAME_QUESTCLEARDELIVERYMSG_H_
#define DNF_GAME_QUESTCLEARDELIVERYMSG_H_

#include <vector>
#include <utility>
#include "CGuildServerProxy.h"   // DeliveryMsg 基类（0x086978c0/0x086978ec）

// QuestClearDeliveryMsg（ORIG W C1/C2 0x08697914，size 0x45；LinkFinal 批按 ctor
// 逐字段建模）。基类 DeliveryMsg（CGuildServerProxy.h，0x10 字节）后五个 int：
class QuestClearDeliveryMsg : public DeliveryMsg
{
public:
    QuestClearDeliveryMsg();   // ORIG W 0x08697914：基类构造后清零 +0x10..+0x24

    int m_questParamA;         // +0x10（CUser::eventDeliveryQuestClear ← ebp+0xc）
    int m_paramD;              // +0x14（← ebp+0x18）
    int m_paramE;              // +0x18（← ebp+0x1c）
    int m_paramF;              // +0x1c（← ebp+0x20）
    std::vector<std::pair<int, int> >* m_itemsPtr;   // +0x20（← ebp+0x24）
};

#endif  // DNF_GAME_QUESTCLEARDELIVERYMSG_H_

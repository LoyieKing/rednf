#ifndef GAME_EVENTCLASSIFY_ACTION_H_
#define GAME_EVENTCLASSIFY_ACTION_H_

// df_game_r 还原 —— EventClassify 事件发奖/发信域类型。
// 供 CEventActionMng 及各事件实体（CLevelUpReward/CAttendance/CAccountFirstLogin 等）
// 的 mail-data（Action_SendMail）与发奖流程使用。布局由
// docs/class_func_reports/EventClassify__CEventActionMng/ 下各 process_* 反编译推导。

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "InGameEventScripts.h"   // ::Action_SendMail 权威全局定义（ORIG
                                  // mangled R15Action_SendMail 为全局作用域）
class CUser;
class Inven_Item;
class CItem;
namespace EventClassify
{
// CEventActionMng —— 事件动作管理器。process_action_send_mail（0x0810cf32）为
// 发奖核心入口，内部按 mail 内容分派 process_coin_send/process_gold_mail_send/
// process_avatar_send/process_mail_send。其余方法见
// EventClassify__CEventActionMng/README（getResultItemFromLottery 等）。
class CEventActionMng
{
public:
    int process_action_send_mail(CUser* user, Action_SendMail& mail, bool flag);  // 0x0810cf32 T
    int process_coin_send(CUser* user, Action_SendMail& mail, Inven_Item& item);   // 0x0810d4ae
    int process_gold_mail_send(CUser* user, Action_SendMail& mail);                // 0x0810d83c
    int getResultItemFromLottery(unsigned int lotteryId,
                                 std::vector<std::pair<int, int> >& out);  // 0x0810dc5a
    int process_avatar_send(CUser* user, Inven_Item& item, CItem const* citem);    // 0x0810d70c
    int process_mail_send(CUser* user, Action_SendMail& mail, Inven_Item& item,
                          CItem const* citem);                                     // 0x0810da1c
};

}  // namespace EventClassify

#endif  // GAME_EVENTCLASSIFY_ACTION_H_

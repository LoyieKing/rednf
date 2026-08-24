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

class CUser;
class Inven_Item;
class CItem;

namespace EventClassify
{

// Action_SendMail —— 事件系统邮件发送数据（process_action_send_mail 读取）。
// 布局（process_action_send_mail / process_coin_send / process_mail_send 反编译推断）：
//   +0x00 std::map<int,std::pair<int,int>> m_jobMap（按职业查 itemIdx/addInfo）
//   +0x18 int m_field18                          （ORIG 有 4 字节区，字段从 +0x1c 起）
//   +0x1c unsigned char m_field1c（0=走发信，非0=走 coin/avatar 直发）
//   +0x20 int m_itemIdx          （物品代码）
//   +0x24 int m_addInfo
//   +0x28 int m_gold             （>0 走 process_gold_mail_send）
//   +0x2c int m_coin             （process_coin_send 金币量 / (char) 传 ReqDB）
//   +0x30 std::string m_field30  （收件人/发件人 key，经 g_scriptStringManager_ 解析）
//   +0x34 std::string m_field34  （标题 key）
//   +0x38 std::vector<int> m_vec38（发奖记录，process_action_send_mail 尾 push_back）
//   sizeof = 0x44。
struct Action_SendMail
{
    std::map<int, std::pair<int, int> > m_jobMap;  // +0x00
    int m_field18;                                  // +0x18
    unsigned char m_field1c;                        // +0x1c
    int m_itemIdx;                                  // +0x20
    int m_addInfo;                                  // +0x24
    int m_gold;                                     // +0x28
    int m_coin;                                     // +0x2c
    std::string m_field30;                          // +0x30
    std::string m_field34;                          // +0x34
    std::vector<int> m_vec38;                       // +0x38
};

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
    int process_avatar_send(CUser* user, Inven_Item& item, CItem const* citem);    // 0x0810d70c
    int process_mail_send(CUser* user, Action_SendMail& mail, Inven_Item& item,
                          CItem const* citem);                                     // 0x0810da1c
};

}  // namespace EventClassify

#endif  // GAME_EVENTCLASSIFY_ACTION_H_

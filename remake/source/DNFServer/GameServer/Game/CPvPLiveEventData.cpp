// df_game_r 还原 —— CPvPLiveEventData::IncreasePlayCount（ORIG 0x826921c）。
// 其余方法在本 TU 复用真实成员布局；loadData 只恢复缓冲区首 dword。
#include "CPvPLiveEventData.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

#include <string.h>
#include <string>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "ServerParameterScript.h"
#include "RDARScriptStringManager.h"
#include "CGameManager.h"      // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail

extern CDataManager* G_CDataManager();

// [b28d-20] ORIG 0x0826921c 逐条对齐：①无 dm/字符串指针/group/characNo 具名局部（P0：
// G_CDataManager() 每消费点重发；findString 结果直传 strncpy 实参）；②计数比较用正向包裹
// `if (m_playCount == GetPlayCount()) { 主体 }` 复现 ORIG sete+je 物化（负形 setne+jne 反极性）；
// ③迭代器解引用写 `(*it).first/.second`（ORIG 两处 deEv，非 ptEv）；④find_item 结果具名
// CItem* found（ORIG -0xc 槽往返）；⑤Inven_Item::m_addInfo 写 itemIdx（ORIG mov %eax,-0x6c(%ebp)）；
// ⑥奖励判定正向包裹 `if (item.m_addInfo != 0 || item.get_add_info() != 0)` 复现 ORIG
// jne settrue / je false 的 OR 物化；⑦strncpy 后无显式 '\0'（ORIG 无该写）。
// [b28d-solo-CPvPLiveEventData] verdict=IDENTICAL/AE（ORIG 160 = OURS 160，逐条助记符全等，
// p167 调用点差集 CLEAR）；本波修法：itemIdx/addInfo 由 int 改 unsigned int（P290 实参升格
// callee-saved 跨 G_CDataManager() 保活），复现 ORIG 0x082692c4/0x082692f6 的 %ebx 预装载。
void CPvPLiveEventData::IncreasePlayCount(CUser* user)
{
    // ORIG 0x826921c：计数达阈值后按奖励表发系统邮件。
    if (user->getCurCharacR() == 0)
        return;

    alter();                       // charac_expand::CData::alter
    m_playCount += 1;              // +0x08

    if (m_playCount == G_CDataManager()->m_serverParameter.GetPvPLiveEventPlayCount())
    {
        for (std::vector<std::pair<int, int> >::iterator it =
                 G_CDataManager()->m_serverParameter.GetPvPLiveEventReward().begin();
             it != G_CDataManager()->m_serverParameter.GetPvPLiveEventReward().end(); ++it)
        {
            // [b28d-solo-CPvPLiveEventData] ORIG 0x082692c4/0x082692f6 在 G_CDataManager()
            // 调用前把两个值装入 %ebx 跨调用保活（P290：局部 int→unsigned 触发实参升格
            // callee-saved）。声明为 unsigned int 复现该装载序（探针 /tmp/probe_p290.cpp f3/f8）。
            unsigned int itemIdx = (*it).first;
            unsigned int addInfo = (*it).second;
            CItem* found = G_CDataManager()->find_item(({ itemIdx; }));
            if (found != 0)
            {
                Inven_Item item;
                item.m_addInfo = itemIdx;
                G_CDataManager()->m_itemList->create_item(
                    ENUM_ITEM_CREATE_TYPE_0, item, ({ addInfo; }));
                if (item.m_addInfo != 0 || item.get_add_info() != 0)
                {
                    char title[0x15];
                    char body[0x100];
                    strncpy(title,
                            g_scriptStringManager_.findString(4, "game_server_msg_140", 0), 0x14);
                    strncpy(body,
                            g_scriptStringManager_.findString(4, "game_server_msg_141", 0), 0xff);

                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        title, item, 0, user->getCurCharacNo(), body, (int)strlen(body), 0,
                        user->GetServerGroup(), false, false);
                    item.reset();
                }
            }
        }
    }
}


CPvPLiveEventData::CPvPLiveEventData()
{
}

bool CPvPLiveEventData::loadData(CUser* /*user*/, char* buffer)
{
    m_playCount = *reinterpret_cast<int*>(buffer);
    return true;
}
void CPvPLiveEventData::_reset()
{
    m_playCount = 0;
}

void CPvPLiveEventData::ResetDailyMidnight()
{
    m_playCount = 0;
}

void CPvPLiveEventData::getData(char* pData) const
{
    // ORIG 0x08269206：函数体为空（仅 push/pop/ret）。
    // 播放计数不在此序列化，由 _saveData 走 CStreamGuard 落库。
}

// [b28d-20] ORIG 0x082690d4：每处 operator<< 前调 CStreamGuard::operator*()（0x080c8c46）
// 并把返回 %eax 直接作 operator<< 的 this ⇒ 源形为 `guard.operator*()->operator<<(x)`
// （operator* 返回 CStreamGuard*，指针即 receiver）。实参跨 operator* 调用保活 ⇒ 4.4 自然
// 升格 %ebx（与 ORIG 的 mov %eax,%ebx 形一致）。
// [b28d-solo-CPvPLiveEventData] verdict=IDENTICAL/AE（ORIG 84 = OURS 84，逐条助记符全等，
// p167 调用点差集 CLEAR）；本波修法：GetUID() 调用点 (int) 收窄修重载选择（P3，
// ORIG 0x08269131 选 _ZN12CStreamGuardlsEi，原先误选 lsEj）。
bool CPvPLiveEventData::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvPLiveEventData.cpp", 0x1e), true);
    guard.operator*()->operator<<(0x1e1);
    // [b28d-solo-CPvPLiveEventData] ORIG 0x08269131 选 `operator<<(int)`（_ZN12CStreamGuardlsEi）：
    // CUser::GetUID() 声明为 unsigned int（CUser.h 冻结），调用点按 P3 显式收窄为 int 复现重载选择
    // （同宽转换零指令，p167 差集 MISSING lsEi/EXTRA lsEj 归零）。
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<((char)user->GetServerGroup());   // ORIG lsEc（movsbl 符号扩展，写 1 字节）
    guard.operator*()->operator<<(user->getCurCharacNo());
    guard.operator*()->operator<<(m_playCount);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

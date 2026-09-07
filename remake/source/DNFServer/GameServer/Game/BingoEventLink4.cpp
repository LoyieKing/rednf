// ============================================================================
// BingoEventLink4.cpp —— Bingo 域实现批次（OrigBingoEvt）
//
// BingoEvent 权威声明：BingoEvent.h（继承 CEventBase，成员
//   boost::shared_ptr<CBingo> m_bingo @ +0x0c；ORIG ctor 0x080c9984 new
//   CBingo(0x1c) 后挂到该槽，getBingoSystem 0x080c9fea 返回其地址）。
// CBingo 唯一类定义：CBingoTest.h/.cpp（Main 属主收敛；C1/D1/
//   getBingoRandomVaule/calNewValue/getIndexTodayBoard/calBoradMatchLine/
//   getToday×2/resetBoard/resetQuiz）。
// 本 TU 提供：BingoEvent C1/D1/D0/StartEvent×2/EndEvent/StartAction/
//   dailyresetData/reqBingoMark/getBingoSystem/sendOneMatchLineReward，
//   CUser::sendBingoData，以及 ORIG 全局位工具 isSetBit_Uint/setBit_Uint。
//
// 报告：docs/class_func_reports/{BingoEvent,CBingo,CUser}/（全文已读）。
// ============================================================================

#include "BingoEvent.h"
#include "CBingoTest.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "CGameManager.h"
#include "Action.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"   // m_vec3a0（+0x3a0 标记号 vector 具名起点）
#include "CMailBoxHelper.h"
#include "RDARScriptStringManager.h"
#include "LogManager.h"
#include "PacketGuard.h"

#include <bitset>
#include <map>
#include <vector>
#include <time.h>
#include <string.h>

// ---------------------------------------------------------------------------
// ORIG 全局位工具（source 无既有 producer，objdump 直转）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
bool isSetBit_Uint(unsigned int value, int bit)
{
    // ORIG: 恒寄存器 `1<<bit`（int 移位）并内联，无具名局部槽；`1u` 会差成 `shr`+分开。
    return (value & (1 << bit)) != 0;
}

void setBit_Uint(unsigned int& value, bool set, int bit)
{
    if (set)
    {
        value |= (1u << bit);
    }
    else
    {
        value &= ~(1u << bit);
    }
}

// ---------------------------------------------------------------------------
// BingoEvent 方法（docs/class_func_reports/BingoEvent/ 13 报告全文已读）。
// ---------------------------------------------------------------------------

// C1 @ 0x080c9984：CEventBase() + vptr + new CBingo(0x1c) 包 shared_ptr<CBingo>
// 挂到 this+0xc。初始化列表 m_bingo(new CBingo()) 顺序与 ORIG 一致。
BingoEvent::BingoEvent()
    : m_bingo(new CBingo())
{
}

// D1 @ 0x080c9a06：vptr + shared_ptr<CBingo> 析构 + CEventBase 析构（编译器按
// 声明逆序生成：m_bingo 先于 CEventBase）。D0 @ 0x080c9a68 由编译器生成。
BingoEvent::~BingoEvent()
{
}

// getBingoSystem @ 0x080c9fea：返回 this+0xc —— 成员 boost::shared_ptr<CBingo>
// 实例地址（ORIG ctor 将 new CBingo(0x1c) 挂到该槽；调用方经 operator-> 解引用）。
void* BingoEvent::getBingoSystem()
{
    return &m_bingo;   // +0x0c boost::shared_ptr<CBingo> 实例地址
}

// StartEvent @ 0x080c9a86：置事件标志；按当前秒与疲劳小时槽（G_CEnvironment
// +0x37c）取日号，1..31 内重置棋盘与 quiz，打日志。
void BingoEvent::StartEvent()
{
    SetEventFlag(true);
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2a, 0);
        trace("BingoEvent Day(%d)", day);
    }
}

// StartEvent(Word_Param) @ 0x080c9b68：经 vptr+0x8（slot 2）调无参 StartEvent()，
// 打日志。param 未参与计算（ORIG 如此）。
void BingoEvent::StartEvent(Word_Param param)
{
    StartEvent();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x30, 0);
    trace("BingoEvent Event Start!");
}

// StartAction @ 0x080c9bb8：遍历 CGameManager+0x22c（accId→CUser* map），
// 对每个在线用户发 bingo 数据包。
void BingoEvent::StartAction()
{
    std::map<unsigned int, CUser*>& users = G_CGameManager()->getUserByAccIdMap();
    for (std::map<unsigned int, CUser*>::iterator it = users.begin();
         it != users.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            user->sendBingoData();
        }
    }
}

// EndEvent @ 0x080c9d30：置事件标志 + 日志。
void BingoEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace trace(__PRETTY_FUNCTION__, 0x3f, 0);
    trace("BingoEvent Event End!");
}

// dailyresetData @ 0x080c9d82：按日重置棋盘/quiz；随后遍历在线用户，清
// BingoData、重发数据包并刷新装饰物计数。
void BingoEvent::dailyresetData()
{
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x50, 0);
        trace("BingoEvent Day(%d)", day);
    }

    std::map<unsigned int, CUser*>& users = G_CGameManager()->getUserByAccIdMap();
    for (std::map<unsigned int, CUser*>::iterator it = users.begin();
         it != users.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            BingoData* data = static_cast<BingoData*>(user->getBingoData());
            data->clear();
            user->sendBingoData();
            user->sendDecorationObjectCnt(0);
        }
    }
}

// reqBingoMark @ 0x080c9ff6（docs/class_func_reports/BingoEvent/reqBingoMark.md）：
// user/item 判空 → 0x13；已标记 3+ 次 → 0x7；物品请求号越界 → 0x17；随机值
// 无效 → 0x13；标记成功且新增连线时逐线发奖励，回 0x1c3 包，返回 0。
int BingoEvent::reqBingoMark(StackableAction::input_param& param)
{
    if (param.m_user == 0)
    {
        return 0x13;
    }
    if (param.m_item == 0)
    {
        return 0x13;
    }
    // [b23S3C] ORIG 0x080c9ff6 `call getBingoData` 共 8 次（+0x39/+0xc4/+0xd7/+0x10b/
    // +0x129/+0x13b/+0x172/+0x1ee），无 `data` 具名指针槽 ⇒ P48 每用点重取；
    // 字段访问一律经 BingoData* 出单条 disp 形（`movb $0x1,0x6(%eax)`/`movzbl 0x4(%eax)`/
    // `mov %dl,0x4(%eax)`）。calNewValue 的 &m_eventId ORIG 落 %ebx 跨 operator-> 存活
    // （P197②），残余记 N。
    // +0x04（连线数，signed char）> 2 即已达 3 次标记上限。
    if (static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4) > 2)
    {
        return 0x7;
    }
    // CStackableItem +0x3a0 = m_vec3a0（vector<int>），第 0 项为请求标记号（推断）。
    // [b23S3C] ORIG `add $0x3a0,%eax; mov %eax,(%esp)` 直落 operator[] 的 this 实参，
    // 无 itemVec 具名槽（P0/P48）⇒ 索引表达式内联；同时少一槽使 PacketGuard 帧基
    // 由 -0x30 回到 ORIG 的 -0x2c。
    int markNo = reinterpret_cast<CStackableItem*>(param.m_item)->m_vec3a0[0];
    if (markNo < 1 || markNo > 0x32)
    {
        return 0x17;
    }
    int randValue = m_bingo->getBingoRandomVaule(markNo);
    if (randValue == 0)
    {
        return 0x13;
    }
    unsigned int oldBoard =
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId;
    int newLines = m_bingo->calNewValue(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId, randValue);
    if (static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId != oldBoard)
    {
        static_cast<BingoData*>(param.m_user->getBingoData())->m_needSave = true;
        int diff = newLines - static_cast<signed char>(
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4);
        if (diff > 0)
        {
            if (diff > 3)
            {
                diff = 3;
            }
            static_cast<BingoData*>(param.m_user->getBingoData())->m_pad4 =
                static_cast<char>(newLines);
            for (int i = 0; i < diff; ++i)
            {
                sendOneMatchLineReward(*param.m_user);
            }
        }
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(randValue);
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(param.m_user->getBingoData())->m_eventId));
    packet.finalize(true);
    param.m_user->Send(packet);
    return 0;
}

// sendOneMatchLineReward @ 0x080ca3d4：读 m_bingo.m_map30[CDataManager+0xb568+0x30
// =+0xb598] 的 key=1（单线奖励配置 <type, value>），按 type 构造 Inven_Item 后
// 以系统邮件发放。
void BingoEvent::sendOneMatchLineReward(CUser& user)
{
    std::map<int, std::pair<int, int> >& rewardMap = G_CDataManager()->m_bingo.m_map30;
    std::map<int, std::pair<int, int> >::iterator it = rewardMap.find(1);
    if (it == rewardMap.end())
    {
        LogManager::logFormat(1, "BingoEvent.cpp",
                              __PRETTY_FUNCTION__, 0xc0,
                              "bingo script error");
        return;
    }

    const int type = it->second.first;
    const int value = it->second.second;

    Inven_Item item;
    unsigned int count = 0;
    if (type == 0)
    {
        count = static_cast<unsigned int>(value);
    }
    else if (type == 1)
    {
        item.m_addInfo = 1;                                  // +0x02 itemIdx=1
        item.m_field1 = 2;                                   // +0x01 ITEM_TYPE=2
        item.m_addInfo2 = value;                             // +0x07 add_info
        item.m_fieldb = 1;                                   // +0x0b endurance
        item.ResetItemAttr();                                // +0x06 attr=0
    }
    else
    {
        CItem* found = G_CDataManager()->find_item(type);
        if (found == 0)
        {
            return;
        }
        item.m_addInfo = type;                               // +0x02 itemIdx
        found->make_item(item);                              // vtbl slot +0x08
        item.set_add_info(value);                            // m_addInfo2 = value
    }

    char sender[21];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_292", 0), 0x14);
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_293", 0), 0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, count,
        static_cast<unsigned int>(user.getCurCharacNo()),
        title, static_cast<int>(strlen(title)),
        0, user.GetServerGroup(), false, false);
}

// ---------------------------------------------------------------------------
// sendBingoData @ 0x0869134e：取 repeat 事件 0x6a（BingoEvent）当日棋盘号，
// 连同用户 bingo 数据（棋盘值 +0x00、连线数 +0x05）打包 0x1c2 发送。
void CUser::sendBingoData()
{
    int today = 0;
    BingoEvent* event = reinterpret_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (event != 0)
    {
        CBingo* bingo =
            *reinterpret_cast<CBingo**>(event->getBingoSystem());
        today = bingo->getToday();
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c2);
    packet.put_byte(static_cast<int>(static_cast<char>(today)));
    packet.put_int(static_cast<int>(
        static_cast<BingoData*>(getBingoData())->m_eventId));
    packet.put_byte(static_cast<BingoData*>(getBingoData())->m_isWin ? 1 : 0);
    packet.finalize(true);
    Send(packet);
}

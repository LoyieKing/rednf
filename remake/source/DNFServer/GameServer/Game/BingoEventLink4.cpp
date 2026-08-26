// ============================================================================
// BingoEventLink4.cpp —— Bingo 域四符号链接批次（LinkMisc4）
//
//   _ZN10BingoEvent15getBingoSystemEv            T 0x080c9fea
//   _ZN10BingoEvent22sendOneMatchLineRewardER5CUser  T 0x080ca3d4
//   _ZN6CBingo11calNewValueERji                  T 0x080cad4c
//   _ZN5CUser13sendBingoDataEv                   T 0x0869134e
//
// 报告：docs/class_func_reports/{BingoEvent,CBingo,CUser}/。
// BingoEvent / CBingo 为 ORIG 全局命名空间类；GmCmds_A.cpp 持同名最小声明，
// 本 TU 提供唯一定义。CBingo 局部布局按 ORIG ctor 0x080ca82c 建模：
//   +0x00 vector<int>（今日棋盘索引）+0x0c int +0x10 shared_ptr<CMTRand>(8B)
//   +0x18 int m_today；BingoEvent : CEventBase，+0x0c 为 boost::shared_ptr<CBingo>
//   （ORIG ctor 0x080c9984：new CBingo(0x1c) 后挂到 this+0xc）。
// ============================================================================

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CMailBoxHelper.h"
#include "RDARScriptStringManager.h"
#include "LogManager.h"
#include "PacketGuard.h"

#include <bitset>
#include <map>
#include <vector>

// ---------------------------------------------------------------------------
// ORIG 全局位工具（source 无既有 producer，objdump 直转）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
bool isSetBit_Uint(unsigned int value, int bit)
{
    unsigned int mask = 1u << bit;
    return (value & mask) != 0;
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
class CBingo
{
public:
    // ORIG 布局（ctor 0x080ca82c）：本 TU 内部使用；无外部构造点。
    std::vector<int> m_boards;  // +0x00 今日棋盘索引表
    int m_fieldC;               // +0x0c（ORIG ctor 清零）
    void* m_randPtr;            // +0x10 boost::shared_ptr<CMTRand>.px
    void* m_randCtrl;           // +0x14 boost::shared_ptr<CMTRand>.pn
    int m_today;                // +0x18

    int calNewValue(unsigned int& board, int idx);      // ORIG T 0x080cad4c
    int getIndexTodayBoard(int value);                  // ORIG T 0x080caafa
    int calBoradMatchLine(int board);                   // ORIG T 0x080cab5a
    int getToday();                                     // ORIG T 0x080ca936（非 const）
};

// getIndexTodayBoard @ 0x080caafa：在今日棋盘索引表中查找 value 的下标。
int CBingo::getIndexTodayBoard(int value)
{
    if (value < 0)
    {
        return -1;
    }
    for (std::size_t i = 0; i < m_boards.size(); ++i)
    {
        if (m_boards[i] == value)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// calBoradMatchLine @ 0x080cab5a：统计 25 位棋盘的连线数
// （5 行 + 5 列 + 2 对角线；对角线1 = bit[r*6]，对角线2 = bit[(r+1)*4]）。
int CBingo::calBoradMatchLine(int board)
{
    std::bitset<25> bits(static_cast<unsigned long>(board));
    int lines = 0;
    int diag1 = 0;
    int diag2 = 0;
    for (int r = 0; r < 5; ++r)
    {
        int rowCount = 0;
        int colCount = 0;
        for (int c = 0; c < 5; ++c)
        {
            if (bits[5 * r + c])
            {
                ++rowCount;
            }
            if (bits[5 * c + r])
            {
                ++colCount;
            }
            if (rowCount < c + 1 && colCount < c + 1)
            {
                break;  // ORIG 早退：行列均已不可能凑满
            }
        }
        if (rowCount == 5)
        {
            ++lines;
        }
        if (colCount == 5)
        {
            ++lines;
        }
        if (bits[6 * r])
        {
            ++diag1;
        }
        if (bits[4 * (r + 1)])
        {
            ++diag2;
        }
    }
    if (diag1 == 5)
    {
        ++lines;
    }
    if (diag2 == 5)
    {
        ++lines;
    }
    return lines;
}

// calNewValue @ 0x080cad4c：idx 在当日棋盘上首次标记并返回新连线数。
int CBingo::calNewValue(unsigned int& board, int idx)
{
    int index = getIndexTodayBoard(idx);
    if (index == -1)
    {
        return 0;
    }
    if (index > 24)  // ORIG：cmp $0x18 / jle
    {
        return -1;
    }
    if (isSetBit_Uint(board, index))
    {
        return 0;
    }
    setBit_Uint(board, true, index);
    return calBoradMatchLine(static_cast<int>(board));
}

// getToday @ 0x080ca936：返回 this+0x18。
int CBingo::getToday()
{
    return m_today;
}

// ---------------------------------------------------------------------------
class BingoEvent
{
public:
    void* getBingoSystem();                     // ORIG T 0x080c9fea
    void sendOneMatchLineReward(CUser& user);   // ORIG T 0x080ca3d4
};

// getBingoSystem @ 0x080c9fea：返回 this+0xc —— 成员 boost::shared_ptr<CBingo>
// 实例地址（ORIG ctor 将 new CBingo(0x1c) 挂到该槽；调用方经 operator-> 解引用）。
void* BingoEvent::getBingoSystem()
{
    return reinterpret_cast<char*>(this) + 0xc;
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
                              "void BingoEvent::sendOneMatchLineReward(CUser&)", 0xc0,
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
    packet.put_int(*reinterpret_cast<int*>(getBingoData()));
    packet.put_byte(
        *reinterpret_cast<unsigned char*>(
            reinterpret_cast<char*>(getBingoData()) + 5));
    packet.finalize(true);
    Send(packet);
}

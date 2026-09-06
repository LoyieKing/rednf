// ============================================================================
// EventBattleLink.cpp —— 事件/战斗链接核销批次（10 符号，LinkHelper.EventBattleLink）
//
//   1. _ZN10BingoEvent16reqBingoQuizMarkER5CUseri          T 0x080ca262
//   2. _ZN10BingoEvent22sendMaxMatchLineRewardER5CUser     T 0x080ca622
//   3. _ZN20CraneMinigameManager10getItemCntEi             T 0x080ebd44
//   4. _ZN20CraneMinigameManager13isPickSuccessEsi         T 0x080ebce0
//   5. _ZN20CraneMinigameManager8sendItemEP5CUseri         T 0x080ebd7a
//   6. _ZN13CBattle_Field14SetDamageValueEii               T 0x08308ff8
//   7. _ZN13CBattle_Field19SetDamagedPacketCntEi           T 0x08308f4e
//   8. _ZN21CCommunityServerProxy16SendPvPBuddyListEjj     T 0x0846cb80
//   9. _ZN22CConditionEventManager27RequestConditionEventRewardEP5CUserscRSt3mapIiiSt4lessIiESaISt4pairIKiiEEE
//                                                          T 0x08334850
//  10. _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj T 0x080fda56
//      （附：_ZN34Packet_Request_PvP_Buddy_Conn_ListC1Ev W 0x0846cefc、
//           _ZN6CBingo11getQuizValueEv W 0x080cae12、_ZN6CBingo12getQuizIndexEv W 0x080cae1e、
//           _ZN19CraneMinigameScript7getItemEs T 0x088e38d6 —— 上述符号的直接依赖，同批产出）
//
// 报告：docs/class_func_reports/{BingoEvent,CBingo,CraneMinigameManager,
// CraneMinigameScript,CBattle_Field,CCommunityServerProxy,
// CConditionEventManager,CInGameAdvertisementManager}/。
// BingoEvent / CBingo 为 ORIG 全局命名空间类；与 BingoEventLink4.cpp /
// GmCmds_A.cpp 一样采用 TU 局部类（成员互不重叠，无强符号冲突）。
// ============================================================================

#include <map>
#include <string.h>

#include "CUser.h"                      // CUser/cUserHistoryLog（+0x79700）
#include "CUserCharacInfo.h"            // GetCurConditionEventStep 族/getCurCharacNo
#include "CDataManager.h"               // G_CDataManager/m_bingo/find_item/getItemList
#include "CItemList.h"                  // create_item/GetRestrictCode/get_rand_int
#include "Inven_Item.h"
#include "CInventory.h"
#include "CBattle_Field.h"              // SetDamageValue/SetDamagedPacketCnt/GetDungeon
#include "CParty.h"                     // GetParty + +0xb24 CBattle_Field 子对象
#include "CDungeon.h"                   // get_index
#include "WarField.h"                   // MapInfo::m_monsterMap/map_monster
#include "CraneMinigameManager.h"
#include "CCommunityServerProxy.h"
#include "Packet_Request_PvP_Buddy_Conn_List.h"
#include "CConditionEventManager.h"
#include "CInGameAdvertisementManager.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "CGuildServerProxy.h"          // CEventBase/CEventManager
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "EventClassify_Action.h"       // EventClassify::CEventActionMng
#include "InGameEventScripts.h"         // ::Action_SendMail
#include "GlobalData.h"
#include "LogManager.h"

// ---------------------------------------------------------------------------

// ORIG 0x0810cf32 T _ZN13EventClassify15CEventActionMng24process_action_send_mailEP5CUserR15Action_SendMailb
// （形参为全局 ::Action_SendMail；EventClassify_Action.h 镜像类型与 ORIG
// mangled 分歧，见 sendItem 注释——以精确 ORIG 符号 extern 调用）。
extern "C" int _ZN13EventClassify15CEventActionMng24process_action_send_mailEP5CUserR15Action_SendMailb(
    void* mng, CUser* user, Action_SendMail* mail, bool flag);
// §1 BingoEvent —— reqBingoQuizMark / sendMaxMatchLineReward（ORIG 0x080ca262 /
// 0x080ca622）。类布局（ORIG ctor 0x080c9984）：+0x0c 为 boost::shared_ptr<CBingo>
// 槽（getBingoSystem 0x080c9fea 返回该地址，operator-> 解引用得 CBingo*）。
// ---------------------------------------------------------------------------
class BingoEvent
{
public:
    int reqBingoQuizMark(CUser& user, int quizValue);          // ORIG T 0x080ca262
    void sendMaxMatchLineReward(CUser& user);                  // ORIG T 0x080ca622
    void sendOneMatchLineReward(CUser& user);                  // ORIG T 0x080ca3d4（定义 BingoEventLink4.cpp）
};

class CBingo
{
public:
    int getQuizValue();                                        // ORIG W 0x080cae12
    int getQuizIndex();                                        // ORIG W 0x080cae1e
    int calNewValue(unsigned int& board, int idx);             // ORIG T 0x080cad4c（定义 BingoEventLink4.cpp）
};

// BingoEventLink4.cpp 定义的全局位工具（强符号，仅声明引用）。
bool isSetBit_Uint(unsigned int value, int bit);

// ORIG sendMaxMatchLineReward 以 static int kMaxMatchLine（.rodata 0x08ae5c40 实例，
// 值 3）为 map key 查询满线奖励。
static const int kMaxMatchLine = 3;

int CBingo::getQuizValue()
{
    struct QuizValueView
    {
        char m_pad[0xc];
        int m_value;
    };
    // P1：组件视图折叠单指令 disp（mov 0xc(%eax),%eax）。
    return reinterpret_cast<QuizValueView*>(this)->m_value;
}

// getQuizIndex @ 0x080cae1e（while 前置 jmp、size() 每次求值 setg 有符号比较、
// operator[] 传 int 下标 ixEj、this 直用不落局部——照 ORIG 汇编）。
int CBingo::getQuizIndex()
{
    int i = 0;
    while (i < static_cast<int>((reinterpret_cast<std::vector<int>*>(this))->size()))
    {
        if ((*(reinterpret_cast<std::vector<int>*>(this)))[i] == 0)
        {
            return i;
        }
        ++i;
    }
    return -1;
}

// reqBingoQuizMark @ 0x080ca262（reqBingoQuizMark.md 汇编逐分支转写）。
int BingoEvent::reqBingoQuizMark(CUser& user, int quizValue)
{
    // 本地布局镜像类（P144）无具名成员；+0x0c = boost::shared_ptr<CBingo> 槽
    // （getBingoSystem ORIG 0x080c9fea 返回该地址）。镜像类布局访问。
    CBingo* bingo = *reinterpret_cast<CBingo**>(reinterpret_cast<char*>(this) + 0xc);

    if (bingo->getQuizValue() != quizValue)
    {
        return 0x17;
    }

    int idx = bingo->getQuizIndex();
    // 汇编 +0x4a..+0x54：idx<0 或 idx>0x31（越界）→ 0x13。
    if (idx < 0 || idx > 0x31)
    {
        return 0x13;
    }

    unsigned char* data = static_cast<unsigned char*>(user.getBingoData());
    if (isSetBit_Uint(*reinterpret_cast<unsigned int*>(data), idx))
    {
        return 0x12;
    }

    unsigned int oldBoard = *reinterpret_cast<unsigned int*>(data);
    int newLines = bingo->calNewValue(*reinterpret_cast<unsigned int*>(data), 0);
    if (*reinterpret_cast<unsigned int*>(data) != oldBoard)
    {
        data[6] = 1;  // +0x06 标记（sendBingoData/isWin 区）
        int diff = newLines - static_cast<int>(static_cast<signed char>(data[4]));
        if (diff > 0)
        {
            if (diff > 3)
            {
                diff = 3;
            }
            data[4] = static_cast<unsigned char>(newLines);
            for (int i = 0; i < diff; ++i)
            {
                sendOneMatchLineReward(user);
            }
        }
    }
    return 0;
}


// sendMaxMatchLineReward @ 0x080ca622（sendMaxMatchLineReward.md 转写）：
// m_bingo.m_map30[kMaxMatchLine] = <type, value>（CDataManager+0xb568+0x30），
// type 经 find_item 构造 Inven_Item 后以系统邮件发放（sender=292/title=294）。
void BingoEvent::sendMaxMatchLineReward(CUser& user)
{
    std::map<int, std::pair<int, int> >& rewardMap = G_CDataManager()->m_bingo.m_map30;
    std::map<int, std::pair<int, int> >::iterator it = rewardMap.find(kMaxMatchLine);
    if (it == rewardMap.end())
    {
        // ORIG 0x080ca695 引用 _ZZN10BingoEvent22sendMaxMatchLineRewardER5CUserE19
        // __PRETTY_FUNCTION__（P139）：必须传 __PRETTY_FUNCTION__ 生成同形局部静态符号。
        LogManager::logFormat(1, "BingoEvent.cpp",
                              __PRETTY_FUNCTION__, 0xef,
                              "bingo script error");
        return;
    }

    const int itemIdx = it->second.first;
    const int addInfo = it->second.second;

    Inven_Item item;
    CItem* found = G_CDataManager()->find_item(itemIdx);
    if (found != 0)
    {
        found->make_item(item);                     // vtable+0x08 槽
        item.set_add_info(addInfo);

        char sender[21];
        strncpy(sender, g_scriptStringManager_.findString(4, "game_server_msg_292", 0), 0x14);
        char title[256];
        strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_294", 0), 0xff);

        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            sender, item, 0,
            static_cast<unsigned int>(user.getCurCharacNo()),
            title, static_cast<int>(strlen(title)),
            0, user.GetServerGroup(), false, false);
    }
}

// ---------------------------------------------------------------------------
// §2 CraneMinigameManager / CraneMinigameScript（ORIG 0x080ebd44/0x080ebce0/
// 0x080ebd7a/0x088e38d6）。ORIG 中 Manager 与 Script 同一对象布局：
// +0x04 vector<CraneItemInfo*>、+0x18 内嵌 Action_SendMail。
// ---------------------------------------------------------------------------
// getItem @ 0x088e38d6。
CraneItemInfo* CraneMinigameScript::getItem(short idx)
{
    if (static_cast<unsigned int>(idx) < m_items.size())
    {
        return m_items.at(idx);   // ORIG 用 at（含界检）
    }
    return 0;
}

// getItemCnt @ 0x080ebd44：item->m_cnt（+0x08 unsigned short）。
int CraneMinigameManager::getItemCnt(int slot)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(static_cast<short>(slot));
    // ORIG：je 到尾部 mov $0 落叶（P47 三目布局）；movzwl+cwtl 符号扩展
    // （(int)(short)m_cnt，0xffff 无效标记返回 -1）。
    return item != 0 ? static_cast<int>(static_cast<short>(item->m_cnt)) : 0;
}

// isPickSuccess @ 0x080ebce0：item 存在且 m_itemIdx(+4)==pickIdx 且
// get_rand_int(100) <= m_pickRatio(+0x10) 才成功。
bool CraneMinigameManager::isPickSuccess(short slot, int pickIdx)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(slot);
    if (item != 0)
    {
        if (item->m_itemIdx == pickIdx)
        {
            // ORIG +0x42..+0x55：get_rand_int(100) 结果落 -0xc 槽，
            // cmp ratio,rand；jl → 尾 mov $0；否则 mov $1 中继。
            int rand = get_rand_int(100);
            if (item->m_pickRatio >= rand)
            {
                return true;
            }
        }
        else
        {
            return false;   // ORIG +0x34 内联 mov $0; jmp（itemIdx 不匹配）
        }
    }
    return false;   // ORIG 尾 mov $0（item==0 / rand 失败共享出口）
}

// sendItem @ 0x080ebd7a：经 +0x18 内嵌 Action_SendMail 发送物品，返回
// process_action_send_mail 结果；item 缺失或 cnt==0xffff 返回 0x15。
int CraneMinigameManager::sendItem(CUser* user, int slot)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(static_cast<short>(slot));
    if (item == 0)
    {
        return 0x15;
    }
    if (item->m_cnt == 0xffff)
    {
        return 0x15;
    }

    // ORIG +0x48/+0x5e：&mail 两次独立计算（P52 不落局部）；
    // +0x3b/+0x3f：movzwl 0x8 + movswl %ax（(int)(short)m_cnt）。
    this->m_sendMail.set_item(
        item->m_itemIdx, static_cast<int>(static_cast<short>(item->m_cnt)));

    EventClassify::CEventActionMng actionMng;
    // ORIG 符号形参为全局 ::Action_SendMail（R15Action_SendMail）；仓库
    // EventClassify_Action.h 的命名空间内镜像类型与 ORIG mangled 分歧（既有
    // 问题），此处以精确 ORIG 符号 extern 调用保证链接正确。
    return _ZN13EventClassify15CEventActionMng24process_action_send_mailEP5CUserR15Action_SendMailb(
        &actionMng, user,
        &this->m_sendMail, true);
}

// ---------------------------------------------------------------------------
// §3 CBattle_Field::SetDamageValue / SetDamagedPacketCnt（0x08308ff8/0x08308f4e）。
// 均在当前地图 m_monsterMap 中按 uid 查找；SetDamageValue 对 map_monster+0x28
// 累加伤害，SetDamagedPacketCnt 对 +0x24 自增；未找到返回 false。
// ---------------------------------------------------------------------------
bool CBattle_Field::SetDamageValue(int uid, int damage)
{
    // P28：先声明迭代器再赋值（find 经临时中转拷贝）；P48：GetCurrentMapInfo
    // 在 find/end 两处各自调用（无 CSE）；-0xc 初 0 为 operator-> 结果槽。
    map_monster* mob = 0;
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
    {
        return false;
    }
    // ORIG +0x88/+0x96：operator-> 后 add $0x4（second）存 mob，再 0x24(%eax)
    // 整型累加（m_field28 为 float 视图，ORIG 按 int 加）。
    mob = &it->second;
    // ORIG +0x96/+0xa1：0x24(%eax) 整型累加 damage = m_field24（int；
    // 非 m_field28，报告注释偏移需以汇编为准）。
    mob->m_field24 += damage;
    return true;
}

bool CBattle_Field::SetDamagedPacketCnt(int uid)
{
    // P28：先声明迭代器再赋值（find 经临时中转拷贝）；P48：GetCurrentMapInfo
    // 在 find/end 两处各自调用（无 CSE）；-0xc 初 0 为 operator-> 结果槽。
    map_monster* mob = 0;
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
    {
        return false;
    }
    // ORIG +0x88/+0x96：operator-> 后 add $0x4（second）存 mob，再 0x20(%eax)
    // 自增（m_field20；Ghidra C 的 +0x24 为 P8 偏移错位，汇编 0x20 为准）。
    mob = &it->second;
    mob->m_field20 += 1;
    return true;
}

// ---------------------------------------------------------------------------
// §4 CCommunityServerProxy::SendPvPBuddyList（0x0846cb80）+ 包构造（0x0846cefc）。
// ---------------------------------------------------------------------------
Packet_Request_PvP_Buddy_Conn_List::Packet_Request_PvP_Buddy_Conn_List()
    : PacketHeader(0x1b5a, 0x12)   // ORIG _ZN12PacketHeaderC1Ett(0x1b5a, 0x12)
{
    m_accId = 0;      // ORIG movl $0x0,0xa(%eax)
    m_characNo = 0;   // ORIG movl $0x0,0xe(%eax)
}

void CCommunityServerProxy::SendPvPBuddyList(unsigned int accId, unsigned int characNo)
{
    Packet_Request_PvP_Buddy_Conn_List packet;
    packet.m_accId = accId;         // -0x14
    packet.m_characNo = characNo;   // -0x10
    SendTcpPacket(reinterpret_cast<char*>(&packet), packet.packetSize);  // 0x12
}

// ---------------------------------------------------------------------------
// §6 CInGameAdvertisementManager::WriteLog（0x080fda56）：
//   state==5 → party+0xb24 战场副本 idx；stream 0x32c + UID；
//   GetInBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG> 填 0x0..0x1f；LOGDB_Q(4) 入队。
// ---------------------------------------------------------------------------
void CInGameAdvertisementManager::WriteLog(CUser* user, char type, short val,
                                           char flag, unsigned int extra)
{
    if (user == 0)
    {
        return;
    }

    // 局部声明序照 ORIG 栈槽（+0x025..+0x0c5）：3 个恒 -1 槽居低地址，
    // sig/party/dungeon 居中，state==3 的 vill/area 居顶（从不写 SIG）。
    int dungeonIdx = -1;                       // ORIG -0x28（SIG +0x14）
    int field18 = -1;                          // ORIG -0x24（SIG +0x18 恒 0xffffffff）
    int field1c = -1;                          // ORIG -0x20（SIG +0x1c 恒 0xffffffff）
    SIG_IN_GAME_ADVERTISEMENT_LOG* sig;        // ORIG -0x1c
    CParty* party;                             // ORIG -0x18
    CDungeon* dungeon;                         // ORIG -0x14（GetDungeon 结果单槽）
    int village;                               // ORIG -0x10（state==3 计算，不写 SIG）
    int area;                                  // ORIG -0xc（state==3 计算，不写 SIG）

    if (user->get_state() == 5)
    {
        party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            dungeon = party->m_battleField.GetDungeon();
            if (dungeon != 0)
            {
                dungeonIdx = dungeon->get_index();
            }
        }
    }
    else if (user->get_state() == 3)
    {
        // ORIG +0x0a1..+0xc5：getCurCharacVill()/get_area(false) 计算结果仅存
        // 局部，从不写入 SIG（+0x18/+0x1c 恒 0xffffffff，忠实保留）。
        village = user->getCurCharacVill();
        area = user->get_area(false);
    }

    // Acquire 结果直接作 guard 实参（ORIG +0x0e0..+0x0f7 无中间槽）；
    // operator<< 走 operator*() 链（P132：ORIG call 0x080c8c46）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("InGameAdvertisementManager.cpp", 0x17a), true);
    (*guard)->operator<<(0x32c);
    (*guard)->operator<<(static_cast<int>(user->GetUID()));

    sig = guard->GetInBuffer<SIG_IN_GAME_ADVERTISEMENT_LOG>();
    sig->m_accId = user->get_acc_id();                       // +0x00
    sig->m_characNo = user->get_charac_no(-1);               // +0x04
    sig->m_pad1c = field1c;                                  // +0x1c（写序照 ORIG）
    sig->m_adType = val;                                     // +0x0a
    sig->m_type = type;                                      // +0x08（char→uchar 隐式转换）
    sig->m_flag = flag;                                      // +0x0c
    sig->m_extra = extra;                                    // +0x10
    sig->m_dungeonIdx = dungeonIdx;                          // +0x14
    sig->m_village = field18;                                // +0x18（恒 -1，ORIG 写 local_28）

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

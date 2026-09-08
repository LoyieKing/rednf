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
#include "BingoEvent.h"                 // BingoEvent 权威头（m_bingo: shared_ptr<CBingo> @+0x0c）

// ---------------------------------------------------------------------------

// b22 桥清零（P201，Model22）：ORIG 0x0810cf32
// _ZN13EventClassify15CEventActionMng24process_action_send_mailEP5CUserR15Action_SendMailb
// 原 extern"C" mangled 直连桥转真成员调用——EventClassify_Action.h:28 权威声明
// 形参即全局 ::Action_SendMail（InGameEventScripts.h 唯一定义，R15 编码=全局
// 作用域，与 ORIG mangled 逐字符一致），定义 EventClassify_Action.cpp:35。旧
// 「镜像类型分歧」注记已随头修正失效。
// §1 BingoEvent —— reqBingoQuizMark / sendMaxMatchLineReward（ORIG 0x080ca262 /
// 0x080ca622）。类布局（ORIG ctor 0x080c9984）：+0x0c 为 boost::shared_ptr<CBingo>
// 槽（getBingoSystem 0x080c9fea 返回该地址，operator-> 解引用得 CBingo*）。
// ---------------------------------------------------------------------------
// BingoEvent 权威头（BingoEvent.h）：成员 `boost::shared_ptr<CBingo> m_bingo` @+0x0c，
// 方法声明 reqBingoQuizMark/sendMaxMatchLineReward/sendOneMatchLineReward 齐全。
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
    // [b23S3C] ORIG 0x080ca262 逐 call 位实证的三处形差（本件真语义缺口 gap 20）：
    // ①m_bingo 是 boost::shared_ptr<CBingo>，ORIG 每用点 `add $0xc; call
    //   _ZNK5boost10shared_ptrI6CBingoEptEv`（W @0x080ce766）共 3 次 ⇒ 改真实成员
    //   `m_bingo->`（原镜像类 reinterpret 缓存 `bingo` 局部丢 2 次 call）；
    // ②user.getBingoData() ORIG 7 次重取（isSetBit/旧值/calNewValue 入参/新值比较/
    //   写 +6/读 +4/写 +4），无 `data` 具名缓存槽 ⇒ P48 每用点重取；
    // ③字段访问经 BingoData（CUser.h:1785）出单条 disp 形（`movb $0x1,0x6(%eax)` /
    //   `movzbl 0x4(%eax)`），原 `data[6]` 下标形多 1 条 add；
    // ④越界判 ORIG `cmpl $0x0;jns; cmpl $0x31;jle` = 死条件 `idx<0 && idx>0x31`
    //   （原 `||` 写成短路或在 idx<0 时误返 0x13，真语义 bug）⇒ 改 &&。
    if (m_bingo->getQuizValue() != quizValue)
    {
        return 0x17;
    }

    int idx = m_bingo->getQuizIndex();
    if (idx < 0 && idx > 0x31)
    {
        return 0x13;
    }

    if (isSetBit_Uint(static_cast<BingoData*>(user.getBingoData())->m_eventId, idx))
    {
        return 0x12;
    }

    unsigned int oldBoard = static_cast<BingoData*>(user.getBingoData())->m_eventId;
    int newLines = m_bingo->calNewValue(
        static_cast<BingoData*>(user.getBingoData())->m_eventId, 0);
    if (static_cast<BingoData*>(user.getBingoData())->m_eventId != oldBoard)
    {
        static_cast<BingoData*>(user.getBingoData())->m_needSave = true;
        int diff = newLines - static_cast<int>(static_cast<signed char>(
            static_cast<BingoData*>(user.getBingoData())->m_pad4));
        if (diff > 0)
        {
            if (diff > 3)
            {
                // [b28d-58] DIFF N（P85 + P238 反向）：ORIG 0x080ca376 段
                // `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle` + `mov $0x3,%eax;
                // mov %eax,-0x10(%ebp)`（局部载入比较 + 常量经 eax 中转 store）；
                // 4.4-O0 对 signed int 局部恒 `cmpl $0x3,mem` 直比 + `movl $3,mem`
                // 直存（`(int)3u` 异型化亦折）⇒ 2 条封顶，语义等价。
                diff = 3;
            }
            static_cast<BingoData*>(user.getBingoData())->m_pad4 =
                static_cast<char>(newLines);
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
    // [b28d-64] P48/P157：ORIG 0x080ca62d find 与 0x080ca652 end 各自独立调用
    // G_CDataManager()（3 次总计，含 find_item）；缓存 map 引用会少 1 次调用。
    std::map<int, std::pair<int, int> >::iterator it =
        G_CDataManager()->m_bingo.m_map30.find(kMaxMatchLine);
    if (it == G_CDataManager()->m_bingo.m_map30.end())
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
    // ORIG +0x32：`cmp pickIdx,%eax; je +0x3b` —— ==成立跳 rand 段、
    // 不等 fall 内联 mov $0x0;jmp 尾叶（探针 p25：if(!=) return false
    // 独立守卫形才出 je 正向 + fall 0 岛；else-return 嵌套形 4.4 翻转 jne）。
    // 外层 item!=0 守卫（ORIG +0x27 je +0x5c 直落共享尾叶）。
    if (item != 0)
    {
        if (item->m_itemIdx != pickIdx)
        {
            return false;   // ORIG +0x34 内联 mov $0; jmp（itemIdx 不匹配）
        }
        // ORIG +0x42..+0x55：get_rand_int(100) 结果落 -0xc 槽，
        // cmp ratio,rand；jl → 共享尾 mov $0 叶（失败 fall 出块）；
        // 成立 fall 内联 mov $1;jmp 尾叶（P47 三目布局反向）。
        int rand = get_rand_int(100);
        if (item->m_pickRatio >= rand)
        {
            return true;
        }
    }
    return false;   // ORIG 尾 mov $0（item==0 / rand 失败共享出口）
}

// sendItem @ 0x080ebd7a：经 +0x18 内嵌 Action_SendMail 发送物品，返回
// process_action_send_mail 结果；item 缺失或 cnt==0xffff 返回 0x15。
int CraneMinigameManager::sendItem(CUser* user, int slot)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(static_cast<short>(slot));
    // ORIG +0x22/+0x36：item==0 → je 尾块 +0x84（`mov $0x15,%eax` 直落 leave）；
    // m_cnt==0xffff → fall 内联块 +0x31（`mov $0x15,%eax; jmp leave`）；body 尾
    // +0x82 `jmp leave` 跳过尾块。该布局 = 正向 if 包主流程、return 0x15 落
    // else（P169/P285 同族：4.4 对平铺两条早退恒把两块都内联在前部，反产
    // `jne 跳过; mov 0x15; jmp` ×2）。
    if (item != 0)
    {
        if (item->m_cnt == 0xffff)
            return 0x15;

        // ORIG +0x48/+0x5e：&mail 两次独立计算（P52 不落局部）；
        // +0x3b/+0x3f：movzwl 0x8 + movswl %ax（(int)(short)m_cnt）。
        this->m_sendMail.set_item(
            item->m_itemIdx, static_cast<int>(static_cast<short>(item->m_cnt)));

        EventClassify::CEventActionMng actionMng;
        return actionMng.process_action_send_mail(user, this->m_sendMail, true);
    }
    return 0x15;
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

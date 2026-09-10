// ============================================================================
// df_game_r 还原 —— GameWorld（G2-4，世界/频道/在线用户聚合根）
// 本文件按 ORIG 反汇编（docs/class_func_reports/GameWorld.md + 弱符号直读）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// 布局见 GameWorld.h（sizeof 0x1b0，ORIG create() new 0x1b0 实证）。
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "GameWorld.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"   // 权威 CInventory/PacketGuard（本文件原本地 PacketGuard 已删除）
#include "InterSelectCreateDnfEventInfo.h"   // Reward 权威定义（本地局部类已删除，§4.4 合并）
#include "CItemList.h"
#include "Inven_Item.h"
#include "CDataManager.h"
#include "CraneMinigameManager.h"  // CraneMinigameManager::updateCraneItemNeedMaterial
#include "CUserProc.h"       // APSystem::CUserProc::SetTodayActionAndCheckMedalReward
#include "QuickParty.h"      // QuickParty::CQuickPartySystemManager（CDungeon.h 本地最小声明在前置）
#include "CDungeon.h"        // CDungeon::get_min_level（getDungeonMinimumRequiredLevel）
#include "CEnvironment.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CGM_Manager.h"  // 权威 CGM_Manager::IsGm（s_GM_Manager 指针解引用）
#include "CNetwork.h"
#include "CSystemTime.h"
#include "CParty.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "Stream.h"
#include "TaiwanInternal.h"
#include "LogManager.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"   // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "CHackLog_InvalidAreaMove.h"
#include "Arad_EventPeriodDataManager.h"
#include "Packet_MiniCraneSeed.h"
#include "STQuestScript.h"      // initTownScript / importTownScript / TownScript
#include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr
#include "CSchoolMgr.h"         // g_schoolMgr
#include "CGuildAgitManager.h"  // g_guildAgitMgr
#include "CPowerManager.h"      // GlobalData::s_power_manager 方法
#include "CAssaultMgr.h"        // pvp_assault::GetInstanceAssaultMgr
#include "CSyncSlangFilter.h"   // CSyncSlangFilter::GetInstance / Filter
#include "CTitleBook.h"         // CTitleBook::sendListOtherUser
#include "CMyFileLog.h"         // CMyFileLog
#include "CFatigueBatteryHandle.h"
#include "CBoosterGage.h"
#include "CEventScriptMng.h"    // EventClassify::CEventScriptMng
#include "CTimeGate.h"
#include "CPartyTelePort.h"     // CPartyTelePort::get_teleport_state
#include "Arad_DataManager.h"   // ARAD::Arad_DataManager / Singleton / notifyOpenMessageDialog
#include "HeroMissionEvent.h"   // ARAD::Singleton<HeroMissionEvent>
#include "AradServerStateMessage.h"
#include "InterSelectPcroomDailyReward.h"
#include "PacketTypes.h"         // MSG_ITEM_HYPER_LINK_MESSAGE 权威定义（原 TU 局部 struct 已删除）
#include "DB_AccountCargoSync.h" // DB_PassPadUpdateFailCnt / APSystem::DB_UpdateActionPoint
#include "CEventStayTime.h"      // CEventStayTime::RewardGoGoFighter 权威（原局部视图 ODR 清理）
#include "Packet_Monitor_Guild_Chat_Hyper_Link.h"    // case 6 超链接公会消息
#include "Packet_Monitor_Member_Chat_Hyper_Link.h"   // case 8 超链接会员消息
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"     // case 0xb/0xc/0xe/0x23 超链接喇叭消息（set 声明，定义已报 Main）
#include "Packet_Monitor_Other_Channel_Chat_Hyper_Link.h"  // case 1 超链接跨服消息
#include "Packet_Monitor_Other_Channel_Chat.h"  // case 1 跨服私聊监控包（ORIG 0x86ca0b9 布局）
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"  // case 6 公会聊天监控包（ORIG 0x86ca6ac；Game/ 空壳头同名，必须用 shared 完整版）
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"  // case 8 会员聊天监控包（ORIG 0x86ca7dc）
#include "../../../shared/packet/include/Packet_Monitor_MegaPhone.h"  // case 0xb/0xc/0xe 喇叭监控包（ORIG 0x86ca8b8）

// ============================================================================
// CStreamGuard::GetInBuffer 模板实参（全局类型：C++98 不允许局部类型作为模板
// 实参；全局同名类型使 guard.GetInBuffer<SIG_*>() 自然产生 ORIG 模板实例符号
// _ZN12CStreamGuard11GetInBufferI24SIG_UPDATE_STATISTIC4LEVEEPT_v 与
// _ZN12CStreamGuard11GetInBufferI17SIG_NAT_TYPE_USEREEPT_v）

// ============================================================================
// sync_script::CSyncScript 最小声明（不 include sync_script.h：其 ST* 镜像
// 声明与本 TU 经 CUser.h/CDataManager.h 引入的权威 STEquipmentScript/
// EquipmentParameterInfo 冲突；本 TU 仅需 GetInstanceSyncScript +
// truncate/insert_town_info，声明与 sync_script.h 同签名（ORIG mangled 一致））
// ============================================================================
namespace sync_script
{
class CSyncScript
{
public:
    bool insert_town_info_to_db(const TownScript& script);  // _ZN11sync_script11CSyncScript22insert_town_info_to_dbERK10TownScript
    bool truncate_town_info_table();                        // _ZN11sync_script11CSyncScript24truncate_town_info_tableEv
};
CSyncScript* GetInstanceSyncScript();   // _ZN11sync_script21GetInstanceSyncScriptEv
}

// ============================================================================
// 跨类方法（原 extern "C" asm 桥已删除；均改用权威头真实调用：
//   CInventory::insertItemIntoInventory — CInventory.h
//   CUser::GetServerGroup / getPowerSide / GetEventCreateDnfReward — CUser.h
//   CGM_Manager::IsGm — CGM_Manager.h（s_GM_Manager 为指针，按 ORIG 解引用））
// ============================================================================


// ServerParameterScript 方法现由真实 ServerParameterScript.h 提供（m_serverParameter 成员），
// 旧 23 错拼长度 asm 桥接已随调用迁移删除。

// ---- Taiwan 事件流（CEventStayTime::RewardGoGoFighter 依赖）----

// ============================================================================
// 全局数据（ORIG：MAX_VILLAGE_NUM 0948b2b0 / g_townScriptFileList 09500ea0）
// ============================================================================
int MAX_VILLAGE_NUM;
std::map<int, std::string> g_townScriptFileList;

// ============================================================================
// b28c-lane3：Village::get_gate_area 定义由 Village.cpp 迁入（P210 帧继承②）。
// ORIG 中 Village 函数与 GameWorld 函数地址相邻（get_gate_area 0x086c3b58 →
// GameWorld::C1 0x086c3d4a）⇒ 同 TU，4.1 对本 caller 精确继承小帧 sub $0x4；
// 我方拆 TU 后 4.4 仅见声明恒保守 sub $0x18。Village.cpp 内无本函数调用
//（grep 实证），符号全局唯一、mangled 不变。
// ============================================================================
int Village::get_gate_area()
{
    for (int i = 0; i < m_areaCount; ++i)
    {
        if (m_pAreas[i].m_areaType == 1)
            return i;
    }
    return 1;
}

// ============================================================================
// 单例
// ============================================================================
// 判据㉓（符号绑定，Main 已批准单点修法）：ORIG `_Z11G_GameWorldv` 0x080da3a7 为
// W 弱绑定，我方原为 T 强绑定 ⇒ 定义加 weak（只改本 .cpp，零头文件风险）。
__attribute__((weak)) GameWorld* G_GameWorld()
{
    return GlobalInstance<GameWorld>::inst_ptr();
}

void CREATE_GameWorld()
{
    GlobalInstance<GameWorld>::inst();
}

void DESTROY_GameWorld()
{
    GlobalInstance<GameWorld>::destroy();
}

// ============================================================================
// 构造 / 析构 / 销毁 / 初始化
// ============================================================================
GameWorld::GameWorld()
    : m_mutex(), m_obsCrash(), m_idMap(), m_shusia(), m_pvp(), m_natType(),
      m_UserInWorld(), m_sessionList(), m_loginUserMap()
{
    m_pVill = NULL;
    m_obsCrash.reset();
    for (int i = 0; i <= 10; i++)
    {
        m_channelUserCount[i] = 0;
    }
    m_userCount = 0;
    m_hangameUserCount = m_userCount;
}

GameWorld::~GameWorld()
{
    destroy();
}

void GameWorld::destroy()
{
    if (m_pVill != NULL)
    {
        delete[] m_pVill;
        m_pVill = NULL;
    }
}

// [b28d-solo-GameWorld] _ZN9GameWorld4initEv verdict=AE 275/275（b28d-solo 本轮）
// 修法：P0/P183 —— `CEnvironment* env = G_CEnvironment();` 具名指针缓存删除，改
// 表达式直传 `G_CEnvironment()->get_gc_no_hardcode(...)`（ORIG 0x086c42aa 结果直作
// receiver、无 env 槽）；删后 gcNo 槽由 -0x20 回到 ORIG -0x1c，多出的 1 call+1 store
// 归零。p167 CLEAR 40=40。
bool GameWorld::init()
{
    // ORIG +0x00c：GetInstanceSyncScript 结果直转实参，无命名局部
    if (sync_script::GetInstanceSyncScript()->truncate_town_info_table() != 1)
    {
        puts("Fail truncate_town_info_tablef");
        return false;
    }

    m_uniqueId = 0;
    initTownScript((char*)"Town/", (char*)"Town.lst");

    TownScript tempTown;
    const char* fileName = NULL;

    int townCount = (int)g_townScriptFileList.size();
    MAX_VILLAGE_NUM = townCount + 1;
    // ORIG +0x09a~+0x13d：m_pVill/m_iVillCount 两个 store 都在 Village 构造循环之后
    m_pVill = new Village[MAX_VILLAGE_NUM];
    m_iVillCount = townCount;

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
        while (index < GlobalData::s_statistic_proxy_mgr->GetEndIndex())
        {
            // ORIG 0x086c42aa：G_CEnvironment() 结果直传 receiver（无具名 env 槽，
            // P0/P183）；具名指针缓存多 1 槽 + 1 条 store/load 并使 gcNo 槽浅移 4。
            InsertChannel(G_CEnvironment()->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = GlobalData::s_statistic_proxy_mgr->GetNextIndex(index);
        }
    }
    else
    {
        // ORIG +0x1cc~+0x1d7：mov 0x198(%eax) 字段直读 m_serverEnvir.m_gcNo（非 get_gc_no 调用）
        InsertChannel(G_CEnvironment()->m_serverEnvir.m_gcNo, E_SERVER_GROUP_NONE);
    }

    std::map<int, std::string>::iterator it;
    for (it = g_townScriptFileList.begin(); it != g_townScriptFileList.end(); it++)
    {
        fileName = it->second.c_str();
        if (fileName == NULL)
        {
            break;
        }
        if (!importTownScript(&tempTown, fileName))
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3d4,
                                  "importTownScript() fail - filename(%s)", fileName);
            return false;
        }
        if (!sync_script::GetInstanceSyncScript()->insert_town_info_to_db(tempTown))
        {
            printf("insert_town_category Error: %s\n", fileName);
            return false;
        }
        if (tempTown.m_field0 > m_iVillCount)   // ORIG +0x2d6 cmp vill,town; jle ok
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3e3,
                                  "tempTown.townIndex_(%d) > m_iVillCount(%d) - file(%s)",
                                  tempTown.m_field0, m_iVillCount, fileName);
            return false;
        }
        if (!m_pVill[tempTown.m_field0].set_village(tempTown))
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x3e9,
                                  "m_pVill[tempTown.townIndex_].set_village() fail - file(%s)",
                                  fileName);
            return false;
        }
    }

    G_CDataManager()->GetPvPChannelGrade();
    return true;
}

// ============================================================================
// 频道
// ============================================================================
void GameWorld::InsertChannel(int gcNo, ENUM_SERVER_GROUP group)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x11a0),
                       true);
    **guard << 0x2d;
    **guard << -1;

    struct __attribute__((packed)) stChannelInsertInfo
    {
        int m_gcNo;                 // +0x00
        unsigned short m_zero;      // +0x04
        char m_serverIP[0x10];      // +0x06
        unsigned short m_tcpPort;   // +0x16
        unsigned short m_maxUserNum;// +0x18
        char m_serverGroup;         // +0x1a
        char m_channelName[0x1e];   // +0x1b
        unsigned short m_channelNo; // +0x39
        char m_channelInfo[0x19];   // +0x3b
        int m_channelType;          // +0x54 ← channel_info_t::m_channelType
        int m_group;                // +0x58
    };
    stChannelInsertInfo buf;
    memset(&buf, 0, 0x5c);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;
    buf.m_zero = 0;

    memcpy(buf.m_serverIP, G_CEnvironment()->m_serverEnvir.m_serverIP,
           strlen(G_CEnvironment()->m_serverEnvir.m_serverIP));
    buf.m_tcpPort = (unsigned short)G_CEnvironment()->m_serverEnvir.m_tcpPort;
    buf.m_maxUserNum =
        (unsigned short)G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    buf.m_serverGroup = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    memcpy(buf.m_channelName, G_CEnvironment()->m_serverEnvir.m_channelName,
           strlen(G_CEnvironment()->m_serverEnvir.m_channelName));
    buf.m_channelNo =
        (unsigned short)G_CEnvironment()->m_serverEnvir.m_channelNo;

    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    channel_info_t* channelInfo = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        G_CEnvironment()->get_channel_no());
    if (channelInfo != NULL)
    {
        // ORIG（0x086cb88a +0x18b）：`add $0x8,%eax` 后 call std::string::c_str，
        // 即取 m_dungeonName 的 C 字符串（非对象裸内存 +8）。
        strncpy(buf.m_channelInfo, channelInfo->m_dungeonName.c_str(), 0x19);
    }
    if (channelInfo != NULL)   // ORIG +0x1b3 第二次独立 cmpl $0（P48 双臂不合并）
    {
        buf.m_channelType = channelInfo->m_channelType;   // +0x1bc mov 0x4(%eax),%eax
    }

    guard->put_binary(&buf, 0x5c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void GameWorld::DeleteChannel(int gcNo, ENUM_SERVER_GROUP group)
{
    // N82/P112 封顶：指令序/call 序/EH 清理段逐条全等（78/78），残差全为
    // guard/buf/trace 槽序（4.1 声明正序 buf→guard→trace；4.4 实测声明逆序
    // 分配，trace 构造时机不可前移，buf 提前声明已试无收益）。帧 0x40/0x30
    // 同源。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x11bc),
                       true);
    **guard << 0x2f;
    **guard << -1;

    struct stChannelDeleteInfo
    {
        int m_gcNo;     // +0x00
        int m_group;    // +0x04
        int m_reserved; // +0x08  ORIG sizeof=0xc（memset 0xc / put_binary 0xc 证据），
                        //        该字段仅由 memset 清零，ORIG 无独立写入
    };
    stChannelDeleteInfo buf;
    memset(&buf, 0, 0xc);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;

    guard->put_binary(&buf, 0xc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    // b28c-lane3：P221 全表达式临时链——具名 trace 使 4.4 把 trace 放最深槽
    // (-0x28)，临时链后 4.4 复现 ORIG 槽序 trace -0x18 / guard -0x20 / buf -0x2c。
    cMyTrace(__PRETTY_FUNCTION__, 0x11c7, 0)(
        "put db message to db thread DeleteChannel()");
}

void GameWorld::UpdateChannelUser()
{
    // 删具名指针中转变量（技巧 36）：ORIG 0x086cb… 区是
    // `call StreamPool::Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp);
    // lea -0x14(%ebp),%eax; call CStreamGuard::C1` —— Acquire 结果 %eax 直落
    // arg2，无 spill 槽；我们多一个 `Stream* stream` 局部 ⇒ 4.4 插入
    // `mov %eax,-0x10(%ebp)` + `mov -0x10(%ebp),%eax` 两条，且该 4 字节把
    // 后续所有槽号（guard -0x14→-0x18、buf -0x4e→-0x52）整体推偏。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x1146), true);
    **guard << 0x2e;
    **guard << -1;

    // DB 0x2e 载荷（ORIG 0x086cb72a 逐指令偏移实证：+0x00 int ←env+0x198、
    // +0x04 word ←map size、+0x06..+0x31 11×int ←this+0x164、
    // +0x32 int ←this+0x194（hangame）、+0x36 int ←this+0x190）。
    struct __attribute__((packed)) stChannelUserCountInfo
    {
        int m_gcNo;                  // +0x00
        unsigned short m_worldUsers; // +0x04
        int m_channelUserCount[11];  // +0x06..+0x31
        int m_hangameUserCount;      // +0x32
        int m_userCount;             // +0x36
    };
    stChannelUserCountInfo buf;
    memset(&buf, 0, 0x3a);
    buf.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
    buf.m_worldUsers = (unsigned short)m_UserInWorld.size();
    for (int i = 0; i < 0xb; i++)
    {
        buf.m_channelUserCount[i] = m_channelUserCount[i];
    }
    buf.m_hangameUserCount = m_hangameUserCount;
    buf.m_userCount = m_userCount;

    guard->put_binary(&buf, 0x3a);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-solo-GameWorld] _ZN9GameWorld30updateDetailChannelServiceInfoEv verdict=NEAR 309/309
// （b28d-solo 本轮；助记符序列全等，仅槽号差 8）。
// 修法：①P163 补未列 case 0 撑宽 range（0x086cb1e8 `cmp $0xa;ja` 直接索引）；
// ②P38/P198① v 提到 switch 外预清复用；③P13 —— ORIG 0x086cb471 begin 后立即
// 0x086cb486 调 end（-0x80，循环前缓存）⇒ 具名 `last` 缓存 + goto 尾条件；
// ④P259 —— `Stream* stream=Acquire(...)` 具名局部删，内联进 CStreamGuard 实参
// （ORIG 0x086cb4d1 %eax 直传，无 stream 槽）；⑤0x086cb5e2 `jne <body>; jmp <exit>`
// 极性：反转条件 + 显式 `return` 出口（4.4 对 `if(cit!=last) goto LOOP3;` 恒产
// `je exit; jmp body`）。余 N82/N115 槽组平移 8（it -0x78 vs -0x80）封顶；p167 CLEAR 49=49。
void GameWorld::updateDetailChannelServiceInfo()
{
    std::map<int, int> countMap;
    // [b28d-solo-GameWorld] P167：ORIG 用 for-init 直构 + 前缀 ppEv
    // （MISSING ppEv / EXTRA C1Ev+ppEi 实证）⇒ 改 for-init 直构 + `++it`。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // ORIG 0x086cb1cd `movl $0x0,-0x14`（在 get_state() 调用之前）⇒ 原码在
            // switch 外声明并预清 v，各 case 复用同一槽（P198① 预清两句式）。
            int v = 0;
            switch (user->get_state())
            {
            // b28d-solo P163：ORIG 0x086cb1d0 段 `cmp $0xa,%eax; ja; mov
            // &jump_table(,%eax,4)` 直接以 get_state() 索引 0..0xa（无 sub）⇒
            // 原码含未列 case 0（与 break 共享）撑宽 range。
            case 0:
                break;
            // ORIG 0x086cb1df 跳转表实证：case 值为 3/5/6&7/8/10（cmp $0xa;ja），非 0/1/2/6/8
            case 3:                    // +0x1f1 → countMap[0]
                // ORIG 0x086cb1f1 段：`call op[]; mov (%eax),%eax; mov %eax,-0x14`
                // 后重算 key 再 `call op[]` ⇒ 具名局部 + 两条语句（P38），
                // 单语句 `m[k]=m[k]+1` 被 4.4 SAVE_EXPR 合并成单次 op[]+%ebx 缓存。
                v = countMap[0];
                countMap[0] = v + 1;
                break;
            case 5:                    // +0x235 → countMap[1]
                v = countMap[1];
                countMap[1] = v + 1;
                break;
            case 6:                    // +0x279：channel 检查 → +0x2ad countMap[9]
            case 7:                    // +0x2f1 → countMap[2]
                if (G_GameWorld()->GetChannelType() == 0xf ||
                    G_GameWorld()->GetChannelType() == 0x10)
                {
                    v = countMap[9];
                    countMap[9] = v + 1;
                }
                else
                {
                    v = countMap[2];
                    countMap[2] = v + 1;
                }
                break;
            case 8:                    // +0x335 → countMap[6]
                v = countMap[6];
                countMap[6] = v + 1;
                break;
            case 10:                   // +0x376 → countMap[8]
                v = countMap[8];
                countMap[8] = v + 1;
                break;
            }
        }
    }

    int privateStoreSize =
        (int)private_store::GetInstancePrivateStoreMgr()->GetPrivateStoreSize();
    if (0 < privateStoreSize)
    {
        countMap[0] = countMap[0] - privateStoreSize;
        countMap[7] = privateStoreSize;
    }

    // [b28d-solo-GameWorld] P13+P167+P283：ORIG 0x086cb471 begin(-0x84) 之后立即
    // 0x086cb486 `end`（-0x80，循环前缓存；条件块只用缓存值）→ `jmp 条件块`（P283
    // 底部条件布局）；增量 ppEv（前缀）⇒ `++cit`；`(*cit)` 选 deEv（P167）。
    std::map<int, int>::iterator cit = countMap.begin();
    std::map<int, int>::iterator last = countMap.end();
    goto COND3;
LOOP3:
    {
        int type = (*cit).first;
        int count = (*cit).second;
        // ORIG 0x086cb4d1 段：Acquire 结果经 %eax 直作 CStreamGuard 实参（无具名
        // stream 槽，P259）；具名指针局部多 store/load 对并使 guard 槽浅移。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x110f), true);
        **guard << 0x6a;
        **guard << -1;
        // LOGDB 0x6a 载荷（ORIG 0x086cb4f9..0x086cb54e：4 个连续 int，
        // +0x08 ←env+0x198 `mov 0x198(%eax),%eax`）。
        struct __attribute__((packed)) stChannelServiceInfo
        {
            int m_type;   // +0x00
            int m_zero;   // +0x04
            int m_gcNo;   // +0x08
            int m_count;  // +0x0c
        };
        stChannelServiceInfo buf;
        buf.m_type = type;
        buf.m_zero = 0;
        buf.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
        buf.m_count = count;
        guard->put_binary(&buf, 0x10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    ++cit;
COND3:
    // [b28d-sweep] P167 差集实测：ORIG 调 `_Rb_tree_iterator::ne`（MISSING ne /
    // EXTRA eq）⇒ 源形本应是 `cit != last`；但 `!=`/while 两形会让 4.4 把 ne 的
    // 比较临时槽放 -0x78（ORIG -0x7c）并使助记符序列错位，TU 四数 near 5→4 /
    // diff 14→15 回退 ⇒ 按「四数不回退」保留 `== last`（NEAR 309/309）。
    // 旧注：ORIG 0x086cb5e2 `jne <body>; jmp <exit>` ⇒ 反转条件 + 显式 return 出口
    // （4.4 对 `if (cit != last) goto LOOP3;` 恒产 `je exit; jmp body`）。
    if (cit == last)
    {
        return;
    }
    goto LOOP3;
}
// [b28d-solo-GameWorld] _ZN9GameWorld30UpdateStatistic4ChannelUserLevEv verdict=AE
// b28d-solo 修法（两处真修）：①P13 —— ORIG 0x086cd18b 先调 end()（last=-0x1c）、
// 0x086cd1a6 后调 begin()（it=-0x20），条件块 0x086cd20d 直接用 last 比较（无每轮
// end call）⇒ 原码缓存 end 迭代器（旧实现每轮 `it != m_UserInWorld.end()`；p167
// 多重集同计数故 CLEAR 掩盖了位置差）；②P0 —— `int level=get_charac_level();
// sig->m_count[level]++` 具名版多 `mov %eax,槽/mov 槽,%eax` 两条，ORIG 0x086cd1ec
// call 结果 %eax 直作 scaled 索引 ⇒ 内联表达式 `sig->m_count[get_charac_level()]++`。
// 余 N82/N93 槽组整体平移（guard/it/sig/info，帧 0x30 vs 0x40）不再试。
void GameWorld::UpdateStatistic4ChannelUserLev()
{
    // [b28d-solo-GameWorld] P259：Acquire 内联进 CStreamGuard 实参（消 stream 具名局部，
    // ORIG 0x086cd0b0 段无 stream 槽、帧 0x30 vs 具名形 0x50）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ab), true);
    **guard << 0x6b;
    **guard << -1;
    SIG_UPDATE_STATISTIC4LEV* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_STATISTIC4LEV>();
    memset(sig, 0, 0x124);
    sig->m_gcNo = G_CEnvironment()->m_serverEnvir.m_channelNo;   // +0x1b0（0x086cd158 movl）
    sig->m_time = GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-GameWorld] P50：ORIG begin sret 直落 -0x20（无 iterator::C1Ev + 拷贝）
    // ⇒ for-init 内直构，勿先声明后赋值。
    // b28d-solo 修：P13 —— ORIG 0x086cd18b 先调 end()（last=-0x1c）、0x086cd1a6 后调
    // begin()（it=-0x20），条件块 0x086cd20d 直接用 last 与 it 比较（无每轮 end call）
    // ⇒ 原码缓存 end 迭代器（旧实现每轮调 end，p167 多重集同计数故 CLEAR 掩盖）。
    std::map<unsigned short, CUser*>::iterator last = m_UserInWorld.end();
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != last; ++it)
    {
        CUserCharacInfo* info = it->second;
        if (info != NULL && info->getCurCharacR() != NULL)
        {
            // b28d-solo：ORIG 0x086cd1ec-0x086cd1fe 无 level 槽往返（call 结果 %eax
            // 直作 scaled 索引）⇒ P0 内联表达式（旧具名版多 mov %eax,槽/mov 槽,%eax 两条）。
            sig->m_count[info->get_charac_level()]++;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

int GameWorld::GetChannelUserCount(ENUM_SERVER_GROUP group, stOccStatisticByAge& age)
{
    int count = 0;
    // ORIG 0x86cb627: for-init 内声明迭代器（技巧25，begin sret 直落 -0x20 免默认ctor+拷贝）；
    // 0x86cb642 end sret=-0x1c(先)、0x86cb6d2 ppEi sret=-0x18(后)、ne 调用@0x86cb717
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        // ORIG 0x86cb655 user 具名槽 -0x10
        CUser* user = it->second;
        if (user != NULL)
        {
            // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
            // ORIG 0x86cb669-0x86cb671 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`。
            if (user->GetServerGroup() == group)
            {
                count++;                       // ORIG 0x86cb673
                unsigned int userAge = user->GetAge();  // ORIG 0x86cb682 槽 -0xc；0x86cb689 `jbe`=unsigned ⇒ 技巧5
                // ORIG 0x86cb685-0x86cb6a2 >99 分支 fall-through 在前（技巧17）
                if (userAge > 99)                        // ORIG 0x86cb685 cmpl $0x63; 0x86cb689 jbe=unsigned 比较
                {
                    age.m_age[99]++;
                }
                else if (userAge == 0)         // ORIG 0x86cb6a5
                {
                    age.m_age[0]++;
                }
                else
                {
                    age.m_age[userAge - 1]++;
                }
            }
        }
    }
    return count;
}

int GameWorld::_GetChannelType()
{
    if (G_CEnvironment()->m_serverEnvir.m_channelType <= 0x12)
    {
        return G_CEnvironment()->m_serverEnvir.m_channelType;
    }
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG 0x086cXXXX `mov 0x1b0(%eax),%eax; mov %eax,%ebx`：channelNo 直落
    // callee-saved %ebx（技巧2 register 同型成员单 mov），免 -0x10 槽、帧对齐
    register unsigned int channelNo =
        G_CEnvironment()->m_serverEnvir.m_channelNo;
    channel_info_t* info = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        channelNo);
    if (info == NULL)
    {
        return 0x13;
    }
    return info->m_channelType;
}

int GameWorld::GetChannelType() const
{
    return m_channelType;
}

// [b28d-solo-GameWorld] 本轮 30+ 形穷举（register one=1u<<ct / one<<=ct / one 预清 /
// 字段 unsigned / 冗余 cast / while+break / for(;;) / 语句表达式 / 逗号 / const register /
// unsigned long ct / 双条件合并 / 提前 return）实测 4.4.6 恒将 ct 落 %ebx 或触发 N6 折叠 @0x080f131e
// [b28d-solo-GameWorld] _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv verdict=DIFF ndiff=2
// N96/N7 封顶：ORIG 4.1 register ct 留 %eax（0x080f1322 装载一次供 cmp/shl 复用），
// 4.4.6 恒落 %ebx（多 mov %eax,%ebx + mov %ebx,%eax）；表达式直读形触发 N6 折叠
// （shr/and $1）反劣，register 形为最优可接受形。
bool GameWorld::IsIntegratedPvPBaseChannel() const
{
    // ORIG _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv @0x080f131e：
    // 0x080f1322/0x080f1325 装载 m_channelType 一次，0x080f1328 cmp、
    // 移位量复用同一 %eax。b28c-lane3：历史 register asm("eax")/asm("edx")
    // 绑定按 P0/N96 清除，改普通 register 局部——4.4 无栈槽且阻断
    // `(1<<t)&C`→`(C>>t)&1` 位折叠（N6），单次装载形态保持。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x1e700)
            return true;
    }
    return false;
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证（ORIG 4.1 ct 留 %eax 供 cmp/shl 复用）@0x081424e8
// [b28d-solo-GameWorld] _ZNK9GameWorld12IsPVPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族：ORIG ct 留 %eax @0x081424e8，4.4.6 落 %ebx；余 2 regmove，不可造）。
bool GameWorld::IsPVPChannel() const
{
    // ORIG _ZNK9GameWorld12IsPVPChannelEv @0x081424e8：m_channelType 只装载一次
    // （0x081424ec `mov 0x8(%ebp),%eax` + 0x081424ef `mov 0x20(%eax),%eax`），
    // 0x081424f2 cmp 与 0x081424fe `mov %eax,%ecx`（移位量）复用同一 %eax
    // （0x081424f5 ja / 0x081424f7 `mov $0x1,%edx` / 0x081424fc `mov %edx,%ebx`
    //  / 0x08142500 `shl %cl,%ebx` / 0x08142502 `mov %ebx,%eax`）；
    // 4.4 默认在 `one << m_channelType` 处再装载一遍 ⇒ 多 2 条。
    // b28c-lane3：历史 register asm("eax")/asm("edx") 绑定按 P0/N96 清除；
    // 普通 register 局部（4.4 无栈槽）阻断 N6 位折叠并保持单次装载。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        // ORIG 0x081424f7 `mov $0x1,%edx` + 0x081424fc `mov %edx,%ebx` 是
        // 4.1 的 scratch→目标两段形态；one 走 register 局部即复现。
        // b28d-lane11 判词 N96/N6：清绑后残余仅 `mov %eax,%ebx`（cmp 前）与
        // `mov %ebx,%eax`（shl 前）两条中转——4.4 把 register ct 落 %ebx，
        // ORIG ct 留 %eax 不可造；试形单 if 短路（合并两条件）触发 N6 折叠
        // 反而劣化 ndiff 4→12（shr/and $1），故维持嵌套 if + register one。
        register unsigned int one = 1u << ct;
        if (one & 0x1e7f0)
            return true;
    }
    return false;
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x081a0ab2
// [b28d-solo-GameWorld] _ZNK9GameWorld16IsFreePvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x081a0ab2，余 2 regmove）。
bool GameWorld::IsFreePvPChannel() const
{
    // ORIG _ZNK9GameWorld16IsFreePvPChannelEv @0x081a0ab2（同 0x081424e8 族布局）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x166a0)
            return true;
    }
    return false;
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x085dfa92
// [b28d-solo-GameWorld] _ZNK9GameWorld20IsWinPointPvPChannelEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x085dfa92，余 2 regmove）。
bool GameWorld::IsWinPointPvPChannel() const
{
    // ORIG _ZNK9GameWorld20IsWinPointPvPChannelEv @0x085dfa92（同族，上界 0xf）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0x8110)
            return true;
    }
    return false;
}

// [b28d-solo-GameWorld] 同上 30+ 形穷举实证 @0x082343c8
// [b28d-solo-GameWorld] _ZN9GameWorld21IsSameTeamChannelJoinEv verdict=DIFF ndiff=2 N96/N7 封顶
// （同族 @0x082343c8，余 2 regmove）。
bool GameWorld::IsSameTeamChannelJoin()
{
    // ORIG _ZN9GameWorld21IsSameTeamChannelJoinEv @0x082343c8（同族，非 const）
    // b28c-lane3：清 register asm 绑定（P0/N96）。
    register unsigned int ct = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one = 1u << ct;
        if (one & 0xc200)
            return true;
    }
    return false;
}

bool GameWorld::IsSchoolPvPChannel() const
{
    if ((unsigned int)(m_channelType - 9) <= 1) return true;
    return false;
}

bool GameWorld::IsPvPSkilTreeChannel() const
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
}

bool GameWorld::IsEquipSlotSwitchChannel() const
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
}

bool GameWorld::IsPvPVillageMapChannel() const
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
}

bool GameWorld::IsCharacterPvPExpRevisionChannel() const  // ORIG 085dfa76
{
    if (m_channelType == 0xf) return true;
    return false;
}

bool GameWorld::IsCharacterLevelRevisionChannel() const  // ORIG 084ed128
{
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;  // ∈ [0xe, 0x10]
    return false;
}

bool GameWorld::IsCharacterStatRevisionChannel() const  // ORIG W 086977da
{
    // sub $0xe / cmp $0x2 / ja：m_channelType ∈ [0xe,0x10] 返回 true
    if ((unsigned int)(m_channelType - 0xe) <= 2) return true;
    return false;
}

bool GameWorld::is_dungeon_tag_matching_channel(char* channel)
{
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG 0x86cbc1a `mov %eax,%ebx`：channelNo 直落 callee-saved %ebx（技巧2）
    register unsigned int channelNo = G_CEnvironment()->get_channel_no();
    channel_info_t* info = channelScript->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        channelNo);
    if (info == NULL)
    {
        return false;
    }
    // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
    // ORIG 0x86cbc6a-0x86cbc71 `test %eax; sete %al; test; je`。
    if (strcmp(channel, info->m_dungeonName.c_str()) == 0)
    {
        return true;
    }
    return false;
}

// ============================================================================
// 在线用户
// ============================================================================
void GameWorld::InsertLoginUser(CUser* user)
{
    m_loginUserMap.insert(
        std::make_pair<unsigned int, CUser*&>(user->get_acc_id(), user));
}

void GameWorld::EraseLoginUser(CUser* user)
{
    std::map<unsigned int, CUser*>::iterator it =
        m_loginUserMap.find(user->get_acc_id());
    if (it != m_loginUserMap.end())
    {
        m_loginUserMap.erase(it);
    }
}

bool GameWorld::InsertUserToIDMap(const char* name, unsigned int id)
{
    // b28d-lane11：P133 拆独立守卫（ORIG 0x086cd7a8 `jne body` + 0x086cd7c3
    // `jbe body2` 两条独立直跳，合并短路形生 setne 汇聚）。
    if (name == NULL)
    {
        return false;
    }
    if (strlen(name) > 0x1e)
    {
        return false;
    }
    std::string strName(name, std::allocator<char>());   // ORIG +0x038 外联 allocator ctor（试复现）
    // b28d-lane11 判词 N82：104=104 助记符全同，残余仅槽组方向——ORIG 0x086cd81a
    // find sret=-0x18 / end sret=-0xc（it 深 last 浅，4.1 声明序）vs 4.4 首次使用序
    // （it=-0x14/last=-0x18）；allocator 临时 -0xd vs -0x9 同族。声明序/两语句序
    // 均不可翻（make_unique_id 同判例）。
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);   // +0x098
    // [b28d-solo-GameWorld] _ZN9GameWorld17InsertUserToIDMapEPKcj verdict=AE
    // （内联 end() 恢复 find 先/end 后调用序 + end sret 浅槽，P220/P293）
    // b28d-solo-GameWorld：ORIG 0x086cd611 end sret 落浅槽 -0xc（匿名临时），
    // 具名 last 会被 4.4 排到深槽 -0x18 ⇒ 内联 end() 保留 find 先/end 后调用序。
    // b28d-lane11：P198② 谓词整体内联（ORIG +0x0c8/+0x0d7 `test %al,%al` 直测
    // 寄存器，具名 bool 局部反多 mov/cmp 落槽两步）。
    if ((it != m_idMap.end()) && (it->second.m_id != 0))
    {
        return true;
    }
    m_idMap[strName].m_id = id;
    return true;
}

bool GameWorld::DeleteUserInIDMap(const char* name)
{
    if (name == NULL)
    {
        return false;
    }
    if (!(strlen(name) < 0x1f))
    {
        return 0;
    }
    {
        std::string strName(name);
        std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
        // b28d-lane11：P198② 去具名 bool（ORIG +0x0dc `test %al,%al; je` 寄存器
        // 直测，具名 found 反落槽并撑帧 0x20→0x30）。
        if (it == m_idMap.end())
        {
            return false;
        }
        m_idMap.erase(it);
        return true;
    }
    return false;
}

int GameWorld::GetIDUserInIDMap(const char* name)
{
    if (name == NULL)
    {
        return 0;
    }
    if (!(strlen(name) < 0x1f))
    {
        return 0;
    }
    {
        std::string strName(name);
        std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
        // ORIG 0x86cd8af-0x86cd8d4：eq 真→mov$0(fall)，false→je 前跳 operator[]
        // `mov (%eax),%ebx` 单步结果 ⇒ ==end 早退形
        if (it == m_idMap.end())
        {
            return 0;
        }
        // ORIG 0x86cd8d2 `mov (%eax),%ebx` 单步 vs 4.4 `mov (%eax),%eax; mov %eax,%ebx`
        // = N18a call 后返回值中转不可造（asm("ebx") 绑定实测同形），残留 1 行
        return m_idMap[strName].m_id;
    }
}

// [b28d-sweep] **真语义 bug 修复**：私商分支虚调用目标还原为真实虚成员调用
// `GetInstancePrivateStoreMgr()->Enter(user)`（P5/P113）。ORIG 0x086c5212
// `mov (%eax),%edx; add $0x4,%edx; mov (%edx),%ecx; call *%ecx` = IUserPos 槽1；
// CPrivateStoreMgr vtable 0x8cbeb38 槽0=Exit(0x85ca994)/槽1=Enter(0x85caa8a)。
// 旧实现 `((void**)storeMgr)[1]` 取对象 +0x04 成员值当函数指针 = 真 bug。
// 271/273 → **271/271**（slotnorm 零残差）；p167 46/46（仅 C1/C2 别名）。
// [b28d-solo-GameWorld] _ZN9GameWorld16reach_game_worldEP5CUser verdict=DIFF 271/271
// 本轮修法（在既有 P50/P61/P192/try-catch 之上）：①P50 声明点 = 发射点 —— 两个
// iterator 声明移到 key（uniqueId/accId）**之前**，复现 ORIG 0x086c4ea0/0x086c4f5b
// 的 C1Ev 先于 get_unique_id/get_acc_id；②P227 全表达式临时链 —— getUserVillage
// 失败分支 cMyTrace 实参 getCurCharacVill 在 ctor 前求值存 %ebx（ORIG 0x086c5185）；
// ③**真语义 bug 修正**：私商分支虚调用改真实虚成员调用（见上方 b28d-sweep 判词）。
// 余 N82：槽组平移（slotnorm 零残差）。p167 46/46（C1/C2 别名对）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c4ea0/0x086c4f5b iterator C1Ev 先声明后赋值）+ P61/P236 具名 key
// （ORIG uniqueId=-0x4a/accId=-0x44/-0x28）+ P192 value_type 直构 +
// **真 try/catch 补缺**（ORIG 0x086c50fb `__cxa_begin_catch` + 0x086c510a
// `strerror(errno)` + 0x086c513a logFormat 0x52d + 0x086c5144 `__cxa_end_catch`，
// 原先缺失 ⇒ p167 MISSING）+ 具名 size/maxUser（-0x10/-0xc）。
bool GameWorld::reach_game_world(CUser* user)
{
    if (user == NULL)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x509,
                              "param pUser is null");
        return false;
    }

    errno = 0;
    // ORIG 0x086c4ea0 iterator C1Ev（**先于** get_unique_id，P50 声明点即发射点）
    // + key 具名 -0x4a + find(&key)。
    std::map<unsigned short, CUser*>::iterator userIt;
    unsigned short uniqueId = (unsigned short)user->get_unique_id();
    userIt = m_UserInWorld.find(uniqueId);
    if (userIt != m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x515,
                              "m_UserInWorld.find() fail. uid(%u)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    // ORIG 0x086c4f5b iterator C1Ev（先于 get_acc_id）+ accId 具名 -0x44 + find(&key)
    std::map<unsigned int, unsigned short>::iterator sessionIt;
    unsigned int accId = user->get_acc_id();
    sessionIt = m_sessionList.find(accId);
    if (sessionIt != m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x51d,
                              "session_list.find() fail accid(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    // ORIG 0x086c5032/0x086c5040 具名 size(-0x10)/maxUserNum(-0xc)
    int worldSize = (int)m_UserInWorld.size();
    int maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    if (worldSize + 1 > maxUserNum)
    {
        return false;
    }

    // ORIG 0x086c50f8..0x086c5149 真 try/catch（landing pad → __cxa_begin_catch
    // → strerror(errno) → logFormat 0x52d → __cxa_end_catch → return false）。
    try
    {
        // ORIG 0x086c505e 第二次 get_unique_id 存 -0x2a 供 insert；0x086d3860 直接
        // 调 pair<const ushort,CUser*>::pair<ushort,CUser*&>(ushort&&,CUser*&)
        // （make_pair 被内联）⇒ value_type 直构（P192）。
        m_UserInWorld.insert(std::map<unsigned short, CUser*>::value_type(
            (unsigned short)user->get_unique_id(), user));
        m_sessionList[user->get_acc_id()] = (unsigned short)user->get_unique_id();
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x52d,
                              "[EXCEPTION insert_user In GameWorld] - code = %s\n",
                              strerror(errno));
        return false;
    }

    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        user->set_area(village->get_gate_area());
    }
    else
    {
        // ORIG 0x086c5185 getCurCharacVill 在 ctor **之前**求值存 %ebx（N80 族），
        // P227 全表达式临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x538, 5)(
            "getUserVillage %d", (int)user->getCurCharacVill());
    }

    user->set_state((ch_state)3);
    EraseLoginUser(user);
    send_AllBasicInfo(user);
    if (user->IsPermissionPrivateStore())
    {
        // ORIG 0x086c520d 单次 GetInstancePrivateStoreMgr（eax 复用为 this）。
        // [b28d-sweep] 真虚调用（P5/P113）：ORIG `mov (%eax),%edx; add $0x4,%edx;
        // mov (%edx),%ecx; call *%ecx` = IUserPos 槽1；CPrivateStoreMgr vtable
        // 0x8cbeb38 槽0=Exit(0x85ca994) / 槽1=Enter(0x85caa8a) ⇒ 目标是 Enter。
        // 旧实现 `((void**)storeMgr)[1]` 取的是对象 +0x04 成员值（非 vtable 槽），
        // 会把成员数据当函数指针调用 = 真语义 bug。
        // P0/P52：结果直作 receiver（ORIG 0x086c520d 单次 call 后 %eax 复用为 this，
        // 无具名 storeMgr 槽）。
        private_store::GetInstancePrivateStoreMgr()->Enter(user);
    }
    else
    {
        private_store::GetInstancePrivateStoreMgr()->LoadPrivateStore(user);
    }
    if (user->isHangameUser())
    {
        m_hangameUserCount++;
    }
    else
    {
        m_userCount++;
    }
    return true;
}

// [b28d-sweep] 298/296 → **298/294**：g_guildAgitMgr 对象化（原指针建模 ⇒ 调用点
// `mov g,%eax; mov %eax,(%esp)`；ORIG 0x086c55e5 `movl $0x943e0e0,(%esp)`）。
// 余 N7/N104：get_area 结果 %esi vs %ebx + EH 尾块相对序。
// [b28d-solo-GameWorld] _ZN9GameWorld16leave_game_worldEP5CUser verdict=DIFF 298/294
// 本轮修法：P50 声明点 = 发射点 —— 两个 iterator 声明移到 key（uniqueId/accId）
// 之前，复现 ORIG 0x086c539c 段 C1Ev 先于 get_unique_id/get_acc_id（旧序差 2 块）。
// 余 N7/N82：get_area 结果 %esi vs %ebx（ORIG 保 %esi 供 check_valid_area 第2实参）
// + 槽组平移；p167 CLEAR 54=54。
// 既有修法（保留）：P50 复活
// （两个 iterator C1Ev 先声明后赋值；p167 MISSING 实证）+ 具名 key + **真 try/catch
// 补缺**（ORIG 0x086c568f `__cxa_begin_catch` + 0x086c569e `strerror(errno)` +
// 0x086c56ce logFormat 0x5a7 "[EXCEPTION Leave GameWorld] - code = %s" +
// 0x086c56d8 `__cxa_end_catch`；原实现缺 try ⇒ p167 MISSING）。
bool GameWorld::leave_game_world(CUser* user)
{
    if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()))
    {
        user->SetGameMasterMode(false);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
    }

    // P50 复活：ORIG 迭代器 C1Ev 在 key 求值之前发射（声明点即发射点）。
    std::map<unsigned short, CUser*>::iterator userIt;
    unsigned short uniqueId = (unsigned short)user->get_unique_id();
    userIt = m_UserInWorld.find(uniqueId);
    if (userIt == m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x56a,
                              "m_UsersInWorld.find(%d)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    // ORIG 0x086c539c 再次 get_unique_id 存 -0x1e 后 erase(&key)（非复用 uniqueId）
    m_UserInWorld.erase((unsigned short)user->get_unique_id());

    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        Village* village = getUserVillage(user);
        if (village != NULL)
        {
            Area* area = village->getArea(user->get_area(false));
            if (area != NULL)
            {
                area->erase_user(user);
            }
        }
    }

    PacketGuard packet;
    packet.put_header(0, 6);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.finalize(true);
    send_all(packet);

    std::map<unsigned int, unsigned short>::iterator sessionIt;
    unsigned int accId = user->get_acc_id();
    sessionIt = m_sessionList.find(accId);
    if (sessionIt == m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x588,
                              "session_list.find(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    m_sessionList.erase(user->get_acc_id());
    // ORIG 0x086c568f..0x086c56d8 真 try/catch（[EXCEPTION Leave GameWorld]）。
    try
    {
        g_schoolMgr.DelUser(user);
        if (user->get_charac_guildkey() != 0)
        {
            g_guildAgitMgr.ReleaseGuildAgitArea(user->get_charac_guildkey());
        }
        if (user->isJoinPowerWar())
        {
            GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        }
        if (user->isHangameUser())
        {
            m_hangameUserCount--;
        }
        else
        {
            m_userCount--;
        }
    }
    catch (...)
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x5a7,
                              "[EXCEPTION Leave GameWorld] - code = %s",
                              strerror(errno));
        return false;
    }
    return true;
}

unsigned int GameWorld::make_unique_id()
{
    // ORIG 0x86c4b0a: 入口 `jmp 0x86c4b13`；回跳落点 = 空块 nop@0x86c4b12（技巧24 goto 落点岛）
    goto COND;
LOOP:
    ;
COND:
    m_uniqueId++;                              // ORIG 0x86c4b13 前缀 ++（技巧4 半字版）
    if (m_uniqueId == 0xffff)                  // ORIG 0x86c4b2b cmp $0xffff,%ax
    {
        m_uniqueId = 0;
    }
    // ORIG 调用序 = 0x086c4b57 `map::find` → 0x086c4b6f `map::end` → eq：
    // 合并成一句 `find(...) == end()` 时 4.4 把 end() 排到前面（实测首个 call
    // 变 end）⇒ 拆成两个具名 iterator 局部、按求值序声明钉住 find 先于 end。
    // b28d-lane11 判词 N82（0x086c4b49..0x086c4b84 实证）：ORIG 4.1 按声明序
    // 先深（it=-0x10、last=-0xc）；4.4 按首次使用序先浅（it=-0xc、last=-0x10），
    // find/end 两处 sret lea 与 operator== 的 this/实参随之互换。试形：逗号同句
    // 声明（实测同形）、直接初始化、声明/赋值分离（多 emit iterator 默认构造
    // call+copy 共 5 条）均不可翻 ⇒ 双 sret 迭代器槽互换封 N82/N98。
    // ORIG 尾 `test %al,%al; je 0x086c4b12` ⇒ 用 eq（真则 fall-through 到 return）。
    // [b28d-solo-GameWorld] _ZN9GameWorld14make_unique_idEv verdict=NEAR 45/45
    // N82/N98 封顶（b28d-solo 本轮归一化复核：助记符/操作数序列全等，仅双 sret
    // 迭代器槽互换）：ORIG 0x86c4b49 lea -0x10(it)/0x86c4b68 lea -0xc(last) 先深，
    // 4.4 先浅 -0xc/-0x10；operator== 两侧 this/实参随之互换（语义等价）。
    // 本轮复测：逗号同句声明/拆两句/声明赋值分离（多 C1Ev）三形均不翻（N98 定案）。
    std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.find(m_uniqueId);
    if (it == std::map<unsigned short, CUser*>::iterator(m_UserInWorld.end()))
    {
    }
    else
    {
        goto LOOP;
    }
    // b28c-lane3：历史 register asm("eax") 绑定按 P0/N96 清除。
    // ORIG 0x086c4b8f-0x086c4b97：`mov 0x8(%ebp),%eax; movzwl 0x24(%eax),%eax;
    // movzwl %ax,%eax` 双零扩展；返回点直读字段的自然形保留该两段。
    return m_uniqueId;
}

int GameWorld::find_session(unsigned int accId)
{
    std::map<unsigned int, unsigned short>::iterator it;
    it = m_sessionList.find(accId);
    if (it == m_sessionList.end())
    {
        return 0;
    }
    // b28c-lane3：历史 register asm("ax") 绑定按 P0/N96 清除。
    // ORIG 0x86c4ca0-0x86c4ca4 双 movzwl（movzwl 0x4(%eax),%eax; movzwl %ax,%eax）。
    return it->second;
}

CUser* GameWorld::find_from_world(unsigned short uniqueId)
{
    std::map<unsigned short, CUser*>::iterator it;
    it = m_UserInWorld.find(uniqueId);
    if (it == m_UserInWorld.end())
    {
        return NULL;
    }
    return it->second;
}

CUser* GameWorld::find_user_from_world_byaccid(unsigned int accId)
{
    // ORIG 0x86c4d46 `movl $0x0,-0xc(%ebp)` = user 函数头声明置空（后覆写）；
    // 两处判空为 `cmpl $0,slot; sete %al; testb %al; je` 物化 ⇒
    // b28d-lane11：历史 register bool asm("al") 绑定按 P0/N96 清除；自然形
    // 具名 bool 保留（ORIG sete 物化系 4.1 槽比零产物，4.4 恒直跳 ⇒ N18a 封顶）。
    unsigned int session;
    CUser* user = NULL;
    // [b28d-solo-GameWorld] _ZN9GameWorld28find_user_from_world_byaccidEj：去具名
    // bool bad（落槽 -0x9 并使 session/user 槽整体下移 4）⇒ 内联谓词恢复
    // session=-0x10 / user=-0xc 与 ORIG 一致；ORIG 的 `sete %al; test` 为 4.1 槽比
    // 零产物（4.4 恒 cmp/je 直跳，P197④ 不可造）。
    // [b28d-solo-GameWorld] _ZN9GameWorld28find_user_from_world_byaccidEj verdict=AE 47=47
    // 修法：两处判空改 `while(cond){body;break;}`（P225/P295）⇒ 4.4 复现 ORIG
    // `cmpl $0,槽; sete %al; test %al,%al; je` 物化（原 if 形恒折直跳，43 vs 47）。
    session = find_session(accId);
    // [b28d-solo-GameWorld] P225/P295：while(cond){body;break;} 复现 ORIG 槽比零
    // 的 sete/test/je 物化（4.4 对 if 槽比恒折 cmp/je 直跳）。
    while (session == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            0x4f2, "session_list.find() fail accid(%s)",
            NumberToString(accId, 0));
        break;
    }
    user = find_from_world((unsigned short)session);
    while (user == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            0x4f5, "m_UserInWorld.find() fail uid(%d)", session);
        break;
    }
    return user;
}

CUser* GameWorld::find_user_from_world_byaccid_loop(unsigned int accId)
{
    // ORIG 0x86c4cb0-0x86c4d03：begin sret 直落 -0x10、++it 前缀 opEv（无 ppEi temp）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        if (it->second->get_acc_id() == accId)
        {
            return it->second;
        }
    }
    return NULL;
}

 CUser* GameWorld::find_user_by_charac_name(const char* name)
 {
     // ORIG 0x86c9… 族：for-init 声明（begin sret 直落槽）；四个子条件各自
     // 独立物化（setg / test;setne;test / sete;test）⇒ 嵌套单层 if 形，
     // 单层 && 链在 4.4 会被折叠成直跳
     for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
          it != m_UserInWorld.end(); it++)
     {
         CUser* user = it->second;
         if (user != NULL)
         {
             if (user->get_state() > 2)
             {
                 if (user->getCurCharacR() != NULL)
                 {
                     if (strcmp(user->getCurCharacName(), name) == 0)
                     {
                         return user;
                     }
                 }
             }
         }
     }
     return NULL;
 }

// [b28d-solo-GameWorld] _ZN9GameWorld13arrange_usersEv verdict=AE 119=119
// b28d-solo 修法：P221/P227 —— 具名 `cMyTrace trace(...); trace(...)` 改全表达式
// 临时链 `cMyTrace(...)(...)`（NumberToString 实参在 ctor 前求值并跨 ctor 保活 %ebx）
// ⇒ 4.4 复现 ORIG it=-0x2c/trace=-0x24 槽方向（旧具名形 trace 恒占最深 -0x2c 而
// it 落 -0x1c，N82附 封顶判词作废）。P0 内联 env 表达式（ORIG env 无槽往返）保留。
void GameWorld::arrange_users()
{
    // [b28d-solo-GameWorld] P283 已试（goto 尾条件复现 ORIG `jmp COND` 形）：
    // NDIFF 8→9 反劣，槽集合仍不可对齐（ORIG it=-0x2c/end=-0x28/ppEi=-0x14；
    // 4.4 聚合对象 trace 恒占最深 -0x2c、迭代器落 -0x1c/-0x18/-0x14）⇒ 回 for 形。
    // N82附 封顶：4.4 聚合对象优先占低槽 vs 4.1 按声明序，槽组整体平移。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // ORIG +0x03f~+0x05a：NumberToString 先于 cMyTrace ctor，结果跨 ctor 缓存 %ebx（P87）
            // b28d-solo：P227/P221 全表达式临时链（实参在 ctor 前求值、%ebx 跨 ctor 保活，
            // 且 trace 不占具名槽）试翻 ORIG it=-0x2c/trace=-0x24 的槽方向。
            cMyTrace(__PRETTY_FUNCTION__, 0x10a6, 0)("GameWorld::arrange_users ACCID: %s",
                NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)7, 1, 0);
        }
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
        while (index < GlobalData::s_statistic_proxy_mgr->GetEndIndex())
        {
            // [b28d-solo-GameWorld] P0：ORIG env 无槽往返（call G_CEnvironment 结果直压实参）
            // ⇒ 内联表达式，勿具名局部。
            DeleteChannel(G_CEnvironment()->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = GlobalData::s_statistic_proxy_mgr->GetNextIndex(index);
        }
    }
    else
    {
        // ORIG +0x191~+0x196：mov 0x198(%eax) 字段直读（非 get_gc_no 调用）
        DeleteChannel(G_CEnvironment()->m_serverEnvir.m_gcNo, E_SERVER_GROUP_NONE);
    }
    G_CEnvironment()->set_suspend_state(true);
}

int GameWorld::CheckUserCount(int add)
{
    int size = m_UserInWorld.size();
    int maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    // ORIG mov 0xc(=add),%eax 先载：左操作数=add 决定 lea 寄存器分配
    if (size + add > maxUserNum)
    {
        return false;
    }
    return true;
}

int GameWorld::GetUserCount(int vill, int area) const
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return 0;
    }
    return m_pVill[vill].GetUserCount(area);
}

int GameWorld::get_UserCount_InWorld()
{
    return (int)m_UserInWorld.size();
}

// [b28d-solo-GameWorld] _ZN9GameWorld17UpdateNatTypeUserEv verdict=NEAR 173/174
// （b28d-solo 本轮；仅 4.4 循环体前 1 条对齐 nop 差，助记符/操作数全等）。
// 修法：①P283 第一循环 goto 尾条件（ORIG 0x086cd324 `jmp COND`）；②P28 增量
// ppEv 前缀 ⇒ `++it`；③P259 Acquire 内联进 CStreamGuard 实参（0x086cd3f6 段无
// stream 具名槽）+ 具名 type/count（ORIG -0x14/-0x10）；④**第二循环改 for 形**
// —— ORIG 0x086cd52f `test %al; jne <LOOP2>; jmp <exit>`，旧 goto 形 4.4 恒产
// `je exit; jmp body`（极性反）；for 形逐条复现（含每轮 end 调用位置）。
// 余 N112（4.4 循环出口对齐 nop，TU 旗标不可改）；p167 CLEAR 30=30。
void GameWorld::UpdateNatTypeUser()
{
    // [b28d-solo-GameWorld] P232：ORIG 槽基比我们深 4（ORIG natMap=-0x44/guard=-0x54，
    // 我们 -0x40/-0x50）⇒ 4 字节死局部撑帧下移槽基（P190 double 过头：帧 0x60→0x70）。
    char framePad[4];
    (void)&framePad;
    std::map<int, int> natMap;
    // ORIG 0x086cd31c begin(-0x48) → 0x086cd324 `jmp 条件块`（P283）
    std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
    goto COND;
LOOP:
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // b28d-solo：natType 槽旋钮四形实测（块内 -0x50 / 头 natMap 后 -0x48 /
            // 头 natMap 前 -0x30 / 头 it 后 -0x4c）均不合 ORIG -0x28；块内形保住
            // natMap=-0x44/it=-0x48 与 ORIG 一致 ⇒ N82/N93 封顶（4.4 块级标量槽方向）。
            int natType = (int)user->m_network.get_nat_type();
            natMap[natType]++;
        }
    }
    ++it;
COND:
    if (it != m_UserInWorld.end())
    {
        goto LOOP;
    }

    int time = GlobalData::s_systemTime_.getCurSec();
    // ORIG 0x086cd3c8 begin(-0x4c) → 0x086cd3d0 `jmp 条件块`（P283）
    for (std::map<int, int>::iterator nit = natMap.begin();
         nit != natMap.end(); nit++)
    {
        // ORIG 0x086cd3e0/0x086cd3f0：nit->first/second 在 Acquire 之前读取并落
        // -0x14(type)/-0x10(count)；GetInBuffer 结果落 -0xc(sig)。
        int type = nit->first;
        int count = nit->second;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ef), true);
        **guard << 0x84;
        **guard << -1;
        SIG_NAT_TYPE_USER* sig = guard.operator->()->GetInBuffer<SIG_NAT_TYPE_USER>();
        // ORIG 0x086cd486/0x086cd48c `memset(sig,0,0xc)`（call 形，p167 MISSING 实证）
        memset(sig, 0, 0xc);
        sig->m_type = type;
        sig->m_count = count;
        sig->m_time = time;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
}

// ============================================================================
// 广播
// ============================================================================
void GameWorld::send_all(PacketGuard& packet)
{
    // ORIG 0x86c8c1a-0x86c8c35：无默认构造 call、begin sret 直落唯一迭代器槽
    // -0x18 ⇒ for-init 内声明初始化形（外声明+赋值会产 ctor call+双槽拷贝）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            user->Send(packet);
        }
    }
}

void GameWorld::send_all(PacketGuard& packet, CUser* user)
{
    if (user->IsCurCharacVisible())
    {
        send_all(packet);
    }
    else
    {
        user->Send(packet);
    }
}

void GameWorld::send_all_excepted_user(PacketGuard& packet, CUser* exceptUser)
{
    // ORIG @0x86c8d3x 族：begin sret 直落迭代器槽、无默认 ctor call ⇒ for-init 声明
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() >= 3 && user != exceptUser)
        {
            user->Send(packet);
        }
    }
}

void GameWorld::send_all_with_state(PacketGuard& packet, int state)
{
    // ORIG：同上，for-init 声明形（无 ctor call/无拷贝）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (state <= user->get_state())
        {
            user->Send(packet);
        }
    }
}

void GameWorld::send_all_dungeon_inout_message(int dungeonIdx)
{
    PacketGuard packet;
    packet.put_header(0, 0x127);
    packet.put_int(dungeonIdx);
    packet.put_byte((int)G_CDataManager()->m_serverParameter.isDungeonOpen(dungeonIdx));
    packet.finalize(true);

    int minLevel = getDungeonMinimumRequiredLevel(dungeonIdx);
    if (minLevel != -1)
    {
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); it++)
        {
            CUser* user = it->second;
            // ORIG 0x86c8…：两子条件各自 `setXX;test;je` 物化 ⇒ 单层 && 被 4.4
            // 折叠为直跳，唯嵌套 if 复现（子条件独立 if 形）
            if (user->get_state() > 2)
            {
                if (minLevel <= user->get_charac_level())
                {
                    user->Send(packet);
                }
            }
        }
    }
}

void GameWorld::send_all_dungeon_inout_message(int dungeonIdx, bool open)
{
    PacketGuard packet;
    packet.put_header(0, 0x127);
    packet.put_int(dungeonIdx);
    packet.put_byte(open);
    packet.finalize(true);

    int minLevel = getDungeonMinimumRequiredLevel(dungeonIdx);
    if (minLevel != -1)
    {
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); it++)
        {
            CUser* user = it->second;
            // ORIG 0x86c8…：两子条件各自 `setXX;test;je` 物化 ⇒ 单层 && 被 4.4
            // 折叠为直跳，唯嵌套 if 复现（子条件独立 if 形）
            if (user->get_state() > 2)
            {
                if (minLevel <= user->get_charac_level())
                {
                    user->Send(packet);
                }
            }
        }
    }
}

void GameWorld::send_user_dungeon_inout_message(CUser* user, int dungeonIdx,
                                                bool open)
{
    PacketGuard packet;
    packet.put_header(0, 0x127);
    packet.put_int(dungeonIdx);
    packet.put_byte(open);
    packet.finalize(true);
    user->Send(packet);
}

void GameWorld::send_to_area(int vill, int area, PacketGuard& packet)
{
    // ORIG 0x86c8168：各失败条件独立落岛 ⇒ void 早退形 + register bool 物化（技巧22）
    if (vill == 0 && area == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑（原 register bool asm("al")）。
    // verdict=AE（P198② 谓词内联 + 全 47=47 助记符对齐）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuard：P198② 谓词
    // 内联（ORIG 0x86c8189 `call size; cmp; setl %al; test; jne` 无槽往返）。
    if ((int)g_townScriptFileList.size() < vill)   // ORIG 0x86c8189 setl %al
    {
        return;
    }
    if (vill <= 0)                     // ORIG 0x86c8194 jle
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)       // ORIG 0x86c819e jge
    {
        return;
    }
    Village* pVillage = &m_pVill[vill]; // ORIG 0x86c81af 具名槽 -0xc
    if (area >= pVillage->m_areaCount) // ORIG 0x86c81bb jle
    {
        return;
    }
    if (area <= -1)                    // ORIG 0x86c81c1 js
    {
        return;
    }
    pVillage->m_pAreas[area].send_to_all(packet);
}

void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             CBelong* belong)
{
    // 同上族（ORIG 0x86c8… send_group 版）
    if (vill == 0 && area == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 谓词内联）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuardP7CBelong：
    // P198② 谓词内联（ORIG setl 无槽往返，具名 sizeBad 多 mov/cmpb 两条）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= 0)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    if (area >= pVillage->m_areaCount)
    {
        return;
    }
    if (area <= -1)
    {
        return;
    }
    pVillage->m_pAreas[area].send_group(packet, belong);
}

void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             std::vector<CUser*>& users)
{
    // 同上族（vector 版）
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 谓词内联）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_areaEiiR11PacketGuardRSt6vectorIP5CUserSaIS4_EE：
    // P198② 谓词内联（ORIG setl 无槽往返）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= -1)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    if (area >= pVillage->m_areaCount)
    {
        return;
    }
    if (area <= -1)
    {
        return;
    }
    pVillage->m_pAreas[area].send_to_all(packet, users);
}

void GameWorld::send_to_user(int uniqueId, PacketGuard& packet)
{
    CUser* user = find_from_world((unsigned short)uniqueId);
    if (user != NULL)
    {
        user->Send(packet);
    }
}

void GameWorld::send_to_vill(int vill, PacketGuard& packet)
{
    // ORIG 0x086c7f48：je 0x86c7ffb(vill==0 岛) / setl 0x86c7f67 / jle 0x86c7f76 / jge 0x86c7f84；
    // pVillage=-0x10、i=-0xc 具名槽（0x86c7f95/0x86c7f98）；
    // COND setg 物化 0x86c7ff2、body 尾 `cmp $-1; setne %al; test; je` 0x86c7fbe-0x86c7fc3
    if (vill == 0)
    {
        return;
    }
    // b28d-lane11：P0/N96 清绑。
    // verdict=AE（P198② 内联 size 谓词 + 去具名 goOn + idxOk 内联 setne）
    // [b28d-solo-GameWorld] _ZN9GameWorld12send_to_villEiR11PacketGuard：P198② 谓词
    // 内联（ORIG 0x086c7f67 setl 物化无槽往返，具名 sizeBad 反落槽 -0x15）。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= 0)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    int i = 0;
    // b28d-lane11：P0/N96 清绑。
    // [b28d-solo-GameWorld] 去具名 goOn（落槽 -0x9 使 pVillage/i 槽下移 4）⇒ 直接
    // 条件恢复 ORIG pVillage=-0x10/i=-0xc；ORIG 0x086c7ff2 `setg %al; test; jne` 系
    // 4.1 槽比物化（4.4 恒 cmp/jle 直跳，P197④ 不可造）。
    for (; (pVillage->m_areaCount > i); i++)
    {
        // [b28d-solo-GameWorld] idxOk 内联：ORIG 0x086c7fbe `call get_area_index;
        // cmp $-1,%eax; setne %al; test; je`（call 结果寄存器直比 ⇒ 4.4 可出 setne）。
        if (pVillage->m_pAreas[i].get_area_index() != -1)
        {
            pVillage->m_pAreas[i].send_to_all(packet);
        }
    }
}

// CBelong 唯一声明点=CSchoolMgr.h（b26 NL4 上移；ORIG send_group 0x86c93d 调
// vtable 槽0，`call *%edx; test %al,%al; je`）。
void GameWorld::send_group(PacketGuard& packet, CBelong* belong)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2 && belong->IsRight(user))
        {
            user->Send(packet);
        }
    }
}

void GameWorld::send_server_group(PacketGuard& packet, char group)
{
    // ORIG 0x86c90ba-0x86c90d5：begin sret 直落 -0x18 单槽，无默认 ctor call
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() >= 3 &&
            (int)user->GetServerGroup() == (int)group)
        {
            user->Send(packet);
        }
    }
}

// [b28d-11b] N：槽位差 4（N82）@0x086c878a
// [b28d-solo-GameWorld] _ZN9GameWorld22send_party_info_to_allEP6CPartyi verdict=AE 309=309
// 修法：P0/P197④ 内联 noTitle 谓词（去具名 bool 落槽）+ P41/P162 删冗余 (short) cast
// （GetPartyIndex 返回 int，ORIG 无 cwtl）。
void GameWorld::send_party_info_to_all(CParty* party, int flag)
{
    int memberCount = party->get_member_count();
    if (!(memberCount > 0))   // ORIG +0x016 cmpl $0/jg 取反极性
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0xc37, "GameWorld::send_party_info_to_all get_member_count() is %d",
            memberCount);
        return;
    }

    PacketGuard packet;
    packet.put_header(0, 9);
    packet.put_short(1);
    // [b28d-solo-GameWorld] P41/P162：ORIG 086c8822 `mov %eax,0x4(%esp)` 直传
    // （GetPartyIndex 返回 int，无 cwtl）⇒ 删冗余 (short) cast（4.4 恒多 1 条 cwtl）。
    packet.put_short(party->GetPartyIndex());
    packet.put_byte(flag);

    if (flag == 0 || flag == 1)
    {
        packet.put_byte((unsigned char)party->getTitleIndex());
        // [b28d-solo-GameWorld] P0/P197④：ORIG +0x0ec 第 2 次调用 getTitleIndex +
        // +0x0f1 `test %eax,%eax; sete %al; test %al,%al; je`（无槽）⇒ 去具名
        // noTitle（落槽 -0x25 多 2 条 mov/cmpb），内联谓词直判。
        if (party->getTitleIndex() == 0)
        {
            // ORIG getTitle×3 / strlen×2 逐访问点重取（P157）；第二 strlen 直落 %ebx（+0x12a，块作用域 register）
            packet.put_int(strlen(party->getTitle()));       // +0x100/+0x108/+0x117
            {
                register int titleLen = strlen(party->getTitle());   // +0x122/+0x12a → %ebx
                packet.put_str(party->getTitle(), titleLen);         // +0x137 getTitle 后于 strlen（P23）
            }
        }
        packet.put_byte((unsigned char)party->IsReturnUserParty());
        packet.put_byte((unsigned char)party->getUserMax());
        packet.put_short(party->getDungIndex());
        packet.put_byte((unsigned char)party->getDungDiffi());
        packet.put_byte((unsigned char)party->IsEventCharacParty());
    }

    if (flag == 0 || flag == 2)
    {
        int managerIdx = 0;
        for (int i = 0; i <= 3; i++)   // ORIG +0x2ad cmp $3（setle 底测形）
        {
            CUser* member = party->get_user(i);
            if (member != NULL)        // ORIG +0x21c：非 NULL 为 fall-through（块反转）
            {
                packet.put_short((unsigned short)member->get_unique_id());
                if (party->getManager() == member)
                {
                    managerIdx = i;
                }
                char sex = member->getSex();   // +0x25c 槽 spill（版本形）
                if (sex == -1)
                {
                    sex = 1;
                }
                packet.put_byte((int)sex);
            }
            else
            {
                packet.put_short(0xffff);   // +0x283
                packet.put_byte(0xff);
            }
        }
        packet.put_byte((unsigned char)party->IsReturnUserParty());
        packet.put_byte(managerIdx);
        packet.put_byte((unsigned char)party->is_quick_party());
        packet.put_byte((unsigned char)party->IsEventCharacParty());
    }

    if (flag == 0 || flag == 1 || flag == 2)
    {
        bool matching = is_dungeon_tag_matching_channel((char*)"[impossible]");
        packet.put_byte((unsigned char)matching);   // +0x34e..0x368 槽物化+movzbl 传参
        if (matching)
        {
            for (int i = 0; i <= 3; i++)
            {
                CUserCharacInfo* info = (CUserCharacInfo*)party->get_user(i);
                if (info != NULL)        // +0x393 非 NULL fall-through（反转）
                {
                    packet.put_byte(6);
                    for (int j = 0; j <= 5; j++)   // +0x403 cmp $5
                    {
                        packet.put_byte((int)G_CDataManager()->get_dimensionInout(j));
                        // ORIG +0x3e2 movzbl 0xeb9(零扩展) 后 +0x3ed movsbl %al（符号再扩展）⇒ (int)(char)
                        packet.put_byte((int)(char)info->getCurCharacR()->m_demensionInout[j]);
                    }
                }
                else
                {
                    packet.put_byte(0);   // +0x410
                }
            }
        }
    }

    packet.finalize(true);
    send_all(packet);
}

int _makeBasicInfoHeader(PacketGuard& packet)
{
    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    int index = packet.get_index();
    packet.put_short(0);
    return index;
}

// [L11·round1 判词更新] _ZN9GameWorld17send_AllBasicInfoEP5CUser：ndiff 33→**10**
// （172/172，p167 28=28 CLEAR）。本轮修法 = P221/P227 两处具名 `cMyTrace trace(f,l,v);
// trace(...)`（ORIG 无独立具名对象，实参在 ctor 前右起求值并跨 ctor 保活）改**全表达式
// 临时链** `cMyTrace(f,l,v)(...)`：帧与主槽组一次对齐（PacketGuard -0x24→-0x44 ✓、
// it -0x2c→-0x4c ✓、begin 临时 -0x5c ✓、cMyTrace 两对象 -0x38/-0x28 就位）。
// 残余 4 处（**不可造，N 封顶**，均附 ORIG 0x086c83fc 段实证）：
//  ① O[51:55] `make_basic_info` 实参装载寄存器 %eax vs %edx（ORIG lea→%eax 直压 0x4(%esp)
//     再装 info；4.4 需 %edx 中转）；
//  ②/③ 两个 cMyTrace 槽 -0x34/-0x24 vs 我方 -0x38/-0x28（差 4）；
//  ④ `m_UserInWorld.end()` sret 临时 ORIG -0x38 vs 我方 -0x18（差 0x20）。
// ②③④ 同根：4.1 在**序言**一次性分配匿名临时槽（end 临时最深），4.4 恒在**使用点**
// 分配（end 出现在循环条件 ⇒ 最浅）。实测两形均不翻：显式 `iterator(m_UserInWorld.end())`
// 拷贝临时形（ndiff 10→10）、操作数交换 `end() != it`（10→11）⇒ 判 N（P75/N82 匿名临时区）。
// 既有修法（保留）：P50 复活
// （ORIG 0x086c842a `_Rb_tree_iterator::C1Ev` 0x086d3826 + 0x086c8466 begin sret
// -0x5c → 拷贝进 it(-0x4c) ⇒ 先声明后赋值）+ P197③ info 预清（0x086c842f
// `movl $0x0,-0x10`）+ P283 goto 尾条件 + P171 照抄 error 分支 `jmp COND`
// （跳过 ++it，原码即如此）。
void GameWorld::send_AllBasicInfo(CUser* user)
{
    PacketGuard packet;
    int countIndex = _makeBasicInfoHeader(packet);
    int count = 0;

    std::map<unsigned short, CUser*>::iterator it;
    CUserCharacInfo* info = NULL;
    if (m_UserInWorld.size() != 0)
    {
        it = m_UserInWorld.begin();
        goto COND;
    LOOP:
        info = it->second;
        // ORIG 0x086c848a `cmpl $0,-0x10; je 086c859d`（info==0 前向跳到函数后部的
        // trace 块）⇒ 正极性 `if (info != NULL) {visible 处理} else {trace}` 布局。
        if (info != NULL)
        {
            if (info->IsCurCharacVisible())
            {
                int index = packet.get_index();
                if (((CUser*)info)->make_basic_info((char*)&packet, 0) != 1)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0xbe3, 5)(
                        "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbe3);
                }
                if (packet.getLastError() != 0)
                {
                    packet.set_index(index);
                    packet.put_short(countIndex, count);
                    packet.finalize(true);
                    user->Send(packet);
                    countIndex = _makeBasicInfoHeader(packet);
                    count = 0;
                    goto COND;
                }
                else
                {
                    count++;
                }
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xbf6, 5)(
                "[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6);
        }
        ++it;
    COND:
        if (it != std::map<unsigned short, CUser*>::iterator(m_UserInWorld.end()))
        {
            goto LOOP;
        }
    }

    packet.put_short(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
}

// [b28d-11b] 修法：具名局部 cur/remain + user 声明前置 + `while (user==NULL){goto L;}`+
// 尾标签 L:;（新范式：while+goto 复现 4.1 setcc 物化 + 尾岛）→ IDENTICAL 73 @0x086c8694
void GameWorld::send_buddy_conn_list(unsigned int accId, BuddyList* list, int count)
{
    CUser* user;
    BuddyList* cur = list;
    int remain = count;
    user = find_user_from_world_byaccid(accId);
    while (user == NULL)
    {
        goto L_end;
    }
    {
        PacketGuard packet;
        packet.put_header(0, 0x38);
        packet.put_byte(count);
        while (remain-- != 0)
        {
            packet.put_binary((char*)cur, 0x2a);
            // §4.3 例外保留地址形式：BuddyList 全仓仅前向声明（GameWorld.h），
            // 无任何字段建模证据；0x2a = ORIG 该记录步长（ORIG 逐条 put_binary
            // 后按 0x2a 递增指针），非普通成员裸偏移访问。
            cur = (BuddyList*)((char*)cur + 0x2a);
        }
        packet.finalize(true);
        user->Send(packet);
    }
L_end:;
}

// [b28d-11b] 修法：&& 链拆嵌套 if（P225 推广）79→74；残余 N：4.4 折叠条件链 + body 块序 @0x086c8008
void GameWorld::send_power_war_hp_of_vill(int vill)
{
    // verdict=AE（早退形 + pVillage 具名槽 + setl 极性翻正）
    // [b28d-solo-GameWorld] _ZN9GameWorld25send_power_war_hp_of_villEi：ORIG 0x086c8026
    // `cmp vill,%eax(size); setl %al; test; jne 尾` ⇒ 第二守卫是 `size < vill`（非
    // `vill <= size`，后者 4.4 产 setge 极性反）；四守卫均为早退形。
    if (vill == 0)
    {
        return;
    }
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= 0)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    // ORIG 0x086c804c-0x086c805b：`&m_pVill[vill]` 具名落槽 -0x10（P0），
    // 循环内每次经该槽取 m_areaCount/m_pAreas（非逐次重算 imul/lea）。
    Village* pVillage = &m_pVill[vill];
    PacketGuard packet;
    packet.put_header(0, 0xe3);
    int index = packet.get_index();
    packet.put_short(0);
    int count = 0;
    for (int i = 0; i < pVillage->m_areaCount; i++)
    {
        if (pVillage->m_pAreas[i].get_area_index() != -1)
        {
            pVillage->m_pAreas[i].make_power_war_hp_of_vill(packet, count);
        }
    }
    packet.put_short(index, count);
}

// ============================================================================
// 村庄 / 区域
// ============================================================================
Village* GameWorld::GetVillage(int vill)
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}

Village* GameWorld::getUserVillage(CUser* user)
{
    if (user == NULL)
    {
        return NULL;
    }
    int vill = (int)(signed char)user->getCurCharacVill();
    if (vill < 1 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}

Village* GameWorld::getUserVillage(int vill)
{
    if (vill < 1 || vill >= MAX_VILLAGE_NUM)
    {
        return NULL;
    }
    return &m_pVill[vill];
}

int GameWorld::get_gate_area(int vill) const
{
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return -1;
    }
    return m_pVill[vill].get_gate_area();
}

// [b28d-solo-GameWorld] _ZNK9GameWorld21get_area_center_pointEiiRiS0_ verdict=AE 80=80
// 修法：P169 外层正向 if 包内层 goto PASS（两失败路径共享后置 RET0 岛，见函数内注释）
// ⇒ 4.4 复现 ORIG `js RET0; mov MAX,%eax; cmp; jl PASS; RET0: mov $0; jmp end`。
bool GameWorld::get_area_center_point(int vill, int area, int& x, int& y) const
{
    // ORIG 0x86c4724-0x86c4732：`js RET0; mov MAX,%eax; cmp %eax,0xc; jl PASS`
    // 第二条件 taken=continue（技巧23 双 goto 钉块序）。
    // 关键：两条失败路径必须**共享同一个 RET0 岛**且该岛位于第二条件之后
    // （ORIG 0x86c4724 `js RET0` → 0x86c4729 `mov MAX,%eax` / 0x86c472e
    // `cmp %eax,0xc` / 0x86c4730 `jl PASS` → RET0: `mov $0x0,%eax; jmp end`）。
    // 写成 `if (vill < 0) { return false; }` 会让 4.4 在原地物化 `mov $0;jmp`
    // （多 2 条）；必须用 goto 把首条失败出口也指向后置于第二条件的 RET0 岛。
    // 变体：外层正向 if（假路径 fall-through 到尾 return），内层唯一 goto PASS
    // ⇒ 期望 ORIG 0x86c4724 `js 岛` + 0x86c4730 `jl PASS` + 岛 `mov $0;jmp end`
    //   且两 return 合并为同一物化块（上一版 `goto RET0` 会多留一条 jmp）。
    if (vill >= 0)
    {
        if (vill < MAX_VILLAGE_NUM)
            goto PASS;
    }
    return false;
PASS:
    // ORIG 0x86c4756 #1：getArea 结果不判空直 `add $0x7c` 调 empty()（每次内联重取，无具名 pArea）
    if (m_pVill[vill].getArea(area)->m_mapAreas.empty())
    {
        return false;
    }
    // b28c-lane3：历史 register bool asm("al") 绑定按 P0/N96 清除（自然形）。
    // ORIG 0x86c4791-0x86c4798 #2：`test %eax,%eax; sete %al; test %al,%al; je`。
    if (m_pVill[vill].getArea(area) == 0)
    {
        return false;
    }
    // ORIG 0x86c47d1 #3：at(0) 结果落具名引用槽 -0xc（引用必物化）
    MapArea& mapArea = m_pVill[vill].getArea(area)->m_mapAreas.at(0);
    x = mapArea.m_x + mapArea.m_w / 2;
    y = mapArea.m_y + mapArea.m_h / 2;
    return true;
}

// [b28d-11b] N：push ebx + 帧差（N82/P210）@0x086c4814
// [b28d-solo-GameWorld] _ZNK9GameWorld18is_available_pointEiiii verdict=AE 112=112
bool GameWorld::is_available_point(int vill, int area, int x, int y) const
{
    // [b28d-solo-GameWorld] _ZNK9GameWorld18is_available_pointEiiii：ORIG
    // 0x086c484c/0x086c4882/0x086c48b2 三次独立 getArea（P183/P157 逐访问点重取），
    // 第 1 检查 empty() 直判（无 NULL 守卫，P171 照抄）、第 2 检查 NULL、
    // 第 3 检查 `pAreas = &getArea(area)->m_mapAreas` 具名 -0x10 + `!pAreas->empty()`
    // xor 物化；循环 = 0x086c48e8 `jmp 条件块` + 尾 `!=` 回跳（P283），
    // `MapArea& ma = *it` 具名引用 -0xc。
    // ORIG 0x086c481a `cmpl $0,0xc; js L_ret0`（vill<0 有符号）+ 0x086c4828
    // `jl L_next`（vill<MAX）双跳共享 L_ret0 ⇒ 短路 OR 共享出口形。
    if (vill < 0 || vill >= MAX_VILLAGE_NUM)
    {
        return false;
    }
    if (m_pVill[vill].getArea(area)->m_mapAreas.empty())
    {
        return false;
    }
    if (m_pVill[vill].getArea(area) == NULL)
    {
        return false;
    }
    std::vector<MapArea>* pAreas = &m_pVill[vill].getArea(area)->m_mapAreas;
    if (!pAreas->empty())
    {
        // ORIG 条件块 0x086c4946 每轮重调 end()（非缓存；临时槽 -0x14，it=-0x18）
        // ⇒ 不声明 e，条件处内联 `pAreas->end()`。
        std::vector<MapArea>::iterator it = pAreas->begin();
        goto COND;
    LOOP:
        {
            MapArea& ma = *it;
            if (ma.m_x <= x && x <= ma.m_x + ma.m_w &&
                ma.m_y <= y && y <= ma.m_y + ma.m_h)
            {
                return true;
            }
        }
        ++it;
    COND:
        if (it != pAreas->end())
        {
            goto LOOP;
        }
    }
    return false;
}

bool GameWorld::check_valid_area(int vill, int area)
{
    if (vill < 1 || m_iVillCount < vill)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4a2,
                              "[GameWorld::check_valid_area] invalid village : %d, m_iVillCount : %d",
                              vill, m_iVillCount);
        return false;
    }
    if (m_pVill[vill].m_areaCount == 0)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4aa,
                              "[GameWorld::check_valid_area] invalid area count : %d",
                              m_pVill[vill].m_areaCount);
        return false;
    }
    if (area < 0 || m_pVill[vill].m_areaCount <= area)
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x4b2,
                              "[GameWorld::check_valid_area] invalid area : %d", area);
        return false;
    }
    return true;
}

// [b28d-11b] 修法：village 声明前置翻槽序（4.4 先声明深/4.1 先声明浅）+ while 复现 sete 物化 → IDENTICAL 73 @0x086cec84
bool GameWorld::isDungeonEntranceArea(CUser* user)
{
    // b28d-lane11：ORIG 0x086cec8a `movl $0x0,-0x10(%ebp)` = Area* area 函数头
    // 声明置空（后覆写，P197③ 两段式）；trace 改 P221 全表达式临时链翻槽序；
    // 尾部 `!=4 return false` 反极性复现 ORIG je 落 mov$1 岛。
    // 残余封顶：①area/village 槽互换（ORIG area=-0x10/village=-0x14 系 4.1
    // 声明序；4.4 首次使用序反，声明序/两段式试形均不可翻，X1 负例）；
    // ②两处 `cmpl $0,槽; sete %al; test; je` 槽比零物化系 4.1 产物，4.4 恒
    // `cmp/jne` 直跳（P197④）⇒ 封 N82/N197。
    Village* village;
    Area* area = NULL;
    village = getUserVillage(user);
    while (village == NULL)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1685, 5)("getUserVillage call failed");
        return false;
    }
    int areaNo = user->get_area(false);
    area = village->getArea(areaNo);
    while (area == NULL)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x168c, 5)("getArea call failed (area no:%d)", areaNo);
        return false;
    }
    if (user->get_charac_level() < 2)
    {
        return true;
    }
    if (area->m_areaType != 4)
    {
        return false;
    }
    return true;
}

int GameWorld::getDungeonMinimumRequiredLevel(int dungeonIdx)
{
    CDungeon* dungeon = (CDungeon*)G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == NULL)
    {
        return -1;
    }
    return dungeon->get_min_level();
}

int GameWorld::GetWorldMapIndex(CUser* user)
{
    // ORIG 0x86cebe6-0x86cebf1：null 检查产 `jne body` + fall-through `mov$-1;jmp end`
    // 独立早退岛（body 内 3 处失败共用尾岛 0x86cec7d）⇒ 源码为 early-return 形
    if (user == NULL)
    {
        return -1;
    }
    {
        int vill = (int)user->getCurCharacVill();
        int area = user->get_area(false);
        if (check_valid_area(vill, area))
        {
            Village* village = getUserVillage(user);
            if (village != NULL)
            {
                Area* pArea = village->getArea(area);
                if (pArea != NULL)
                {
                    return pArea->get_world_map_index();
                }
            }
        }
    }
    return -1;
}

void GameWorld::get_user_id_list(int vill, int area,
                                 std::vector<unsigned short>& out)
{
    // ORIG 0x86c8…：五条件独立跳转岛；`cmp 0xc(%ebp),%eax; setl %al; test; jne`
    // ⇒ 历史 register bool asm("al") 物化（技巧22）；b28d-lane11 按 P0/N96 清绑；
    // village 指针落 -0xc 具名槽（技巧32）
    // verdict=AE（P198② 谓词整体内联，去具名 sizeBad）
    // [b28d-solo-GameWorld] _ZN9GameWorld16get_user_id_listEiiRSt6vectorItSaItEE：ORIG
    // 0x086c8328 `call size; cmp 0xc(%ebp),%eax; setl %al; test; jne` 无槽往返 ⇒ 具名
    // bool sizeBad 落槽（多 mov %al,-0xd + cmpb）⇒ P198② 谓词整体内联。
    if ((int)g_townScriptFileList.size() < vill)
    {
        return;
    }
    if (vill <= -1)
    {
        return;
    }
    if (vill >= MAX_VILLAGE_NUM)
    {
        return;
    }
    Village* pVillage = &m_pVill[vill];
    if (area >= pVillage->m_areaCount)
    {
        return;
    }
    if (area <= -1)
    {
        return;
    }
    pVillage->m_pAreas[area].get_user_id_list(out);
}

bool GameWorld::get_disposition(int vill, int area,
                                pvp_assault::CDisposition* disposition)
{
    // ORIG 0x086c4xxx：正向 test;je 早跳 + 尾部 return false（! 形态会多出 xor 物化）
    if (check_valid_area(vill, area))
    {
        return (*(bool (**)(void*, void*))*(void**)disposition)(
            disposition, (void*)&m_pVill[vill].getArea(area)->m_mapAreas);   // Area +0x7c
    }
    return false;
}

// ORIG CUserModifier 最小镜像（df_game_r modify_user 0x86c9…：`mov (%eax),%eax;
// mov (%eax),%edx … call *%edx` vtable 槽0=Modify(CUser*)，thiscall 栈传 this+user；
// 无虚析构 ⇒ slot0。与 InterDispatcher.cpp:16732 同口径镜像。）
class CUserModifier
{
public:
    virtual void Modify(CUser* user) = 0;
};

void GameWorld::modify_user(CUserModifier* modifier)
{
    // ORIG 0x86c9308：begin sret 直落具名 it=-0x18（技巧25 内声明）、user 具名槽 -0xc
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        modifier->Modify(user);
    }
}

// [b28d-11b] N：帧差 + push ebx（N82/P210）@0x086c45aa
// [b28d-solo-GameWorld] _ZN9GameWorld45check_restrictive_commercial_transaction_zoneEiiii
// verdict=AE 82=82：P248档②（Area.h m_field6c uchar→bool 消 xor/极性）+ P283/P13
// （goto 尾条件 + 每轮 end()）+ 具名 pair pt（ORIG C1Ev @0x086c45d7）。
// 历史：旧判词 DIFF 82/83（m_field6c uchar + 内联 it->first/second）已被上述
// AE 修法取代；Area.h 改动已 R2 广播（S175_Area 复验零回归，Area.cpp 用自身
// TU 局部定义保持 uchar = P248 两端冲突判例）。
int GameWorld::check_restrictive_commercial_transaction_zone(int vill, int area,
                                                             int x, int y)
{
    if (check_valid_area(vill, area))
    {
        // [b28d-solo-GameWorld] ORIG 0x086c45d1 `lea -0x14; call pair<int,int>::C1Ev`
        // 在 getArea 之前 ⇒ 具名 pt 先声明（4.1 先声明深：pt=-0x14 深于 pArea=-0xc）。
        std::pair<int, int> pt;
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea != NULL)
        {
            // [b28d-solo-GameWorld] ORIG 0x086c460f `movzbl 0x6c; xor $0x1; test; je`
            // ⇒ P292 字段 bool + 裸 !field（uchar 恒多 setne/test 或 cmp $1 形）。
            if (!pArea->m_field6c)
            {
                return 0x3e;
            }
            // ORIG 0x86c461e：遍历 Area+0x70 的 vector<pair<int,int>>
            // （m_npcPositions，非 +0x7c 的 m_mapAreas）。
            // [b28d-solo-GameWorld] P283+P13：ORIG 0x086c462e begin(-0x18) →
            // 0x086c4646 end(-0x1c) → 0x086c464e `jmp 条件块` → body → `++it`(ppEv)
            // → 条件块 `!=` 回跳。4.4 对 for 恒条件置顶且每轮重调 end() ⇒ 用
            // 具名 it/e + goto 尾条件复现；`pt = *it` 逐字段拷贝（ORIG 0x086c465b）。
            std::vector<std::pair<int, int> >::iterator it =
                pArea->m_npcPositions.begin();
            std::vector<std::pair<int, int> >::iterator e =
                pArea->m_npcPositions.end();
            goto COND;
        LOOP:
            pt = *it;
            if (Is_restrictive_commercial_transaction_zone(
                    pt.first, pt.second, x, y))
            {
                return 0x52;
            }
            ++it;
        COND:
            if (it != e)
            {
                goto LOOP;
            }
        }
    }
    return 0;
}

// ============================================================================
// 移动
// ============================================================================
// [b28d-11b] N：xor 物化 + 块序（P168a/N93）@0x086c5706
void GameWorld::move_position(CUser* user, int x, int y, int z,
                              unsigned short dir)
{
    // [b28d-solo-GameWorld] _ZN9GameWorld13move_positionEP5CUseriiit verdict=AE 152=152
    // 修法：P221 全表达式临时链 cMyTrace(...)(...)（原具名 trace 与 PacketGuard
    // 槽互换 N93/N82；临时链复现 ORIG trace=-0x28 浅于 guard=-0x34）。
    // [b28d-solo-GameWorld] _ZN9GameWorld13move_positionEP5CUseriiit：ORIG 0x086c5712
    // `call IsCurCharacGhost; test %al,%al; jne 跳过` ⇒ 早退形（bool 直判，无 xor）。
    if (user == NULL)
    {
        return;
    }
    if (user->IsCurCharacGhost())
    {
        return;
    }
    user->set_position((unsigned short)x, (unsigned short)y, (char)z);

    PacketGuard packet;
    packet.put_header(0, 0x16);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(x);
    packet.put_short(y);
    packet.put_byte(z);
    packet.put_short(dir);
    packet.finalize(true);

    int vill = (int)user->getCurCharacVill();
    int area = user->get_area(false);
    if (check_valid_area(vill, area))
    {
        Village* village = getUserVillage(user);
        // ORIG 0x086c57e2 `cmpl $0,-0x10; je 后置 trace 块` ⇒ 正极性 if(village!=0) 直落 body。
        if (village != NULL)
        {
            Area* pArea = village->getArea(area);
            if (pArea != NULL)
            {
                // ORIG 0x086c5814 `test %al,%al; je Send 块` ⇒ 正极性 if(visible) 直落 send_to_all。
                if (user->IsCurCharacVisible())
                {
                    pArea->send_to_all(packet);
                }
                else
                {
                    user->Send(packet);
                }
            }
        }
        else
        {
            // [b28d-solo-GameWorld] P221 全表达式临时链翻 sibling 块槽方向
            // （ORIG trace 槽 -0x28 浅于 PacketGuard -0x34）。
            cMyTrace(__PRETTY_FUNCTION__, 0x5ec, 5)("%d %d", vill, area);
        }
    }
}

// [b28d-11b] 修法：party/curVill/curArea 声明序翻槽 + && 链拆嵌套 if 复现 sete +
// party 预清前移 + ret 移入内层块 + while(ret!=0){return ret;} 复现 setne
// → IDENTICAL 205 @0x086d1002
int GameWorld::check_move_area(CUser* user, int vill, int area, int x, int y,
                               int z, bool flag, int param)
{
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (user->CheckInTrade())
    {
        return 0xda;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0xbf;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->GetCurCharacExpertJob() != NULL)      // +0x085 第 1 次调用
    {
        // +0x093 第 2 次独立调用（P48）；+0x0a9 sete 物化
        if (((expert_job::CExpertJob*)user->GetCurCharacExpertJob())->GetType() == 1)
        {
        return 0x38;
        }
    }
    CParty* party = 0;                              // +0x0b4 预清槽 -0x1c
    if (user->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }
    if (user->IsCurCharacGhost() &&
        !GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() != 7)
    {
        return 0x9f;
    }

    if (!user->isGMUser() &&
        IsPowerWarSafeZone(vill, area, (ENUM_POWER_SIDE_TYPE)user->getPowerSide()))
    {
        return 4;
    }

    int curVill;
    int curArea;
    curVill = (int)user->getCurCharacVill();
    curArea = user->get_area(false);
    party = (CParty*)user->GetParty();
    // ORIG +0x1bf..+0x1f3：正向 && 链、全过落 mov $0x13；主路径为跳转目标（块布局反转）
    if (party != NULL)
    {
        if (party->getManager() == user)   // +0x1cc sete 物化
        {
            if (vill == curVill)
            {
                if (area == curArea)
                {
                    if (param != 2)
                    {
                        return 0x13;
                    }
                }
            }
        }
    }
    if (!check_valid_area(vill, area) ||                   // +0x20f xor $1
        !check_valid_area(curVill, curArea))               // +0x231
    {
        return 0x13;
    }
    Village* pV = 0;                                       // +0x24d 先置 0 槽
    if (vill >= 0 && vill < MAX_VILLAGE_NUM)               // +0x254 js/+0x262 jge 正向
    {
        pV = &m_pVill[vill];                               // +0x267 imul $0x34
        if (pV != 0)                                       // +0x27a
        {
            int ret = 0;                                   // +0x27c 先置 0 槽
            ret = user->CheckMoveTown(vill);               // +0x290
            while (ret != 0)
    {
                return ret;                                // +0x2a3
    }
        }
    }
    return 0;                                              // +0x2aa
}

// [b28d-11b] N：帧/槽差（N82）@0x086c5a84
// [b28d-sweep] 864/872 → **864/864**（gap 0）：真因 = g_guildAgitMgr 建模错误
// （头声明指针 4B，ORIG 为 B 段 0x58 对象）；对象化后 4 处调用点由
// `mov g,%eax; mov %eax,(%esp)` 变 `movl $g_guildAgitMgr,(%esp)`，与 ORIG
// 0x86c658d/0x86c65be/0x86c65ec/0x86c661a 逐条一致（每处 -2 条 ×4）。
// [L11·round1 判词更新] verdict=**AE/IDENTICAL** 864/864（ndiff 230→0；p167 127/127 CLEAR）。
// 本轮 5 项修法（均以 ORIG 0x086c5a84 段实证，逐条复编核消）：
//  ① P227 全表达式临时链（guild-agit 分支 cMyTrace 具名→链式，230→60）；
//  ② P248 档①/P292 被调方返回类型：CHackLog_InvalidAreaMove::isMovableRoute
//     `char`→`bool`（ORIG 调用点 `call; xor $0x1,%eax; test %al,%al; je` 实证，
//     60→57；该头/实现为 Main 授权的无主 TU，已 grep 全仓仅本文件调用）；
//  ③ 声明顺序：`char buf[5]={0}` 提到 hackA/hackB 之前（ORIG O[591] 段清零在前，57→53）；
//  ④ P194/P222 正极性包裹：destArea 两处守卫 `if (destArea < 0){neg}else{pos}` →
//     `if (destArea >= 0){pos}else{neg}`（ORIG `js` 负值跳走 + pos 直落，53→7）；
//  ⑤ P198① 预清两句式 + P225 while+break：destVillage 段 `&&` 链内赋值（4.4 多 4 条
//     `mov $1/$0` 物化）拆为独立语句；moveTown `int moveTown=0; moveTown=call(...);`
//     预清 + `while (moveTown != 0){…; return false;}` 复现 `cmpl $0,-0x38; setne %al;
//     test %al,%al; je`（7→**0**）。
// 旧判词「P75/N82 封顶」作废：真因是 ②③④⑤ 四处源形，非跨版本结构差。
bool GameWorld::move_area(CUser* user, int vill, int area, int x, int y, int z,
                          bool flag, int param, int prevVill, int prevArea)
{
    // §6.2 巨件核销：P167 127/127 [0缺0多]。逐块人工比对完成，语义零缺口。
    // 真修四处：①teleport_state==1 比较物化；②elapsed 具名局部（P0 槽 -0x2c）；
    // ③buf[5] 清零 movl+movb（P202①）；④getCurCharacVillPrev 返回 signed char
    // （ORIG 调用点 movsbl，头已同步）。
    PacketGuard errorPacket;

    if (!GlobalData::s_timeGate_->isOpen() &&
        vill == 0xb && area == 5)
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x13);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (user->IsBusyDisjointStore())
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0xbf);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x3c);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (user->IsCurCharacGhost() && !GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() != 7)
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(0x13);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }
    if (!user->isGMUser() &&
        IsPowerWarSafeZone(vill, area, (ENUM_POWER_SIDE_TYPE)user->getPowerSide()))
    {
        errorPacket.clear();
        errorPacket.put_header(1, 0x26);
        errorPacket.put_byte(0);
        errorPacket.put_byte(4);
        errorPacket.put_byte(vill);
        errorPacket.finalize(true);
        user->Send(errorPacket);
        return false;
    }

    int curVill = (int)user->getCurCharacVill();
    int curArea = user->get_area(false);
    bool teleport = false;
    CParty* party = (CParty*)user->GetParty();
    // ORIG @0x086c5ea2：party!=0 守卫后内层 state==1（char 比较物化 sete 为
    // 4.1 特有，4.4 直 jne，N 类）。
    if (party != NULL)
    {
        if (((CPartyTelePort*)party->GetPartyTelePort())->get_teleport_state() == 1)
        {
            teleport = true;
        }
    }
    if (param == 2)
    {
        teleport = true;
    }
    if (!teleport && vill == curVill && area == curArea)
    {
        return false;
    }

    PacketGuard packet;
    if (!check_valid_area(vill, area) || !check_valid_area(curVill, curArea))
    {
        packet.put_header(1, 0x26);
        packet.put_byte(0);
        packet.put_byte(0x15);
        packet.put_byte(0);
        packet.finalize(true);
        user->Send(packet);
        return false;
    }

    Village* destVillage = NULL;
    // ORIG 0x086c5xxx 段：`mov %eax,-0x48; cmpl $0,-0x48; je` ⇒ 赋值与判零拆成两句
    // （&& 链内赋值使 4.4 多发 mov $1/$0 物化 4 条）。
    if (-1 < vill && vill < MAX_VILLAGE_NUM)
    {
        destVillage = &m_pVill[vill];
        if (destVillage != NULL)
        {
            // ORIG `movl $0x0,-0x38`（P198① 预清）+ `cmpl $0,-0x38; setne %al;
            // test %al,%al; je`（P225 while+break 复现 bool 四段物化）。
            int moveTown = 0;
            moveTown = user->CheckMoveTown(vill);
            while (moveTown != 0)
            {
                packet.put_header(1, 0x26);
                packet.put_byte(0);
                packet.put_byte(moveTown);
                packet.put_byte(vill);
                packet.finalize(true);
                user->Send(packet);
                return false;
            }
        }
    }

    Village* curVillage = NULL;
    if (-1 < curVill && curVill < MAX_VILLAGE_NUM)
    {
        curVillage = &m_pVill[curVill];
    }
    int prevVill2 = (int)user->getCurCharacVillPrev();
    int prevArea2 = user->get_before_area();
    user->set_position((unsigned short)x, (unsigned short)y, (char)z);
    user->setCurCharacVill((char)vill);
    user->set_area(area);

    if (curVillage != NULL)
    {
        Area* curAreaObj = curVillage->getArea(curArea);
        if (flag && !IsPVPChannel())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            // ORIG 0x086c61b5/0x086c61cd（P0/P48 去缓存）：条件臂与体各独立调用
            // getBeforeAreaTime() 一次，不缓存到具名局部（缓存则少 1 call 点，P167 差集 -1）。
            if (user->getBeforeAreaTime() != 0)
            {
                // ORIG @0x086c61d7..：差值落 -0x2c 槽（P0 ⇒ 具名局部 elapsed），
                // cmpl $0x2 后 jg 走失败汇聚（&& 短路链首条件）。
                int elapsed = now - user->getBeforeAreaTime();
                if (elapsed <= 2 &&
                    CHackLog_InvalidAreaMove::IsInvalidMovableArea(
                        curVill, curArea, vill, area) &&
                    !CHackLog_InvalidAreaMove::isMovableRoute(
                        prevVill2, prevArea2, curVill, curArea, vill, area) &&
                    prevVill2 != vill && prevArea2 != area)
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACK_TYPE)0x1fc, 1, 0, 0);
                }
            }
            user->setBeforeAreaTime(now);

            if (param == 0)
            {
                if (!curAreaObj->IsMovalbeArea(vill, area) &&
                    CHackLog_InvalidAreaMove::IsInvalidMovableArea(
                        curVill, curArea, vill, area) &&
                    curVill == prevVill && curArea == prevArea)
                {
                    // ORIG 0x86c6352：先记日志（User acc_id 与四区坐标），
                    // 再把 (prevVill,prevArea)/(vill,area) 各拼成 "%d%d" 字符串
                    // 经 atoi 还原后作为 hack 计数参数（数值拼接怪癖）。
                    LogManager::logFormat(
                        1, "world.cpp", __PRETTY_FUNCTION__, 0x832,
                        "move area error. User:%s (%d,%d)->(%d,%d)",
                        NumberToString(user->get_acc_id(), 0), curVill,
                        curArea, vill, area);
                    char buf[5] = {0};
                    int hackA = 0;
                    int hackB = 0;
                    if (prevVill >= 0 && prevVill <= 10 &&
                        prevArea >= 0 && prevArea <= 10)
                    {
                        sprintf(buf, "%d%d", prevVill, prevArea);
                        hackA = atoi(buf);
                        memset(buf, 0, 5);
                    }
                    if (vill >= 0 && vill <= 10 && area >= 0 && area <= 10)
                    {
                        sprintf(buf, "%d%d", vill, area);
                        hackB = atoi(buf);
                    }
                    user->getHackAnalyzer()->addServerHackCnt(
                        user,
                        (WongWork::ENUM_HACK_TYPE)CHackLog_InvalidAreaMove::GetHackType(),
                        1, hackA, hackB);
                }
            }
        }
        curAreaObj->delete_user(user);
    }

    if (destVillage != NULL)
    {
        int destArea = area;
        // ORIG 0x086c6a1d..（P167 差集 -1 G_GameWorld）：ORIG 经 G_GameWorld() 取
        // 单例后再调 IsSchoolPvPChannel()，而非直接用 this —— 忠实保留跨调用形。
        if (G_GameWorld()->IsSchoolPvPChannel() && vill == 2 && area == 4)
        {
            // ORIG 0x086c64e5 `call CUser::GetSchoolNo; mov %eax,0x4(%esp);
            // movl $&g_schoolMgr,(%esp); call CSchoolMgr::GetSchoolArea` ⇒ 学号
            // 来自 GetSchoolNo() 返回值（原实现丢弃返回值传 0 = 真语义 bug）。
            destArea = g_schoolMgr.GetSchoolArea(user->GetSchoolNo());
            // ORIG 0x086c6a1d 段 `js <neg 块>`：负值跳走、非负直落 set_area ⇒
            // 原码按 `>= 0` 写（P194/P222 正极性包裹）；`< 0` 反产 `jns`。
            if (destArea >= 0)
            {
                user->set_area(destArea);
                user->set_trick_area(area);
            }
            else
            {
                destArea = area;
                user->set_trick_area(-1);
            }
        }
        else if (vill == 8 && (area == 0 || area == 1 ||
                               area == 2 || area == 3))
        {
            // ORIG 0x086c6ab0..（P0/P48 去缓存 + 循环内重调形）：area 4 值各成一
            // 独立 if-branch，每支各调 get_charac_guildkey() + GetGuildAgitAreaIndex()
            // 一次；cMyTrace 的 Guild(%d) 实参第 5 次独立调用 get_charac_guildkey()
            // （不缓存到具名局部）。P167 差集 -4/-3 根因。
            // [b28d-sweep] g_guildAgitMgr 为对象（ORIG B 0x58）⇒ 值语义调用；ORIG
            // 0x86c658d/0x86c65be/0x86c65ec/0x86c661a 全为 `movl $0x943e0e0,(%esp)`。
            if (area == 0)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 0);
            else if (area == 1)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 1);
            else if (area == 2)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 2);
            else if (area == 3)
                destArea = g_guildAgitMgr.GetGuildAgitAreaIndex(user->get_charac_guildkey(), 3);
            cMyTrace(__PRETTY_FUNCTION__, 0x88e, 0)(
                "Move Guild Agit Area User(%d) Guild(%d) dest area(%d), virtual area(%d) ",
                user->getCurCharacNo(), user->get_charac_guildkey(), area, destArea);
            // 同 2699 段：ORIG `js` 负值跳走 ⇒ 源按 `>= 0` 正极性写。
            if (destArea >= 0)
            {
                user->set_area(destArea);
                user->set_trick_area(area);
            }
            else
            {
                destArea = area;
                user->set_trick_area(-1);
            }
        }
        else
        {
            user->set_trick_area(-1);
        }
        Area* destAreaObj = destVillage->getArea(destArea);
        destAreaObj->insert_user(user);
    }

    // ORIG 0x86c6710：MoveArea(curVill, curArea, vill, area, param)
    // （汇编实参：+0x4=curVill、+0x8=curArea、+0xc=vill(0x10ebp)、
    //   +0x10=area(0x14ebp)、+0x14=param(0x28ebp)；Ghidra C 掉第 5 实参，
    //   e 在实现中未使用，但调用侧必须按原寄存器序列补齐）。
    user->m_historyLog.MoveArea(curVill, curArea, vill, area, param);
    return true;
}

void GameWorld::goto_dungeon(CUser* user)
{
    if (user == NULL)
    {
        return;
    }
    if (user->getMoveSpace() != 1)
    {
        Village* village = getUserVillage(user);
        if (village == NULL)
        {
            // ORIG +0x051 movsbl→%ebx / +0x077 mov %ebx,0x8(%esp)：P37 链式临时提前实参
            cMyTrace(__PRETTY_FUNCTION__, 0x8d1, 5)("%d", (int)user->getCurCharacVill());
            return;
        }
        Area* area = village->getArea(user->get_area(false));
        if (area == NULL)
        {
            // ORIG +0x0d1 mov %eax,%ebx / +0x0f6 mov %ebx,0x8(%esp)
            cMyTrace(__PRETTY_FUNCTION__, 0x8d8, 5)("%d", user->get_area(false));
            return;
        }
        area->erase_user(user);

        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xff);
        packet.put_short((unsigned short)user->get_posX());
        packet.put_short((unsigned short)user->get_posY());
        packet.put_byte((int)user->get_direction());
        packet.put_byte((int)user->get_charac_visible_values());
        packet.finalize(true);
        user->Send(packet);
    }
    user->set_state((ch_state)5);
}

void GameWorld::out_from_dungeon(CUser* user)
{
    user->SetRevivalTime(0);
    user->SetUseAPCPotionTime(0);
    // ORIG 0x086c6a16：+0x3c setne（`!= 1` then 在前）；+0x7d xor$1（`!check_valid_area` log 在前）
    if (user->getMoveSpace() != 1)
    {
        if (!check_valid_area((int)user->getCurCharacVill(),
                              user->get_area(false)))
        {
            // ORIG +0x084..+0x0ba：name 直写 0x14(%esp)（单语句无跨调用缓存，勿加 register）
            LogManager::logFormat(
                1, "world.cpp", __PRETTY_FUNCTION__, 0x926,
                "user %s - check_valid_area() fail", user->getCurCharacName());
        }
        else
        {
            Village* village = getUserVillage(user);
            if (village != NULL)
            {
                Area* area = village->getArea(user->get_area(false));
                if (area != NULL)
                {
                    area->insert_user(user);
                }
            }
            PacketGuard packet;
            packet.put_header(0, 3);
            packet.put_byte(1);
            packet.put_short((unsigned short)user->get_unique_id());
            packet.put_byte(0);
            packet.finalize(true);
            send_all(packet, user);
            user->SendFatigue();
            static_cast<XNuclear::CHades*>(user->getHades())->Send_ReturnToVillage();
        }
    }
    else
    {
        user->SendFatigue();
        user->set_state((ch_state)0xb);
        PacketGuard packet;
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte(7);
        packet.finalize(true);
        user->Send(packet);
    }
}

void GameWorld::goto_pvp(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(), user->get_area(false)))
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__,
                              0x998, "user %s - check_valid_area() fail",
                              user->getCurCharacName());
        return;
    }
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->erase_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte((int)user->getCurCharacVill());
    packet.put_byte(0xfe);
    packet.put_short((unsigned short)user->get_posX());
    packet.put_short((unsigned short)user->get_posY());
    packet.put_byte((int)user->get_direction());
    packet.put_byte((int)user->get_charac_visible_values());
    packet.finalize(true);
    user->Send(packet);
    user->set_state((ch_state)6);
    packet.clear();
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(2);
    packet.finalize(true);
    send_all(packet, user);
}

void GameWorld::out_from_pvp(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(), user->get_area(false)))
    {
        // ORIG +0x50 mov %eax,%ebx / +0x75 mov %ebx,0x8(%esp)：P37 链式临时提前实参
        cMyTrace(__PRETTY_FUNCTION__, 0x9ce, 0)(
            "user %s - check_valid_area() fail", user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }
    user->SetPvpIndex(-1);
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->insert_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(0);
    packet.finalize(true);
    send_all(packet, user);
}

void GameWorld::goto_warroom(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(), user->get_area(false)))
    {
        LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__,
                              0x9f3, "user %s - check_valid_area() fail",
                              user->getCurCharacName());
        return;
    }
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->erase_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte((int)user->getCurCharacVill());
    packet.put_byte(0xfe);
    packet.put_short((unsigned short)user->get_posX());
    packet.put_short((unsigned short)user->get_posY());
    packet.put_byte((int)user->get_direction());
    packet.put_byte((int)user->get_charac_visible_values());
    packet.finalize(true);
    user->Send(packet);
    user->set_state((ch_state)8);
    packet.clear();
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(3);
    packet.finalize(true);
    send_all(packet, user);
}

void GameWorld::out_from_warroom(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(), user->get_area(false)))
    {
        // ORIG +0x50 mov %eax,%ebx / +0x75 mov %ebx,0x8(%esp)：P37 链式临时提前实参
        cMyTrace(__PRETTY_FUNCTION__, 0xa29, 0)(
            "user %s - check_valid_area() fail", user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }
    user->SetWarRoomIndex(-1);
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->insert_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(0);
    packet.finalize(true);
    send_all(packet, user);
    user->getCurCharacInvenW()->RemoveKCItem();
    user->InitFinishPoint();
}

void GameWorld::gotoBlueMarble(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(),
                          user->get_area(false)))
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0xa54,
                              "user %s - check_valid_area() fail",
                              user->getCurCharacName());
        return;
    }

    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->erase_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 0x17);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte((int)user->getCurCharacVill());
    packet.put_byte(0xfe);
    packet.put_short((unsigned short)user->get_posX());
    packet.put_short((unsigned short)user->get_posY());
    packet.put_byte((int)user->get_direction());
    packet.put_byte((int)user->get_charac_visible_values());
    packet.finalize(true);
    user->Send(packet);
    user->set_state((ch_state)0xb);
    packet.clear();
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(7);
    packet.finalize(true);
    send_all(packet, user);
}

void GameWorld::outFromBlueMarble(CUser* user)
{
    if (!check_valid_area((int)user->getCurCharacVill(),
                          user->get_area(false)))
    {
        // ORIG +0x50 mov %eax,%ebx / +0x75 mov %ebx,0x8(%esp)：P37 链式临时提前实参
        cMyTrace(__PRETTY_FUNCTION__, 0xa7f, 0)(
            "user %s - check_valid_area() fail", user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }

    user->setBlueMarbleIndex(-1);
    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        Area* area = village->getArea(user->get_area(false));
        if (area != NULL)
        {
            area->insert_user(user);
        }
    }
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_byte(0);
    packet.finalize(true);
    send_all(packet, user);
}

// [b28d-solo-GameWorld] _ZN9GameWorld7fishingEP5CUser verdict=AE 263/263（b28d-solo 本轮）
// b28d-solo 修法（四处真修）：①P167 —— `CItemList* itemList=G_CDataManager()->
// getItemList(); itemList->create_item(...)` 改直读 `G_CDataManager()->m_itemList`
// 字段（ORIG 0x086c7cf1 `mov 0xc(%eax),%eax`；getItemList 访问器 4.4 -O0 不内联
// ⇒ 旧形 p167 EXTRA 1 个调用点）；②P59/P168a —— `if (check_valid_area(...)){body}
// else{fail}` 改否定早退 `if (!check_valid_area(...)) {trace;DisConnSig;return;}`
// （ORIG `call; xor $1; test %al; je <主流程>`）；③P222 —— areaType 分支交换 then/else
// 并反条件（ORIG `cmp $0x2; je <鱼获块>` then 后置）；④P227 全表达式临时链（trace
// 实参 getCurCharacName 跨 ctor 保活 %ebx）。
// ⑤本轮新增：`if (result < 1) {失败} else {成功}` 臂序反置 —— ORIG 0x086c7db6
// `jle <失败块>`（条件取反跳 else、成功块 fall-through）⇒ 改正极性
// `if (result > 0) {成功} else {失败}`（4.4 对 `< 1` 恒产 `jg <成功>` 反向布局）。
// 现 263=263 逐条归一化全等、p167 CLEAR 45=45、check_tu_routed verdict=AE。
void GameWorld::fishing(CUser* user)
{
    // b28d-solo P59/P168a：ORIG 0x086c7b.. `call check_valid_area; xor $0x1,%eax;
    // test %al,%al; je <主流程>` ⇒ 原码是否定早退形（bool callee 取反物化）。
    if (!check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        // ORIG 0x086c7b.. 段：getCurCharacName 结果先落 %ebx（跨 ctor 保活）⇒
        // P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xafd, 0)("user %s - check_valid_area() fail",
            user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
        return;
    }
    {
        Village* village = getUserVillage(user);
        Area* area = NULL;
        if (village != NULL)
        {
            area = village->getArea(user->get_area(false));
        }
        PacketGuard packet;
        packet.put_header(1, 0x27);
        // b28d-solo P222：ORIG 0x086c7c22 `cmp $0x2,%eax; je <鱼获块>`（then 后置）
        // ⇒ 交换 then/else 并反条件复现（4.4 对 `!=` 生 je 跳后置块）。
        if (area->m_areaType != 2)
        {
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            user->Send(packet);
        }
        else
        {
            int fishIndex = area->take_fish();
            if (fishIndex == -1)
            {
                packet.put_byte(0);
                packet.put_byte(1);
                packet.finalize(true);
                user->Send(packet);
            }
            else
            {
                Inven_Item item;
                // ORIG 0x086c7ce9：`mov %eax,-0x6f(%ebp)`（item base = ebp-0x71）
                // ＝ 4 字节写 Inven_Item +0x02（m_addInfo），非 +0x00。
                item.m_addInfo = fishIndex;
                // b28d-solo p167 修：ORIG 0x086c7cf1 `mov 0xc(%eax),%eax` 直读
                // CDataManager::m_itemList 字段（+0x0c），无 getItemList() call
                // （该访问器 4.4 -O0 不内联 ⇒ EXTRA 1 个调用点）。
                G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
                int result = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)5, true, true);
                // ORIG 0x086c7db6 `jle <失败块>`（条件取反跳 else、成功块 fall-through）
                // ⇒ 正极性 `if (result > 0) {成功} else {失败}`；4.4 对 `result < 1`
                // 恒产 `jg <成功>` 反向布局。
                if (result > 0)
                {
                    packet.put_byte(1);
                    packet.put_short(result);
                    packet.put_int(fishIndex);
                    // ORIG `mov -0x6a(%ebp),%eax`（item+0x07）4B、
                    // `movzwl -0x66(%ebp),%eax; movzwl %ax,%eax`（item+0x0b，
                    // 2 字节零扩展）→ 均非 +0x13 的 1 字节读。
                    packet.put_int(item.m_addInfo2);
                    packet.put_short(item.m_fieldb);
                }
                else
                {
                    packet.put_byte(0);
                    packet.put_byte(4);
                }
                packet.finalize(true);
                user->Send(packet);
                packet.clear();
                packet.put_header(0, 0x19);
                packet.put_short((unsigned short)user->get_unique_id());
                packet.put_int(fishIndex);
                packet.finalize(true);
                area->send_to_all(packet);
            }
        }
    }
}

bool GameWorld::IsPowerWarSafeZone(int vill, int area, ENUM_POWER_SIDE_TYPE side)
{
    // ORIG 同 0x86c59e4 族（见 2 参重载注释）：! 取反早退 + 独立 Village* 槽
    if (!(vill == 7 && GlobalData::s_power_manager->IsPowerWarEventOn()))
    {
        return false;
    }
    {
        if (vill < 0 || vill >= MAX_VILLAGE_NUM)
        {
            return true;
        }
        Village* pVill = &m_pVill[vill];
        Area* pArea = pVill->getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if ((side == 1 && pArea->m_areaType == 5) ||
            (side == 2 && pArea->m_areaType == 6))
        {
            return true;
        }
    }
    return false;
}

bool GameWorld::IsPowerWarSafeZone(int vill, int area)
{
    // ORIG 0x86c59e4-0x86c5a0c：双 bool 物化 mov$1/mov$0+共享 test ⇒
    // `!(A&&B)` 取反早退形（技巧1 xor 来自 !boolcall）
    if (!(vill == 7 && GlobalData::s_power_manager->IsPowerWarEventOn()))
    {
        return false;
    }
    {
        if (vill < 0 || vill >= MAX_VILLAGE_NUM)
        {
            return true;
        }
        // ORIG 0x86c5a2c-0x86c5a48：Village* 独立槽 -0x10（lea+mov slot+reload）
        Village* pVill = &m_pVill[vill];
        Area* pArea = pVill->getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if (pArea->m_areaType == 5 || pArea->m_areaType == 6)
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// 禁言 / 禁移动族（ORIG 086cd908..086ced1d；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

bool GameWorld::IsForbiddenChat(const char* name)
{
    
    // ORIG 0x086cd908：NULL/strlen 独立早退块；ban 正向测试+末尾 return false（P16/P63 块排布）
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    // 槽序探针记录（N82 封顶，勿再试）：指令数已 124/124 与 ORIG 完全相等，
    // 差集只剩 str/iterator 组整体 +16（ORIG -0x24/-0x1d/-0x1c vs
    // OURS -0x14/-0xd/-0xc；-0x28/-0x18 两边一致）。已试并否决的三种写法：
    //  ① 删具名 iterator 合并成 find(...)==end() → 30 NEAR 恶化到 42 DIFF；
    //  ② iterator 声明/赋值分离 → 多 emit 默认构造 call；
    //  ③ 删 str 直接传 name → key 类型不符（见 GetIDUserInIDMap 同类 N18a）。
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_chatBan)
    {
        if (GetLeftTimeOfForbiddenChat(name) == 0)
        {
            EnableChat(name);
            return false;
        }
        // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x153d, 0)(
            "%s is forbidden to chat\n", name);
        return true;
    }
    return false;
}

bool GameWorld::IsForbiddenMove(const char* name)
{
    
    // ORIG 0x086cdb60：同 IsForbiddenChat 形态，字段 m_moveBan(+0x05)
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_moveBan != 0)
    {
        if (GetLeftTimeOfForbiddenMove(name) == 0)
        {
            EnableMove(name);
            return false;
        }
        // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽。
        cMyTrace(__PRETTY_FUNCTION__, 0x1557, 0)(
            "%s is forbidden to move\n", name);
        return true;
    }
    return false;
}

unsigned int GameWorld::WhenIsTheEndOfForbiddenChat(const char* name)
{
    // ORIG 0x086ce4a0：两次独立 operator[]（P157 逐字段重读，勿缓存引用）
    if (name == 0)
    {
        return 0;
    }
    if (strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return 0;
    }
    if (m_idMap[str].m_chatBan)
    {
        // ORIG @086ce590 mov 0x8(%eax),%ebx 直落 callee-saved：块内 register int
        // 与字段类型（int +0x08）精确一致（P-技巧）
        register int tick = m_idMap[str].m_chatEndTick;
        return tick;
    }
    return 0;
}

unsigned int GameWorld::WhenIsTheEndOfForbiddenMove(const char* name)
{
    // ORIG 0x086ce5f0：同 WhenIsTheEndOfForbiddenChat 形态，字段 m_moveBan/m_moveEndTick
    if (name == 0)
    {
        return 0;
    }
    if (strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return 0;
    }
    if (m_idMap[str].m_moveBan)
    {
        register int tick = m_idMap[str].m_moveEndTick;
        return tick;
    }
    return 0;
}

unsigned int GameWorld::GetLeftTimeOfForbiddenChat(const char* name)
{
    // N89 封顶（与 IsForbiddenChat/IsForbiddenMove 同根因）：ORIG 4.1 对
    // `std::string str(name)` 发 allocator 临时 SaIcEC1Ev/D1Ev call（槽
    // -0x2d），4.4 判 trivial 省略 ⇒ str/it/end/trace 槽组连锁平移；指令数
    // 147/147 与 call 序全等，槽组不可翻（显式 allocator 形 4.4 仍省略）。
    // ORIG 0x086ce73c：64-bit end/diff（有符号比较）；经 G_GameWorld() 调 WhenIsTheEnd
    if (name == 0)
    {
        return 0;    // ORIG：name 空直接返回，无 trace
    }
    if (strlen(name) >= 31)
    {
        // b28d-lane11：P221 全表达式临时链（兄弟块 trace 槽序翻正试形）。
        cMyTrace(__PRETTY_FUNCTION__, 0x15f2, 0)("GetLeftTimeOfForbiddenChat return 1\n");
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x15fa, 0)("GetLeftTimeOfForbiddenChat return 2\n");
        return 0;
    }
    long long end = 0;
    end = G_GameWorld()->WhenIsTheEndOfForbiddenChat(name);
    // 探针实证：4.4 下 `end > 0` 正向判定生成 ORIG 的 js/jg/jbe 树；`end <= 0` 生成 jg/js/ja
    if (end > 0)
    {
        unsigned int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;   // ORIG 零扩展（mov $0,%edx）
        end -= cur1000;
        if (end > 0)
        {
            if (end < 60000)
            {
                return 60000;
            }
            return (unsigned int)end;
        }
        return 0;
    }
    return 0;
}

unsigned int GameWorld::GetLeftTimeOfForbiddenMove(const char* name)
{
    // ORIG 0x086ce958：同 Chat 版但无 trace（strlen/find==end 直返 0）
    if (name == 0)
    {
        return 0;
    }
    if (strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return 0;
    }
    long long end = 0;
    end = G_GameWorld()->WhenIsTheEndOfForbiddenMove(name);
    if (end > 0)
    {
        unsigned int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        end -= cur1000;
        if (end > 0)
        {
            if (end < 60000)
            {
                return 60000;
            }
            return (unsigned int)end;
        }
        return 0;
    }
    return 0;
}

// ============================================================================
// 每日重置族（ORIG 086cc340..086cd15f；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

bool GameWorld::EnableChat(const char* name)
{
    // ORIG 0x086cdc60：operator[] 逐访问点重调 4 次（P157），ban==0→already 后置块
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);   // ORIG 4.4 一行式会先求值 end()；两语句保 find→end 序
    if (it == m_idMap.end())
    {
        return false;
    }
    // bool 字段 `== 1` 探针实证生成 movzbl;test;je（xor-free），与 ORIG @086cdd53 一致
    if (m_idMap[str].m_chatBan == 1)
    {
        m_idMap[str].m_chatBan = false;
        m_idMap[str].m_chatEndTick = 0;
        // b28d-lane11：P221/P227 全表达式临时链（槽组翻正 + c_str 实参跨 ctor 保活）。
        cMyTrace(__PRETTY_FUNCTION__, 0x156c, 0)("%s is able to chat now\n", name);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1571, 0)("%s is already able to chat!\n", str.c_str());
    return true;
}

bool GameWorld::EnableMove(const char* name)
{
    // ORIG 0x086cde60：同 EnableChat 形态，字段 m_moveBan(+0x05)/m_moveEndTick(+0x0c)
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_moveBan == 1)
    {
        m_idMap[str].m_moveBan = false;
        m_idMap[str].m_moveEndTick = 0;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x1584, 0)("%s is able to move now\n", name);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x1589, 0)("%s is already able to move!\n", str.c_str());
    return true;
}

bool GameWorld::DisableChat(const char* name, int secs)
{
    // ORIG 0x086ce054：ban==1 检查（xor $0x1 物化，P158）；already 块后置；写侧 2 次独立 operator[]
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    // bool 字段 `== 0`/`!x` 探针实证生成 movzbl;xor $1;test;je，与 ORIG @086ce147 一致
    if (m_idMap[str].m_chatBan == 0)
    {
        m_idMap[str].m_chatBan = true;
        int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        m_idMap[str].m_chatEndTick = secs * 1000 + cur1000;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15a2, 0)("%s is not able to chat now for %d secs\n", name, secs);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x15a7, 0)("%s is already not able to chat!\n", str.c_str());
    return true;
}

bool GameWorld::DisableMove(const char* name, int secs)
{
    // ORIG 0x086ce280：同 DisableChat 形态，字段 m_moveBan/m_moveEndTick
    if (name == 0)
    {
        return false;
    }
    if (strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        return false;
    }
    if (m_idMap[str].m_moveBan == 0)
    {
        m_idMap[str].m_moveBan = true;
        int cur1000 = GlobalData::s_systemTime_.getCurSec() * 1000;
        m_idMap[str].m_moveEndTick = secs * 1000 + cur1000;
        // b28d-lane11：P221/P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x15c0, 0)("%s is not able to move now for %d secs\n", name, secs);
        return true;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x15c5, 0)("%s is already not able to move!\n", str.c_str());
    return true;
}


void GameWorld::ResetMidnight()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->ResetDailyCharacExpandDataMidnight();
    }
}

void GameWorld::resetDailyData()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 1))
            continue;
        user->resetDailyData();
    }
}

void GameWorld::ResetAllCoins()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->RecoverCoin(1);
        if (user->GetOpencoin() == 0)
        {
            user->SendOpenflag();
        }
    }
}

void GameWorld::ResetAllCleadpadPoint()
{
    // N82/P112 封顶：残差全为 it/end临时/ppEi/cMyTrace/user 槽排布跨版本差
    //（ORIG it=-0x2c,trace=-0x20；4.4 it 族占低位、trace=-0x24），逐条指令
    // 与调用序全等，槽号集不等；声明序/提前声明均已试，不可翻。
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        user->m_mcap.reset();
    }
    // b28c-lane3：P221 全表达式临时链（单次调用），复现 ORIG trace 浅槽 -0x1c。
    cMyTrace(__PRETTY_FUNCTION__, 0x1385, 0)(
        "RESET ALL USER CLEANPAD POINT OK!!!!");
}

// [b28d-11b] 修法：内联 bool 条件（P198②）+ 内联 makeRequest 实参（P52/P87）→ IDENTICAL 109 @0x086cc992
void GameWorld::ResetAllPassPadFailCount()
{
    // ORIG 0x086cc992：needReset bool 显式物化（mov $1/$0; test; je，P84）；
    // makeRequest 实参求值序 web→fail→acc（register 缓存 %ebx/%esi，P87/P52）
    int resetCount = 0;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
        {
            continue;   // ORIG 086cc9e5 setle/jne skip 形态
        }
        if (user->getPad()->isActivate() &&
            user->getPad()->getFailCnt() > 0)
        {
            user->getPad()->setFailCnt(0);
            DB_PassPadUpdateFailCnt::makeRequest(
                user->get_acc_id(),
                user->getPad()->getFailCnt(),
                (char*)user->getWebAddress());
            resetCount += 1;
        }
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x139d, 0)(
        "RESET %d USERS GOBLIN PAD RESETT OK!!!!", resetCount);
}

void GameWorld::ResetAllEventData()
{
    GlobalData::s_event_manager->dailyresetData();
}

// [b28d-solo-GameWorld] _ZN9GameWorld15ResetAllFatigueEv verdict=DIFF-N
// b28d-solo 修法：镜像 if —— ORIG 0x086cc3f2 `cmp $0x2,%eax; setle %al; test;
// jne <循环尾>` ⇒ `if (get_state() <= 2) goto L_next;` + 尾标签（P223/AddDailyItem 族），
// 旧 `if (state > 2) {body}` 恒产 setg/je 反向形。
// 残余 N83/N91/N82 封顶：p167 EXTRA _ZnwjPv/_ZdlPvS_（4.1 placement-new 内联 vs
// 4.4 extern call，N83）+ _Unwind_Resume×2/_ZNSsD1Ev×2（EH landing 差异，N91）+
// 帧 0x7c vs 0x8c 槽组平移（N82）。
void GameWorld::ResetAllFatigue()
{
    // [L11·round1 判词更新] 帧 0x70 vs ORIG 0x7c 实测复核：4.4 带 call 帧恒 16k+4 量化
    // （P190 附注②，本函数族 0x44/0x54/0x64/0x74… 可达），ORIG 的 0x7c **非量化值**
    // 任何 pad 组合不可凑（2 个取址 double 已达 0x70，4 个过头到 0x80）⇒ N 封顶。
    // 其余 0x10 整帧槽平移（-0x20/-0x1c/-0x3c vs -0x10/-0xc/-0x2c）为避免 N126/N82，
    // 且缺 ORIG 的 `push %edi`（ORIG 用 %edi 承载 getCurSec/bApplied，4.4 恒 %ebx；
    // 裸 register 形实测 302→302 零效）⇒ 维持 N。
    static bool isFirst = true;
    // [b28d-sweep] P190 撑帧实测（推翻旧「double framePad 零效」判词）：ORIG 帧
    // 0x7c/深槽组（badge -0x46 / iterator -0x4c / msg -0x58）比我们深 0x10；
    // 2 个取址 double 撑帧 0x60→0x70，深槽组三处逐一对齐（ndiff 125→110）；
    // 4 个则过头（帧 0x80、badge -0x56，ndiff 回 125）。浅槽组（user -0x20/active
    // -0x1c/trace -0x3c/string -0x2c/allocator -0x25）仍浅 0x10 = N126 首槽常数 8
    // + 4.1 匿名 return-temp 专属槽，属 N82/N126 封顶。
    double framePad0;
    (void)&framePad0;
    double framePad1;
    (void)&framePad1;
    // [b28d-solo-GameWorld] _ZN9GameWorld15ResetAllFatigueEv verdict=DIFF 322/307
    // N7+N82+N104 封顶。本轮修法（均以 ORIG 0x086cc3ec 段实证）：
    //  ① 字面量直传 findGameScript(std::string)（0x086cc71e 单次 C1EPKc）——删具名
    //     name 局部后 p167 EXTRA _ZNSsC1ERKSs+_ZNSsD1Ev×2 全消（73→66 calls CLEAR）；
    //  ② 栈对象 AradServerStateMessage msg（0x086cc7c8 直 call C1，无 _ZnwjPv）——
    //     placement-new EXTRA 归零；
    //  ③ charging 内联实参（0x086cc46e movswl 直传，P0/P200）；
    //  ④ active 改 int 具名局部（0x086cc494 cwtl+movl 槽往返）；
    //  ⑤ cMyTrace 全表达式临时链（P227）复现 ctor 前求值 varargs；
    //  ⑥ bApplied 条件置位形复现 ORIG 物化岛。
    // 残余（不可造）：%edi↔%ebx 分配差（ORIG getCurSec/bApplied 用 %edi ⇒ push/pop
    // %edi 2 条；4.4 恒 %ebx；register bool 形实测 302→302 零效）N7；浅槽组 0x10 浅移
    // + 帧 0x7c vs 0x70（push edi 4 + 8）N82/N126；isApplied 后 EH 尾块相对序 rotate
    // N104；m_actionPoint receiver 多 2 条 add 寻址（N107 族）。
    // [b28d-solo-GameWorld] ORIG 0x086cc40a `movl $0,-0x46` + `movw $0,-0x42`（4+2 字节）
    // ⇒ 试聚合初始化 `= {0}`（P202①/P214③ 展开形）。
    short badge[3] = {0};
    SetWeekendBounsEvent();
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // b28d-solo：ORIG 0x086cc3f2 段 `cmp $0x2,%eax; setle %al; test %al,%al;
        // jne <循环尾>` ⇒ 镜像 if（<=2 早退）+ 负条件 goto 尾标签（P223/AddDailyItem 族）。
        if (user->get_state() <= 2)
        {
            goto L_next;
        }
        {
            // [b28d-solo-GameWorld] ORIG 0x086cc46e 段：ChargingFatigueBattery 结果
            // `movswl %ax,%edx` 直传（无槽，P0/P200 内联实参）；具名 short 局部会落槽。
            user->IncFatigueBatteryCharging(
                GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, 0));
            // ORIG 0x086cc494 `cwtl; mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp)`
            // ⇒ active 是 int 具名局部（非 short）。
            int active = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
            if (active != 0)
            {
                user->DecFatigueBatteryCharging(active);
                // [b28d-solo-GameWorld] P227 全表达式临时链：ORIG 0x086cc4be/0x086cc4cc
                // 在 ctor **之前**右起求值 varargs（%esi=GetFatigueBatteryCharging、
                // %ebx=getCurCharacNo）并跨 ctor 保活；具名 trace 对象在 4.4 恒 ctor 后求值。
                cMyTrace(__PRETTY_FUNCTION__, 0x1286, 0)(
                    "FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
                    user->getCurCharacNo(), active, user->GetFatigueBatteryCharging());
            }
            user->DBUpdateDBLogItem();
            user->ResetDBLogItem();
            user->RecoverFatigue(0);
            user->SendFatigue();
            user->resetCurCharacMemberBonusFatigue();
            user->resetAccountMemberBonusFatigue();
            user->resetAccountUsedFatigue();
            user->SetDailyBadge(badge);
            user->ResetDailyCharacExpandData();
            ((CBoosterGage*)user->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0x11))->send_data(user, 0);
            user->send_MissionList();
            user->ClearCurConditionEventStep();
            user->ClearCurConditionEventRewardStep();
            user->SendConditionEventInfo();
            user->ClearProperDungeonClearCount();
            user->SendProperDungeonClearCount();
            user->ResetRevengeDungeonClear();
            user->resetNPCRelationShipDailyData();
            user->sendNPCRelationShipFavor();
            user->getDungeonGainedGold()->reset();
            APSystem::DB_UpdateActionPoint::makeRequest(user->GetUID(),
                *(const APSystem::_SIG_LOAD_ACTION_POINT*)user->m_actionPoint.GetActionPoint(), true);
            APSystem::CUserProc::SetTodayActionAndCheckMedalReward(user);
            user->GetGoldControl()->SavetoDB(false, true, false);
            ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
                ->send_event_init_data(user, true);
            user->resetBlueMarbleEnterCount();
            user->sendBlueMarbleEnterCount();
            user->ResetCurCharacUsedGiftFatigueQuantity();
            // ORIG 0x086cc750-0x086cc760 `test %al; je; mov $1,%eax; jmp; mov $0,%eax;
            // mov %eax,%edi` ⇒ bApplied 为 callee-saved 寄存器 bool（跨 string D1 保活，
            // 无槽）。P248 裸 register 三条件全满足（P231/P307）。
            register bool bApplied = false;
            if (isFirst)
            {
                // [b28d-solo-GameWorld] P52/P134 链式单语句：ORIG 0x086cc6f2 getCurSec
                // 先求值存 %edi（跨 Get/findGameScript 保活），字面量直构按值 string
                // 临时（0x086cc71e 单次 C1EPKc，无 _ZNSsC1ERKSs）；具名 name 局部会多发
                // 一次拷贝构造 + 2 次析构（p167 EXTRA 实证）。
                if (static_cast<ARAD::Arad_EventPeriodDataManager*>(
                        ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                            std::string("Arad_EventPeriodDataManager")))
                        ->isApplied((ENUM_REPEAT_EVENT_CODE)0x8e,
                                    GlobalData::s_systemTime_.getCurSec()) != 0)
                {
                    bApplied = true;
                }
                else
                {
                    bApplied = false;
                }
            }
            if (bApplied)
            {
                // ORIG 0x086cc7c8 `lea -0x58(%ebp),%eax; call AradServerStateMessage::C1`
                // = 直接栈对象（无 placement-new extern call/landing，N83）。
                AradServerStateMessage msg;
                ARAD::notifyOpenMessageDialog(user, 0, 0x8e, msg);
            }
            user->m_field8ec32 = 4;
            InterSelectPcroomDailyReward::SendPacket(user);
            ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
                user, (HeroMissionCondition::MissionNo::T)5, 0);
        }
    L_next:;
    }
    if (!m_UserInWorld.empty())
    {
        isFirst = false;
    }
}

// ============================================================================
// 缓存 / 商店 / 任务重置族（ORIG 086cf684..086d13ba）
// ============================================================================

void GameWorld::ResetCacheCharactorMemory()
{
    for (std::map<unsigned int, CUser*>::iterator it = m_loginUserMap.begin();
         it != m_loginUserMap.end(); it++)
    {
        CUser* user = it->second;
        user->SetValidLastLoginCharac(false);
        user->SetLastLoginCharacNo(0);
        user->SetLastLoginChannelNo(0);
    }
}

bool GameWorld::ResetCacheCharactorMemory(unsigned int accId)
{
    std::map<unsigned int, CUser*>::iterator it = m_loginUserMap.find(accId);
    if (it != m_loginUserMap.end())
    {
        CUser* user = it->second;
        user->SetValidLastLoginCharac(false);
        user->SetLastLoginCharacNo(0);
        user->SetLastLoginChannelNo(0);
        return true;
    }
    return false;
}

void GameWorld::ResetEntireReliablePerson()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            if (user->getCurCharacR() != 0)
            {
                user->setCurCharacTradeGoldDaily(0);
                user->SetOverTradeGoldCount(0);
            }
        }
    }
}

void GameWorld::ResetOneADayItemShop()
{
    // b28c-lane3：P223 空块岛复现。ORIG 0x86cf82c `jne 86cf901`（尾 nop 岛）、
    // 0x86cf886 `jne 86cf8ac`（内层 nop 岛）、0x86cf8aa `jmp`、0x86cf8ff `jmp`
    // 共 4 处；4.4 对 `if(c){}else{}` 恒 jump-thread 掉 jmp+nop（-2 条）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
        goto L_outer;
    G_CDataManager()->m_itemShop->updateOneADayItemList();
    // ORIG 0x86cf8c2 ppEi 后缀 ++（temp 槽 -0x10）；it=-0x18 / end=-0x14（技巧25）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
            goto L_inner;
        user->SendOneADayItemShopIndex(
            G_CDataManager()->m_itemShop->GetOneADayItemList());
    L_inner: ;
    }
L_outer: ;
}

void GameWorld::testResetOneADayItemShop()
{
    // b28c-lane3：P223 空块岛复现（同 ResetOneADayItemShop，ORIG 0x086cf904）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
        goto L_outer;
    G_CDataManager()->m_itemShop->testupdateOneADayItemList();
    // ORIG 0x86cf8c2 ppEi 后缀 ++（temp 槽 -0x10）；it=-0x18 / end=-0x14（技巧25）
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
            goto L_inner;
        user->SendOneADayItemShopIndex(
            G_CDataManager()->m_itemShop->GetOneADayItemList());
    L_inner: ;
    }
L_outer: ;
}

// [b28d-11b] 修法：else 尾 goto END → 裸 return; 复现 ORIG 尾 nop 岛 → IDENTICAL 76 @0x086d12b2
void GameWorld::SetWeekendBounsEvent()
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&t, &local);
    // ORIG 86d12dc/86d12ec/86d12f3: mov -0x24(%ebp) = local+0x18 = tm_wday
    // b28d-lane11 新试形（P223 定案① 负条件 goto 共享尾标签串）：ORIG 尾
    // 0x086d1358/0x086d13ac 两 jmp + 0x086d13ae/0x086d13b1/0x086d13b4 三 nop 岛
    // + 0x086d13b7 fall nop；标签序 L0(0x3ac)/L3(0x3ae)/L1(0x3b1)/L2(0x3b4)。
    if ((local.tm_wday == 6 && local.tm_hour > 5) || local.tm_wday == 0 ||
        (local.tm_wday == 1 && local.tm_hour < 6))
    {
        if (!GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            Word_Param param;
            param.m_duration = 0;
            param.m_field2 = 0;
            GlobalData::s_event_manager->TriggerEventStart(0x57, param);
        }
        else
        {
            goto L3;
        }
        goto END;
    }
    else
    {
        if (local.tm_wday != 1)
        {
            goto L1;
        }
        if (local.tm_hour <= 5)
        {
            goto L2;
        }
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            GlobalData::s_event_manager->TriggerEventEnd(0x57);
            goto L0;
        }
        return;
    }
    // b28d-lane11 判词（0x086d1358/0x086d13ac/0x086d13ae/0x086d13b1/0x086d13b4
    // /0x086d13b7 实证）：if/else 保留 then 块物理前置 + 尾标签串造岛后 76=76，
    // 唯 L3(0x3ae) 岛的 jmp 被 4.4 thread 到相邻同目标 L1 岛的 jmp（P223 边界：
    // 单源 je 进入的尾岛恒合并）⇒ ndiff 封 1（N223）。
L0:;
L3:;
L1:;
L2:;
END:;
}

void GameWorld::DeleteDailyItem()
{
    // b28d-lane11：P227 全表达式临时链（ORIG 0x086c7b5c 先求 size/tick 实参再
    // ctor，且 trace 槽 -0x30/-0x1c）。
    // [b28d-solo-GameWorld] _ZN9GameWorld15DeleteDailyItemEv verdict=AE 82=82
    // 修法：删 size 实参的冗余 (int) cast（P41 变体）⇒ 4.4.6 复现 ORIG
    // tick→%esi / size→%ebx（0x086ccca2/0x086cccb4）；cast 形恒 tick→%ebx/size→%esi。
    cMyTrace(__PRETTY_FUNCTION__, 0x13da, 0)("DeleteDailyItem size(%d) start tick(%u)",
        m_UserInWorld.size(), GlobalData::s_systemTime_.getCurTickCount());
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            user->processDelDailyItem();
        }
    }
    GlobalData::s_systemTime_.update();
    cMyTrace(__PRETTY_FUNCTION__, 0x13e6, 0)("DeleteDailyItem end tick(%u)",
        GlobalData::s_systemTime_.getCurTickCount());
}

// [b28d-11b] 修法：it++（P28 增量侧）+ 镜像 if（setle/jne）+ 负条件 goto 尾标签
// 补尾 jmp 岛（P223）→ IDENTICAL 116 @0x086ccdce
void GameWorld::AddDailyItem()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
        {
            goto L_next;
        }
        user->AddDailyItem();
        if (user->getCurCharacR() != 0)
        {
            user->ResetDailyQuest();
            user->ResetTrainingQuest();
            PacketGuard packet;
            user->getCurCharacQuestR()->get_quest_info((char*)&packet);
            user->Send(packet);
        }
        // b28d-lane11：P225 推广——ORIG 两条件各发 `setne %al; test; je`，
        // 合并 && 被 4.4 折成直跳 ⇒ 拆嵌套 if。
        if (user->getCurCharacR() != 0)
        {
            if (user->get_charac_level() > 0x3b)
            {
                user->DimensionInoutUpdate(true, true);
            }
        }
    L_next:;
    }
}

bool GameWorld::ReselectDailyQuest()
{
    if (!G_CDataManager()->reselectDailyTrainingQuest())
    {
        return false;
    }
    return true;
}

void GameWorld::DailyEventModify()
{
    char buf[0x140];
    (void)buf;
}

void GameWorld::ResetCraneItemNeedMaterial(int param)
{
    G_CGameManager()->GetCraneMinigameManager()->updateCraneItemNeedMaterial(param);
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        // ORIG 0x86d1415-0x86d143d：`cmp $2; setle; test; jne SKIP` + body 尾
        // `jmp END` + SKIP 落点 nop ⇒ goto 形态钉块序（if/else 各写法均被 4.4
        // threading/朝向折叠，唯 goto 复现）；state>2 时 SendCraneItemNeedMaterial
        if (!(user->get_state() > 2))
        {
            goto CRANE_SKIP;
        }
        G_CGameManager()->GetCraneMinigameManager()->SendCraneItemNeedMaterial(user);
    CRANE_SKIP:
        ;
    }
}

// ORIG 全局自由函数 @0x086cbc85（定义于 CUserOnlyOrig.cpp）
int deleteSpecificItems(CUser* user,
                        const std::vector<std::pair<int, int> >* list);

void GameWorld::deleteSpecificItems(const std::vector<std::pair<int, int> >* list)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        ::deleteSpecificItems(user, list);
    }
}

// [b28d-11b] 修法：it++（P28 增量侧）158→138；残余 N82 帧/槽 @0x086d14d4
// [b28d-solo-GameWorld] _ZN9GameWorld20UpdateServerSnapShotEv verdict=DIFF-N 186/147
// N-DEP 封顶（等价替代）：ORIG 用 boost::posix_time::second_clock::local_time() +
// gregorian::to_iso_extended_string + posix_time::to_simple_string（39 条 boost 链）。
// b28d 实测：真 boost 形（仓内 Library3rd/Boost/Include 头 + 已挂 -I）可编译且本件
// 186=186 指令数精确对齐（NDIFF 76 全槽号差），但 ORIG 的 boost 是预编译库产物
// （0x086d4419/0x086d28b5/0x086d288c/0x080cde16/0x080ce4bc 为独立 T 符号），头内
// 实现展开致 TU +273 符号、missing 140→227（+87）且本仓无 libboost 定义 ⇒ 越界，
// 维持 localtime_r+strftime 等价替代（同值语义）。P225 推广拆嵌套 if 已复现两条件
// 的 setne 物化（150→147）。帧 0xac vs 0xcc 系 boost 对象槽组。
// [L11·round1 判词更新] ndiff 91→**79**（186/147）。本轮修法 = P221/P227：具名
// `cMyTrace trace(...)` 改全表达式临时链。残余不可造（附 ORIG 0x086d14d4 段实证）：
//  ① p167 MISSING 三件 boost（`second_clock::local_time` / `to_simple_string` /
//     `to_iso_extended_string`）+ EXTRA `CSystemTime::getCurSec` —— 维持 N-DEP
//     （真 boost 形实测 TU missing_in_game 135→222，+82 个 C++11 ABI 别名仅NEW ⇒ 纪律禁）；
//  ② ORIG 六个实参值先落 -0x80..-0x6c 槽（4.1 具名局部）再统一装载，我方内联无槽：
//     属版本装载策略差（P244/P0 反向），补具名亦无法补回 boost 的 string 链。
void GameWorld::UpdateServerSnapShot()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // [b28d-solo-GameWorld] P225 推广：ORIG 0x086d14d4 段两条件各发
        // `setne %al; test %al,%al; je` ⇒ 拆嵌套 if（合并 && 被 4.4 折直跳）。
        if (user != 0)
        {
        if (user->getCurCharacR() != 0)
        {
            CParty* party = (CParty*)user->GetParty();
            // ORIG 0x086d1541 `movl $0x0,-0x1c` + 0x086d1559 `movzwl %ax,%eax;
            // mov %eax,-0x1c` ⇒ dungIdx 是 int（4 字节清零 + ushort 零扩展存回）。
            int dungIdx = 0;
            if (party != 0)
            {
                dungIdx = party->getDungIndex();
            }
            // ORIG 用 boost::posix_time::second_clock::local_time()
            // 生成 iso_extended(date) 与 to_simple_string(time)，再以 "%s %s" 写入快照。
            // [b28d-solo-GameWorld] N-DEP 复验（b28d-solo 二次实测）：ORIG 0x086d4419/
            // 0x086d28b5/0x086d288c/0x080cde16/0x080ce4bc 确为 **W 弱符号**（nm 实证：
            // boost 头内模板实例，非外部库；仓内 Boost 1.48 头在 -I 中）。真 boost 形
            // 编译通过、本件 p167 30=30 CLEAR（旧 22/30，8 MISSING 归零）、本件 186/187，
            // 但 TU 级 missing_in_game 135→222（+87，其中 82 个为 gnu++0x 下 boost 类
            // 的 C++11 ABI 别名 C5/D5，ORIG 全库仅 2 个同类符号）⇒ 违反「仅NEW 不得
            // 上升」+ 禁发明符号纪律 ⇒ 回退等价替代（localtime_r+strftime 同值语义：
            // %Y-%m-%d / %H:%M:%S）。判词 N-DEP 维持，本函数余寄存器/槽差（N82附）。
            time_t now = GlobalData::s_systemTime_.getCurSec();
            struct tm local;
            localtime_r(&now, &local);
            char dateStr[16];
            char timeStr[16];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &local);
            strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &local);
            cMyTrace(__PRETTY_FUNCTION__, 0x1b61, 8)(
                "%u,%d,%u,%d,%d,%d,%d,%d,%s %s",
                  user->get_acc_id(),
                  G_CEnvironment()->get_server_group(),
                  user->getCurCharacNo(),
                  (int)user->getCurCharacVill(),
                  user->get_area(false),
                  dungIdx,
                  user->getCurCharacExp(),
                  user->getCurCharacMoney(),
                  dateStr, timeStr);
        }
        }
    }
}

void GameWorld::UpdateMiniCraneSeed()
{
    // Packet_MiniCraneSeed：PacketHeader(0x27f8, 0xe) + m_data(+0x0a)。内联 ctor
    // 产出 ORIG 弱 W 符号 _ZN20Packet_MiniCraneSeedC1Ev（@0x086d1d18）。
    int group = G_CEnvironment()->get_server_group();
    Packet_MiniCraneSeed packet;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
        ->SendTcpPacket((char*)&packet, 0xe);
}

// ============================================================================
// 频道判定 / 统计访问器 / 力战族（ORIG 82343ba..86cf56c）
// ============================================================================

bool GameWorld::InitChannelType()
{
    m_channelType = _GetChannelType();
    if (m_channelType == 0x13) return false;
    return true;
}


bool GameWorld::IsEnchantRevisionChannel()
{
    switch (m_channelType)
    {
    case 7:
    case 0xe:
        return true;
    default:
        return false;
    }
}

bool GameWorld::IsCheckUnusableEquipmentChannel()
{
    if (m_channelType == 0xe) return true;
    return false;
}

StatisticsPvP* GameWorld::getStatisticsPvP()
{
    return &m_pvp;
}

StatisticsNatType* GameWorld::GetStatisticsNatType()
{
    return &m_natType;
}

StatisticsShusiaService* GameWorld::getStatisticsShusiaService()
{
    return &m_shusia;
}

StatisticsToObserveCrash* GameWorld::getStatisticsToObserveCrash()
{
    return &m_obsCrash;
}

void GameWorld::CancelPowerUp()
{
    std::vector<CUser*> list;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        // b28c-lane3：P3——ORIG 0x86cf… 直测 `test %al,%al`（无 setne 归一化），
        // 说明被调实际返回单字节非 bool；头声明 bool 时 4.4 会加 test+setne。
        if ((unsigned char)user->IsPowerUp())
        {
            user->SetPowerUp(false);
            list.push_back(user);
        }
    }
    SendPowerUpList((ENUM_POWER_UP_ACTIVATE_TYPE)0, list);
}

// [b28d-11b] N：浮点/双精度局部槽位差 0x18 + 帧差 0x10（P252/N82）@0x086cf1fe
void GameWorld::GetPowerUpDomain(std::vector<CUser*>& listA, std::vector<CUser*>& listB)
{
    // m_powerParameter(+0xa628) + 0x50
    double threshold =
        (double)(G_CDataManager()->m_powerParameter.m_powerUpAssaultVictoryRatio) / 100.0;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->isJoinPowerWar())
        {
            // [b28d-solo-GameWorld] _ZN9GameWorld16GetPowerUpDomainERSt6vectorIP5CUserSaIS2_EES5_
            // verdict=DIFF 104/103（b28d-solo 本轮；唯一残差 = x87 比较 lowering 1 处）。
            // 修法：①ORIG 0x086cf274 `test %eax,%eax; sete %al; test; jne 尾` ⇒ ==0 早退；
            // ②**本轮 P248 档①**：ORIG 0x086cf28c/0x086cf2a8 为 `mov $0,%edx;
            // mov %eax,-0x40; mov %edx,-0x3c; fildll`（unsigned 32→double 零扩展 64 位装载）
            // ⇒ 两处 `(double)(unsigned int)call()`（旧 `(double)` 恒产 fildl；`(long long)`
            // 探针无效）；③P252/P243：具名 double assaultCount/victoryCount 按 ORIG 求值序
            // （分母 -0x20 先、分子 -0x18 后，`fldl -0x18; fdivl -0x20`）；④比较操作数序
            // `threshold >= winRate`（ORIG `fldl -0x28(threshold); fldl -0x10`）。
            // 残余 N85：ORIG `fucompp; fnstsw %ax; sahf`（3 条）vs 4.4 `fucomip; fstp`（2 条），
            // 跨版本 x87 比较 lowering；P230 补的 -march=i486 属 TU 级旗标（禁改路由）。
            if (user->GetPowerWarAssaultCount() == 0)
            {
                goto L_NEXT;
            }
            {
                // ORIG 0x086cf28c..0x086cf2bf：分母先 fildll+fstpl -0x20、分子
                // fildll+fstpl -0x18，再 `fldl -0x18; fdivl -0x20` ⇒ 两个具名
                // double 局部按 ORIG 求值序声明（P252 槽往返 + P243 第一操作数）。
                double assaultCount =
                    (double)(unsigned int)user->GetPowerWarAssaultCount();
                double victoryCount =
                    (double)(unsigned int)user->GetPowerWarAssaultVictoryCount();
                double winRate = victoryCount / assaultCount;
                if (threshold >= winRate)
                {
                    if (user->getPowerSide() == 1)
                    {
                        listA.push_back(user);
                    }
                    else
                    {
                        listB.push_back(user);
                    }
                }
            }
        L_NEXT:;
        }
    }
}

void GameWorld::SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE activateType, std::vector<CUser*>& list)
{
    PacketGuard packet;
    packet.put_header(0, 0xd8);
    // ORIG 0x86cf498-0x86cf4a8: `movzbl %al,%eax` = uchar cast (非 char)
    packet.put_byte((unsigned char)activateType);
    // ORIG 0x86cf4b3-0x86cf4b8: size() 结果直落 int 形参，无 (short) 截断
    packet.put_short(list.size());
    for (std::vector<CUser*>::iterator it = list.begin(); it != list.end(); ++it)
    {
        // ORIG 0x86cf4ee-0x86cf4f6: get_unique_id(ushort)→int 仅 movzwl，无 (short)
        packet.put_short((*it)->get_unique_id());
    }
    packet.finalize(true);
    // ORIG 0x86cf54e-0x86cf565: call G_GameWorld 后 %eax 直落 this 槽 ⇒ 自由函数形
    G_GameWorld()->send_to_vill(7, packet);
}

void GameWorld::OnEndPowerWar(PacketGuard& packet)
{
    int var0 = 0;
    std::map<unsigned short, CUser*>::iterator it;
    int var1 = 0;
    packet.put_byte(GlobalData::s_power_manager->GetWinnerSide());
    packet.put_short(0);
}

void GameWorld::OnRefreshPowerWarProcessInfo()
{
    // ORIG 0x86cedfb dead init + 0x86cee0d `size(); test; setne; test; je` 守卫
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        user = it->second;
        // ORIG 0x86cee5b `test %al; setne %al; test; je`：uchar 返回单判物化
        if (user->isJoinPowerWar())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            int last = user->GetPowerWarProcessInfoLastUpdateTime();
            // m_powerParameter(+0xa628) + 0x30
            int interval =
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo * 0x3c;
            // ORIG 0x86ceea1-0x86ceeb0 `cmp -0xc,%eax; jle`：差值先算、方向 (now-last)>interval
            if (now - last > interval)
            {
                GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
                    ->SendPowerWarProcessInfo(user->get_charac_no(-1));
            }
        }
    }
    }
}

// [b28d-11b] N：局部/槽位差（N82）@0x086cef36
// [b28d-solo-GameWorld] _ZN9GameWorld23OnBanAbuserFromPowerWarEv verdict=AE 140=140
// 修法：P197③ 两段式预清 `CUser* user = NULL;` + `if (m_UserInWorld.size() != 0)` 守卫
// （复现 ORIG 0x086cef3e movl $0,-0x18 + 0x086cef50 call size; test; setne; test; je）
// + P221 全表达式临时链 cMyTrace(...)(...)（具名 trace 槽 -0x2c vs ORIG -0x3c；130→139→140）。
void GameWorld::OnBanAbuserFromPowerWar()
{
    // [b28d-solo-GameWorld] P197③ 两段式预清 + size()!=0 守卫：ORIG 0x086cef3e
    // `movl $0,-0x18` + 0x086cef50 `call size; test %eax,%eax; setne %al; test; je`。
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        user = it->second;
        if (user->isJoinPowerWar())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            int last = user->GetPowerWarLastPlayTime();
            // m_powerParameter(+0xa628) + 0x34
            int limit = G_CDataManager()->m_powerParameter.m_abuserBanTime * 0x3c;
            if (limit < now - last)
            {
                GlobalData::s_power_manager->ProcessJoinPowerWar(user, 0);
                PacketGuard packet;
                packet.put_header(0, 0xe2);
                packet.finalize(true);
                user->Send(packet);
                // [b28d-solo-GameWorld] P221：全表达式临时链复现 ORIG trace 深槽 -0x3c
                // （具名 trace 落 -0x2c，迭代器槽随之浅移）。
                cMyTrace(__PRETTY_FUNCTION__, 0x178e, 0)(
                      "[PowerWar] POWER WAR BAN USER (USER:%s(%d), LAST:%d, CURR:%d)",
                      user->getCurCharacName(), user->getCurCharacNo(), last, now);
            }
        }
    }
    }
}

void GameWorld::CleanupPowerWarVillToStart()
{
    // ORIG 0x86cf13e dead init + 0x86cf150 `call size; test; setne; test; je`
    // ⇒ size()!=0 守卫 + 循环外 user 声明
    CUser* user = NULL;
    if (m_UserInWorld.size() != 0)
    {
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); ++it)
        {
            user = it->second;
            // ORIG 0x86cf19b `cmp $7,%al`：getCurCharacVill char 返回 QI 直判
            if (user->getCurCharacVill() == 7)
            {
                GlobalData::s_power_manager->ProcessJoinPowerWar(user, 0);
            }
        }
    }
}

// [b28d-11b] N：帧差 0x10 + PacketGuard 槽位（N82）@0x086cad68
void GameWorld::get_user_info(CUser* user, int targetId, int type)
{
    PacketGuard packet;
    if (type == 2)
    {
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(2);
        user->make_basic_info((char*)&packet, 2);
        packet.finalize(true);
        user->Send(packet);
        return;
    }
    if (targetId == 0xffff)
    {
        return;
    }
    CUser* target = find_from_world((unsigned short)targetId);
    if (target == 0)
    {
        LogManager::logFormat(1, "world.cpp",
            __PRETTY_FUNCTION__, 0x107e,
            "[GameWorld::get_user_info] can't find user in world");
        return;
    }
    // verdict=AE（三条件短路链早退，去具名 hidden 落槽）
    // [b28d-solo-GameWorld] ORIG 0x086cae5a `cmp $3; jne 0x86caeac` + 0x086cae7d
    // `xor $1; test; je 0x86caeac` + 0x086caeb3 `test %al; jne 尾` ⇒ 三条件短路链
    // 汇合到统一 false 块（无具名 bool/无预置槽）；具名 hidden 落槽 -0x9 并使帧 +0x10。
    if (type == 3 &&
        !GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) &&
        GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
    {
        return;
    }
    {
        if (type == 3)
        {
            // ORIG +0x15f：receiver = target（-0xc 槽=find_from_world 结果），非 user
            ((CTitleBook*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe))
                ->sendListOtherUser(user, (ENUM_TITLE_BOOK_INFO_TYPE)2);
        }
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(type);   // ORIG +0x1b4 int 直传无扩展；(char) 仅 make_basic_info(+0x1d5 movsbl)
        if (type != 2)
        {
            packet.put_short(1);
        }
        target->make_basic_info((char*)&packet, (char)type);
        packet.finalize(true);
        user->Send(packet);
    }
}

void GameWorld::make_packet_chat_msg(PacketGuard& packet, CUser* user, char* msg, int len,
                                     char type, ENUM_NOTIPACKET notify)
{
    packet.clear();
    packet.put_header(0, (int)notify);
    packet.put_byte((int)type);
    if (user == 0)
    {
        packet.put_short(0);
        packet.put_byte(0);
    }
    else
    {
        packet.put_short(user->get_unique_id());
        packet.put_byte(user->GetServerGroup());
    }
    packet.put_int(len);
    packet.put_str(msg, len);
    packet.finalize(true);
}

void GameWorld::make_packet_chat_msg_hyper_link(PacketGuard& packet, CUser* user,
                                                MSG_ITEM_HYPER_LINK_MESSAGE& msg,
                                                ENUM_NOTIPACKET notify)
{
    packet.clear();
    // ORIG 86cf…：对 msg 的每次读取均为 `mov 0x14(%ebp),%eax` 直用来参槽
    // ⇒ 4.4 中“引用参无局部别名绑定”时不落独立槽；显式 `T& msg = linkMsg;` 会物化 -0x10 槽
    packet.put_header(0, (int)notify);
    packet.put_byte(msg.m_typeD);
    if (user == 0)
    {
        packet.put_short(0);
        packet.put_byte(0);
    }
    else
    {
        packet.put_short(user->get_unique_id());
        packet.put_byte(user->GetServerGroup());
    }
    packet.put_int(msg.m_textLen14);
    packet.put_str(msg.m_text18, msg.m_textLen14);
    packet.put_byte(msg.m_linkCount13b);
    for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
    {
        packet.put_binary((char*)msg.m_links13c[i], 0x68);
    }
    packet.finalize(true);
}

// [b28d-11b] 修法：memset→char path[256]={0} 复现 4.1 rep stos（P214③）56→45；残余 N80 变参求值序/具名局部 @0x086c9638
// [b28d-solo-GameWorld] _ZN9GameWorld15WriteLogChatMsgEP5CUsercPc verdict=NEAR 75=75
// 助记符全等，仅 N82/P112 槽分配差：ORIG path=-0x120/log=-0x20/state=-0x134/pvpIdx=-0x130；
// 4.4.6 path=-0x128/log=-0x28/state=-0x20/pvpIdx=-0x1c（标量区与聚合对象区方向相反，声明序不可翻）。
// [L11·round1 追加实测（判据⑩边界）] 两形均零效（复编复测 ndiff 恒 10）：
//   ① `int state; int pvpIdx;` 前置到 `char path[256]` 之前 + 赋值留原位（纯声明重排）；
//   ② 声明/赋值分离（`int state; … state = user->get_state();`）。
// ⇒ 本件 4.4 按**首次使用序**分配：`char path[256]={0}` 的 rep stos 是最早使用点 ⇒
// 占最深槽，标量只能在浅区；ORIG 4.1 按类型分组（标量浅/聚合深）相反 ⇒ 判 N（P112/N98）。
void GameWorld::WriteLogChatMsg(CUser* user, char type, char* msg)
{
    // [b28d-solo-GameWorld] _ZN9GameWorld15WriteLogChatMsgEP5CUsercPc：ORIG 0x086c964d
    // `call G_GameWorld; mov %eax,(%esp); call GetChannelType`（两段链，非成员直调）；
    // 0x086c965a `cmp $6; sete %al; test; je` 需拆嵌套 if 让 4.4 对寄存器直比物化。
    if (G_GameWorld()->GetChannelType() == 6)
    {
        if (user != 0)
        {
            // [b28d-solo-GameWorld] P197③/P28 已试：标量先声明（无初始化）+ 后赋值
            // ⇒ 4.4 读数逐值不变（仍 path=-0x128/log=-0x28/state=-0x20/pvpIdx=-0x1c）
            // ⇒ 4.4 槽按首次使用序 + 聚合对象优先深槽（path/log 深、标量浅），
            // 与 ORIG 4.1 声明序（state/pvpIdx 最深）方向相反，N82/P112 封顶。
            // b28d-solo 复验：把 state/pvpIdx 声明前置到 path 之前（赋值仍留在
            // sprintf 之后）实测槽位逐值不变（state=-0x20/pvpIdx=-0x1c）
            // ⇒ 4.4 恒按首次使用序分配（N98），声明序旋钮无效。
            // ORIG 0x086c96b5/0x086c96c6 两句前置具名槽：state=-0x134、pvpIdx(cwtl)=-0x130。
            char path[256] = {0};
            sprintf(path, "./log/%s/GuildWarChat",
                    G_CEnvironment()->get_file_name());
            int state = user->get_state();
            int pvpIdx = (int)user->GetPvpIndex();
            // ORIG ctor 在全部 getter 之后 ⇒ P227 全表达式临时链。
            CMyFileLog(__PRETTY_FUNCTION__, 0xd6d)(path,
                "T:%d\tMsg:%s\tC_Name:%s\tG_Key:%d\tR_Idx:%d\tState:%d",
                (int)type, msg, user->getCurCharacName(), user->get_charac_guildkey(),
                pvpIdx, state);
        }
    }
}

void GameWorld::send_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                               int a, int b, const char* msg, int len)
{
    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte((int)type);
    packet.put_byte(a);
    int nameLen = (int)strlen(from->getCurCharacName());
    packet.put_int(nameLen);
    packet.put_str(from->getCurCharacName(), nameLen);
    packet.put_byte(0);
    packet.put_int(len);
    packet.put_str(msg, len);
    packet.finalize(true);
    to->Send(packet);
}

void GameWorld::check_peerTypemsg_sameWorldUser(CUser* from, CUser* to, char type,
                                                char* msg, int len)
{
    // ORIG 0x86cab92 段1：`call G_GameWorld; mov %eax,(%esp); call IsPvP` 自由函数形；
    // pvpDiff/black 物化无槽（`test %al` 直判）⇒ 历史 register bool asm("al")；
    // b28d-lane11 按 P0/N96 清绑。
    // verdict=AE（去具名 pvpDiff/black，&&/|| 直判无槽）
    // [b28d-solo-GameWorld] _ZN9GameWorld31check_peerTypemsg_sameWorldUserEP5CUserS1_cPci：
    // ORIG 0x86cab.. `&& 结果 %al; test %al,%al; je`（无槽）⇒ 去具名 pvpDiff/black 内联
    // （具名版多 mov %al,-0x9/-0xa + cmpb 且帧 0x34→0x44）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        (from->GetServerGroup() != to->GetServerGroup()))   // +0x03a mov $1/$0 物化
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x15, 0x15, msg, len);
        return;
    }
    if (to->isBlackUser(from->get_charac_no(-1)) ||        // +0x08e..0x0ab 臂1
        from->isBlackUser(to->get_charac_no(-1)))          // +0x0b4..0x0d1 臂2
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x4d, 0xfe, msg, len);
    }
    else
    {
        if (GlobalData::s_pGMAccounts_->isGM(to->get_acc_id()))     // +0x134 acc_id 先算
        {
            send_peerTypemsg_sameWorldUser(to, from, type, 0x5a, 0x20, msg, len);
        }
        else
        {
            send_peerTypemsg_sameWorldUser(from, to, type, 0, 1, msg, len);
        }
    }
}

// [b28d-11b] N：帧差 0x10 + 槽位（N82）@0x086c975e
// [b28d-sweep] **真缺口修复**：case 3 与 case 0x22 在 ORIG 中共享同一 tail —— 跳表
// 0x8cfba20[3]=0x86ca20b（needClean 逻辑）、[0x22]=0x86ca3de（make_packet_chat_msg +
// send_to_area），case 3 正常路径**落到** 0x86ca3de（Ghidra C 亦为 fallthrough）。
// 旧实现两个 case 各写一份 ⇒ 多 1×{make_packet_chat_msg, get_area, getCurCharacVill,
// send_to_area}（p167 EXTRA 各 +1）。改 fallthrough 后 1299→1267（-32 条），
// p167 EXTRA 4 项归零。余 1255/1267：N82/N87/N89/N91（string/PacketGuard 临时与 EH
// landing 块序；p167 残差仅 C1↔C2 / D1↔D2 同址别名对 +1/+2 个 EH 析构）。
// [b28d-solo-GameWorld] _ZN9GameWorld13send_chat_msgEP5CUserctjPciS2_i15ENUM_NOTIPACKET
// verdict=DIFF 1255/1267 N82/N87/N89 封顶（帧 0x21c vs 0x22c + 大帧槽组平移）。
// b28d-solo 本轮真修：case 0xd/0xf 原只写一次 `int nameLen=strlen(...)` + put_int(nameLen)；
// ORIG 0x086caa08 为 `put_int(strlen(from->getCurCharacName()))` 内联 + 0x086caa2d 具名
// nameLen（两次独立 strlen/getCurCharacName）⇒ p167 MISSING 各 1 归零。
// b28d-solo 本轮归一化复核（46 块残差抽样）：①N87 —— ORIG 0x086c9784 裸
// `shl %cl` 无掩码 vs 4.4 恒插 `and $0x1f`；②符号序 —— ORIG `movsbl; cmp $0x1e;
// ja` vs 4.4 `cmpb $0x1e,mem; jg`（等价，P196 判据：值域 char 无 ≥0x80 分叉）；
// ③string 临时槽组/寄存器（N82/N89 STL 内联）。p167 CLEAR 语义完成。
// p167 调用点差集（/tmp/tu_GameWorld.o）：MISSING G_GameWorld×3 / getCurCharacName×1 /
// strlen×1；EXTRA make_packet_chat_msg×1 / send_to_area×1 / G_CGameManager×2 /
// PacketGuardC1/D1×3 / basic_string D1×1（ORIG 部分 case 走 G_GameWorld()-> 自由函数
// receiver 形 = P34/P277 族 + 分支调用点归位，巨件未逐 case 收敛，语义待三件套复验）。
void GameWorld::send_chat_msg(CUser* from, char type, unsigned short target,
                              unsigned int characNo, char* msg, int len, char* nick,
                              int nickLen, ENUM_NOTIPACKET notify)
{
    // [GameWorld-sweep] P196 符号性直证：ORIG 0x086c9774 `movsbl -0x1ec(%ebp),%eax;
    // cmp $0x1e,%eax; ja` —— 符号扩展到 int 后**无符号**比较 ⇒ 原码是
    // `(unsigned int)type < 0x1f`（写裸 `type < 0x1f` 生 `cmpb $0x1e; jg` 有符号
    // 形，type≥0x80 时语义分叉）。
    if ((unsigned int)type < 0x1f &&
        ((1 << (type & 0x1f)) & 0x7fff0411U) != 0 && from != 0)
    {
        from->DisConnSig((DISCONN_SIG)0x18, 1, 0);
        return;
    }
    if (type != 0)
    {
        std::string filtered = CSyncSlangFilter::GetInstance()->Filter(std::string(msg));
        // [b28d-sweep] ORIG 0x086c985b 段 `test %eax,%eax; sete %al; test %al,%al;
        // je <body>` = 空判定正极性（P197④ 镜像族）⇒ 写 `size()==0` 早退。
        if (filtered.size() == 0)
        {
            return;
        }
        strncpy(msg, filtered.c_str(), filtered.size());
    }
    PacketGuard packet;
    if (from != 0)
    {
        if (IsForbiddenChat(from->getCurCharacName()))
        {
            unsigned int left = GetLeftTimeOfForbiddenChat(from->getCurCharacName());
            if (left == 0)
            {
                EnableChat(from->getCurCharacName());
            }
            else
            {
                // [GameWorld-sweep] ORIG 0x086c9989 段在 forbidden 分支**无独立
                // PacketGuard 构造**（唯一 PacketGuard C2 在 0x086c9918，即函数级
                // packet）⇒ 原码复用 packet，不另建 err 对象。
                packet.put_header(0, 0xc);
                packet.put_byte(10);
                packet.put_short(0);
                cMyTrace trace(__PRETTY_FUNCTION__, 0xdf3, 0);
                trace("%s is forbidden to chat for %d msec.\n",
                      from->getCurCharacName(), left);
                packet.put_int(left / 60000);
                packet.finalize(true);
                from->Send(packet);
            }
            return;
        }
    }
    WriteLogChatMsg(from, type, msg);
    switch (type)
    {
    case 0:
        if (from == 0)
        {
            make_packet_chat_msg(packet, 0, msg, len, type, notify);
            send_all(packet);
        }
        else
        {
            // ORIG 0x86c9ac1：type==0 且 from 非空时仅调用 get_acc_id（结果丢弃）。
            from->get_acc_id();
        }
        break;
    case 1:
    case 0x21:
    {
        CUser* to = find_from_world(target);
        if (to == 0)
        {
            // ORIG 0x86ca0b9：跨服私聊监控包（m_chatType/m_characNo/
            // m_recverCharId/m_msgLen/m_msg/m_buddyNameLen/buddy_n_user_id_what；
            // SendPacket 长度 0x3b+len）。
            Packet_Monitor_Other_Channel_Chat pkt;
            pkt.m_chatType = (int)type;   // ORIG 0x86c9b35 case 1 离线分支 +0xa 槽（local_1dd）
            pkt.m_characNo = from->getCurCharacNo();
            pkt.m_recverCharId = characNo;
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            pkt.m_buddyNameLen = (char)nickLen;
            memcpy(pkt.buddy_n_user_id_what, nick, nickLen);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, 0x3b + len);
            break;
        }
        PacketGuard pm;
        bool black = to->isBlackUser(from->get_charac_no(-1)) ||
                     from->isBlackUser(to->get_charac_no(-1));
        if (black)
        {
            pm.put_header(0, 0x41);
            pm.put_byte((int)type);
            pm.put_byte(0x4d);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(len);
            pm.put_str(msg, len);
            pm.finalize(true);
            from->Send(pm);
        }
        else if (GlobalData::s_GM_Manager->IsGm(to->get_acc_id()) == 0)
        {
            // [b28d-solo-GameWorld] P34/P277：ORIG 0x086c9e1b `call G_GameWorld;
            // mov %eax,(%esp); call IsIntegratedPvPBaseChannel`（自由函数形 receiver）。
            if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                from->GetServerGroup() != to->GetServerGroup())
            {
                pm.put_header(0, 0x41);
                pm.put_byte((int)type);
                pm.put_byte(0x15);
                int nameLen = (int)strlen(to->getCurCharacName());
                pm.put_int(nameLen);
                pm.put_str(to->getCurCharacName(), nameLen);
                pm.put_byte(0);
                pm.put_int(len);
                pm.put_str(msg, len);
                pm.finalize(true);
                from->Send(pm);
            }
            else
            {
                pm.put_header(0, 0x41);
                pm.put_byte((int)type);
                pm.put_byte(0);
                int nameLen = (int)strlen(from->getCurCharacName());
                pm.put_int(nameLen);
                pm.put_str(from->getCurCharacName(), nameLen);
                pm.put_byte(0);
                pm.put_int(len);
                pm.put_str(msg, len);
                pm.finalize(true);
                to->Send(pm);
            }
        }
        else
        {
            pm.put_header(0, 0x41);
            pm.put_byte((int)type);
            pm.put_byte(0x5a);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(len);
            pm.put_str(msg, len);
            pm.finalize(true);
            from->Send(pm);
        }
        break;
    }
    case 2:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            CParty* party = (CParty*)from->GetParty();
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }
        break;
    case 3:
    {
        // [b28d-solo-GameWorld] ORIG 0x086ca212 `call G_GameWorld; call getUserVillage`
        Village* village = G_GameWorld()->getUserVillage(from);
        if (village == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0xf1d, 5);
            trace("getUserVillage() call failed");
            break;
        }
        bool needClean = false;
        if (village->get_gate_area() != from->get_area(false) &&
            from->getCurCharacVill() != 8 && (int)notify != 0x72 && (int)notify != 0x77)
        {
            needClean = true;
        }
        if (needClean)
        {
            int cleanCount = G_CDataManager()->m_serverParameter.GetCleanChattingCount(
                from->get_charac_level());
            std::string smsg(msg);
            int add = from->isDuplicationMessage(smsg) != 0
                          ? G_CDataManager()->m_serverParameter.GetPaneltyDupMessage()
                          : 1;
            from->incChattingMessageCount(add * cleanCount);
            if (from->getChattingMessageCount() >
                G_CDataManager()->m_serverParameter.GetMaxCleanChattingCount())
            {
                from->setChattingMessageCount(0);
                from->reqHumanCertify4ClearMap(false);
                break;
            }
        }
        // [b28d-sweep] ORIG 跳表 0x8cfba20[3]=0x86ca20b / [0x22]=0x86ca3de：case 3 的
        // 正常路径（needClean 假或 count<=max）**落到** 0x86ca3de，与 case 0x22 共享同一
        // 块（ORIG 全函数仅一次 make_packet_chat_msg/send_to_area，0x86ca41f/0x86ca45f）
        // ⇒ 此处不写 break，fallthrough 到 case 0x22。
    }
    case 0x22:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            send_to_area(from->getCurCharacVill(), from->get_area(false), packet);
        }
        break;
    case 4:
        make_packet_chat_msg(packet, from, msg, len, type, notify);
        send_all(packet);
        break;
    case 5:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            if (from->GetAssaultPlace() == 0)
            {
                // [b28d-solo-GameWorld] ORIG 0x086ca552 单次 `call G_CGameManager`
                // + `movswl %ax,%ebx` + GetPvp（无判空三元；p167 EXTRA 实证）。
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp(packet);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, false, &packet);
            }
        }
        break;
    case 6:
        if (from->getCurCharacNo() != 0 && from->get_charac_guildkey() != 0)
        {
            // ORIG 0x86ca6ac：Packet_Monitor_Guild_Chat（ORIG 实证：+0xa 槽写
            // guildkey、+0xe 槽写 characNo，与头文件字段名相反；SendPacket 长度
            // 0x13+len）。
            Packet_Monitor_Guild_Chat pkt;
            pkt.m_charNo = from->get_charac_guildkey();
            pkt.m_guildKey = from->getCurCharacNo();
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            GlobalData::s_guild_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, len + 0x13);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                __PRETTY_FUNCTION__,
                0xf8c, "GUILD_CHAT_ERROR : charac no == %d || guild key == %d ",
                from->getCurCharacNo(), from->get_charac_guildkey());
        }
        break;
    case 8:
        if (from->getCurCharacNo() != 0 &&
            from->get_charac_memberkey() != 0)
        {
            // ORIG 0x86ca7dc：Packet_Monitor_Member_Chat（SendPacket 长度 0xf+len）。
            Packet_Monitor_Member_Chat pkt;
            pkt.m_memberId = from->get_charac_memberkey();
            pkt.m_msgLen = (unsigned char)len;
            memcpy(pkt.m_msg, msg, len);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, len + 0xf);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                __PRETTY_FUNCTION__,
                0xfa3, "MEMBER_CHAT_ERROR : member key == %d ",
                (int)from->get_charac_memberkey());
        }
        break;
    case 9:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            if (from->GetAssaultPlace() == 0)
            {
                // [b28d-solo-GameWorld] ORIG 0x086ca62c 单次 G_CGameManager（同上）
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp_team(packet, from);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, true, &packet);
            }
        }
        break;
    case 0xb:
    case 0xc:
    case 0xe:
    {
        // ORIG 0x86ca8b8：Packet_Monitor_MegaPhone::set(channel, uniqueId,
        // name, msgLen, msg, type)；SendPacket 长度 0x2e+len。
        Packet_Monitor_MegaPhone pkt;
        pkt.set(G_CEnvironment()->get_channel_no(), from->get_unique_id(),
                from->getCurCharacName(), (unsigned char)len, msg,
                (unsigned char)type);
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
            ->SendPacket((char*)&pkt, len + 0x2e);
        break;
    }
    case 0xd:
    case 0xf:
    {
        PacketGuard pm;
        pm.put_header(0, 0x76);
        pm.put_byte((int)type);
        pm.put_byte(G_CEnvironment()->get_channel_no());
        pm.put_short(from->get_unique_id());
        // ORIG 0x086caa08 `put_int(strlen(from->getCurCharacName()))` 内联 + 0x086caa2d
        // 具名 nameLen（%ebx）⇒ 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 实证）。
        pm.put_int((int)strlen(from->getCurCharacName()));
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(len);
        pm.put_str(msg, len);
        pm.finalize(true);
        // [b28d-solo-GameWorld] ORIG 0x086caa9c `call G_GameWorld; call send_all`
        G_GameWorld()->send_all(pm);
        break;
    }
    case 0x28:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            BlueMarble* bm = (BlueMarble*)from->getBlueMarble();
            if (bm != 0)
            {
                bm->sendToBlueMarble(packet);
            }
        }
        break;
    }
}

// [b28d-11b] N：帧差 + 位折叠（N6/N82）@0x086cfa10
// [b28d-solo-GameWorld] _ZN9GameWorld24send_chat_msg_hyper_linkEP5CUserR27MSG_ITEM_HYPER_LINK_MESSAGE15ENUM_NOTIPACKET
// verdict=DIFF 1321/1318 N82/N6/N89 封顶（帧 0xa5c vs 0x35c + 位折叠 + 大帧槽组）。
// b28d-solo 本轮真修：同 send_chat_msg case 0xd/0xf —— `put_int(strlen(...))` 内联 +
// 具名 nameLen 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 归零）。
// p167 调用点差集（/tmp/tu_GameWorld.o）：MISSING G_GameWorld×3 / getCurCharacName×1 /
// strlen×1；EXTRA G_CGameManager×2 / basic_string D1×1（同 P34/P277 族 receiver 形 +
// 巨件分支调用点未逐 case 收敛，语义待三件套复验）。
void GameWorld::send_chat_msg_hyper_link(CUser* from,
                                        MSG_ITEM_HYPER_LINK_MESSAGE& msg,
                                        ENUM_NOTIPACKET notify)
{
    if (msg.m_typeD < 0x1f && ((1 << (msg.m_typeD & 0x1f)) & 0x7fff0411U) != 0 &&
        from != 0)
    {
        from->DisConnSig((DISCONN_SIG)0x18, 1, 0);
        return;
    }
    if (msg.m_typeD != 0)
    {
        std::string filtered = CSyncSlangFilter::GetInstance()->Filter(std::string(msg.m_text18));
        if (filtered.size() != 0)
        {
            strncpy(msg.m_text18, filtered.c_str(), filtered.size());
        }
        else
        {
            return;
        }
    }
    PacketGuard packet;
    WriteLogChatMsg(from, msg.m_typeD, msg.m_text18);
    switch (msg.m_typeD)
    {
    case 0:
    case 7:
    case 10:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
    case 0x1f:
    case 0x20:
    case 0x21:
    case 0x24:
    case 0x25:
    case 0x26:
        break;
    case 1:
    {
        CUser* to = find_from_world(msg.m_itemSpaceE);
        if (to == 0)
        {
            // ORIG 0x86d01c5：Packet_Monitor_Other_Channel_Chat_Hyper_Link
            // （m_chatType/m_characNo/m_recverCharId/m_msgLen/m_msg/
            //  m_buddyNameLen/buddy/m_itemCount/m_items；SendPacket 长度
            //  0x174+textLen）。
            Packet_Monitor_Other_Channel_Chat_Hyper_Link pkt;
            pkt.m_chatType = msg.m_typeD;   // ORIG 0x86cfa10 case 1 离线分支 +0xa 槽（local_a15）
            pkt.m_characNo = from->getCurCharacNo();
            pkt.m_recverCharId = msg.m_slot10;
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            pkt.m_buddyNameLen = (char)msg.m_nameLen118;
            memcpy(pkt.buddy_n_user_id_what, msg.m_name11c, msg.m_nameLen118);
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items[i], msg.m_links13c[i], 0x68);
            }
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, msg.m_textLen14 + 0x174);
            break;
        }
        PacketGuard pm;
        bool black = to->isBlackUser(from->get_charac_no(-1)) ||
                     from->isBlackUser(to->get_charac_no(-1));
        if (black)
        {
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0x4d);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            from->Send(pm);
        }
        // [b28d-solo-GameWorld] P34/P277：ORIG 自由函数形 receiver（hyper_link 同族）。
        else if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
                 from->GetServerGroup() != to->GetServerGroup())
        {
            // ORIG 0x86cfe74（put_byte 0x15）：跨服私聊超链接，用接收方名，
            // 包回发给发送方。Ghidra C 与汇编均无普通版同处的 GM(0x5a) 分支。
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0x15);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            from->Send(pm);
        }
        else
        {
            pm.put_header(0, 0x173);
            pm.put_byte(msg.m_typeD);
            pm.put_byte(0);
            int nameLen = (int)strlen(from->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(from->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg.m_textLen14);
            pm.put_str(msg.m_text18, msg.m_textLen14);
            pm.put_byte(msg.m_linkCount13b);
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                pm.put_binary((char*)msg.m_links13c[i], 0x68);
            }
            pm.finalize(true);
            to->Send(pm);
        }
        break;
    }
    case 2:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            CParty* party = (CParty*)from->GetParty();
            if (party != 0)
            {
                party->send_to_party(packet);
            }
        }
        break;
    case 3:
    case 0x27:
    {
        Village* village = G_GameWorld()->getUserVillage(from);
        if (village == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1934, 5);
            trace("getUserVillage() call failed");
            break;
        }
        bool needClean = false;
        if (village->get_gate_area() != from->get_area(false) &&
            from->getCurCharacVill() != 8 && (int)notify != 0x72)
        {
            needClean = true;
        }
        if (needClean)
        {
            int cleanCount =
                G_CDataManager()->m_serverParameter.GetCleanChattingCount(
                    from->get_charac_level());
            std::string smsg(msg.m_text18);
            int add = from->isDuplicationMessage(smsg) != 0
                          ? G_CDataManager()->m_serverParameter.GetPaneltyDupMessage()
                          : 1;
            from->incChattingMessageCount(add * cleanCount);
            if (from->getChattingMessageCount() >
                G_CDataManager()->m_serverParameter.GetMaxCleanChattingCount())
            {
                from->setChattingMessageCount(0);
                from->reqHumanCertify4ClearMap(false);
                break;
            }
        }
    }
    case 0x22:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            send_to_area(from->getCurCharacVill(), from->get_area(false), packet);
        }
        break;
    case 4:
        make_packet_chat_msg_hyper_link(packet, from, msg, notify);
        send_all(packet);
        break;
    case 5:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            if (from->GetAssaultPlace() == 0)
            {
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp(packet);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, false, &packet);
            }
        }
        break;
    case 6:
        if (from->getCurCharacNo() != 0 && from->get_charac_guildkey() != 0)
        {
            // ORIG 0x86d06ac：Packet_Monitor_Guild_Chat_Hyper_Link（同普通版
            // 0x86ca6ac 怪癖：+0xa 槽(local_53a)先写 guildkey、+0xe 槽(local_536)
            // 后写 characNo，与头文件字段名相反；SendPacket 长度 0x14c+textLen）。
            Packet_Monitor_Guild_Chat_Hyper_Link pkt;
            pkt.m_charNo = from->get_charac_guildkey();
            pkt.m_guildKey = from->getCurCharacNo();
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items + i * 0x68, msg.m_links13c[i], 0x68);
            }
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            GlobalData::s_guild_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, msg.m_textLen14 + 0x14c);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x19b8,
                "GUILD_CHAT_HYPER_LINK_ERROR : charac no == %d || guild key == %d ",
                from->getCurCharacNo(), from->get_charac_guildkey());
        }
        break;
    case 8:
        if (from->getCurCharacNo() != 0 &&
            from->get_charac_memberkey() != 0)
        {
            Packet_Monitor_Member_Chat_Hyper_Link pkt;
            pkt.m_memberId = from->get_charac_memberkey();
            pkt.m_itemCount = msg.m_linkCount13b;
            for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
            {
                memcpy(pkt.m_items[i], msg.m_links13c[i], 0x68);
            }
            pkt.m_msgLen = (unsigned char)msg.m_textLen14;
            memcpy(pkt.m_msg, msg.m_text18, msg.m_textLen14);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, msg.m_textLen14 + 0x148);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp", __PRETTY_FUNCTION__, 0x19d6,
                "MEMBER_CHAT_HYPER_LINK_ERROR : member key == %d ",
                (int)from->get_charac_memberkey());
        }
        break;
    case 9:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            if (from->GetAssaultPlace() == 0)
            {
                PvP_Room* room = G_CGameManager()->GetPvp(
                    (int)from->GetPvpIndex(), from, 0);
                if (room != 0)
                {
                    room->send_to_pvp_team(packet, from);
                }
            }
            else
            {
                pvp_assault::GetInstanceAssaultMgr()->SendPacket(from, true, &packet);
            }
        }
        break;
    case 0xb:
    case 0xc:
    case 0xe:
    case 0x23:
        if (from->getCurrCharacBlackCount() < 0x12d)
        {
            Packet_Monitor_MegaPhone_Hyper_Link pkt;
            pkt.set(G_CEnvironment()->get_channel_no(), from->get_unique_id(),
                    from->getCurCharacName(), (unsigned char)msg.m_textLen14,
                    msg.m_text18, msg.m_typeD, msg.m_linkCount13b,
                    (const hyperlink_item_info*)msg.m_links13c[0]);
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(from->GetServerGroup())
                ->SendPacket((char*)&pkt, msg.m_textLen14 + 0x167);
        }
        break;
    case 0xd:
    case 0xf:
    {
        PacketGuard pm;
        pm.put_header(0, 0x174);
        pm.put_byte((int)msg.m_typeD);
        pm.put_byte(G_CEnvironment()->get_channel_no());
        pm.put_short(from->get_unique_id());
        // ORIG 0x086d0xxx 同 send_chat_msg case 0xd/0xf：`put_int(strlen(...))` 内联
        // + 具名 nameLen ⇒ 两次独立 strlen/getCurCharacName（p167 MISSING 各 1 实证）。
        pm.put_int((int)strlen(from->getCurCharacName()));
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(msg.m_textLen14);
        pm.put_str(msg.m_text18, msg.m_textLen14);
        pm.put_byte(msg.m_linkCount13b);
        for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
        {
            pm.put_binary((char*)msg.m_links13c[i], 0x68);
        }
        pm.finalize(true);
        G_GameWorld()->send_all(pm);
        break;
    }
    case 0x28:
        if (from != 0)
        {
            make_packet_chat_msg_hyper_link(packet, from, msg, notify);
            BlueMarble* bm = (BlueMarble*)from->getBlueMarble();
            if (bm != 0)
            {
                bm->sendToBlueMarble(packet);
            }
        }
        break;
    default:
        break;
    }
}

// ============================================================================
// ProcessAllUsers 模板（ORIG 实例 0863d3bc / 0816270c）
// ============================================================================

// CEventStayTime::RewardGoGoFighter 权威声明已提升至 CEventStayTime.h
// （此前的局部遮蔽视图按 ODR 清理移除）。

void CEventStayTime::RewardGoGoFighter::operator()(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    int stayTime = m_4 - (int)user->GetLoginTime();
    if (m_0 > stayTime)
    {
        return;
    }
    // P197② register 缓存 accId 见下；数据代理经 GlobalData 真符号（不裸地址）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("GameWorld.cpp", 0x1e),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(5), user->GetUID());
    Taiwan::SigStayTimeEvent* evt =
        guard.operator->()->GetInBuffer<Taiwan::SigStayTimeEvent>();
    if (evt != 0)
    {
        // P197②：ORIG +0x0d3 `mov %eax,%ebx` 把 get_acc_id 结果 callee-saved
        // (%ebx) 缓存，跨 cMyTrace ctor 存活供 trace("%d",accId) 用；evt->set 再
        // fresh 调 get_acc_id（不复用）。
        // b28d-lane11：历史 asm("ebx") 绑定按 P0/N96 清除；改 P227 全表达式临时链
        // （P221 同族）——实参在 ctor 前求值并以 %ebx 跨 ctor 保活，且 trace 不占
        // 具名槽，翻正 guard(-0x28)/trace(-0x20) 槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x24, 0)("%d", (unsigned int)user->get_acc_id());
        evt->init();
        evt->set(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }
}

template <typename T>
void GameWorld::ProcessAllUsers(T functor)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (!(user->get_state() > 2))
            continue;
        functor(user);
    }
}

template void GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(
    CEventStayTime::RewardGoGoFighter);
template void GameWorld::ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(
    InterSelectCreateDnfEventInfo::Reward);

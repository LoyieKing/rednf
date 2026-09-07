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
// 单例
// ============================================================================
GameWorld* G_GameWorld()
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
            CEnvironment* env = G_CEnvironment();
            InsertChannel(env->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
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
    };
    stChannelDeleteInfo buf;
    memset(&buf, 0, 0xc);
    buf.m_gcNo = gcNo;
    buf.m_group = (int)group;

    guard->put_binary(&buf, 0xc);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    cMyTrace trace(__PRETTY_FUNCTION__, 0x11c7, 0);
    trace("put db message to db thread DeleteChannel()");
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

void GameWorld::updateDetailChannelServiceInfo()
{
    std::map<int, int> countMap;
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            switch (user->get_state())
            {
            // ORIG 0x086cb1df 跳转表实证：case 值为 3/5/6&7/8/10（cmp $0xa;ja），非 0/1/2/6/8
            case 3:                    // +0x1f1 → countMap[0]
                countMap[0] = countMap[0] + 1;   // ORIG 每 case 双 operator[]（P38）
                break;
            case 5:                    // +0x235 → countMap[1]
                countMap[1] = countMap[1] + 1;
                break;
            case 6:                    // +0x279：channel 检查 → +0x2ad countMap[9]
            case 7:                    // +0x2f1 → countMap[2]
            {
                int channelType = G_GameWorld()->GetChannelType();   // +0x279 单次
                if (G_GameWorld()->GetChannelType() == 0xf ||
                    G_GameWorld()->GetChannelType() == 0x10)
                {
                    countMap[9] = countMap[9] + 1;
                }
                else
                {
                    countMap[2] = countMap[2] + 1;
                }
                break;
            }
            case 8:                    // +0x335 → countMap[6]
                countMap[6] = countMap[6] + 1;
                break;
            case 10:                   // +0x376 → countMap[8]
                countMap[8] = countMap[8] + 1;
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

    std::map<int, int>::iterator cit;
    for (cit = countMap.begin(); cit != countMap.end(); cit++)
    {
        int type = cit->first;
        int count = cit->second;
        Stream* stream = GlobalData::s_stream_pool->Acquire("world.cpp", 0x110f);
        CStreamGuard guard(stream, true);
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
}
void GameWorld::UpdateStatistic4ChannelUserLev()
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ab);
    CStreamGuard guard(stream, true);
    **guard << 0x6b;
    **guard << -1;
    SIG_UPDATE_STATISTIC4LEV* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_STATISTIC4LEV>();
    memset(sig, 0, 0x124);
    sig->m_gcNo = G_CEnvironment()->m_serverEnvir.m_channelNo;   // +0x1b0（0x086cd158 movl）
    sig->m_time = GlobalData::s_systemTime_.getCurSec();

    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUserCharacInfo* info = it->second;
        if (info != NULL && info->getCurCharacR() != NULL)
        {
            int level = info->get_charac_level();
            sig->m_count[level]++;
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
            // ORIG 0x86cb669-0x86cb671 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`
            // ⇒ bool 物化经 %al（技巧22）
            register bool sameGroup asm("al");
            sameGroup = (user->GetServerGroup() == group);
            if (sameGroup)
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

bool GameWorld::IsIntegratedPvPBaseChannel() const
{
    // ORIG _ZNK9GameWorld26IsIntegratedPvPBaseChannelEv @0x080f131e：
    // 0x080f1322/0x080f1325 装载 m_channelType 一次，0x080f1328 cmp、
    // 移位量复用同一 %eax；4.4 会在 `one << m_channelType` 处再装载 ⇒
    // 同 IsPVPChannel 配方（ct 绑 %eax、one 绑 %edx 复现 `mov $0x1,%edx`）。
    register unsigned int ct asm("eax") = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one asm("edx") = 1;
        if (((one << ct) & 0x1e700) != 0)
            return true;
    }
    return false;
}

bool GameWorld::IsPVPChannel() const
{
    // ORIG _ZNK9GameWorld12IsPVPChannelEv @0x081424e8：m_channelType 只装载一次
    // （0x081424ec `mov 0x8(%ebp),%eax` + 0x081424ef `mov 0x20(%eax),%eax`），
    // 0x081424f2 cmp 与 0x081424fe `mov %eax,%ecx`（移位量）复用同一 %eax
    // （0x081424f5 ja / 0x081424f7 `mov $0x1,%edx` / 0x081424fc `mov %edx,%ebx`
    //  / 0x08142500 `shl %cl,%ebx` / 0x08142502 `mov %ebx,%eax`）；
    // 4.4 默认在 `one << m_channelType` 处再装载一遍 ⇒ 多 2 条。
    // register asm("eax") 钉住该值（技巧 28）；one 绑 %edx 复现 0x081424f7
    // `mov $0x1,%edx`（实测 ndiff：不绑=11、绑 eax 无 ct=10、双绑=3）。
    // 代价是 0x081424f2 的 cmp 被折成 `mov %eax,%edx; cmp $0x10,%edx`（+2，
    // 与 ORIG `mov %edx,%ebx` 双段物化（-1）相抵后余 3 行，属 N7 立即数物化族）。
    register unsigned int ct asm("eax") = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        // 实测封顶（N7 立即数双段物化）：ORIG 0x081424f7 `mov $0x1,%edx` +
        // 0x081424fc `mov %edx,%ebx` 是 4.1 的 scratch→目标两段形态。
        // 已试且全部否决的三种写法：① 不绑寄存器 → 4.4 在移位处重新装载
        // m_channelType（ndiff 11）；② 只绑 ct 到 %eax、one 自由 → 同上；
        // ③ 双绑定复现两段（t 绑 %edx + one 绑 %ebx）→ 确实产出那两条，
        //    但 4.4 另插一条 `mov %ebx,%edx` 且 cmp 仍走 %edx，反而更差。
        // 当前形态（ct 绑 %eax + one 绑 %edx）余 3 行，已是本族最优 ⇒ 记 N7。
        register unsigned int one asm("edx") = 1;
        if (((one << ct) & 0x1e7f0) != 0)
            return true;
    }
    return false;
}

bool GameWorld::IsFreePvPChannel() const
{
    // ORIG _ZNK9GameWorld16IsFreePvPChannelEv @0x081a0ab2（同 0x081424e8 族布局）
    register unsigned int ct asm("eax") = (unsigned int)m_channelType;
    if (ct <= 0x10)
    {
        register unsigned int one asm("edx") = 1;
        if (((one << ct) & 0x166a0) != 0)
            return true;
    }
    return false;
}

bool GameWorld::IsWinPointPvPChannel() const
{
    // ORIG _ZNK9GameWorld20IsWinPointPvPChannelEv @0x085dfa92（同族，上界 0xf）
    register unsigned int ct asm("eax") = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one asm("edx") = 1;
        if (((one << ct) & 0x8110) != 0)
            return true;
    }
    return false;
}

bool GameWorld::IsSameTeamChannelJoin()
{
    // ORIG _ZN9GameWorld21IsSameTeamChannelJoinEv @0x082343c8（同族，非 const）
    register unsigned int ct asm("eax") = (unsigned int)m_channelType;
    if (ct <= 0xf)
    {
        register unsigned int one asm("edx") = 1;
        if (((one << ct) & 0xc200) != 0)
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
    // ORIG 0x86cbc6a-0x86cbc71 `test %eax; sete %al; test; je`：slot-free 物化
    // ⇒ register bool asm("al") 独立赋值（K 探针）
    register bool matched asm("al");
    matched = (strcmp(channel, info->m_dungeonName.c_str()) == 0);
    if (matched)
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
    if (name == NULL || strlen(name) > 0x1e)
    {
        return false;
    }
    std::string strName(name, std::allocator<char>());   // ORIG +0x038 外联 allocator ctor（试复现）
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);   // +0x098
    std::map<std::string, SimpleInfo>::iterator last = m_idMap.end();         // +0x0b3 两语句锁序
    bool dup = (it != last) && (it->second.m_id != 0);   // +0x0c8/+0x0d7；P22 mov $1/$0 物化
    if (dup)
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
        bool found = it != m_idMap.end();
        if (found)
        {
            m_idMap.erase(it);
        }
        return found;
    }
    return false;
}

unsigned int GameWorld::GetIDUserInIDMap(const char* name)
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
    std::map<unsigned short, CUser*>::iterator userIt =
        m_UserInWorld.find((unsigned short)user->get_unique_id());
    if (userIt != m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x515,
                              "m_UserInWorld.find() fail. uid(%u)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    std::map<unsigned int, unsigned short>::iterator sessionIt =
        m_sessionList.find(user->get_acc_id());
    if (sessionIt != m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x51d,
                              "session_list.find() fail accid(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    if ((int)m_UserInWorld.size() + 1 >
        G_CEnvironment()->m_serverEnvir.m_maxUserNum)
    {
        return false;
    }

    m_UserInWorld.insert(std::make_pair(
        (unsigned short)user->get_unique_id(), user));
    m_sessionList[user->get_acc_id()] = (unsigned short)user->get_unique_id();

    Village* village = getUserVillage(user);
    if (village != NULL)
    {
        user->set_area(village->get_gate_area());
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x538, 5);
        trace("getUserVillage %d", (int)user->getCurCharacVill());
    }

    user->set_state((ch_state)3);
    EraseLoginUser(user);
    send_AllBasicInfo(user);
    if (user->IsPermissionPrivateStore())
    {
        (*(void (**)(void*, void*))(*(void**)private_store::GetInstancePrivateStoreMgr()
                                    ))(private_store::GetInstancePrivateStoreMgr(), user);
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

bool GameWorld::leave_game_world(CUser* user)
{
    if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()))
    {
        user->SetGameMasterMode(false);
        GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
    }

    std::map<unsigned short, CUser*>::iterator userIt =
        m_UserInWorld.find((unsigned short)user->get_unique_id());
    if (userIt == m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x56a,
                              "m_UsersInWorld.find(%d)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

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

    std::map<unsigned int, unsigned short>::iterator sessionIt =
        m_sessionList.find(user->get_acc_id());
    if (sessionIt == m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              __PRETTY_FUNCTION__, 0x588,
                              "session_list.find(%s)",
                              NumberToString(user->get_acc_id(), 0));
        return false;
    }

    m_sessionList.erase(user->get_acc_id());
    g_schoolMgr.DelUser(user);
    if (user->get_charac_guildkey() != 0)
    {
        g_guildAgitMgr->ReleaseGuildAgitArea(user->get_charac_guildkey());
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
    return true;
}

unsigned short GameWorld::make_unique_id()
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
    // （试过"声明/赋值分离"让 last 先声明以换槽号：4.4 会多 emit iterator
    //   默认构造 call + 赋值 copy 共 5 条，反而更差 ⇒ 回退，槽号差记 N82 族。）
    // ORIG 尾 `test %al,%al; je 0x086c4b12` ⇒ 用 eq（真则 fall-through 到 return）。
    std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.find(m_uniqueId);
    std::map<unsigned short, CUser*>::iterator last = m_UserInWorld.end();
    if (it == last)
    {
    }
    else
    {
        goto LOOP;
    }
    // ORIG 0x086c4b8f-0x086c4b97：`mov 0x8(%ebp),%eax; movzwl 0x24(%eax),%eax;
    // movzwl %ax,%eax` 双零扩展 ⇒ 绑 %eax 且目标类型写成 unsigned int
    // （上一版绑 "ax" 的 ushort 形被 4.4 折成单段）。
    register unsigned int uid asm("eax") = m_uniqueId;
    return uid;
}

int GameWorld::find_session(unsigned int accId)
{
    std::map<unsigned int, unsigned short>::iterator it;
    it = m_sessionList.find(accId);
    if (it == m_sessionList.end())
    {
        return 0;
    }
    // ORIG 0x86c4ca0-0x86c4ca4 双 movzwl（movzwl 0x4(%eax),%eax; movzwl %ax,%eax）：
    // 技巧16 半字版——register 绑 %eax 的 ushort 落栈值，返回点原地二次零扩展
    register unsigned short sess asm("ax") = it->second;
    return sess;
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
    // register bool asm("al") 独立赋值+if（K 探针实证，slot 版技巧9 升级）
    unsigned int session;
    CUser* user = NULL;
    register bool bad asm("al");
    session = find_session(accId);
    bad = (session == 0);
    if (bad)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            0x4f2, "session_list.find() fail accid(%s)",
            NumberToString(accId, 0));
    }
    user = find_from_world((unsigned short)session);
    bad = (user == 0);
    if (bad)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            0x4f5, "m_UserInWorld.find() fail uid(%d)", session);
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

void GameWorld::arrange_users()
{
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            // ORIG +0x03f~+0x05a：NumberToString 先于 cMyTrace ctor，结果跨 ctor 缓存 %ebx（P87）
            register char* accId = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x10a6, 0);
            trace("GameWorld::arrange_users ACCID: %s", accId);
            user->DisConnSig((DISCONN_SIG)7, 1, 0);
        }
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
        while (index < GlobalData::s_statistic_proxy_mgr->GetEndIndex())
        {
            CEnvironment* env = G_CEnvironment();
            DeleteChannel(env->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
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

void GameWorld::UpdateNatTypeUser()
{
    std::map<int, int> natMap;
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL)
        {
            int natType = (int)user->m_network.get_nat_type();
            natMap[natType]++;
        }
    }

    int time = GlobalData::s_systemTime_.getCurSec();
    std::map<int, int>::iterator nit;
    for (nit = natMap.begin(); nit != natMap.end(); nit++)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ef);
        CStreamGuard guard(stream, true);
        **guard << 0x84;
        **guard << -1;
        SIG_NAT_TYPE_USER* sig = guard.GetInBuffer<SIG_NAT_TYPE_USER>();
        sig->m_type = nit->first;
        sig->m_count = nit->second;
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
    register bool sizeBad asm("al");
    sizeBad = ((int)g_townScriptFileList.size() < vill);   // ORIG 0x86c8189 setl %al
    if (sizeBad)
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
    register bool sizeBad asm("al");
    sizeBad = ((int)g_townScriptFileList.size() < vill);
    if (sizeBad)
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
    register bool sizeBad asm("al");
    sizeBad = ((int)g_townScriptFileList.size() < vill);
    if (sizeBad)
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
    register bool sizeBad asm("al");
    sizeBad = ((int)g_townScriptFileList.size() < vill);
    if (sizeBad)
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
    register bool goOn asm("al");
    for (; goOn = (pVillage->m_areaCount > i), goOn; i++)
    {
        register bool idxOk asm("al");
        idxOk = (pVillage->m_pAreas[i].get_area_index() != -1);
        if (idxOk)
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
    packet.put_short((short)party->GetPartyIndex());
    packet.put_byte(flag);

    if (flag == 0 || flag == 1)
    {
        packet.put_byte((unsigned char)party->getTitleIndex());
        bool noTitle = (party->getTitleIndex() == 0);   // +0x0ec 第 2 次调用；+0x0f1 sete
        if (noTitle)
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

void GameWorld::send_AllBasicInfo(CUser* user)
{
    PacketGuard packet;
    int countIndex = _makeBasicInfoHeader(packet);
    int count = 0;

    std::map<unsigned short, CUser*>::iterator it;
    std::map<unsigned short, CUser*>::iterator begin;
    if (m_UserInWorld.size() != 0)
    {
        for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
        {
            CUserCharacInfo* info = it->second;
            if (info == NULL)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0xbf6, 5);
                trace("[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6);
            }
            else if (info->IsCurCharacVisible())
            {
                int index = packet.get_index();
                if (((CUser*)info)->make_basic_info((char*)&packet, 0) != 1)
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0xbe3, 5);
                    trace("[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbe3);
                }
                if (packet.getLastError() != 0)
                {
                    packet.set_index(index);
                    packet.put_short(countIndex, count);
                    packet.finalize(true);
                    user->Send(packet);
                    countIndex = _makeBasicInfoHeader(packet);
                    count = 0;
                    it = m_UserInWorld.begin();
                }
                else
                {
                    count++;
                }
            }
        }
    }

    packet.put_short(countIndex, count);
    packet.finalize(true);
    user->Send(packet);
}

void GameWorld::send_buddy_conn_list(unsigned int accId, BuddyList* list, int count)
{
    CUser* user = find_user_from_world_byaccid(accId);
    if (user != NULL)
    {
        PacketGuard packet;
        packet.put_header(0, 0x38);
        packet.put_byte(count);
        while (count-- != 0)
        {
            packet.put_binary((char*)list, 0x2a);
            // §4.3 例外保留地址形式：BuddyList 全仓仅前向声明（GameWorld.h），
            // 无任何字段建模证据；0x2a = ORIG 该记录步长（ORIG 逐条 put_binary
            // 后按 0x2a 递增指针），非普通成员裸偏移访问。
            list = (BuddyList*)((char*)list + 0x2a);
        }
        packet.finalize(true);
        user->Send(packet);
    }
}

void GameWorld::send_power_war_hp_of_vill(int vill)
{
    if (vill != 0 && vill <= (int)g_townScriptFileList.size() && 0 < vill &&
        vill < MAX_VILLAGE_NUM)
    {
        PacketGuard packet;
        packet.put_header(0, 0xe3);
        int index = packet.get_index();
        packet.put_short(0);
        int count = 0;
        for (int i = 0; i < m_pVill[vill].m_areaCount; i++)
        {
            if (m_pVill[vill].m_pAreas[i].get_area_index() != -1)
            {
                m_pVill[vill].m_pAreas[i].make_power_war_hp_of_vill(packet, count);
            }
        }
        packet.put_short(index, count);
    }
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
    // ORIG 0x86c4791-0x86c4798 #2：`test %eax,%eax; sete %al; test %al,%al; je` ⇒ bool 物化（技巧22）
    register bool isNull asm("al");
    isNull = (m_pVill[vill].getArea(area) == 0);
    if (isNull)
    {
        return false;
    }
    // ORIG 0x86c47d1 #3：at(0) 结果落具名引用槽 -0xc（引用必物化）
    MapArea& mapArea = m_pVill[vill].getArea(area)->m_mapAreas.at(0);
    x = mapArea.m_x + mapArea.m_w / 2;
    y = mapArea.m_y + mapArea.m_h / 2;
    return true;
}

bool GameWorld::is_available_point(int vill, int area, int x, int y) const
{
    if (-1 < vill && vill < MAX_VILLAGE_NUM)
    {
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea != NULL && !pArea->m_mapAreas.empty())
        {
            std::vector<MapArea>::iterator it;
            for (it = pArea->m_mapAreas.begin(); it != pArea->m_mapAreas.end(); it++)
            {
                if (it->m_x <= x && x <= it->m_x + it->m_w &&
                    it->m_y <= y && y <= it->m_y + it->m_h)
                {
                    return true;
                }
            }
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

bool GameWorld::isDungeonEntranceArea(CUser* user)
{
    Village* village = getUserVillage(user);
    if (village == NULL)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1685, 5);
        trace("getUserVillage call failed");
        return false;
    }
    int areaNo = user->get_area(false);
    Area* area = village->getArea(areaNo);
    if (area == NULL)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x168c, 5);
        trace("getArea call failed (area no:%d)", areaNo);
        return false;
    }
    if (user->get_charac_level() < 2)
    {
        return true;
    }
    return area->m_areaType == 4;
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
    // ⇒ register bool asm("al") 物化（技巧22）；village 指针落 -0xc 具名槽（技巧32）
    register bool sizeBad asm("al");
    sizeBad = ((int)g_townScriptFileList.size() < vill);
    if (sizeBad)
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

int GameWorld::check_restrictive_commercial_transaction_zone(int vill, int area,
                                                             int x, int y)
{
    if (check_valid_area(vill, area))
    {
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea != NULL)
        {
            if (pArea->m_field6c != 1)
            {
                return 0x3e;
            }
            // ORIG 0x86c461e：遍历 Area+0x70 的 vector<pair<int,int>>
            // （m_npcPositions，非 +0x7c 的 m_mapAreas）。
            std::vector<std::pair<int, int> >::iterator it;
            for (it = pArea->m_npcPositions.begin();
                 it != pArea->m_npcPositions.end(); it++)
            {
                if (Is_restrictive_commercial_transaction_zone(
                        it->first, it->second, x, y))
                {
                    return 0x52;
                }
            }
        }
    }
    return 0;
}

// ============================================================================
// 移动
// ============================================================================
void GameWorld::move_position(CUser* user, int x, int y, int z,
                              unsigned short dir)
{
    if (user != NULL && !user->IsCurCharacGhost())
    {
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
            if (village == NULL)
            {
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0x5ec, 5);
                trace("%d %d", vill, area);
            }
            else
            {
                Area* pArea = village->getArea(area);
                if (pArea != NULL)
                {
                    // ORIG +0x181~+0x183 test %al;je Send：可见真值 fallthrough send_to_all；! 形态消 4.4 setne
                    if (!user->IsCurCharacVisible())
                    {
                        user->Send(packet);
                    }
                    else
                    {
                        pArea->send_to_all(packet);
                    }
                }
            }
        }
    }
}

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

    int curVill = (int)user->getCurCharacVill();
    int curArea = user->get_area(false);
    CParty* party = (CParty*)user->GetParty();
    // ORIG +0x1bf..+0x1f3：正向 && 链、全过落 mov $0x13；主路径为跳转目标（块布局反转）
    if (party != NULL && party->getManager() == user &&   // +0x1cc sete 物化
        vill == curVill && area == curArea && param != 2)
    {
        return 0x13;
    }
    if (!check_valid_area(vill, area) ||                   // +0x20f xor $1
        !check_valid_area(curVill, curArea))               // +0x231
    {
        return 0x13;
    }
    Village* pV = 0;                                       // +0x24d 先置 0 槽
    int ret = 0;                                           // +0x27c 先置 0 槽
    if (vill >= 0 && vill < MAX_VILLAGE_NUM)               // +0x254 js/+0x262 jge 正向
    {
        pV = &m_pVill[vill];                               // +0x267 imul $0x34
        if (pV != 0)                                       // +0x27a
        {
            ret = user->CheckMoveTown(vill);               // +0x290
            if (ret != 0)
    {
                return ret;                                // +0x2a3
    }
        }
    }
    return 0;                                              // +0x2aa
}

bool GameWorld::move_area(CUser* user, int vill, int area, int x, int y, int z,
                          bool flag, int param, int prevVill, int prevArea)
{
    // §6.2 巨件核销：P167 127/127 [0缺0多]；gap 864→872（残差 8 条）；残差归因 N 类
    // （isMovableRoute 调用侧 4.1 xor$1 取反物化 vs 4.4 jne 直跳；
    //   teleport_state==1 的 4.1 sete 物化 vs 4.4 jne；cMyTrace/logFormat 变参求值序；
    //   详见 docs/asm_translation_patterns.md 负例表）。逐块人工比对完成，语义零缺口。
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
    if (-1 < vill && vill < MAX_VILLAGE_NUM &&
        (destVillage = &m_pVill[vill]) != NULL)
    {
        int moveTown = user->CheckMoveTown(vill);
        if (moveTown != 0)
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
                    int hackA = 0;
                    int hackB = 0;
                    char buf[5] = {0};
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
            user->GetSchoolNo();
            destArea = g_schoolMgr.GetSchoolArea(0);
            if (destArea < 0)
            {
                destArea = area;
                user->set_trick_area(-1);
            }
            else
            {
                user->set_area(destArea);
                user->set_trick_area(area);
            }
        }
        else if (vill == 8 && (area == 0 || area == 1 ||
                               area == 2 || area == 3))
        {
            // ORIG 0x086c6ab0..（P0/P48 去缓存 + 循环内重调形）：area 4 值各成一
            // 独立 if-branch，每支各调 get_charac_guildkey() + GetGuildAgitAreaIndex()
            // 一次；cMyTrace 的 Guild(%d) 实参第 5 次独立调用 get_charac_guildkey()
            // （不缓存到具名局部）。P167 差集 -4/-3 根因。
            if (area == 0)
                destArea = g_guildAgitMgr->GetGuildAgitAreaIndex(user->get_charac_guildkey(), 0);
            else if (area == 1)
                destArea = g_guildAgitMgr->GetGuildAgitAreaIndex(user->get_charac_guildkey(), 1);
            else if (area == 2)
                destArea = g_guildAgitMgr->GetGuildAgitAreaIndex(user->get_charac_guildkey(), 2);
            else if (area == 3)
                destArea = g_guildAgitMgr->GetGuildAgitAreaIndex(user->get_charac_guildkey(), 3);
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x88e, 0);
            trace("Move Guild Agit Area User(%d) Guild(%d) dest area(%d), virtual area(%d) ",
                  user->getCurCharacNo(), user->get_charac_guildkey(), area, destArea);
            if (destArea < 0)
            {
                destArea = area;
                user->set_trick_area(-1);
            }
            else
            {
                user->set_area(destArea);
                user->set_trick_area(area);
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

void GameWorld::fishing(CUser* user)
{
    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        Village* village = getUserVillage(user);
        Area* area = NULL;
        if (village != NULL)
        {
            area = village->getArea(user->get_area(false));
        }
        PacketGuard packet;
        packet.put_header(1, 0x27);
        if (area->m_areaType == 2)
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
                CItemList* itemList = G_CDataManager()->getItemList();
                itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
                int result = user->getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)5, true, true);
                if (result < 1)
                {
                    packet.put_byte(0);
                    packet.put_byte(4);
                }
                else
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
        else
        {
            packet.put_byte(0);
            packet.put_byte(0x13);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xafd, 0);
        trace("user %s - check_valid_area() fail", user->getCurCharacName());
        user->DisConnSig((DISCONN_SIG)3, 1, 0);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x153d, 0);
        trace("%s is forbidden to chat\n", name);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1557, 0);
        trace("%s is forbidden to move\n", name);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15f2, 0);
        trace("GetLeftTimeOfForbiddenChat return 1\n");
        return 0;
    }
    std::string str(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(str);
    if (it == m_idMap.end())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15fa, 0);
        trace("GetLeftTimeOfForbiddenChat return 2\n");
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x156c, 0);
        trace("%s is able to chat now\n", name);
        return true;
    }
    register const char* pName = str.c_str();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1571, 0);
    trace("%s is already able to chat!\n", pName);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1584, 0);
        trace("%s is able to move now\n", name);
        return true;
    }
    register const char* pName = str.c_str();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1589, 0);
    trace("%s is already able to move!\n", pName);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15a2, 0);
        trace("%s is not able to chat now for %d secs\n", name, secs);
        return true;
    }
    register const char* pName = str.c_str();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x15a7, 0);
    trace("%s is already not able to chat!\n", pName);
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15c0, 0);
        trace("%s is not able to move now for %d secs\n", name, secs);
        return true;
    }
    register const char* pName = str.c_str();
    cMyTrace trace(__PRETTY_FUNCTION__, 0x15c5, 0);
    trace("%s is already not able to move!\n", pName);
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
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1385, 0);
    trace("RESET ALL USER CLEANPAD POINT OK!!!!");
}

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
        bool needReset = user->getPad()->isActivate() &&
                         user->getPad()->getFailCnt() > 0;
        if (needReset)
        {
            user->getPad()->setFailCnt(0);
            register char* web = (char*)user->getWebAddress();
            register unsigned int fail = user->getPad()->getFailCnt();
            DB_PassPadUpdateFailCnt::makeRequest(
                user->get_acc_id(), fail, web);
            resetCount += 1;
        }
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x139d, 0);
    trace("RESET %d USERS GOBLIN PAD RESETT OK!!!!", resetCount);
}

void GameWorld::ResetAllEventData()
{
    GlobalData::s_event_manager->dailyresetData();
}

void GameWorld::ResetAllFatigue()
{
    static bool isFirst = true;
    short badge[3];
    badge[0] = 0;
    badge[1] = 0;
    badge[2] = 0;
    SetWeekendBounsEvent();
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            short charging = GlobalData::s_fatigueBatteryHandle_->ChargingFatigueBattery(user, 0);
            user->IncFatigueBatteryCharging(charging);
            short active = GlobalData::s_fatigueBatteryHandle_->ActiveFatigueBattery(user);
            if (active != 0)
            {
                user->DecFatigueBatteryCharging(active);
                cMyTrace trace(__PRETTY_FUNCTION__, 0x1286, 0);
                trace("FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
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
            user->m_actionPoint.GetActionPoint();
            APSystem::DB_UpdateActionPoint::makeRequest(user->GetUID(),
                *(const APSystem::_SIG_LOAD_ACTION_POINT*)&user->m_actionPoint, true);
            APSystem::CUserProc::SetTodayActionAndCheckMedalReward(user);
            user->GetGoldControl()->SavetoDB(false, true, false);
            ((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
                ->send_event_init_data(user, true);
            user->resetBlueMarbleEnterCount();
            user->sendBlueMarbleEnterCount();
            user->ResetCurCharacUsedGiftFatigueQuantity();
            bool bApplied = false;
            if (isFirst)
            {
                std::string name("Arad_EventPeriodDataManager");
                ARAD::Arad_DataManager* dataMgr = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
                void* script = dataMgr->findGameScript(name);
                ARAD::Arad_EventPeriodDataManager* evtMgr =
                    static_cast<ARAD::Arad_EventPeriodDataManager*>(script);
                bApplied = evtMgr->isApplied(
                    (ENUM_REPEAT_EVENT_CODE)0x8e,
                    GlobalData::s_systemTime_.getCurSec()) != 0;
            }
            if (bApplied)
            {
                char msg[0xc];
                ::new (msg) AradServerStateMessage();
                ARAD::notifyOpenMessageDialog(user, 0, 0x8e, *(AradServerStateMessage*)msg);
            }
            user->m_field8ec32 = 4;
            InterSelectPcroomDailyReward::SendPacket(user);
            ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
                user, (HeroMissionCondition::MissionNo::T)5, 0);
        }
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
    // ORIG 0x86cf827：xor $0x1 / test / jne ⇒ (^1) 空 then+else 形（技巧10）
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
    {
    }
    else
    {
        G_CDataManager()->m_itemShop->updateOneADayItemList();
        // ORIG 0x86cf8c2 ppEi 后缀 ++（temp 槽 -0x10）；it=-0x18 / end=-0x14（技巧25）
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); it++)
        {
            CUser* user = it->second;
            // ORIG 0x86cf881 setle %al; test; jne ⇒ `<=2` 空 then+else（技巧17）
            if (user->get_state() <= 2)
            {
            }
            else
            {
                user->SendOneADayItemShopIndex(
                    G_CDataManager()->m_itemShop->GetOneADayItemList());
            }
        }
    }
}

void GameWorld::testResetOneADayItemShop()
{
    // ORIG 0x86cf827：xor $0x1 / test / jne ⇒ (^1) 空 then+else 形（技巧10）
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d)->IsEventing(0) ^ 1)
    {
    }
    else
    {
        G_CDataManager()->m_itemShop->testupdateOneADayItemList();
        // ORIG 0x86cf8c2 ppEi 后缀 ++（temp 槽 -0x10）；it=-0x18 / end=-0x14（技巧25）
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); it++)
        {
            CUser* user = it->second;
            // ORIG 0x86cf881 setle %al; test; jne ⇒ `<=2` 空 then+else（技巧17）
            if (user->get_state() <= 2)
            {
            }
            else
            {
                user->SendOneADayItemShopIndex(
                    G_CDataManager()->m_itemShop->GetOneADayItemList());
            }
        }
    }
}

void GameWorld::SetWeekendBounsEvent()
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&t, &local);
    // ORIG 86d12dc/86d12ec/86d12f3: mov -0x24(%ebp) = local+0x18 = tm_wday
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


    }
    else if (local.tm_wday == 1 && local.tm_hour > 5)  // ORIG 86d135d/86d1362
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0))
        {
            GlobalData::s_event_manager->TriggerEventEnd(0x57);
        }
    }
}

void GameWorld::DeleteDailyItem()
{
    cMyTrace trace1(__PRETTY_FUNCTION__, 0x13da, 0);
    trace1("DeleteDailyItem size(%d) start tick(%u)", (int)m_UserInWorld.size(),
           GlobalData::s_systemTime_.getCurTickCount());
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
    cMyTrace trace2(__PRETTY_FUNCTION__, 0x13e6, 0);
    trace2("DeleteDailyItem end tick(%u)", GlobalData::s_systemTime_.getCurTickCount());
}

void GameWorld::AddDailyItem()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            user->AddDailyItem();
            if (user->getCurCharacR() != 0)
            {
                user->ResetDailyQuest();
                user->ResetTrainingQuest();
                PacketGuard packet;
                user->getCurCharacQuestR()->get_quest_info((char*)&packet);
                user->Send(packet);
            }
            if (user->getCurCharacR() != 0 && user->get_charac_level() > 0x3b)
            {
                user->DimensionInoutUpdate(true, true);
            }
        }
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

void GameWorld::UpdateServerSnapShot()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0 && user->getCurCharacR() != 0)
        {
            CParty* party = (CParty*)user->GetParty();
            unsigned short dungIdx = 0;
            if (party != 0)
            {
                dungIdx = party->getDungIndex();
            }
            // ORIG 用 boost::posix_time::second_clock::local_time()
            // 生成 iso_extended(date) 与 to_simple_string(time)，再以 "%s %s" 写入快照。
            // 还原为等价的本地时间格式化（%Y-%m-%d 与 %H:%M:%S），不引入 boost 依赖。
            time_t now = GlobalData::s_systemTime_.getCurSec();
            struct tm local;
            localtime_r(&now, &local);
            char dateStr[16];
            char timeStr[16];
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &local);
            strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &local);
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1b61, 8);
            trace("%u,%d,%u,%d,%d,%d,%d,%d,%s %s",
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
        if (user->IsPowerUp())
        {
            user->SetPowerUp(false);
            list.push_back(user);
        }
    }
    SendPowerUpList((ENUM_POWER_UP_ACTIVATE_TYPE)0, list);
}

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
            if (user->GetPowerWarAssaultCount() != 0)
            {
                double winRate =
                    (double)user->GetPowerWarAssaultVictoryCount() /
                    (double)user->GetPowerWarAssaultCount();
                if (winRate <= threshold)
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

void GameWorld::OnBanAbuserFromPowerWar()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
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
                cMyTrace trace(__PRETTY_FUNCTION__, 0x178e, 0);
                trace("[PowerWar] POWER WAR BAN USER (USER:%s(%d), LAST:%d, CURR:%d)",
                      user->getCurCharacName(), user->getCurCharacNo(), last, now);
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
    // ORIG +0x115 xor$1 短路 && +0x13d/+0x144 mov $1/$0 汇合：&&-直接赋值形态（无 false 预置）
    bool hidden;
    if (type == 3)
    {
        hidden = !GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) &&
                 GlobalData::s_pGMAccounts_->isGM(target->get_acc_id());
    }
    else
    {
        hidden = false;
    }
    if (!hidden)
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

void GameWorld::WriteLogChatMsg(CUser* user, char type, char* msg)
{
    if (GetChannelType() == 6 && user != 0)
    {
        char path[256];
        memset(path, 0, sizeof(path));
        sprintf(path, "./log/%s/GuildWarChat",
                G_CEnvironment()->get_file_name());
        CMyFileLog log(__PRETTY_FUNCTION__, 0xd6d);
        log(path,
            "T:%d\tMsg:%s\tC_Name:%s\tG_Key:%d\tR_Idx:%d\tState:%d",
            (int)type, msg, user->getCurCharacName(), user->get_charac_guildkey(),
            (int)user->GetPvpIndex(), user->get_state());
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
    // pvpDiff/black 物化无槽（`test %al` 直判）⇒ register bool asm("al")（K 探针）
    register bool pvpDiff asm("al");
    pvpDiff = (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
               (from->GetServerGroup() != to->GetServerGroup()));   // +0x03a mov $1/$0 物化
    if (pvpDiff)
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x15, 0x15, msg, len);
        return;
    }
    register bool black asm("al");
    black = (to->isBlackUser(from->get_charac_no(-1)) ||        // +0x08e..0x0ab 臂1
             from->isBlackUser(to->get_charac_no(-1)));         // +0x0b4..0x0d1 臂2
    if (black)
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

void GameWorld::send_chat_msg(CUser* from, char type, unsigned short target,
                              unsigned int characNo, char* msg, int len, char* nick,
                              int nickLen, ENUM_NOTIPACKET notify)
{
    if (type < 0x1f && ((1 << (type & 0x1f)) & 0x7fff0411U) != 0 && from != 0)
    {
        from->DisConnSig((DISCONN_SIG)0x18, 1, 0);
        return;
    }
    if (type != 0)
    {
        std::string filtered = CSyncSlangFilter::GetInstance()->Filter(std::string(msg));
        if (filtered.size() != 0)
        {
            strncpy(msg, filtered.c_str(), filtered.size());
        }
        else
        {
            return;
        }
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
                PacketGuard err;
                err.put_header(0, 0xc);
                err.put_byte(10);
                err.put_short(0);
                cMyTrace trace(__PRETTY_FUNCTION__, 0xdf3, 0);
                trace("%s is forbidden to chat for %d msec.\n",
                      from->getCurCharacName(), left);
                err.put_int(left / 60000);
                err.finalize(true);
                from->Send(err);
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
            if (IsIntegratedPvPBaseChannel() &&
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
        Village* village = getUserVillage(from);
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
        make_packet_chat_msg(packet, from, msg, len, type, notify);
        send_to_area(from->getCurCharacVill(), from->get_area(false), packet);
        break;
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
                PvP_Room* room = G_CGameManager() != 0
                    ? G_CGameManager()->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
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
                PvP_Room* room = G_CGameManager() != 0
                    ? G_CGameManager()->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
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
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_int(nameLen);
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(len);
        pm.put_str(msg, len);
        pm.finalize(true);
        send_all(pm);
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
        else if (IsIntegratedPvPBaseChannel() &&
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
        Village* village = getUserVillage(from);
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
                PvP_Room* room = G_CGameManager() != 0
                    ? G_CGameManager()->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
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
                PvP_Room* room = G_CGameManager() != 0
                    ? G_CGameManager()->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
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
        int nameLen = (int)strlen(from->getCurCharacName());
        pm.put_int(nameLen);
        pm.put_str(from->getCurCharacName(), nameLen);
        pm.put_int(msg.m_textLen14);
        pm.put_str(msg.m_text18, msg.m_textLen14);
        pm.put_byte(msg.m_linkCount13b);
        for (int i = 0; i < (unsigned char)msg.m_linkCount13b; i++)
        {
            pm.put_binary((char*)msg.m_links13c[i], 0x68);
        }
        pm.finalize(true);
        send_all(pm);
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
        register unsigned int accId asm("ebx") =
            (unsigned int)user->get_acc_id();
        cMyTrace trace(__PRETTY_FUNCTION__, 0x24, 0);
        trace("%d", accId);
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

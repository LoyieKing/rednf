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
#include "CItemList.h"
#include "Inven_Item.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
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
#include "CHackLog_InvalidAreaMove.h"
#include "Arad_EventPeriodDataManager.h"
#include "Packet_MiniCraneSeed.h"

// ============================================================================
// CStreamGuard::GetInBuffer 模板实参（全局类型：C++98 不允许局部类型作为模板
// 实参；全局同名类型使 guard.GetInBuffer<SIG_*>() 自然产生 ORIG 模板实例符号
// _ZN12CStreamGuard11GetInBufferI24SIG_UPDATE_STATISTIC4LEVEEPT_v 与
// _ZN12CStreamGuard11GetInBufferI17SIG_NAT_TYPE_USEREEPT_v）
// ============================================================================
struct SIG_UPDATE_STATISTIC4LEV
{
    int m_gcNo;
    int m_count[70];
    int m_time;
};
struct SIG_NAT_TYPE_USER
{
    int m_type;
    int m_count;
    int m_time;
};

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// CInventory::insertItemIntoInventory（CInventory.h 的 PacketGuard 与本 TU 本地
// 类冲突，经 ORIG asm 名直调，符号 _ZN10CInventory23insertItemIntoInventoryE
// 10Inven_Item14eItemAddReasonbb；CInventory TU 翻译后移除）
extern "C" int sub_CInventory_insertItemIntoInventory(
    void* self, Inven_Item item, int reason, bool bLog, bool bSlot)
    asm("_ZN10CInventory23insertItemIntoInventoryE10Inven_Item14eItemAddReasonbb");



// ---- CUser 缺失方法（asm 名直调；CUser TU 翻译后移除）----
// CUser::deleteSpecificItems 现由真实 CUser.cpp（19 长度符号）提供，旧 18 错拼桥接已删除。
extern "C" int sub_CUser_GetServerGroup(void* user) asm("_ZNK5CUser14GetServerGroupEv");
extern "C" char sub_CUser_getPowerSide(void* user) asm("_ZN5CUser12getPowerSideEv");
extern "C" char sub_CUser_GetEventCreateDnfReward(void* user)
    asm("_ZN5CUser23GetEventCreateDnfRewardEv");
extern "C" char sub_CGM_Manager_IsGm(void* self, unsigned int accId)
    asm("_ZN11CGM_Manager4IsGmEj");

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
    GlobalInstance<GameWorld>::create();
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
    sync_script::CSyncScript* syncScript =
        (sync_script::CSyncScript*)sub_GetInstanceSyncScript();
    if (sub_CSyncScript_truncate_town_info_table(syncScript) != 1)
    {
        puts("Fail truncate_town_info_tablef");
        return false;
    }

    m_uniqueId = 0;
    sub_initTownScript((char*)"Town/", (char*)"Town.lst");

    TownScript tempTown;
    const char* fileName = NULL;

    int townCount = (int)g_townScriptFileList.size();
    MAX_VILLAGE_NUM = townCount + 1;
    m_iVillCount = townCount;
    m_pVill = new Village[MAX_VILLAGE_NUM];

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = sub_CServerProxyMgr_GetStartIndex(
            GlobalData::s_statistic_proxy_mgr);
        while (index < sub_CServerProxyMgr_GetEndIndex(
                           GlobalData::s_statistic_proxy_mgr))
        {
            CEnvironment* env = G_CEnvironment();
            InsertChannel(env->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = sub_CServerProxyMgr_GetNextIndex(
                GlobalData::s_statistic_proxy_mgr, &index);
        }
    }
    else
    {
        CEnvironment* env = G_CEnvironment();
        InsertChannel(env->get_gc_no(), E_SERVER_GROUP_NONE);
    }

    std::map<int, std::string>::iterator it;
    for (it = g_townScriptFileList.begin(); it != g_townScriptFileList.end(); it++)
    {
        fileName = it->second.c_str();
        if (fileName == NULL)
        {
            break;
        }
        if (!sub_importTownScript(&tempTown, fileName))
        {
            LogManager::logFormat(1, "world.cpp", "bool GameWorld::init()", 0x3d4,
                                  "importTownScript() fail - filename(%s)", fileName);
            return false;
        }
        if (!sub_CSyncScript_insert_town_info_to_db(
                sub_GetInstanceSyncScript(), &tempTown))
        {
            printf("insert_town_category Error: %s\n", fileName);
            return false;
        }
        if (m_iVillCount < tempTown.m_0)
        {
            LogManager::logFormat(1, "world.cpp", "bool GameWorld::init()", 0x3e3,
                                  "tempTown.townIndex_(%d) > m_iVillCount(%d) - file(%s)",
                                  tempTown.m_0, m_iVillCount, fileName);
            return false;
        }
        if (!m_pVill[tempTown.m_0].set_village(tempTown))
        {
            LogManager::logFormat(1, "world.cpp", "bool GameWorld::init()", 0x3e9,
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

    struct stChannelInsertInfo
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
        int m_field54;              // +0x54
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
    void* channelInfo = sub_channel_script_t_getChannelInfo(
        channelScript,
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup,
        G_CEnvironment()->get_channel_no());
    if (channelInfo != NULL)
    {
        strncpy(buf.m_channelInfo, (const char*)channelInfo + 8, 0x19);
        buf.m_field54 = *(int*)((char*)channelInfo + 4);
    }

    guard->put_binary(&buf, 0x5c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void GameWorld::DeleteChannel(int gcNo, ENUM_SERVER_GROUP group)
{
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

    cMyTrace trace("void GameWorld::DeleteChannel(int, ENUM_SERVER_GROUP)", 0x11c7, 0);
    trace("put db message to db thread DeleteChannel()");
}

void GameWorld::UpdateChannelUser()
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("world.cpp", 0x1146);
    CStreamGuard guard(stream, true);
    **guard << 0x2e;
    **guard << -1;

    char buf[0x3a];
    memset(buf, 0, 0x3a);
    *(int*)(buf + 0x00) = *(int*)((char*)G_CEnvironment() + 0x198);
    *(unsigned short*)(buf + 0x04) =
        (unsigned short)m_UserInWorld.size();
    for (int i = 0; i < 0xb; i++)
    {
        *(int*)(buf + 0x06 + i * 4) = m_channelUserCount[i];
    }
    *(int*)(buf + 0x34) = m_hangameUserCount;
    *(int*)(buf + 0x36) = m_userCount;

    guard->put_binary(buf, 0x3a);
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
            int count = 0;
            switch (user->get_state())
            {
            case 3:
                countMap[0]++;
                break;
            case 5:
                countMap[1]++;
                break;
            case 6:
            case 7:
                if (G_GameWorld()->GetChannelType() == 0xf ||
                    G_GameWorld()->GetChannelType() == 0x10)
                {
                    countMap[9]++;
                }
                else
                {
                    countMap[2]++;
                }
                break;
            case 8:
                countMap[6]++;
                break;
            case 10:
                countMap[8]++;
                break;
            }
        }
    }

    int privateStoreSize =
        (int)sub_private_store_GetPrivateStoreSize(sub_private_store_GetInstancePrivateStoreMgr());
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
        char buf[0x10];
        *(int*)(buf + 0x00) = type;
        *(int*)(buf + 0x04) = 0;
        *(int*)(buf + 0x08) = *(int*)((char*)G_CEnvironment() + 0x198);
        *(int*)(buf + 0x0c) = count;
        guard->put_binary(buf, 0x10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
}
void GameWorld::UpdateStatistic4ChannelUserLev()
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("world.cpp", 0x14ab);
    CStreamGuard guard(stream, true);
    **guard << 0x6b;
    **guard << -1;
    SIG_UPDATE_STATISTIC4LEV* sig = guard.GetInBuffer<SIG_UPDATE_STATISTIC4LEV>();
    memset(sig, 0, 0x124);
    sig->m_gcNo = *(int*)((char*)G_CEnvironment() + 0x1b0);
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
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user != NULL && (int)user->GetServerGroup() == (int)group)
        {
            count++;
            unsigned int userAge = user->GetAge();
            if (userAge < 100)
            {
                if (userAge == 0)
                {
                    age.m_age[0]++;
                }
                else
                {
                    age.m_age[userAge - 1]++;
                }
            }
            else
            {
                age.m_age[99]++;
            }
        }
    }
    return count;
}

int GameWorld::_GetChannelType()
{
    CEnvironment* env = G_CEnvironment();
    const Server_Envir& server = env->m_serverEnvir;
    if (server.m_channelType <= 0x12)
    {
        return server.m_channelType;
    }
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    void* info = sub_channel_script_t_getChannelInfo(
        channelScript, (unsigned char)server.m_channelType,
        (unsigned int)server.m_serverGroup);
    if (info == NULL)
    {
        return 0x13;
    }
    return *(int*)((char*)info + 4);
}

int GameWorld::GetChannelType() const
{
    return m_channelType;
}

bool GameWorld::IsIntegratedPvPBaseChannel() const
{
    return m_channelType <= 0x10 && (1 << m_channelType) & 0x1e700;
}

bool GameWorld::IsPVPChannel() const
{
    return m_channelType <= 0x10 && (1 << m_channelType) & 0x1e7f0;
}

bool GameWorld::IsFreePvPChannel() const
{
    return m_channelType <= 0x10 && (1 << m_channelType) & 0x166a0;
}

bool GameWorld::IsSchoolPvPChannel() const
{
    if ((unsigned int)(m_channelType - 9) <= 1) return true;
    return false;
}

bool GameWorld::IsWinPointPvPChannel() const
{
    return m_channelType <= 0xf && (1 << m_channelType) & 0x8110;
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

bool GameWorld::is_dungeon_tag_matching_channel(char* channel)
{
    channel_script_t* channelScript =
        (channel_script_t*)G_CDataManager()->GetChannelScript();
    void* info = sub_channel_script_t_getChannelInfo(
        channelScript, (unsigned char)*(int*)((char*)G_CEnvironment() + 0x378),
        G_CEnvironment()->get_channel_no());
    if (info == NULL)
    {
        return false;
    }
    return strcmp(channel, (const char*)info + 8) == 0;
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
    std::string strName(name);
    std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
    if (it != m_idMap.end() && it->second.m_id != 0)
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
    if (strlen(name) < 0x1f)
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
    if (strlen(name) < 0x1f)
    {
        std::string strName(name);
        std::map<std::string, SimpleInfo>::iterator it = m_idMap.find(strName);
        if (it != m_idMap.end())
        {
            return m_idMap[strName].m_id;
        }
        return 0;
    }
    return 0;
}

bool GameWorld::reach_game_world(CUser* user)
{
    if (user == NULL)
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::reach_game_world(CUser*)", 0x509,
                              "param pUser is null");
        return false;
    }

    errno = 0;
    std::map<unsigned short, CUser*>::iterator userIt =
        m_UserInWorld.find((unsigned short)user->get_unique_id());
    if (userIt != m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::reach_game_world(CUser*)", 0x515,
                              "m_UserInWorld.find() fail. uid(%u)",
                              (unsigned short)user->get_unique_id());
        return false;
    }

    std::map<unsigned int, unsigned short>::iterator sessionIt =
        m_sessionList.find(user->get_acc_id());
    if (sessionIt != m_sessionList.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::reach_game_world(CUser*)", 0x51d,
                              "session_list.find() fail accid(%s)",
                              sub_NumberToString(user->get_acc_id(), 0));
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
        cMyTrace trace("bool GameWorld::reach_game_world(CUser*)", 0x538, 5);
        trace("getUserVillage %d", (int)user->getCurCharacVill());
    }

    user->set_state((ch_state)3);
    EraseLoginUser(user);
    send_AllBasicInfo(user);
    if (sub_CUser_IsPermissionPrivateStore(user))
    {
        (*(void (**)(void*, void*))(*(void**)sub_private_store_GetInstancePrivateStoreMgr()
                                    ))(sub_private_store_GetInstancePrivateStoreMgr(), user);
    }
    else
    {
        sub_private_store_LoadPrivateStore(
            sub_private_store_GetInstancePrivateStoreMgr(), user);
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
    if (sub_CGM_Manager_GetCurrentGmMode(GlobalData::s_GM_Manager,
                                         user->get_acc_id()))
    {
        sub_CUser_SetGameMasterMode(user, false);
        sub_CGM_Manager_TurnGmMode(GlobalData::s_GM_Manager, user->get_acc_id());
    }

    std::map<unsigned short, CUser*>::iterator userIt =
        m_UserInWorld.find((unsigned short)user->get_unique_id());
    if (userIt == m_UserInWorld.end())
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::leave_game_world(CUser*)", 0x56a,
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
                              "bool GameWorld::leave_game_world(CUser*)", 0x588,
                              "session_list.find(%s)",
                              sub_NumberToString(user->get_acc_id(), 0));
        return false;
    }

    m_sessionList.erase(user->get_acc_id());
    sub_CSchoolMgr_DelUser(NULL, user);
    if (user->get_charac_guildkey() != 0)
    {
        sub_CGuildAgitManager_ReleaseGuildAgitArea(
            NULL, user->get_charac_guildkey());
    }
    if (user->isJoinPowerWar())
    {
        sub_CPowerManager_DecreasePowerUserCount(
            GlobalData::s_power_manager, (char)user->getPowerSide());
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
    do
    {
        m_uniqueId++;
        if (m_uniqueId == 0xffff)
        {
            m_uniqueId = 0;
        }
    } while (m_UserInWorld.find(m_uniqueId) != m_UserInWorld.end());
    return m_uniqueId;
}

unsigned short GameWorld::find_session(unsigned int accId)
{
    std::map<unsigned int, unsigned short>::iterator it;
    it = m_sessionList.find(accId);
    if (it == m_sessionList.end())
    {
        return 0;
    }
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
    unsigned short session = find_session(accId);
    if (session == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            "CUser* GameWorld::find_user_from_world_byaccid(memberIdentificationNumber_t)",
            0x4f2, "session_list.find() fail accid(%s)",
            sub_NumberToString(accId, 0));
    }
    CUser* user = find_from_world(session);
    if (user == NULL)
    {
        LogManager::logFormat(
            1, "world.cpp",
            "CUser* GameWorld::find_user_from_world_byaccid(memberIdentificationNumber_t)",
            0x4f5, "m_UserInWorld.find() fail uid(%d)", session);
    }
    return user;
}

CUser* GameWorld::find_user_from_world_byaccid_loop(unsigned int accId)
{
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
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
     std::map<unsigned short, CUser*>::iterator it;
     for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
     {
         CUser* user = it->second;
         // ORIG 条件：user!=0 && get_state()>2 && getCurCharacR()!=0 && strcmp(name)==0
         if (user != NULL && user->get_state() > 2 &&
             user->getCurCharacR() != NULL &&
             strcmp(user->getCurCharacName(), name) == 0)
         {
             return user;
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
            cMyTrace trace("void GameWorld::arrange_users()", 0x10a6, 0);
            trace("GameWorld::arrange_users ACCID: %s",
                  sub_NumberToString(user->get_acc_id(), 0));
            sub_CUser_DisConnSig(user, 7, 1, 0);
        }
    }

    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        int index = 0;
        int gcNo = sub_CServerProxyMgr_GetStartIndex(
            GlobalData::s_statistic_proxy_mgr);
        while (index < sub_CServerProxyMgr_GetEndIndex(
                           GlobalData::s_statistic_proxy_mgr))
        {
            CEnvironment* env = G_CEnvironment();
            DeleteChannel(env->get_gc_no_hardcode((ENUM_SERVER_GROUP)gcNo),
                          (ENUM_SERVER_GROUP)gcNo);
            gcNo = sub_CServerProxyMgr_GetNextIndex(
                GlobalData::s_statistic_proxy_mgr, &index);
        }
    }
    else
    {
        CEnvironment* env = G_CEnvironment();
        DeleteChannel(env->get_gc_no(), E_SERVER_GROUP_NONE);
    }
    G_CEnvironment()->set_suspend_state(true);
}

int GameWorld::CheckUserCount(int add)
{
    unsigned int size = m_UserInWorld.size();
    int maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    if ((int)size + add > maxUserNum)
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
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
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
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
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
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
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
        std::map<unsigned short, CUser*>::iterator it;
        for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
        {
            CUserCharacInfo* info = it->second;
            if (info->getCurCharacR() != NULL &&
                minLevel <= info->get_charac_level())
            {
                ((CUser*)info)->Send(packet);
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
        std::map<unsigned short, CUser*>::iterator it;
        for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
        {
            CUserCharacInfo* info = it->second;
            if (info->getCurCharacR() != NULL &&
                minLevel <= info->get_charac_level())
            {
                ((CUser*)info)->Send(packet);
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
    if ((vill != 0 || area != 0) && vill <= (int)g_townScriptFileList.size() &&
        0 < vill && vill < MAX_VILLAGE_NUM && area < m_pVill[vill].m_areaCount &&
        -1 < area)
    {
        m_pVill[vill].m_pAreas[area].send_to_all(packet);
    }
}

void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             CBelong* belong)
{
    if ((vill != 0 || area != 0) && vill <= (int)g_townScriptFileList.size() &&
        0 < vill && vill < MAX_VILLAGE_NUM && area < m_pVill[vill].m_areaCount &&
        -1 < area)
    {
        m_pVill[vill].m_pAreas[area].send_group(packet, belong);
    }
}

void GameWorld::send_to_area(int vill, int area, PacketGuard& packet,
                             std::vector<CUser*>& users)
{
    if (vill <= (int)g_townScriptFileList.size() && -1 < vill &&
        vill < MAX_VILLAGE_NUM && area < m_pVill[vill].m_areaCount && -1 < area)
    {
        m_pVill[vill].m_pAreas[area].send_to_all(packet, users);
    }
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
    if (vill != 0 && vill <= (int)g_townScriptFileList.size() && 0 < vill &&
        vill < MAX_VILLAGE_NUM)
    {
        for (int i = 0; i < m_pVill[vill].m_areaCount; i++)
        {
            if (m_pVill[vill].m_pAreas[i].get_area_index() != -1)
            {
                m_pVill[vill].m_pAreas[i].send_to_all(packet);
            }
        }
    }
}

void GameWorld::send_group(PacketGuard& packet, CBelong* belong)
{
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() >= 3)
        {
            if ((*(bool (**)(void*, CUser*))*(void**)belong)(belong, user))
            {
                user->Send(packet);
            }
        }
    }
}

void GameWorld::send_server_group(PacketGuard& packet, char group)
{
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
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
    int memberCount = sub_CParty_get_member_count(party);
    if (memberCount < 1)
    {
        LogManager::logFormat(
            1, "world.cpp", "void GameWorld::send_party_info_to_all(CParty*, int)",
            0xc37, "GameWorld::send_party_info_to_all get_member_count() is %d",
            memberCount);
        return;
    }

    PacketGuard packet;
    packet.put_header(0, 9);
    packet.put_short(1);
    packet.put_short((int)sub_CParty_GetPartyIndex(party));
    packet.put_byte(flag);

    if (flag == 0 || flag == 1)
    {
        packet.put_byte((unsigned char)sub_CParty_getTitleIndex(party));
        if (sub_CParty_getTitleIndex(party) == 0)
        {
            char* title = sub_CParty_getTitle(party);
            int titleLen = strlen(title);
            packet.put_int(titleLen);
            packet.put_str(title, titleLen);
        }
        packet.put_byte((unsigned char)sub_CParty_IsReturnUserParty(party));
        packet.put_byte((unsigned char)sub_CParty_getUserMax(party));
        packet.put_short(sub_CParty_getDungIndex(party));
        packet.put_byte((unsigned char)sub_CParty_getDungDiffi(party));
        packet.put_byte((unsigned char)sub_CParty_IsEventCharacParty(party));
    }

    if (flag == 0 || flag == 2)
    {
        int managerIdx = 0;
        for (int i = 0; i < 4; i++)
        {
            CUser* member = (CUser*)sub_CParty_get_user(party, i);
            if (member == NULL)
            {
                packet.put_short(0xffff);
                packet.put_byte(0xff);
            }
            else
            {
                packet.put_short((unsigned short)member->get_unique_id());
                if ((CUser*)sub_CParty_getManager(party) == member)
                {
                    managerIdx = i;
                }
                char sex = sub_CUser_getSex(member);
                if (sex == -1)
                {
                    sex = 1;
                }
                packet.put_byte((int)sex);
            }
        }
        packet.put_byte((unsigned char)sub_CParty_IsReturnUserParty(party));
        packet.put_byte(managerIdx);
        packet.put_byte((unsigned char)sub_CParty_is_quick_party(party));
        packet.put_byte((unsigned char)sub_CParty_IsEventCharacParty(party));
    }

    if (flag == 0 || flag == 1 || flag == 2)
    {
        bool matching = is_dungeon_tag_matching_channel((char*)"[impossible]");
        packet.put_byte(matching);
        if (matching)
        {
            for (int i = 0; i < 4; i++)
            {
                CUserCharacInfo* info = (CUserCharacInfo*)sub_CParty_get_user(party, i);
                if (info == NULL)
                {
                    packet.put_byte(0);
                }
                else
                {
                    packet.put_byte(6);
                    for (int j = 0; j < 6; j++)
                    {
                        packet.put_byte((int)G_CDataManager()->get_dimensionInout(j));
                        packet.put_byte((int)*(char*)((char*)info->getCurCharacR() +
                                                      0xeb9 + j));
                    }
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
                cMyTrace trace("void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6, 5);
                trace("[%s][%d]", "void GameWorld::send_AllBasicInfo(CUser*)", 0xbf6);
            }
            else if (info->IsCurCharacVisible())
            {
                int index = packet.get_index();
                if (sub_CUser_make_basic_info((CUser*)info, (char*)&packet, 0) != 1)
                {
                    cMyTrace trace("void GameWorld::send_AllBasicInfo(CUser*)", 0xbe3, 5);
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
    if (vill < 0 || MAX_VILLAGE_NUM <= vill)
    {
        return false;
    }
    Area* pArea = m_pVill[vill].getArea(area);
    if (pArea == NULL || pArea->m_mapAreas.empty())
    {
        return false;
    }
    MapArea& mapArea = pArea->m_mapAreas.at(0);
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
                              "bool GameWorld::check_valid_area(int, int)", 0x4a2,
                              "[GameWorld::check_valid_area] invalid village : %d, m_iVillCount : %d",
                              vill, m_iVillCount);
        return false;
    }
    if (m_pVill[vill].m_areaCount == 0)
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::check_valid_area(int, int)", 0x4aa,
                              "[GameWorld::check_valid_area] invalid area count : %d",
                              m_pVill[vill].m_areaCount);
        return false;
    }
    if (area < 0 || m_pVill[vill].m_areaCount <= area)
    {
        LogManager::logFormat(1, "world.cpp",
                              "bool GameWorld::check_valid_area(int, int)", 0x4b2,
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
        cMyTrace trace("bool GameWorld::isDungeonEntranceArea(CUser*)", 0x1685, 5);
        trace("getUserVillage call failed");
        return false;
    }
    int areaNo = user->get_area(false);
    Area* area = village->getArea(areaNo);
    if (area == NULL)
    {
        cMyTrace trace("bool GameWorld::isDungeonEntranceArea(CUser*)", 0x168c, 5);
        trace("getArea call failed (area no:%d)", areaNo);
        return false;
    }
    if (user->get_charac_level() < 2)
    {
        return true;
    }
    return area->m_field68 == 4;
}

int GameWorld::getDungeonMinimumRequiredLevel(int dungeonIdx)
{
    CDungeon* dungeon = (CDungeon*)G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == NULL)
    {
        return -1;
    }
    return sub_CDungeon_get_min_level(dungeon);
}

int GameWorld::GetWorldMapIndex(CUser* user)
{
    if (user != NULL)
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
    if (vill <= (int)g_townScriptFileList.size() && -1 < vill &&
        vill < MAX_VILLAGE_NUM && area < m_pVill[vill].m_areaCount && -1 < area)
    {
        m_pVill[vill].m_pAreas[area].get_user_id_list(out);
    }
}

bool GameWorld::get_disposition(int vill, int area,
                                pvp_assault::CDisposition* disposition)
{
    if (!check_valid_area(vill, area))
    {
        return false;
    }
    return (*(bool (**)(void*, void*))*(void**)disposition)(
        disposition, (void*)((char*)m_pVill[vill].getArea(area) + 0x7c));
}

void GameWorld::modify_user(CUserModifier* modifier)
{
    std::map<unsigned short, CUser*>::iterator it;
    for (it = m_UserInWorld.begin(); it != m_UserInWorld.end(); it++)
    {
        (*(void (**)(void*, CUser*))*(void**)modifier)(modifier, it->second);
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
            std::vector<MapArea>::iterator it;
            for (it = pArea->m_mapAreas.begin(); it != pArea->m_mapAreas.end(); it++)
            {
                if (sub_Is_restrictive_commercial_transaction_zone(
                        it->m_x, it->m_y, x, y))
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
                    "void GameWorld::move_position(CUser*, int, int, int, short unsigned int)",
                    0x5ec, 5);
                trace("%d %d", vill, area);
            }
            else
            {
                Area* pArea = village->getArea(area);
                if (pArea != NULL)
                {
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
    if (sub_private_store_IsBusyPrivateStore(
            sub_private_store_GetInstancePrivateStoreMgr(), user))
    {
        return 0x3c;
    }
    if (user->GetCurCharacExpertJob() != NULL &&
        sub_expert_job_CExpertJob_GetType(
            (void*)user->GetCurCharacExpertJob()) == 1)
    {
        return 0x38;
    }
    if (user->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }
    if (user->IsCurCharacGhost() &&
        !sub_CPowerManager_IsPowerWarEventOn() &&
        user->getCurCharacVill() != 7)
    {
        return 0x9f;
    }

    bool powerWarSafe = false;
    if (sub_CUser_isGMUser(user) != 1)
    {
        if (IsPowerWarSafeZone(vill, area, user->getPowerSide()))
        {
            powerWarSafe = true;
        }
    }
    if (powerWarSafe)
    {
        return 4;
    }

    int curVill = (int)user->getCurCharacVill();
    int curArea = user->get_area(false);
    CParty* party = (CParty*)user->GetParty();
    if (party == NULL || sub_CParty_getManager(party) != user ||
        vill != curVill || area != curArea || param == 2)
    {
        if (!check_valid_area(vill, area) ||
            !check_valid_area(curVill, curArea))
        {
            return 0x13;
        }
        if (vill < 0 || MAX_VILLAGE_NUM <= vill ||
            &m_pVill[vill] == NULL || !sub_CUser_CheckMoveTown(user, vill))
        {
            return 0;
        }
    }
    else
    {
        return 0x13;
    }
    return 0;
}

bool GameWorld::move_area(CUser* user, int vill, int area, int x, int y, int z,
                          bool flag, int param, int prevVill, int prevArea)
{
    PacketGuard errorPacket;

    if (!sub_CTimeGate_isOpen(GlobalData::s_timeGate_) &&
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
    if (sub_private_store_IsBusyPrivateStore(
            sub_private_store_GetInstancePrivateStoreMgr(), user))
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
    if (user->IsCurCharacGhost() && !sub_CPowerManager_IsPowerWarEventOn() &&
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
    if (!sub_CUser_isGMUser(user) &&
        IsPowerWarSafeZone(vill, area, user->getPowerSide()))
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
    if (party != NULL &&
        sub_CPartyTelePort_get_teleport_state(sub_CParty_GetPartyTelePort(party)))
    {
        teleport = true;
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
        int moveTown = sub_CUser_CheckMoveTown(user, vill);
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
            int before = user->getBeforeAreaTime();
            if (before != 0)
            {
                if (now - before < 3)
                {
                    if (CHackLog_InvalidAreaMove::IsInvalidMovableArea(
                            curVill, curArea, vill, area) &&
                        !CHackLog_InvalidAreaMove::isMovableRoute(
                            prevVill2, prevArea2, curVill, curArea, vill, area) &&
                        prevVill2 != vill && prevArea2 != area)
                    {
                        user->getHackAnalyzer()->addServerHackCnt(
                            user, (WongWork::ENUM_HACK_TYPE)0x1fc, 1, 0, 0);
                    }
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
                    user->getHackAnalyzer()->addServerHackCnt(
                        user,
                        (WongWork::ENUM_HACK_TYPE)CHackLog_InvalidAreaMove::GetHackType(),
                        1, prevVill, prevArea);
                }
            }
        }
        curAreaObj->delete_user(user);
    }

    if (destVillage != NULL)
    {
        int destArea = area;
        if (IsSchoolPvPChannel() && vill == 2 && area == 4)
        {
            user->GetSchoolNo();
            destArea = sub_CSchoolMgr_GetSchoolArea(NULL, 0);
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
            unsigned int guildKey = user->get_charac_guildkey();
            destArea = sub_CGuildAgitManager_GetGuildAgitAreaIndex(
                NULL, guildKey, area);
            cMyTrace trace(
                "bool GameWorld::move_area(CUser*, int, int, int, int, int, bool, int, int, int)",
                0x88e, 0);
            trace("Move Guild Agit Area User(%d) Guild(%d) dest area(%d), virtual area(%d) ",
                  user->getCurCharacNo(), guildKey, area, destArea);
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

    sub_CUserHistoryLog_MoveArea((char*)user + 0x79700, curVill, curArea,
                                 vill, area);
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
            cMyTrace trace("void GameWorld::goto_dungeon(CUser*)", 0x8d1, 5);
            trace("%d", (int)user->getCurCharacVill());
            return;
        }
        Area* area = village->getArea(user->get_area(false));
        if (area == NULL)
        {
            cMyTrace trace("void GameWorld::goto_dungeon(CUser*)", 0x8d8, 5);
            trace("%d", user->get_area(false));
            return;
        }
        area->erase_user(user);

        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xff);
        packet.put_short((unsigned short)sub_CUser_get_posX(user));
        packet.put_short((unsigned short)sub_CUser_get_posY(user));
        packet.put_byte((int)sub_CUser_get_direction(user));
        packet.put_byte((int)user->get_charac_visible_values());
        packet.finalize(true);
        user->Send(packet);
    }
    user->set_state((ch_state)5);
}

void GameWorld::out_from_dungeon(CUser* user)
{
    sub_CUser_SetRevivalTime(user, 0);
    sub_CUser_SetUseAPCPotionTime(user, 0);
    if (user->getMoveSpace() == 1)
    {
        sub_CUser_SendFatigue(user);
        user->set_state((ch_state)0xb);
        PacketGuard packet;
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte(7);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
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
            sub_CUser_SendFatigue(user);
            static_cast<XNuclear::CHades*>(user->getHades())->Send_ReturnToVillage();
        }
        else
        {
            LogManager::logFormat(
                1, "world.cpp", "void GameWorld::out_from_dungeon(CUser*)", 0x926,
                "user %s - check_valid_area() fail", user->getCurCharacName());
        }
    }
}

void GameWorld::goto_pvp(CUser* user)
{
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
        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xfe);
        packet.put_short((unsigned short)sub_CUser_get_posX(user));
        packet.put_short((unsigned short)sub_CUser_get_posY(user));
        packet.put_byte((int)sub_CUser_get_direction(user));
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
    else
    {
        LogManager::logFormat(1, "world.cpp", "void GameWorld::goto_pvp(CUser*)",
                              0x998, "user %s - check_valid_area() fail",
                              user->getCurCharacName());
    }
}

void GameWorld::out_from_pvp(CUser* user)
{
    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        sub_CUser_SetPvpIndex(user, -1);
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
    else
    {
        cMyTrace trace("void GameWorld::out_from_pvp(CUser*)", 0x9ce, 0);
        trace("user %s - check_valid_area() fail", user->getCurCharacName());
        sub_CUser_DisConnSig(user, 3, 1, 0);
    }
}

void GameWorld::goto_warroom(CUser* user)
{
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
        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xfe);
        packet.put_short((unsigned short)sub_CUser_get_posX(user));
        packet.put_short((unsigned short)sub_CUser_get_posY(user));
        packet.put_byte((int)sub_CUser_get_direction(user));
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
    else
    {
        LogManager::logFormat(1, "world.cpp", "void GameWorld::goto_warroom(CUser*)",
                              0x9f3, "user %s - check_valid_area() fail",
                              user->getCurCharacName());
    }
}

void GameWorld::out_from_warroom(CUser* user)
{
    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        sub_CUser_SetWarRoomIndex(user, -1);
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
        sub_CInventory_RemoveKCItem((void*)user->getCurCharacInvenW());
        user->InitFinishPoint();
    }
    else
    {
        cMyTrace trace("void GameWorld::out_from_warroom(CUser*)", 0xa29, 0);
        trace("user %s - check_valid_area() fail", user->getCurCharacName());
        sub_CUser_DisConnSig(user, 3, 1, 0);
    }
}

void GameWorld::gotoBlueMarble(CUser* user)
{
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
        PacketGuard packet;
        packet.put_header(0, 0x17);
        packet.put_short((unsigned short)user->get_unique_id());
        packet.put_byte((int)user->getCurCharacVill());
        packet.put_byte(0xfe);
        packet.put_short((unsigned short)sub_CUser_get_posX(user));
        packet.put_short((unsigned short)sub_CUser_get_posY(user));
        packet.put_byte((int)sub_CUser_get_direction(user));
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
    else
    {
        LogManager::logFormat(1, "world.cpp",
                              "void GameWorld::gotoBlueMarble(CUser*)", 0xa54,
                              "user %s - check_valid_area() fail",
                              user->getCurCharacName());
    }
}

void GameWorld::outFromBlueMarble(CUser* user)
{
    if (check_valid_area(user->getCurCharacVill(), user->get_area(false)))
    {
        sub_CUser_setBlueMarbleIndex(user, -1);
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
    else
    {
        cMyTrace trace("void GameWorld::outFromBlueMarble(CUser*)", 0xa7f, 0);
        trace("user %s - check_valid_area() fail", user->getCurCharacName());
        sub_CUser_DisConnSig(user, 3, 1, 0);
    }
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
        if (area->m_field68 == 2)
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
                *(int*)&item = fishIndex;
                CItemList* itemList = *(CItemList**)((char*)G_CDataManager() + 0xc);
                itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, item, 1);
                int result = sub_CInventory_insertItemIntoInventory(
                    user->getCurCharacInvenW(), item, 5, true, true);
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
                    packet.put_int(*(int*)((char*)&item + 7));
                    packet.put_short((unsigned short)*(char*)((char*)&item + 0x13));
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
        cMyTrace trace("void GameWorld::fishing(CUser*)", 0xafd, 0);
        trace("user %s - check_valid_area() fail", user->getCurCharacName());
        sub_CUser_DisConnSig(user, 3, 1, 0);
    }
}

bool GameWorld::IsPowerWarSafeZone(int vill, int area, int side)
{
    if (vill == 7 && sub_CPowerManager_IsPowerWarEventOn())
    {
        if (vill < 0 || MAX_VILLAGE_NUM <= vill)
        {
            return true;
        }
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if ((side == 1 && pArea->m_field68 == 5) ||
            (side == 2 && pArea->m_field68 == 6))
        {
            return true;
        }
    }
    return false;
}

bool GameWorld::IsPowerWarSafeZone(int vill, int area)
{
    if (vill == 7 && sub_CPowerManager_IsPowerWarEventOn())
    {
        if (vill < 0 || MAX_VILLAGE_NUM <= vill)
        {
            return true;
        }
        Area* pArea = m_pVill[vill].getArea(area);
        if (pArea == NULL)
        {
            return true;
        }
        if (pArea->m_field68 == 5 || pArea->m_field68 == 6)
        {
            return true;
        }
    }
    return false;
}

// ============================================================================
// 禁言 / 禁移动族（ORIG 086cd908..086ced1d；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

bool GameWorld::EnableChat(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_chatBan == 0)
    {
        cMyTrace trace("bool GameWorld::EnableChat(const char*)", 0x1571, 0);
        trace("%s is already able to chat!\n", str.c_str());
        return true;
    }
    info.m_chatBan = 0;
    info.m_chatEndTick = 0;
    cMyTrace trace("bool GameWorld::EnableChat(const char*)", 0x156c, 0);
    trace("%s is able to chat now\n", name);
    return true;
}

bool GameWorld::EnableMove(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_moveBan == 0)
    {
        cMyTrace trace("bool GameWorld::EnableMove(const char*)", 0x1589, 0);
        trace("%s is already able to move!\n", str.c_str());
        return true;
    }
    info.m_moveBan = 0;
    info.m_moveEndTick = 0;
    cMyTrace trace("bool GameWorld::EnableMove(const char*)", 0x1584, 0);
    trace("%s is able to move now\n", name);
    return true;
}

bool GameWorld::DisableChat(const char* name, int secs)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_chatBan == 1)
    {
        cMyTrace trace("bool GameWorld::DisableChat(const char*, int)", 0x15a7, 0);
        trace("%s is already not able to chat!\n", str.c_str());
        return true;
    }
    info.m_chatBan = 1;
    info.m_chatEndTick = secs * 1000 + GlobalData::s_systemTime_.getCurSec() * 1000;
    cMyTrace trace("bool GameWorld::DisableChat(const char*, int)", 0x15a2, 0);
    trace("%s is not able to chat now for %d secs\n", name, secs);
    return true;
}

bool GameWorld::DisableMove(const char* name, int secs)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_moveBan == 1)
    {
        cMyTrace trace("bool GameWorld::DisableMove(const char*, int)", 0x15c5, 0);
        trace("%s is already not able to move!\n", str.c_str());
        return true;
    }
    info.m_moveBan = 1;
    info.m_moveEndTick = secs * 1000 + GlobalData::s_systemTime_.getCurSec() * 1000;
    cMyTrace trace("bool GameWorld::DisableMove(const char*, int)", 0x15c0, 0);
    trace("%s is not able to move now for %d secs\n", name, secs);
    return true;
}

bool GameWorld::IsForbiddenChat(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_chatBan == 0)
    {
        return false;
    }
    if (GetLeftTimeOfForbiddenChat(name) == 0)
    {
        EnableChat(name);
        return false;
    }
    cMyTrace trace("bool GameWorld::IsForbiddenChat(const char*)", 0x153d, 0);
    trace("%s is forbidden to chat\n", name);
    return true;
}

bool GameWorld::IsForbiddenMove(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return false;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return false;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_moveBan == 0)
    {
        return false;
    }
    if (GetLeftTimeOfForbiddenMove(name) == 0)
    {
        EnableMove(name);
        return false;
    }
    cMyTrace trace("bool GameWorld::IsForbiddenMove(const char*)", 0x1557, 0);
    trace("%s is forbidden to move\n", name);
    return true;
}

unsigned int GameWorld::WhenIsTheEndOfForbiddenChat(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return 0;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_chatBan == 0)
    {
        return 0;
    }
    return info.m_chatEndTick;
}

unsigned int GameWorld::WhenIsTheEndOfForbiddenMove(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    if (m_idMap.find(str) == m_idMap.end())
    {
        return 0;
    }
    SimpleInfo& info = m_idMap[str];
    if (info.m_moveBan == 0)
    {
        return 0;
    }
    return info.m_moveEndTick;
}

unsigned int GameWorld::GetLeftTimeOfForbiddenChat(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        cMyTrace trace("DWORD GameWorld::GetLeftTimeOfForbiddenChat(const char*)", 0x15f2, 0);
        trace("GetLeftTimeOfForbiddenChat return 1\n");
        return 0;
    }
    std::string str(name);
    if (m_idMap.find(str) != m_idMap.end())
    {
        unsigned int end = WhenIsTheEndOfForbiddenChat(name);
        if (end == 0)
        {
            return 0;
        }
        int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int curTick = (unsigned int)curSec * 1000;
        unsigned int diff = end - curTick;
        if (end < curTick || diff == 0)
        {
            return 0;
        }
        if (diff < 60000)
        {
            return 60000;
        }
        return diff;
    }
    cMyTrace trace("DWORD GameWorld::GetLeftTimeOfForbiddenChat(const char*)", 0x15fa, 0);
    trace("GetLeftTimeOfForbiddenChat return 2\n");
    return 0;
}

unsigned int GameWorld::GetLeftTimeOfForbiddenMove(const char* name)
{
    if (name == 0 || strlen(name) >= 31)
    {
        return 0;
    }
    std::string str(name);
    if (m_idMap.find(str) != m_idMap.end())
    {
        unsigned int end = WhenIsTheEndOfForbiddenMove(name);
        if (end == 0)
        {
            return 0;
        }
        int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int curTick = (unsigned int)curSec * 1000;
        unsigned int diff = end - curTick;
        if (end < curTick || diff == 0)
        {
            return 0;
        }
        if (diff < 60000)
        {
            return 60000;
        }
        return diff;
    }
    return 0;
}

// ============================================================================
// 每日重置族（ORIG 086cc340..086cd15f；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

void GameWorld::ResetMidnight()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            sub_CUser_ResetDailyCharacExpandDataMidnight(user);
        }
    }
}

void GameWorld::resetDailyData()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 1)
        {
            user->resetDailyData();
        }
    }
}

void GameWorld::ResetAllCoins()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            sub_CUser_RecoverCoin(user, 1);
            if (user->GetOpencoin() == 0)
            {
                user->SendOpenflag();
            }
        }
    }
}

void GameWorld::ResetAllCleadpadPoint()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            sub_WongWork_CMCAPManager_reset((char*)user + 0x8e3f0);
        }
    }
    cMyTrace trace("void GameWorld::ResetAllCleadpadPoint()", 0x1385, 0);
    trace("RESET ALL USER CLEANPAD POINT OK!!!!");
}

void GameWorld::ResetAllPassPadFailCount()
{
    int resetCount = 0;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            bool needReset = false;
            if (sub_Sanicova_CPad_isActivate(sub_CUser_getPad(user)) != 0 &&
                sub_Sanicova_CPad_getFailCnt(sub_CUser_getPad(user)) >= 1)
            {
                needReset = true;
            }
            if (needReset)
            {
                sub_Sanicova_CPad_setFailCnt(sub_CUser_getPad(user), 0);
                sub_DB_PassPadUpdateFailCnt_makeRequest(
                    user->get_acc_id(), sub_Sanicova_CPad_getFailCnt(sub_CUser_getPad(user)),
                    sub_CUser_getWebAddress(user));
                resetCount += 1;
            }
        }
    }
    cMyTrace trace("void GameWorld::ResetAllPassPadFailCount()", 0x139d, 0);
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
            short charging = sub_CFatigueBatteryHandle_ChargingFatigueBattery(
                &GlobalData::s_fatigueBatteryHandle_, user, 0);
            user->IncFatigueBatteryCharging(charging);
            short active = sub_CFatigueBatteryHandle_ActiveFatigueBattery(
                &GlobalData::s_fatigueBatteryHandle_, user);
            if (active != 0)
            {
                user->DecFatigueBatteryCharging(active);
                cMyTrace trace("void GameWorld::ResetAllFatigue()", 0x1286, 0);
                trace("FATIGUE BATTERY ACTIVE: charno(%d) , descFatigueAmount(%d), cur charging(%d)",
                      user->getCurCharacNo(), active, user->GetFatigueBatteryCharging());
            }
            user->DBUpdateDBLogItem();
            user->ResetDBLogItem();
            sub_CUser_RecoverFatigue(user, 0);
            user->SendFatigue();
            user->resetCurCharacMemberBonusFatigue();
            user->resetAccountMemberBonusFatigue();
            user->resetAccountUsedFatigue();
            user->SetDailyBadge(badge);
            sub_CUser_ResetDailyCharacExpandData(user);
            sub_CBoosterGage_send_data(user->GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)0x11), (int)user);
            user->send_MissionList();
            user->ClearCurConditionEventStep();
            user->ClearCurConditionEventRewardStep();
            sub_CUser_SendConditionEventInfo(user);
            user->ClearProperDungeonClearCount();
            user->SendProperDungeonClearCount();
            user->ResetRevengeDungeonClear();
            user->resetNPCRelationShipDailyData();
            user->sendNPCRelationShipFavor();
            sub_CDungeonGainedGold_reset(user->getDungeonGainedGold());
            sub_APSystem_CActionPointManager_GetActionPoint((char*)user + 0x8d264);
            sub_APSystem_DB_UpdateActionPoint_makeRequest(user->GetUID(), 0, true);
            sub_APSystem_CUserProc_SetTodayActionAndCheckMedalReward(user);
            sub_Secu_GoldControl_SavetoDB(user->GetGoldControl(), false, true, false);
            sub_CEventScriptMng_send_event_init_data(
                sub_CDataManager_get_event_script_mng(G_CDataManager()), user, true);
            user->resetBlueMarbleEnterCount();
            user->sendBlueMarbleEnterCount();
            sub_CUser_ResetCurCharacUsedGiftFatigueQuantity(user);
            bool bApplied = false;
            if (isFirst)
            {
                std::string name("Arad_EventPeriodDataManager");
                void* dataMgr = sub_ARAD_Singleton_Arad_DataManager_Get();
                void* script = sub_ARAD_DataManager_findGameScript(dataMgr, name);
                ARAD::Arad_EventPeriodDataManager* evtMgr =
                    static_cast<ARAD::Arad_EventPeriodDataManager*>(script);
                bApplied = evtMgr->isApplied(
                    (ENUM_REPEAT_EVENT_CODE)0x8e,
                    GlobalData::s_systemTime_.getCurSec()) != 0;
            }
            if (bApplied)
            {
                char msg[0xc];
                sub_AradServerStateMessage_C1(msg);
                sub_ARAD_notifyOpenMessageDialog(user, 0, 0x8e, msg);
            }
            *(int*)((char*)user + 0x8ec32) = 4;
            sub_InterSelectPcroomDailyReward_SendPacket(user);
            sub_HeroMissionEvent_processMission(
                sub_ARAD_Singleton_HeroMissionEvent_Get(), user, 5, 0);
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
         it != m_loginUserMap.end(); ++it)
    {
        CUser* user = it->second;
        sub_CUser_SetValidLastLoginCharac(user, false);
        sub_CUser_SetLastLoginCharacNo(user, 0);
        sub_CUser_SetLastLoginChannelNo(user, 0);
    }
}

bool GameWorld::ResetCacheCharactorMemory(unsigned int accId)
{
    std::map<unsigned int, CUser*>::iterator it = m_loginUserMap.find(accId);
    if (it != m_loginUserMap.end())
    {
        CUser* user = it->second;
        sub_CUser_SetValidLastLoginCharac(user, false);
        sub_CUser_SetLastLoginCharacNo(user, 0);
        sub_CUser_SetLastLoginChannelNo(user, 0);
        return true;
    }
    return false;
}

void GameWorld::ResetEntireReliablePerson()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
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
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d) != 0)
    {
        sub_CItemShop_updateOneADayItemList((char*)G_CDataManager() + 0x8cbc);
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); ++it)
        {
            CUser* user = it->second;
            if (user->get_state() > 2)
            {
                sub_CUser_SendOneADayItemShopIndex(
                    user, sub_CItemShop_GetOneADayItemList((char*)G_CDataManager() + 0x8cbc));
            }
        }
    }
}

void GameWorld::testResetOneADayItemShop()
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6d) != 0)
    {
        sub_CItemShop_testupdateOneADayItemList((char*)G_CDataManager() + 0x8cbc);
        for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
             it != m_UserInWorld.end(); ++it)
        {
            CUser* user = it->second;
            if (user->get_state() > 2)
            {
                sub_CUser_SendOneADayItemShopIndex(
                    user, sub_CItemShop_GetOneADayItemList((char*)G_CDataManager() + 0x8cbc));
            }
        }
    }
}

void GameWorld::SetWeekendBounsEvent()
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    localtime_r(&t, &local);
    if ((local.tm_wday == 6 && local.tm_hour > 5) || local.tm_wday == 0 ||
        (local.tm_wday == 1 && local.tm_hour < 6))
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57) == 0)
        {
            Word_Param param;
            param.m_duration = 0;
            param.m_field2 = 0;
            GlobalData::s_event_manager->TriggerEventStart(0x57, param);
        }


    }
    else if (local.tm_wday == 1 && local.tm_hour > 5)
    {
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57) != 0)
        {
            GlobalData::s_event_manager->TriggerEventEnd(0x57);
        }
    }
}

void GameWorld::DeleteDailyItem()
{
    cMyTrace trace1("void GameWorld::DeleteDailyItem()", 0x13da, 0);
    trace1("DeleteDailyItem size(%d) start tick(%u)", (int)m_UserInWorld.size(),
           GlobalData::s_systemTime_.getCurTickCount());
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user != 0)
        {
            sub_CUser_processDelDailyItem(user);
        }
    }
    GlobalData::s_systemTime_.update();
    cMyTrace trace2("void GameWorld::DeleteDailyItem()", 0x13e6, 0);
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
            sub_CUser_AddDailyItem(user);
            if (user->getCurCharacR() != 0)
            {
                sub_CUser_ResetDailyQuest(user);
                sub_CUser_ResetTrainingQuest(user);
                PacketGuard packet;
                sub_CUserQuest_get_quest_info(sub_CUser_getCurCharacQuestR(user), (char*)&packet);
                user->Send(packet);
            }
            if (user->getCurCharacR() != 0 && user->get_charac_level() > 0x3b)
            {
                sub_CUser_DimensionInoutUpdate(user, true, true);
            }
        }
    }
}

bool GameWorld::ReselectDailyQuest()
{
    if (!sub_CDataManager_reselectDailyTrainingQuest(G_CDataManager()))
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
    sub_CraneMinigameManager_updateCraneItemNeedMaterial(
        sub_CGameManager_GetCraneMinigameManager(sub_G_CGameManager()), param);
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            sub_CraneMinigameManager_updateCraneItemNeedMaterial(
                sub_CGameManager_GetCraneMinigameManager(sub_G_CGameManager()), (int)user);
        }
    }
}

void GameWorld::deleteSpecificItems(const std::vector<std::pair<int, int> >* list)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        user->deleteSpecificItems(list);
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
            cMyTrace trace("void GameWorld::UpdateServerSnapShot()", 0x1b61, 8);
            trace("%u,%d,%u,%d,%d,%d,%d,%d,%s %s",
                  user->get_acc_id(),
                  sub_CEnvironment_get_server_group(G_CEnvironment()),
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
    int group = sub_CEnvironment_get_server_group(G_CEnvironment());
    Packet_MiniCraneSeed packet;
    sub_CMonitorServerProxy_SendTcpPacket(
        sub_CServerProxyMgr_GetServerProxy_monitor(&GlobalData::s_monitor_proxy_mgr,
            (ENUM_SERVER_GROUP)group),
        (char*)&packet, 0xe);
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

bool GameWorld::IsSameTeamChannelJoin()
{
    if (m_channelType <= 0xf)
    {
        if ((1 << m_channelType) & 0xc200)
        {
            return true;
        }
    }
    return false;
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
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (sub_CUser_IsPowerUp(user))
        {
            sub_CUser_SetPowerUp(user, false);
            list.push_back(user);
        }
    }
    SendPowerUpList(0, list);
}

void GameWorld::GetPowerUpDomain(std::vector<CUser*>& listA, std::vector<CUser*>& listB)
{
    double threshold = (double)(*(int*)((char*)G_CDataManager() + 0xa678)) / 100.0;
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
                    if (sub_CUser_getPowerSide(user) == 1)
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

void GameWorld::SendPowerUpList(int activateType, std::vector<CUser*>& list)
{
    PacketGuard packet;
    packet.put_header(0, 0xd8);
    packet.put_byte((char)activateType);
    packet.put_short((short)list.size());
    for (std::vector<CUser*>::iterator it = list.begin(); it != list.end(); ++it)
    {
        packet.put_short((short)(*it)->get_unique_id());
    }
    packet.finalize(true);
    send_to_vill(7, packet);
}

void GameWorld::OnEndPowerWar(PacketGuard& packet)
{
    int var0 = 0;
    std::map<unsigned short, CUser*>::iterator it;
    int var1 = 0;
    packet.put_byte(sub_CPowerManager_GetWinnerSide(&GlobalData::s_power_manager));
    packet.put_short(0);
}

void GameWorld::OnRefreshPowerWarProcessInfo()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->isJoinPowerWar())
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            int last = user->GetPowerWarProcessInfoLastUpdateTime();
            int interval = *(int*)((char*)G_CDataManager() + 0xa658) * 0x3c;
            if (interval < now - last)
            {
                sub_CGuildServerProxy_SendPowerWarProcessInfo(
                    sub_CServerProxyMgr_GetServerProxy_guild(
                        &GlobalData::s_guild_proxy_mgr, sub_CUser_GetServerGroup(user)),
                    sub_CUser_get_charac_no(user, -1));
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
            int limit = *(int*)((char*)G_CDataManager() + 0xa65c) * 0x3c;
            if (limit < now - last)
            {
                sub_CPowerManager_ProcessJoinPowerWar(
                    &GlobalData::s_power_manager, user, 0);
                PacketGuard packet;
                packet.put_header(0, 0xe2);
                packet.finalize(true);
                user->Send(packet);
                cMyTrace trace("void GameWorld::OnBanAbuserFromPowerWar()", 0x178e, 0);
                trace("[PowerWar] POWER WAR BAN USER (USER:%s(%d), LAST:%d, CURR:%d)",
                      user->getCurCharacName(), user->getCurCharacNo(), last, now);
            }
        }
    }
}

void GameWorld::CleanupPowerWarVillToStart()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->getCurCharacVill() == 7)
        {
            sub_CPowerManager_ProcessJoinPowerWar(&GlobalData::s_power_manager, user, 0);
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
            "void GameWorld::get_user_info(CUser*, int, int)", 0x107e,
            "[GameWorld::get_user_info] can't find user in world");
        return;
    }
    bool hidden = false;
    if (type == 3)
    {
        if (sub_WongWork_CGMAccounts_isGM(&GlobalData::s_pGMAccounts_, user->get_acc_id()) != 1 &&
            sub_WongWork_CGMAccounts_isGM(&GlobalData::s_pGMAccounts_, target->get_acc_id()) == 1)
        {
            hidden = true;
        }
    }
    if (!hidden)
    {
        if (type == 3)
        {
            sub_CTitleBook_sendListOtherUser(
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe), user, 2);
        }
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte((char)type);
        if (type != 2)
        {
            packet.put_short(1);
        }
        target->make_basic_info((char*)&packet, (char)type);
        packet.finalize(true);
        user->Send(packet);
    }
}

// ============================================================================
// 聊天族（ORIG 86c954c..86d0f9e；逻辑来自 Ghidra C + 反汇编）
// ============================================================================

// MSG_ITEM_HYPER_LINK_MESSAGE（本 TU 布局；packet 域权威头未翻译）
struct MSG_ITEM_HYPER_LINK_MESSAGE
{
    char m_pad0[0xd];
    char m_type;              // +0x0d
    unsigned short m_target;  // +0x0e
    unsigned int m_param;     // +0x10
    int m_msgLen;             // +0x14
    char m_msg[0x100];        // +0x18
    int m_nickLen;            // +0x118
    char m_nick[0x20];        // +0x11c
    char m_pad13b;            // +0x13b
    char m_items[1][0x68];    // +0x13c
};

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
                                                void* linkMsg, ENUM_NOTIPACKET notify)
{
    MSG_ITEM_HYPER_LINK_MESSAGE* msg = (MSG_ITEM_HYPER_LINK_MESSAGE*)linkMsg;
    packet.clear();
    packet.put_header(0, (int)notify);
    packet.put_byte(msg->m_type);
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
    packet.put_int(msg->m_msgLen);
    packet.put_str(msg->m_msg, msg->m_msgLen);
    packet.put_byte(msg->m_pad13b);
    for (int i = 0; i < (unsigned char)msg->m_pad13b; i++)
    {
        packet.put_binary(msg->m_items[i], 0x68);
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
                sub_CEnvironment_get_file_name(G_CEnvironment()));
        char log[20];
        sub_CMyFileLog_C1(log, "void GameWorld::WriteLogChatMsg(CUser*, char, char*)", 0xd6d);
        sub_CMyFileLog_cl(log, path,
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
    if (IsIntegratedPvPBaseChannel() && from->GetServerGroup() != to->GetServerGroup())
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x15, 0x15, msg, len);
        return;
    }
    bool black = false;
    if (to->isBlackUser(from->get_charac_no(-1)) ||
        from->isBlackUser(to->get_charac_no(-1)))
    {
        black = true;
    }
    if (black)
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x4d, 0xfe, msg, len);
    }
    else if (sub_WongWork_CGMAccounts_isGM(&GlobalData::s_pGMAccounts_, to->get_acc_id()) == 0)
    {
        send_peerTypemsg_sameWorldUser(from, to, type, 0, 1, msg, len);
    }
    else
    {
        send_peerTypemsg_sameWorldUser(to, from, type, 0x5a, 0x20, msg, len);
    }
}

void GameWorld::send_chat_msg(CUser* from, char type, unsigned short target,
                              unsigned int characNo, char* msg, int len, char* nick,
                              int nickLen, ENUM_NOTIPACKET notify)
{
    if (type < 0x1f && ((1 << (type & 0x1f)) & 0x7fff0411U) != 0 && from != 0)
    {
        sub_CUser_DisConnSig(from, 0x18, 1, 0);
        return;
    }
    if (type != 0)
    {
        std::string filtered = msg;
        sub_CSyncSlangFilter_Filter(sub_CSyncSlangFilter_GetInstance(), filtered);
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
                cMyTrace trace("void GameWorld::send_chat_msg(CUser*, char, short unsigned int, unsigned int, char*, int, char*, int, ENUM_NOTIPACKET)", 0xdf3, 0);
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
        break;
    case 1:
    case 0x21:
    {
        CUser* to = find_from_world(target);
        if (to == 0)
        {
            char buf[0x12c];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0x04) = type;
            *(int*)(buf + 0x08) = from->getCurCharacNo();
            *(char*)(buf + 0x0c) = (char)len;
            memcpy(buf + 0x0d, nick, nickLen);
            memcpy(buf + 0x2c, msg, len);
            int sendLen = len + 0x3b;
            ((CMonitorServerProxy*)sub_CServerProxyMgr_GetServerProxy_monitor(
                &GlobalData::s_monitor_proxy_mgr, from->GetServerGroup()))
                ->SendPacket(buf, sendLen);
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
        else if (sub_CGM_Manager_IsGm(&GlobalData::s_GM_Manager, to->get_acc_id()) == 0)
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
            cMyTrace trace("void GameWorld::send_chat_msg(CUser*, char, short unsigned int, unsigned int, char*, int, char*, int, ENUM_NOTIPACKET)", 0xf1d, 5);
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
            int add = sub_CUser_isDuplicationMessage(from, smsg) != 0
                          ? G_CDataManager()->m_serverParameter.GetPaneltyDupMessage()
                          : 1;
            sub_CUser_incChattingMessageCount(from, add * cleanCount);
            if (sub_CUser_getChattingMessageCount(from) >
                G_CDataManager()->m_serverParameter.GetMaxCleanChattingCount())
            {
                sub_CUser_setChattingMessageCount(from, 0);
                sub_CUser_reqHumanCertify4ClearMap(from, false);
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
                PvP_Room* room = sub_G_CGameManager() != 0
                    ? ((CGameManager*)sub_G_CGameManager())->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
                if (room != 0)
                {
                    room->send_to_pvp(packet);
                }
            }
            else
            {
                sub_pvp_assault_CAssaultMgr_SendPacket(
                    sub_pvp_assault_GetInstanceAssaultMgr(), from, false, &packet);
            }
        }
        break;
    case 6:
        if (from->getCurCharacNo() != 0 && from->get_charac_guildkey() != 0)
        {
            char buf[0x40];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0x04) = from->get_charac_guildkey();
            *(int*)(buf + 0x08) = from->getCurCharacNo();
            *(char*)(buf + 0x0c) = (char)len;
            memcpy(buf + 0x0d, msg, len);
            ((CGuildServerProxy*)sub_CServerProxyMgr_GetServerProxy_guild(
                &GlobalData::s_guild_proxy_mgr, from->GetServerGroup()))
                ->SendPacket(buf, len + 0x13);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                "void GameWorld::send_chat_msg(CUser*, char, short unsigned int, unsigned int, char*, int, char*, int, ENUM_NOTIPACKET)",
                0xf8c, "GUILD_CHAT_ERROR : charac no == %d || guild key == %d ",
                from->getCurCharacNo(), from->get_charac_guildkey());
        }
        break;
    case 8:
        if (from->getCurCharacNo() != 0 &&
            sub_CUserCharacInfo_get_charac_memberkey(from) != 0)
        {
            char buf[0x40];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0x04) = (int)sub_CUserCharacInfo_get_charac_memberkey(from);
            *(char*)(buf + 0x08) = (char)len;
            memcpy(buf + 0x09, msg, len);
            ((CMonitorServerProxy*)sub_CServerProxyMgr_GetServerProxy_monitor(
                &GlobalData::s_monitor_proxy_mgr, from->GetServerGroup()))
                ->SendPacket(buf, len + 0xf);
        }
        else
        {
            LogManager::logFormat(1, "world.cpp",
                "void GameWorld::send_chat_msg(CUser*, char, short unsigned int, unsigned int, char*, int, char*, int, ENUM_NOTIPACKET)",
                0xfa3, "MEMBER_CHAT_ERROR : member key == %d ",
                (int)sub_CUserCharacInfo_get_charac_memberkey(from));
        }
        break;
    case 9:
        if (from != 0)
        {
            make_packet_chat_msg(packet, from, msg, len, type, notify);
            if (from->GetAssaultPlace() == 0)
            {
                PvP_Room* room = sub_G_CGameManager() != 0
                    ? ((CGameManager*)sub_G_CGameManager())->GetPvp(
                          (int)from->GetPvpIndex(), from, 0)
                    : 0;
                if (room != 0)
                {
                    room->send_to_pvp_team(packet, from);
                }
            }
            else
            {
                sub_pvp_assault_CAssaultMgr_SendPacket(
                    sub_pvp_assault_GetInstanceAssaultMgr(), from, true, &packet);
            }
        }
        break;
    case 0xb:
    case 0xc:
    case 0xe:
    {
        char buf[0x2e + 256];
        memset(buf, 0, sizeof(buf));
        *(char*)(buf + 0x00) = type;
        memcpy(buf + 0x01, from->getCurCharacName(),
               strlen(from->getCurCharacName()));
        *(unsigned short*)(buf + 0x01 + 0x20) = from->get_unique_id();
        *(char*)(buf + 0x25) = (char)len;
        memcpy(buf + 0x26, msg, len);
        ((CMonitorServerProxy*)sub_CServerProxyMgr_GetServerProxy_monitor(
            &GlobalData::s_monitor_proxy_mgr, from->GetServerGroup()))
            ->SendPacket(buf, len + 0x2e);
        break;
    }
    case 0xd:
    case 0xf:
    {
        PacketGuard pm;
        pm.put_header(0, 0x76);
        pm.put_byte((int)type);
        pm.put_byte(sub_CEnvironment_get_channel_no(G_CEnvironment()));
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
            BlueMarble* bm = (BlueMarble*)sub_CUser_getBlueMarble(from);
            if (bm != 0)
            {
                bm->sendToBlueMarble(packet);
            }
        }
        break;
    }
}

void GameWorld::send_chat_msg_hyper_link(CUser* from, void* linkMsg, ENUM_NOTIPACKET notify)
{
    MSG_ITEM_HYPER_LINK_MESSAGE* msg = (MSG_ITEM_HYPER_LINK_MESSAGE*)linkMsg;
    if (msg->m_type < 0x1f && ((1 << (msg->m_type & 0x1f)) & 0x7fff0411U) != 0 &&
        from != 0)
    {
        sub_CUser_DisConnSig(from, 0x18, 1, 0);
        return;
    }
    if (msg->m_type != 0)
    {
        std::string filtered = msg->m_msg;
        sub_CSyncSlangFilter_Filter(sub_CSyncSlangFilter_GetInstance(), filtered);
        if (filtered.size() != 0)
        {
            strncpy(msg->m_msg, filtered.c_str(), filtered.size());
        }
        else
        {
            return;
        }
    }
    PacketGuard packet;
    WriteLogChatMsg(from, msg->m_type, msg->m_msg);
    switch (msg->m_type)
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
        CUser* to = find_from_world(msg->m_target);
        if (to == 0)
        {
            char buf[0x12c + 0x100];
            memset(buf, 0, sizeof(buf));
            *(int*)(buf + 0x04) = msg->m_type;
            *(unsigned int*)(buf + 0x08) = msg->m_param;
            *(char*)(buf + 0x0c) = (char)msg->m_msgLen;
            memcpy(buf + 0x0d, msg->m_nick, msg->m_nickLen);
            *(char*)(buf + 0x0d + 0x20) = (char)msg->m_msgLen;
            memcpy(buf + 0x2c, msg->m_msg, msg->m_msgLen);
            *(char*)(buf + 0x2c + msg->m_msgLen) = msg->m_pad13b;
            for (int i = 0; i < (unsigned char)msg->m_pad13b; i++)
            {
                memcpy(buf + 0x2c + msg->m_msgLen + 1 + i * 0x68,
                       msg->m_items[i], 0x68);
            }
            ((CMonitorServerProxy*)sub_CServerProxyMgr_GetServerProxy_monitor(
                &GlobalData::s_monitor_proxy_mgr, from->GetServerGroup()))
                ->SendPacket(buf, msg->m_msgLen + 0x174);
            break;
        }
        PacketGuard pm;
        bool black = to->isBlackUser(from->get_charac_no(-1)) ||
                     from->isBlackUser(to->get_charac_no(-1));
        if (black)
        {
            pm.put_header(0, 0x173);
            pm.put_byte(msg->m_type);
            pm.put_byte(0x4d);
            int nameLen = (int)strlen(to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg->m_msgLen);
            pm.put_str(msg->m_msg, msg->m_msgLen);
            pm.put_byte(msg->m_pad13b);
            for (int i = 0; i < (unsigned char)msg->m_pad13b; i++)
            {
                pm.put_binary(msg->m_items[i], 0x68);
            }
            pm.finalize(true);
            from->Send(pm);
        }
        else
        {
            int code;
            if (IsIntegratedPvPBaseChannel() &&
                from->GetServerGroup() != to->GetServerGroup())
            {
                code = 0x15;
            }
            else if (sub_CGM_Manager_IsGm(&GlobalData::s_GM_Manager,
                                          to->get_acc_id()) != 0)
            {
                code = 0x5a;
            }
            else
            {
                code = 0;
            }
            pm.put_header(0, 0x173);
            pm.put_byte(msg->m_type);
            pm.put_byte(code);
            int nameLen = (int)strlen(
                code == 0 ? from->getCurCharacName() : to->getCurCharacName());
            pm.put_int(nameLen);
            pm.put_str(code == 0 ? from->getCurCharacName() : to->getCurCharacName(), nameLen);
            pm.put_byte(0);
            pm.put_int(msg->m_msgLen);
            pm.put_str(msg->m_msg, msg->m_msgLen);
            pm.put_byte(msg->m_pad13b);
            for (int i = 0; i < (unsigned char)msg->m_pad13b; i++)
            {
                pm.put_binary(msg->m_items[i], 0x68);
            }
            pm.finalize(true);
            if (code == 0)
            {
                to->Send(pm);
            }
            else
            {
                from->Send(pm);
            }
        }
        break;
    }
    default:
        break;
    }
}

// ============================================================================
// ProcessAllUsers 模板（ORIG 实例 0863d3bc / 0816270c）
// ============================================================================

class CEventStayTime
{
public:
    class RewardGoGoFighter
    {
    public:
        void operator()(CUser* user);
        int m_0;
        int m_4;
    };
};

namespace InterSelectCreateDnfEventInfo
{
class Reward
{
public:
    void operator()(CUser* user);
    unsigned short m_0;
    char m_2;
};
}

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
    CStreamGuard guard(((StreamPool*)0x940bd6c)->Acquire("GameWorld.cpp", 0x1e),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(5), user->GetUID());
    Taiwan::SigStayTimeEvent* evt = guard.GetInBuffer<Taiwan::SigStayTimeEvent>();
    if (evt != 0)
    {
        cMyTrace trace("CEventStayTime::RewardGoGoFighter::operator()", 0x24, 0);
        trace("%d", user->get_acc_id());
        evt->init();
        evt->set(user->get_acc_id());
        ((MsgQueueMgr*)0x940bd68)->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    }
}

void InterSelectCreateDnfEventInfo::Reward::operator()(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 0x22d);
    packet.put_short(m_0);
    bool reward = sub_CUser_GetEventCreateDnfReward(user) == 0 && m_2 != 0;
    packet.put_byte(reward ? 1 : 0);
    packet.finalize(true);
    user->Send(packet);
}

template <typename T>
void GameWorld::ProcessAllUsers(T functor)
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
        {
            functor(user);
        }
    }
}

template void GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(
    CEventStayTime::RewardGoGoFighter);
template void GameWorld::ProcessAllUsers<InterSelectCreateDnfEventInfo::Reward>(
    InterSelectCreateDnfEventInfo::Reward);

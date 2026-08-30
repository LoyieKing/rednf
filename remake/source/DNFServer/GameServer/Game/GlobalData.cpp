#include "LogManager.h"
// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。

#include "GlobalData.h"
#include "PCRoomServerProxy.h"   // CPCRoomServerProxy 权威（原 TU 局部定义收敛，Fix14_I）
#include "Action.h"  // StackableAction::Action 定义（GlobalData.h 不再复制）
#include "OnlinePreliminary.h"   // online_preliminary 域权威声明（本 TU 持有 s_onlinePreliminaryTeamMgr 构造）
#include "RestrictGeolocation.h"   // RestrictGeolocation 权威（原局部镜像删除，实现迁 RestrictGeolocation.cpp）
#include "CDailyScheduleManager.h"  // _DailySchedule / CDailyScheduleManager 权威（原本地镜像删除，ODR 收敛）
#include "CCapchaDataManager.h"  // WongWork::CCapchaDataManager / MSG_UDP_CAPCHA_CERTIFY 权威（显式，防间接 include 漂移）
#include "Tick.h"                // Tick::nowAsInternalValue（CaptchaGen 播种源，对齐 ORIG）
#include "GameMasterCmd.h"        // game_master::CGameMasterMgr（占位骨架已删，实现在 GmCore/GmRegister）
#include "CItemList.h"             // get_rand_int
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）

#include <iostream>
#include <new>
#include <stdio.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <list>
#include <string>
#include <string>
#include <deque>
#include <math.h>
#include <dlfcn.h>
#include <set>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include "TaiwanInternal.h"
#include "MoreTypes.h"
#include "CStreamGuard.h"
#include "CStatisticServerProxy.h"   // CServerProxyMgr<CStatisticServerProxy> 完整类型
#include "CGM_Manager.h"             // CGM_Manager 真实布局（ORIG 0x3c）
#include "SpecialItemHandler.h"      // ORIG 全局类 SpecialItemHandler 权威声明
#include "CSyncSlangFilter.h"
#include "AradServerStateMessage.h"  // ARAD::Arad_ServerStateManager 成员类型
#include "Arad_ServerStateManager.h"  // ARAD::Arad_ServerStateManager 权威定义（原局部镜像已删除）
#include "Arad_DataManager.h"  // ARAD::Singleton 唯一权威定义
#include "CInventory.h"              // CSecu_ProtectionField 权威定义（CInventory.h）
#include "BaseServerProxy.h"         // ENUM_SOCKET_STATE（CPCRoomServerProxy socketState）
#include "CMyFileLog.h"              // 租赁 log（RandomRentalA2 批次）
#include "CUser.h"    // StackableAction proc_* 需要 CUser 完整类型
#include "CSimpleSSO.h"  // WongWork::CSimpleSSO 权威定义（原局部类已收敛）
#include "CItem.h"    // StackableAction proc_* 需要 CItem 完整类型
#include "CCeraShop.h"        // WongWork::CCeraShop 权威声明
#include "SigTypes.h"         // MSG_BUY_CERASHOP_ITEM（BuyItem/ProcessBuyRequestEx 形参 const 引用）
#include "InstanceRentalSystem.h"  // InstanceRentalSystem 权威定义（§7 合并）
#include "CGlobalEffectManager.h" // CGlobalEffectManager 权威声明（本地占位已移除）
#include "CSystemTime.h"      // CSystemTime::getCurSec（IPG 请求 curSec）
#include "RefPvpGrade.h"      // RefPvpGrade 权威定义（ORIG 布局 0x14c）
#include "Teleport.h"        // Teleport/NPC_Teleport/User_Teleport 权威声明
#include "CBusinessImpl.h"   // IBusiness/CBusinessImpl 权威声明
#include "CHackLog_InvalidAreaMove.h"  // checkSeriaRoom（User_Teleport::check_rule）
#include "CFatigueBatteryHandle.h"  // CFatigueBatteryHandle 权威定义（ORIG ctor 0x84a7ff8 空体）
#include "sync_script.h"     // sync_script::CSyncScript 权威定义（GetInstanceSyncScript/SetEnable/IsEnable）
#include "TimerQueue.h"      // G_TimerQueue / TimerEntry（RegistNextTimer）
#include "CBlackIPMonitor.h"  // WongWork::CBlackIPMonitor 权威定义（ORIG ctor 0x82a6526）
#include "CTimeGate.h"        // CTimeGate : village_object::IObject 权威定义（ORIG ctor 0x8284512）
#include "CVillageObjectMgr.h" // village_object::{IObject,CVillageObjectMgr} 权威定义（本地镜像已移除）
#include "TaiwanCash.h"  // WongWork::IPG::{IIPGHelperInterface,CIPGMgr} 权威定义（局部 mirror 已删除）
#include "CountryCode.h"   // CountryCode 权威定义（原 TU-local struct 已删除，实现迁 CountryCode.cpp）

#include "CCommunityServerProxy.h"
#include "CSchoolServerProxy.h"

// StaticPool/expert_job::CEnchanter/CDisjointer 权威声明已迁 CExpertJob.h
//（含 CExpertJobFactory/CExpertJobMgr），本 TU 经 include 获取。
#include "CExpertJob.h"

#include "CEnvironment.h"
#include "GameTypes.h"
// PacketGuard 完整类型经 CInventory.h（权威定义点）；此处不再重复 include。
#include "CRijndael.h"
// expert_extraction 域权威声明（原局部类定义已删，ODR 收敛；实现 ExpertExtraction.cpp）
#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"
#include "PvPTypes.h"      // CMTRand 完整类型（CExtraction::m_rand）
namespace DNFFLib { int Binary2Hex(unsigned char const*, int, char*); }   // _ZN7DNFFLib10Binary2HexEPKhiPc，定义见 DNFFLib 权威 TU
#include "Cipher.h"
#include "BaseServerProxy.h"
#include "CHadesServerProxy.h"
#include "CaptchaGenData.h"
#include "CQueryCounter.h"
#include "ThreadInterface.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "ServiceRestrictManager.h"  // 权威定义（本地类已迁移）
#include "BingoEvent.h"
#include "CMonitorServerProxy.h"
#include "W6_HackPunishDeps.h"   // WongWork::CAutoPunishRule* 权威完整类定义
#include "CLogGameChannel.h"   // WongWork::CLogGameChannel 完整定义（s_logGameChannel 初始化）
#include "CServerProxyMgr.h"
#include "CItemAmplifier.h"
#include "CPowerManager.h"   // 权威 CPowerManager 类（替代下方本地 stub，解除 ODR）
#include "CItemLimitEdition.h"       // ProcessIPG_ResultOutput case 10（限购展示位）
#include "Arad_IPGProcess.h"         // static IPGProcess（ORIG W ctor 0x0817a2ba）
#include "Arad_MileageProcess.h"     // static mMileageProcess（ORIG W ctor 0x0817a2ce）
#include "HeroMissionEvent.h"        // setMissionValue（case 7 任务计数）
#include "PacketTracer.h"            // CPacketTracer::AddUdpLog / CPacketTracerInstance（UdpThread::_processUDPData）
#include "EncryptTool.h"             // GlobalInstance<EncryptTool> 显式实例化（ORIG W 0x848e1d3/0x848e1e5）
#include "InterDispatcher.h"         // InterDispatcher 权威类（GlobalInstance<InterDispatcher> 实例化）
#include "TimerDispatcher.h"         // TimerDispatcher 权威类（GlobalInstance<TimerDispatcher> 实例化）
#include "PacketDispatcher.h"       // PacketDispatcher / G_PacketDispatcher / pUserPacketCounter_
#include "GameServerDemonManager.h" // GameserverDemonManagerInstance / IsProhibitConnectState
#include "GameWorld.h"               // GameWorld 权威类（GlobalInstance<GameWorld> 实例化）
#include "CDataManager.h"            // CDataManager 权威类（GlobalInstance<CDataManager> 实例化）
#include "AuctionProxy.h"           // CAuctionServerProxy/CCeraAuctionServerProxy 权威类（MonitorTcpThread::dispatch）
#include "CGameManager.h"           // CGameManager/StaticPool<CUser,600> 完整类型（AcceptThread::dispatch）
#include "PacketGuard.h"            // PacketGuard 完整类型（AcceptThread::dispatch 栈对象）
#include "EpollHandler.h"          // EpollHandler 完整类型（NetworkThread::dispatch）
#include "W8LT_DBTask.h"          // DBTask 完整类型（DBThread::dispatch）
#include "MessageCounter.h"       // message_counter::{CTimeInterval,CMessageCounter}（DB/DispatchThread）
#include "UserPacketCounter.h"    // message_counter::CUserPacketCounter（DispatchThread::dispatch）
#include "OS_API.h"               // OS_API::{Exit,Sleep,uSleep}（线程 dispatch）

// ============================================================================
// 依赖类最小 stub（符号/布局属后续批次；本 TU 只用于还原 GlobalData 函数）
// ============================================================================

typedef std::map<ENUM_DB_HANDLE_IDX, ENUM_SERVER_GROUP> DBGroupMap;
typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DBInfoMap;

// ARAD::Arad_ServerStateManager 局部镜像已删除：权威定义在 Arad_ServerStateManager.h。

class PacketPool;

// ORIG W 0x0857f5d6（_Z13G_EpollHandlerv）：GlobalInstance<EpollHandler>::inst_ptr()。
// 与 CUser.cpp 同形态的本地声明（EpollHandler.h 权威头未收录，避免共享头改动）。
EpollHandler* G_EpollHandler();

// ORIG W：G_TimerDispatcher 0x0847e8a4 / G_InterDispatcher 0x0847e8b7；
// GlobalInstance 单例访问器，定义见本文件尾部（同 G_TimerQueue 形态）。
class TimerDispatcher;
class InterDispatcher;
TimerDispatcher* G_TimerDispatcher();
InterDispatcher* G_InterDispatcher();

class DBThread : public ThreadInterface
{
public:
    DBThread();
    void setLogDB(bool isLogDB);
    bool isLogDB();  // ORIG T 0x083ff77a：读 +0x0c
    virtual void dispatch(void* arg);          // ORIG T 0x083ff786（线程体：DB 事件循环）
private:
    // ORIG 布局（objdump 0x083ff5f2）：ThreadInterface 基类 0x00..0x0b
    // （vptr +0 / m_tid +4 / m_tidCopy +8）；+0x0c bool m_bLogDB；
    // +0x10 CQueryCounter（sizeof 0x3c → 总 0x4c）。
    bool m_bLogDB;                 // +0x0c
    CQueryCounter m_queryCounter;  // +0x10（sizeof 0x3c → 总 0x4c）
};

class DispatchThread : public ThreadInterface
{
public:
    DispatchThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0847e2d2（线程体：主分发循环）
};

class NetworkThread : public ThreadInterface
{
public:
    NetworkThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857d5be（线程体：epoll 收包）
};

class UdpThread : public ThreadInterface
{
public:
    UdpThread();
    virtual void dispatch(void* arg);          // ORIG 0x0857c9de（线程体：bind + select/recv 循环）
private:
    int _bindUDPSocket(unsigned short port);   // ORIG 0x0857c3be
    void _processUDPData(char* data, int len); // ORIG 0x0857c5a4
};

class MonitorTcpThread : public ThreadInterface
{
public:
    MonitorTcpThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857d95a（线程体：代理 select 轮询）
};

template <class T> class CServerProxyMgr;
class AcceptThread : public ThreadInterface
{
public:
    AcceptThread();
    virtual void dispatch(void* arg);          // ORIG T 0x0857cc5a（线程体：listen/accept 循环）
private:
    // ORIG ctor 0x0857cbe8：置 +0x0c 字节为 0 → 基类 0xc 之后一个 bool，总 0x10。
    bool m_acceptFlag;  // +0x0c
};

class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x10];
};



namespace secretshop
{
class CSecretShop
{
public:
    CSecretShop();
private:
    char m_pad[0x64];
};
}

class CmmChannelProxy
{
public:
    CmmChannelProxy(std::string ip, int port);
    bool Init();
    void SendChannelInfo();   // _ZN15CmmChannelProxy15SendChannelInfoEv 0x0810619e
    bool ConnectBridgeServer();               // _ZN15CmmChannelProxy19ConnectBridgeServerEv 0x08106278
    int SendPacket(char* buf, int len);       // _ZN15CmmChannelProxy10SendPacketEPci 0x0810637c
    int RecvPacket();                         // _ZN15CmmChannelProxy10RecvPacketEv 0x0810645c
    bool SendCheckChannelScript();            // _ZN15CmmChannelProxy22SendCheckChannelScriptEv 0x081066ea
private:
    // ORIG 布局：+0x00 string、+0x04 int、+0x10 CUdpHandler、+0x1c 字段、
    // +0x20 buf[0x80000]（sizeof 0x80020）。
    std::string m_ip;       // +0x00
    int m_port;             // +0x04
    char m_pad8[8];         // +0x08
    CUdpHandler m_udp;      // +0x10
    char m_pad18[4];        // +0x18（ORIG 空隙，InitEv 不触碰）
    int m_field1c;          // +0x1c
    char m_buf[0x80000];    // +0x20
};

// ORIG 0x810619e：构造栈上 tagCS_UPDATE_CHANNEL_INFO（header cat=1/id=2/size=0x33）
// 并经 m_udp.SendToServer 发往频道服务器。
void CmmChannelProxy::SendChannelInfo()
{
    char pkt[0x40];
    std::memset(pkt, 0, sizeof(pkt));
    pkt[0x00] = (char)1;                              // setCategory(1)
    pkt[0x01] = (char)2;                              // setPacketID(2)
    *reinterpret_cast<unsigned short*>(pkt + 0x02) = 0x33; // setSize(0x33)

    CEnvironment* env = G_CEnvironment();
    *reinterpret_cast<int*>(pkt + 0x0b) =
        env->m_serverEnvir.m_gcNo;
    *reinterpret_cast<int*>(pkt + 0x0f) = env->get_server_group();
    *reinterpret_cast<int*>(pkt + 0x13) =
        env->m_serverEnvir.m_channelNo; // Server_Envir.m_channelNo（推断）
    *reinterpret_cast<int*>(pkt + 0x17) =
        env->m_serverEnvir.m_maxUserNum;
    *reinterpret_cast<int*>(pkt + 0x1b) = static_cast<int>(
        G_GameWorld()->m_UserInWorld.size());         // m_UserInWorld
    std::strcpy(pkt + 0x1f, env->m_serverEnvir.m_serverIP); // Server_Envir.m_serverIP
    *reinterpret_cast<int*>(pkt + 0x2f) =
        env->m_serverEnvir.m_tcpPort;

    m_udp.SendToServer(pkt,
                       *reinterpret_cast<unsigned short*>(pkt + 0x02),
                       static_cast<unsigned short>(m_port),
                       m_ip.c_str());
}


// SendCheckChannelScript 依赖的系统头/外部符号（ORIG T：OS_API::Sleep
// 0x0858c4e0、DNFFLib::Make_Dir 0x08108ad7，定义见 DomainMiscDeps.cpp）。
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
namespace OS_API { void Sleep(int ms); }
namespace DNFFLib { void Make_Dir(char* dir); }

// 布局：char category@+0、char packetID@+1、int size@+2（setSize/getSize 均
// dword 访问实证）、负载自 +0x0b；packed 保证 +2 落位。
class tagPacketHeader
{
public:
    tagPacketHeader() {}
    void setCategory(int c) { m_category = (char)c; }   // 0x08106be6：mov %dl,(%eax)
    void setPacketID(int id) { m_packetID = (char)id; } // 0x08106bbc：mov %dl,0x1(%eax)
    void setSize(int s) { m_size = s; }                 // 0x08106bd8：mov %edx,0x2(%eax)
    int getSize() const { return m_size; }              // 0x08106bcc：mov 0x2(%eax),%eax
private:
    char m_category;   // +0x00
    char m_packetID;   // +0x01
public:
    int m_size;        // +0x02
    char m_pad6[5];    // +0x06..+0x0a（头总长 0xb）
} __attribute__((packed));

struct tagCS_GET_SCRIPT   // ORIG ctor 0x08106c48：cat=2/id=9/size=0xb
{
    tagPacketHeader m_hdr;   // +0x00
    tagCS_GET_SCRIPT()
    {
        m_hdr.setCategory(2);
        m_hdr.setPacketID(9);
        m_hdr.setSize(0xb);
    }
} __attribute__((packed));   // sizeof 0xb（无负载）

struct tagCS_CHECK_SCRIPT_VERSION   // ORIG ctor 0x08106c94：cat=1/id=5/size=0x1b
{
    tagPacketHeader m_hdr;   // +0x00
    char m_version[0x10];    // +0x0b（SendCheckChannelScript 写版本串）
    tagCS_CHECK_SCRIPT_VERSION()
    {
        m_hdr.setCategory(1);
        m_hdr.setPacketID(5);
        m_hdr.setSize(0x1b);
    }
} __attribute__((packed));   // sizeof 0x1b

// ORIG ConnectBridgeServer 0x08106278：socket/connect 桥接服务器并设非阻塞。
bool CmmChannelProxy::ConnectBridgeServer()
{
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    int fd = socket(AF_INET, SOCK_STREAM, 0);          // socket(2,1,0)
    m_udp.setClientSocket(fd);                         // this+0x18 直写等价
    if (fd < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't create socket");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_ip.c_str());    // inet_addr(c_str(this))
    addr.sin_port = htons((unsigned short)G_CEnvironment()->get_tcp_port_channel());
    socklen_t addrlen = 16;
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), addrlen) < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't connect channel bridge server");
        return false;
    }
    int nonblock = 1;
    ioctl(fd, FIONBIO, &nonblock);                     // 0x5421
    return true;
}

// ORIG SendPacket 0x0810637c：write 发送；EAGAIN/EINTR/0 → 0（可重试），
// 其它错误 cMyTrace 后返回 -1。
int CmmChannelProxy::SendPacket(char* buf, int len)
{
    int fd = m_udp.clientSocket();
    ssize_t n = write(fd, buf, len);
    if (n < 1)
    {
        int e = errno;
        if (e == EAGAIN || e == EINTR || e == 0)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 200, 0);
        trace("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
              fd, e, strerror(e));
        return -1;
    }
    return static_cast<int>(n);
}

// ORIG RecvPacket 0x0810645c：select(300s) → SO_ERROR 探测（结果忽略）→
// Sleep(100ms) → read 追加到 m_buf 尾部并累加 m_field1c。
int CmmChannelProxy::RecvPacket()
{
    int fd = m_udp.clientSocket();
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    timeval tv;
    tv.tv_sec = 300;
    tv.tv_usec = 0;
    if (select(fd + 1, &readfds, 0, 0, &tv) < 1)
    {
        return 0;
    }
    socklen_t optlen = 4;
    int optval = 0;
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    OS_API::Sleep(100);
    ssize_t n = ::read(fd, m_buf + m_field1c,
                       static_cast<size_t>(0x80000 - m_field1c));
    if (n < 0)
    {
        int e = errno;
        if (e == EAGAIN || e == EINTR || e == 0)
        {
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x114, 0);
        trace("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
              fd, e, strerror(e));
        return -1;
    }
    if (n == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x11a, 0);
        trace("Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
              errno, strerror(errno), 0x80000, 0);
        return -1;
    }
    m_field1c += static_cast<int>(n);
    return static_cast<int>(n);
}

// ORIG SendCheckChannelScript 0x081066ea（0x453 字节）：桥接后校验/拉取
// channel_info.etc 并落盘。stat 失败且非 ENOENT → 0；version 文件读写两次；
// CHECK_SCRIPT_VERSION(cat1/id5) 应答尾包写入 version；GET_SCRIPT(cat2/id9)
// 应答尾包为脚本体（fwrite size-0xb 字节）。
bool CmmChannelProxy::SendCheckChannelScript()
{
    ConnectBridgeServer();

    char name[0x14];
    std::memset(name, 0, sizeof(name));
    std::strncpy(name, "channel_info", 0x14);
    struct stat st;
    if (stat(name, &st) < 0)
    {
        if (errno != ENOENT)
        {
            return false;
        }
        DNFFLib::Make_Dir(const_cast<char*>(name));
    }

    char line[0x400];
    FILE* fp = std::fopen("channel_info/version", "wb+");
    if (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x143, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    while (std::fgets(line, 0x400, fp) != 0)
    {
        // 原地去除行首空白（ORIG local_18/local_14 双指针转写）。
        char* src = line;
        char* dst = src;
        do
        {
            while (*src == ' ' || *src == '\t')
            {
                ++src;
            }
            *dst++ = *src++;
        } while (*(src - 1) != '\0');
    }
    std::fclose(fp);

    tagCS_CHECK_SCRIPT_VERSION reqVer;
    std::memcpy(reqVer.m_version, line, 0x10);
    if (SendPacket(reinterpret_cast<char*>(&reqVer),
                   reqVer.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    while (m_field1c != *reinterpret_cast<int*>(m_buf + 2))
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }

    fp = std::fopen("channel_info/version", "rb+");
    if (fp == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0x188, 9,
            true, true);
        trace("\t- [ERROR] : cannt open channel_script_version file\n");
        return false;
    }
    std::fputs(m_buf + 0xf, fp);       // ORIG fputs(local_20 + 0xf)：头 0xb + first 4
    std::fclose(fp);

    m_field1c = 0;
    std::memset(m_buf, 0, 0x80000);

    tagCS_GET_SCRIPT reqGet;
    if (SendPacket(reinterpret_cast<char*>(&reqGet),
                   reqGet.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    while (m_field1c != *reinterpret_cast<int*>(m_buf + 2))
    {
        if (RecvPacket() == 0)
        {
            return false;
        }
    }
    fp = std::fopen("channel_info/channel_info.etc", "w+");
    if (fp == 0)
    {
        return false;
    }
    int total = *reinterpret_cast<int*>(m_buf + 2);
    std::fwrite(m_buf + 0xb, 1, total - 0xb, fp);
    std::fclose(fp);
    close(m_udp.clientSocket());
    return true;
}
// SpecialItemHandler（ORIG 全局类，mangled _ZN18SpecialItemHandler…，非 WongWork 前缀；
// T 0x827bcec：new(0x74) 后构造；对象 0x74 字节）。
// 权威声明见 SpecialItemHandler.h（dispatch 表 ctor 0x0827bcec / execute 0x0827be0a），
// 实现见 CCeraShop_Special.cpp。此处仅需其完整类型以支持 new/delete。

// CLogGameChannel 权威定义见 CLogGameChannel.h（§7 合并）。

// CAutoPunishRuleBlackIPMgr / CAutoPunishRuleHackTypeMgr / Hack*IPCounter_t /
// ctor/reset 成员函数定义）。

// WongWork::CSimpleSSO 权威定义已收敛到 CSimpleSSO.h（顶部 include）；
// 成员函数 ctor/dtor/init 在本 TU，其余方法在 WongWorkSSO_A.cpp。

// WongWork::CSecurityCardCenter 权威类定义已收敛到 GlobalData.h（W6_SecMiscDeps
// 批），ctor/init/其余方法定义在 W6_SecMiscDeps.cpp。

// CIPGHelper 保留本 TU（ORIG 方法集属 GlobalData 域）；基类 IIPGHelperInterface
// 与成员 CIPGMgr 用 TaiwanCash.h 权威定义（WongWork::IPG，include 见文件头部）。

namespace IPG
{

// [dedup] CIPGHelper 权威定义见 TaiwanCash.h（Wave6 IPG 16 槽接口重建）；
// 本 TU 仅保留 out-of-line 方法定义（Initialize/Finalize 等），成员布局以权威头为准。
}
// ItemVendingMachine 权威声明 + ARAD::PurcahseBonus / CeraShopPurcahseCountBonus
// 见 ItemVendingMachine.h；此处不再重复定义（避免 ODR）。
#include "ItemVendingMachine.h"

class Secu_DataControl
{
public:
    Secu_DataControl();
    void UpdateSecurityServiceState(CUser* user, unsigned char service,
                                    unsigned char flag, char* msg);  // ORIG T 0x08286e40
private:
    char m_pad[1];
};

class CDBConnector
{
public:
    CDBConnector(DBMgr* dbMgr);
    ~CDBConnector();
    bool Init(STDBConnInfo& info);
    bool GetConnInfo(DBGroupMap& groupMap, DBInfoMap& infoMap);
    bool Connect(DBInfoMap infoMap);
    bool ConnectWithServerGroup(DBInfoMap infoMap);
    bool QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                       DBInfoMap& outMap);  // ORIG T 0x080eca6c
    bool OpenMaster();  // ORIG T 0x080ec67c（支撑）
    bool Open(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
              STDBConnInfo& info);  // ORIG T 0x080ec6da（支撑）
    void Close();      // ORIG T 0x080ec668
    void CloseMaster();  // ORIG T 0x080ec6bc
private:
    DBMgr* m_dbMgr;            // +0x00
    STDBConnInfo m_connInfo;   // +0x04（0x168 字节）
};

#include "CLevelDungeonPlayStatistic.h"

class CStatisticMgr : public CLevelDungeonPlayStatistic
{
public:
    CStatisticMgr();
private:
    // ORIG ctor 0x82aa89e：CLevelDungeonPlayStatistic@+0（0x18，map<int,
    // stDungeonPlayInfo>）+ CDungeonExpStatistic@+0x18（0x18，map<int,
    // stDungeonExp>）。基类即 CLevelDungeonPlayStatistic（首基类同址）。
    struct stDungeonExp { char m_pad[4]; };
    std::map<int, stDungeonExp> m_expMap;             // +0x18
};

class CGM_Manager;  // 完整定义见 CGM_Manager.h（真实布局）

#include "BreakAwayPreventSystem.h"   // break_away_prevent 唯一权威声明点


// CExpertJobFactory/CExpertJobMgr 权威声明已迁 CExpertJob.h（含 StaticPool 双池），
// 本 TU 仅保留其成员函数定义（下方）。
// village_attacked::{Zone,CVillageMonsterArea,CVillageMonsterMgr} 权威定义见
// CVillageMonsterMgr.h（经 GameWorld.h→CUser.h→Area.h 链引入，原本地镜像已删除，
// ODR 收敛）；ctor/dtor 由 CVillageMonsterMgr.cpp 提供。

// RentalMachinePcRoomImpl（ORIG ctor 0x8271a80：set<unsigned>@0x104 构造 +
// memset 前 0x104；总 0x11c；D1 0x8271d50 仅析构 set）。嵌套于
// RentalMachinePcRoom（mangled _ZN19RentalMachinePcRoom23RentalMachinePcRoomImpl…，
// 与 ORIG 4 符号 C1/C2/D1/D2 一致）。
class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom();
    virtual ~RentalMachinePcRoom() {}

    // 槽2 Rent：ORIG T 0x082704e6（0x258 字节 pc_room 租赁事务链，本体待
    // pc_room 域批次 —— 报告 RentalMachinePcRoom/Rent.md）。声明占位保 vtable 序。
    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0827073e = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x08270752 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    struct RentalMachinePcRoomImpl
    {
        RentalMachinePcRoomImpl() { memset(m_data, 0, sizeof(m_data)); }
        ~RentalMachinePcRoomImpl() {}   // ORIG D1 @0x08271d50：仅析构 set
        char m_data[0x104];            // +0x00（日志路径等，未还原字段区）
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
    bool checkAllRentalItemScript();   // ORIG T 0x082708cc（本批实现）
    void initLoggingEnvironment();     // ORIG T 0x8270886
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x08270d1e
    void log(CUser& user, int itemId);                                      // T 0x082707c0
    std::auto_ptr<RentalMachinePcRoomImpl> m_impl;  // +0x04（ORIG auto_ptr）
};
// RentalMachineDimensionImpl（ORIG ctor 0x826fa6c：与 PcRoom 同构，推断）。
struct RentalMachineDimensionImpl
{
    RentalMachineDimensionImpl() { memset(m_data, 0, sizeof(m_data)); }
    char m_data[0x104];            // +0x00
    std::set<unsigned int> m_set;  // +0x104
};  // 0x11c
class RentalMachineDimension : public IRentalMachine
{
public:
    RentalMachineDimension();
    virtual ~RentalMachineDimension() {}

    // 槽2 Rent：ORIG T 0x0826ec22（与 PcRoom 同构，本体待 pc_room 域批次）。
    virtual int Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0826ee94 = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x0826eea8 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    bool checkAllRentalItemScript();   // ORIG T 0x0826f022（本批实现）
    int checkSelectionListScript(
        const std::vector<std::vector<EventRewardItemInfo> >& list);         // T 0x0826f14e
    int GetRentItemInfo(CUser& user,
                        const InstanceRentalSystem::RentInElem& elem,
                        EventRewardItemInfo& out);                           // T 0x0826f842
    int GetRewardItemGiveList(int job, int growType);                        // T 0x0826f814（透传）
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x0826f52c
    bool checkRentalLevel(int level);                                        // T 0x0826f7e4
    bool checkRentalGrowJob(int growType);                                   // T 0x0826f7fc
    void log(CUser& user, int itemId);                                       // T 0x0826ef16
    void initLoggingEnvironment();                                           // ORIG T 0x826efdc
    RentalMachineDimensionImpl* m_impl;    // +0x04（ORIG auto_ptr）
};


// _DailySchedule / CDailyScheduleManager 权威定义见 CDailyScheduleManager.h
// （include 引入；本 TU 的 C1/D1 定义保留于下方实现区）。

// online_preliminary 域（CTeamFactory/COnlinePreliminaryTeamMgr）权威声明已迁
// OnlinePreliminary.h（OnlyOrig_OnlinePvp 批）；本 TU 构造/析构/OnLoadTeamInfo
// 保留于下方原实现位置，经该头取完整类型。

// game_master 命令体系完整声明见 GameMasterCmd.h（GmCore/GmRegister 等 TU 实现）。


class DB_ReloadAutoPunishRuleBackIP
{
public:
    static void makeRequest();
};

class DB_ReloadAutoPunishRuleHackType
{
public:
    static void makeRequest();
};

class DB_LoadCleanPadPoint
{
public:
    static void makeRequest();
};

class DB_BlackIPMonitor
{
public:
    static void makeRequest();
};

void* CreateAntiServerObj(const char* path);

// ORIG 0x0822ad44 W：return *(char*)GetLightServerFlag()（GetLightServerFlag 见
// CDataManagerScripts.cpp，ORIG 0x822ad3a）。
bool* GetLightServerFlag();   // extern（CDataManagerScripts.cpp 定义）
char IsLightServer()
{
    // ORIG 0x822ad44：旗标指针先落局部变量再解引用。
    bool* flag = GetLightServerFlag();
    return *flag;
}


namespace random_option
{
void* GetRandomOption();  // ORIG T 0x085f0eb0：&G_CDataManager()->+0x148

}

class CHumanCertifier
{
public:
    static void init();
    // ORIG B 0x93f5758：map<string, vector<int>> 人证类型表。
    static std::map<std::string, std::vector<int> > humanCertifyTable_;
};

class CDBEnviroment : public CEnvironment
{
public:
    CDBEnviroment();
    ~CDBEnviroment();
    bool parse(char* buf);   // ORIG T 0x0829d8e8（覆盖 CEnvironment::parse）
private:
    // ORIG 布局：CEnvironment(+0x3dc 止) 之后 +0x3e0 为 db_type 枚举（parse 写
    // 1..0xd/0x63/0x64/0x11；G_CEnvironment() 的 m_serverGroupMap 按此键归类）。
    ENUM_SERVER_GROUP m_dbType;   // +0x3e0
    char m_pad[4];                // +0x3e4（总 0x3e8）
};

// ---- Captcha ----
// ---- XorRand4（ORIG 0x08adfa90；m_counter + 4 张字节替换表）----
class XorRand4
{
public:
    XorRand4() { m_counter = 0; }
    void seed(unsigned int s) { m_counter = s; }
    unsigned int getUInt32()
    {
        // ORIG 0x8adfa90：后缀自增取旧值；再以字节指针逐字节查 4 张表异或。
        unsigned int v = m_counter++;
        const unsigned char* p = (const unsigned char*)&v;
        return ms_table0[p[0]] ^ ms_table1[p[1]] ^
               ms_table2[p[2]] ^ ms_table3[p[3]];
    }
    unsigned int operator()() { return getUInt32(); }
private:
    unsigned int m_counter;
    static const unsigned int ms_table0[256];  // VA 0x08e32cc0（定义在 XorRand.cpp）
    static const unsigned int ms_table1[256];  // VA 0x08e330c0
    static const unsigned int ms_table2[256];  // VA 0x08e334c0
    static const unsigned int ms_table3[256];  // VA 0x08e338c0
};

class GreyImage
{
public:
    GreyImage();
    ~GreyImage();
    void init(int width, int height, unsigned char fill);
    void reset();
    int getWidth() const;
    int getHeight() const;
    void* getImage();
    void* getImage() const;
    unsigned char getAlpha(int x, int y);
    void setAlpha(int x, int y, unsigned char alpha);
    void setAlphaAsMax(int x, int y, unsigned char alpha);
    int sampleAlpha(int x, int y);
private:
    int m_width;          // +0x00
    int m_height;         // +0x04
    unsigned char* m_image;  // +0x08
};

// GreyImage 方法实现在 GameStubs.cpp（ORIG 中 GreyImage 独立 TU；GlobalData TU
// 仅保留声明，使 GenerateCaptchaData 的 EH 清理结构对齐 ORIG）。

// ---- drawLine（ORIG 0x080dea17，文件内静态）----
static void drawLine(GreyImage* img, int x1, int y1, int x2, int y2)
{
    if (x1 >= x2)
    {
        return;
    }
    int x = x1;
    int y = y1;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int err = dx / 2;
    int step = 1;
    if (y2 < y1)
    {
        step = -1;
        dy = -dy;
    }
    while (x < x2)
    {
        if (x != x1 || y != y1)
        {
            img->setAlphaAsMax(x, y, 0xff);
        }
        ++x;
        err -= dy;
        if (err < 0)
        {
            y += step;
            err += dx;
        }
    }
}

// ---- 字形渲染：A-Z 用内置 8x8 点阵；韩/日等码点用确定性伪字形。
//      ORIG 走 FreeType 栅格化；本环境无 FreeType/字体文件，算法等价占位。----
static void renderGlyph(unsigned short code, unsigned char* out, int& gw, int& gh)
{
    gw = 8;
    gh = 8;
    static const unsigned char font8x8[26][8] = {
        {0x18,0x3c,0x66,0x7e,0x66,0x66,0x66,0x00},  // A
        {0x7c,0x66,0x66,0x7c,0x66,0x66,0x7c,0x00},  // B
        {0x3c,0x66,0x60,0x60,0x60,0x66,0x3c,0x00},  // C
        {0x78,0x6c,0x66,0x66,0x66,0x6c,0x78,0x00},  // D
        {0x7e,0x60,0x60,0x7c,0x60,0x60,0x7e,0x00},  // E
        {0x7e,0x60,0x60,0x7c,0x60,0x60,0x60,0x00},  // F
        {0x3c,0x66,0x60,0x6e,0x66,0x66,0x3e,0x00},  // G
        {0x66,0x66,0x66,0x7e,0x66,0x66,0x66,0x00},  // H
        {0x3c,0x18,0x18,0x18,0x18,0x18,0x3c,0x00},  // I
        {0x1e,0x0c,0x0c,0x0c,0x0c,0x6c,0x38,0x00},  // J
        {0x66,0x6c,0x78,0x70,0x78,0x6c,0x66,0x00},  // K
        {0x60,0x60,0x60,0x60,0x60,0x60,0x7e,0x00},  // L
        {0x63,0x77,0x7f,0x6b,0x63,0x63,0x63,0x00},  // M
        {0x66,0x76,0x7e,0x7e,0x6e,0x66,0x66,0x00},  // N
        {0x3c,0x66,0x66,0x66,0x66,0x66,0x3c,0x00},  // O
        {0x7c,0x66,0x66,0x7c,0x60,0x60,0x60,0x00},  // P
        {0x3c,0x66,0x66,0x66,0x66,0x6c,0x36,0x00},  // Q
        {0x7c,0x66,0x66,0x7c,0x78,0x6c,0x66,0x00},  // R
        {0x3c,0x66,0x60,0x3c,0x06,0x66,0x3c,0x00},  // S
        {0x7e,0x18,0x18,0x18,0x18,0x18,0x18,0x00},  // T
        {0x66,0x66,0x66,0x66,0x66,0x66,0x3c,0x00},  // U
        {0x66,0x66,0x66,0x66,0x66,0x3c,0x18,0x00},  // V
        {0x63,0x63,0x63,0x6b,0x7f,0x77,0x63,0x00},  // W
        {0x66,0x66,0x3c,0x18,0x3c,0x66,0x66,0x00},  // X
        {0x66,0x66,0x66,0x3c,0x18,0x18,0x18,0x00},  // Y
        {0x7e,0x06,0x0c,0x18,0x30,0x60,0x7e,0x00}   // Z
    };
    if (code >= 'A' && code <= 'Z')
    {
        const unsigned char* g = font8x8[code - 'A'];
        for (int r = 0; r < 8; ++r)
        {
            for (int c = 0; c < 8; ++c)
            {
                out[r * 8 + c] = (g[r] & (1 << (7 - c))) ? 0xff : 0;
            }
        }
        return;
    }
    unsigned int seed = code * 2654435761u;
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            seed = seed * 1664525u + 1013904223u;
            out[r * 8 + c] = ((seed >> 16) & 1) ? 0xff : 0;
        }
    }
}

class CaptchaGen
{
public:
    struct ImageOption
    {
        ImageOption();
        int m_field0;    // +0x00 噪点圆数量
        int m_field4;    // +0x04 噪点圆最大半径
        int m_field8;    // +0x08 干扰线数量
        float m_fieldc;  // +0x0c 旋角下限
        float m_field10; // +0x10 旋角上限
        float m_field14; // +0x14 缩放下限
        float m_field18; // +0x18 缩放上限
        int m_field1c;   // +0x1c 字符纵向偏移范围
        float m_field20; // +0x20 字符推进系数
        float m_field24; // +0x24 波形频率下限
        float m_field28; // +0x28 波形频率上限
        float m_field2c; // +0x2c 波形幅度下限
        float m_field30; // +0x30 波形幅度上限
    };

    CaptchaGen();
    ~CaptchaGen();
    bool addFont(const char* font, int size);
    void generateText(CountryCode code, int len, unsigned short* out);
    void generateImage(int width, int height, const unsigned short* text, int len,
                       const ImageOption& opt, GreyImage* image);
    // ---- generateText 字符集（ORIG 原表；类静态成员，定义见类后）----
    static const unsigned short kKoreanGeneral[978];   // VA 0x08b2bca0
    static const unsigned short kJapaneseHirakana[72]; // VA 0x08b2c460
    static const unsigned short kTaiwanAlphabet[26];   // VA 0x08b2c500
    static const unsigned int kKoreanGeneralCount;     // VA 0x08b2c444（978）
    static const unsigned int kTaiwanAlphabetCount;    // VA 0x08b2c534（26）
    static const unsigned int kJapaneseHirakanaCount;  // VA 0x08b2c4f0（72）
private:
    static float getFloat(XorRand4& rng, float lo, float hi);
    void* m_library;             // +0x00（ORIG FT_Library；本还原中为句柄占位）
    std::vector<void*> m_faces;  // +0x04
    XorRand4 m_rng;              // +0x10
};

// ---- CaptchaGen 字符集（ORIG .rodata；类静态成员定义，见 CaptchaGen.h）----
const unsigned short CaptchaGen::kKoreanGeneral[978] = {
    0xac00, 0xac01, 0xac04, 0xac07, 0xac08, 0xac10, 0xac11, 0xac12, 0xac13, 0xac15, 0xac16, 0xac19,
    0xac1a, 0xac1b, 0xac1c, 0xac1d, 0xac40, 0xac54, 0xac70, 0xac71, 0xac74, 0xac77, 0xac78, 0xac80,
    0xac81, 0xac83, 0xac89, 0xac8c, 0xaca8, 0xaca9, 0xacaa, 0xacac, 0xacb0, 0xacb9, 0xacbd, 0xacc1,
    0xacc4, 0xace0, 0xace1, 0xace4, 0xace7, 0xace8, 0xacf0, 0xacf1, 0xacf3, 0xacf5, 0xacfc, 0xad00,
    0xad11, 0xad1c, 0xad34, 0xad49, 0xad50, 0xad6c, 0xad6d, 0xad70, 0xad73, 0xad74, 0xad75, 0xad76,
    0xad7d, 0xad81, 0xad8c, 0xadc0, 0xadd3, 0xaddc, 0xade0, 0xade4, 0xadf8, 0xadf9, 0xadfc, 0xae00,
    0xae01, 0xae08, 0xae09, 0xae0b, 0xae0d, 0xae30, 0xae34, 0xae38, 0xae40, 0xae45, 0xae4a, 0xae4c,
    0xae4d, 0xae4e, 0xae50, 0xae54, 0xae5c, 0xae5d, 0xae61, 0xae65, 0xae68, 0xaebc, 0xaebe, 0xaecc,
    0xaecd, 0xaecf, 0xaed1, 0xaed8, 0xaef4, 0xaf2c, 0xaf2d, 0xaf34, 0xaf3c, 0xaf3d, 0xaf42, 0xaf43,
    0xaf49, 0xaf64, 0xafb8, 0xafbc, 0xafc0, 0xafc8, 0xb00c, 0xb044, 0xb048, 0xb04a, 0xb04c, 0xb053,
    0xb054, 0xb057, 0xb05d, 0xb07c, 0xb08c, 0xb098, 0xb099, 0xb09a, 0xb09c, 0xb0a0, 0xb0a1, 0xb0a8,
    0xb0a9, 0xb0ab, 0xb0ad, 0xb0ae, 0xb0af, 0xb0b1, 0xb0b3, 0xb0b4, 0xb0bc, 0xb0c4, 0xb0c7, 0xb0c9,
    0xb0d0, 0xb0e5, 0xb108, 0xb109, 0xb10c, 0xb110, 0xb113, 0xb118, 0xb123, 0xb124, 0xb125, 0xb137,
    0xb140, 0xb141, 0xb144, 0xb150, 0xb155, 0xb178, 0xb179, 0xb17c, 0xb180, 0xb188, 0xb18d, 0xb192,
    0xb193, 0xb194, 0xb1cc, 0xb1e8, 0xb204, 0xb208, 0xb215, 0xb258, 0xb274, 0xb284, 0xb290, 0xb291,
    0xb294, 0xb298, 0xb299, 0xb2a5, 0xb2a6, 0xb2ac, 0xb2c8, 0xb2d0, 0xb2d8, 0xb2e4, 0xb2e5, 0xb2e6,
    0xb2e8, 0xb2eb, 0xb2ec, 0xb2ed, 0xb2ee, 0xb2f4, 0xb2f5, 0xb2f7, 0xb2f9, 0xb2ff, 0xb300, 0xb301,
    0xb310, 0xb313, 0xb354, 0xb355, 0xb358, 0xb35c, 0xb35f, 0xb364, 0xb365, 0xb367, 0xb369, 0xb36e,
    0xb370, 0xb378, 0xb3c4, 0xb3c5, 0xb3c8, 0xb3cc, 0xb3d5, 0xb3d7, 0xb3d9, 0xb3fc, 0xb418, 0xb41c,
    0xb450, 0xb451, 0xb458, 0xb460, 0xb461, 0xb465, 0xb4a4, 0xb4b7, 0xb4dc, 0xb4dd, 0xb4e0, 0xb4e3,
    0xb4e4, 0xb4ec, 0xb4ed, 0xb4ef, 0xb4f1, 0xb514, 0xb529, 0xb52a, 0xb530, 0xb531, 0xb534, 0xb538,
    0xb540, 0xb545, 0xb54c, 0xb55c, 0xb5a0, 0xb5a1, 0xb5a4, 0xb5a8, 0xb5bb, 0xb5bc, 0xb610, 0xb611,
    0xb69c, 0xb6ab, 0xb6b1, 0xb6f0, 0xb728, 0xb729, 0xb72f, 0xb730, 0xb73b, 0xb744, 0xb77c, 0xb77d,
    0xb780, 0xb78c, 0xb78d, 0xb791, 0xb797, 0xb798, 0xb79c, 0xb7a8, 0xb7ab, 0xb7b5, 0xb7c9, 0xb7ec,
    0xb7ed, 0xb7f0, 0xb7f4, 0xb7fc, 0xb7fd, 0xb7ff, 0xb801, 0xb807, 0xb808, 0xb809, 0xb80c, 0xb824,
    0xb825, 0xb828, 0xb82c, 0xb835, 0xb839, 0xb840, 0xb85c, 0xb85d, 0xb860, 0xb86c, 0xb86d, 0xb86f,
    0xb8cc, 0xb8e8, 0xb8e9, 0xb8f9, 0xb8fb, 0xb904, 0xb958, 0xb959, 0xb960, 0xb96d, 0xb974, 0xb978,
    0xb97c, 0xb984, 0xb987, 0xb98e, 0xb9ac, 0xb9ad, 0xb9b0, 0xb9bc, 0xb9bd, 0xb9bf, 0xb9c1, 0xb9c8,
    0xb9c9, 0xb9cc, 0xb9ce, 0xb9d0, 0xb9d1, 0xb9d8, 0xb9d9, 0xb9db, 0xb9dd, 0xb9de, 0xb9e1, 0xb9e3,
    0xb9e4, 0xb9e5, 0xb9e8, 0xb9f5, 0xb9fa, 0xba38, 0xba39, 0xba3c, 0xba40, 0xba48, 0xba4b, 0xba4d,
    0xba4e, 0xba54, 0xba58, 0xba69, 0xba70, 0xba74, 0xba78, 0xba85, 0xba87, 0xbaa8, 0xbaa9, 0xbaac,
    0xbab0, 0xbab8, 0xbab9, 0xbabb, 0xbabd, 0xbb18, 0xbb34, 0xbb35, 0xbb36, 0xbb38, 0xbb3b, 0xbb3c,
    0xbb44, 0xbb47, 0xbb50, 0xbb58, 0xbb63, 0xbbc0, 0xbbf8, 0xbbfc, 0xbbff, 0xbc00, 0xbc09, 0xbc0c,
    0xbc0f, 0xbc11, 0xbc14, 0xbc15, 0xbc16, 0xbc18, 0xbc1b, 0xbc1c, 0xbc1d, 0xbc1f, 0xbc24, 0xbc25,
    0xbc29, 0xbc2d, 0xbc30, 0xbc31, 0xbc40, 0xbc43, 0xbc49, 0xbc84, 0xbc88, 0xbc8c, 0xbc94, 0xbc95,
    0xbc97, 0xbca0, 0xbca4, 0xbca8, 0xbcbc, 0xbcbd, 0xbcc0, 0xbcc4, 0xbccd, 0xbcd1, 0xbcd5, 0xbcf4,
    0xbcf5, 0xbcf6, 0xbcf8, 0xbcfc, 0xbd04, 0xbd07, 0xbd09, 0xbd48, 0xbd59, 0xbd80, 0xbd81, 0xbd84,
    0xbd88, 0xbd89, 0xbd90, 0xbd93, 0xbd95, 0xbd99, 0xbdf0, 0xbe0c, 0xbe10, 0xbe14, 0xbe44, 0xbe4c,
    0xbe54, 0xbe57, 0xbe5a, 0xbe5b, 0xbe60, 0xbe61, 0xbe68, 0xbe75, 0xbe7c, 0xbe8f, 0xbea8, 0xbed0,
    0xbed4, 0xbed7, 0xbf08, 0xbf09, 0xbf51, 0xbfcc, 0xbfd0, 0xc058, 0xc068, 0xc0ac, 0xc0ad, 0xc0b0,
    0xc0b4, 0xc0b6, 0xc0bc, 0xc0bf, 0xc0c1, 0xc0c8, 0xc0c9, 0xc0cc, 0xc0dd, 0xc0e4, 0xc11c, 0xc11d,
    0xc11e, 0xc120, 0xc124, 0xc12c, 0xc12d, 0xc12f, 0xc131, 0xc138, 0xc139, 0xc13c, 0xc148, 0xc14b,
    0xc154, 0xc158, 0xc168, 0xc18c, 0xc18d, 0xc190, 0xc194, 0xc19c, 0xc19f, 0xc1a1, 0xc1a5, 0xc1c4,
    0xc1e0, 0xc1fc, 0xc218, 0xc219, 0xc21c, 0xc21f, 0xc220, 0xc228, 0xc22b, 0xc22d, 0xc232, 0xc26c,
    0xc270, 0xc27d, 0xc288, 0xc2a4, 0xc2a8, 0xc2ac, 0xc2b4, 0xc2b5, 0xc2b7, 0xc2b9, 0xc2dc, 0xc2dd,
    0xc2e0, 0xc2e3, 0xc2e4, 0xc2eb, 0xc2ec, 0xc2ed, 0xc2ef, 0xc2f1, 0xc2f6, 0xc2f8, 0xc2f9, 0xc2fc,
    0xc300, 0xc30d, 0xc313, 0xc368, 0xc369, 0xc370, 0xc379, 0xc384, 0xc3d8, 0xc3df, 0xc464, 0xc4f0,
    0xc4f4, 0xc4f8, 0xc500, 0xc50c, 0xc528, 0xc529, 0xc52c, 0xc539, 0xc53b, 0xc544, 0xc545, 0xc548,
    0xc549, 0xc54a, 0xc54c, 0xc553, 0xc554, 0xc555, 0xc557, 0xc559, 0xc55e, 0xc560, 0xc561, 0xc568,
    0xc57c, 0xc57d, 0xc580, 0xc584, 0xc587, 0xc591, 0xc595, 0xc597, 0xc598, 0xc5b4, 0xc5b5, 0xc5b8,
    0xc5b9, 0xc5bb, 0xc5bc, 0xc5c4, 0xc5c5, 0xc5c6, 0xc5c7, 0xc5c9, 0xc5ca, 0xc5cc, 0xc5ce, 0xc5d0,
    0xc5d4, 0xc5d8, 0xc5ec, 0xc5ed, 0xc5f0, 0xc5f4, 0xc5f7, 0xc5fc, 0xc5fd, 0xc5ff, 0xc601, 0xc606,
    0xc608, 0xc61b, 0xc624, 0xc625, 0xc628, 0xc62c, 0xc62e, 0xc633, 0xc637, 0xc639, 0xc640, 0xc644,
    0xc655, 0xc65c, 0xc660, 0xc678, 0xc67c, 0xc694, 0xc695, 0xc6a9, 0xc6b0, 0xc6b1, 0xc6b4, 0xc6b8,
    0xc6c0, 0xc6c3, 0xc6c5, 0xc6cc, 0xc6d0, 0xc6d4, 0xc6e8, 0xc6ec, 0xc704, 0xc717, 0xc720, 0xc721,
    0xc728, 0xc73c, 0xc73d, 0xc740, 0xc744, 0xc74c, 0xc751, 0xc758, 0xc774, 0xc775, 0xc778, 0xc77c,
    0xc77d, 0xc783, 0xc784, 0xc785, 0xc787, 0xc788, 0xc78a, 0xc78e, 0xc790, 0xc791, 0xc794, 0xc796,
    0xc798, 0xc7a0, 0xc7a1, 0xc7a3, 0xc7a5, 0xc7a6, 0xc7ac, 0xc7c1, 0xc7e4, 0xc800, 0xc801, 0xc804,
    0xc808, 0xc80a, 0xc810, 0xc811, 0xc813, 0xc815, 0xc816, 0xc81c, 0xc820, 0xc82f, 0xc838, 0xc870,
    0xc871, 0xc874, 0xc878, 0xc880, 0xc881, 0xc885, 0xc88b, 0xc88c, 0xc8c4, 0xc8fc, 0xc8fd, 0xc900,
    0xc904, 0xc90c, 0xc90d, 0xc911, 0xc950, 0xc988, 0xc989, 0xc98c, 0xc990, 0xc998, 0xc99d, 0xc9c0,
    0xc9c1, 0xc9c4, 0xc9c8, 0xc9d0, 0xc9d1, 0xc9d3, 0xc9d5, 0xc9d9, 0xc9da, 0xc9dc, 0xc9dd, 0xc9e0,
    0xc9e2, 0xc9e7, 0xc9f8, 0xca0c, 0xca4c, 0xca4d, 0xca50, 0xca54, 0xca5c, 0xcabd, 0xcad3, 0xcb48,
    0xcb49, 0xcc0c, 0xcc0d, 0xcc22, 0xcc28, 0xcc29, 0xcc2c, 0xcc2e, 0xcc30, 0xcc38, 0xcc3b, 0xcc3d,
    0xcc3e, 0xcc44, 0xcc45, 0xcc54, 0xcc59, 0xcc98, 0xcc99, 0xcc9c, 0xcca0, 0xcca9, 0xccab, 0xccad,
    0xccb4, 0xccd0, 0xcd08, 0xcd09, 0xcd0c, 0xcd1b, 0xcd1d, 0xcd2c, 0xcd5c, 0xcd94, 0xcd95, 0xcd98,
    0xcd9c, 0xcda4, 0xcda5, 0xcda7, 0xcda9, 0xcde8, 0xce20, 0xce21, 0xce30, 0xce35, 0xce58, 0xce59,
    0xce5c, 0xce60, 0xce68, 0xce6b, 0xce6d, 0xce74, 0xce78, 0xce7c, 0xce84, 0xce90, 0xcea0, 0xcee4,
    0xcee8, 0xceec, 0xcef4, 0xcef5, 0xcef7, 0xcf00, 0xcf13, 0xcf1c, 0xcf54, 0xcf58, 0xcf5c, 0xcf64,
    0xcf69, 0xcf8c, 0xcfc4, 0xcfe0, 0xd034, 0xd06c, 0xd070, 0xd074, 0xd07c, 0xd0a4, 0xd0ac, 0xd0c0,
    0xd0c1, 0xd0c4, 0xd0c8, 0xd0d1, 0xd0d3, 0xd0d5, 0xd0dc, 0xd0dd, 0xd0e4, 0xd130, 0xd131, 0xd134,
    0xd138, 0xd145, 0xd14c, 0xd14d, 0xd154, 0xd15c, 0xd1a0, 0xd1a4, 0xd1a8, 0xd1b1, 0xd1b5, 0xd1f4,
    0xd22c, 0xd234, 0xd23c, 0xd241, 0xd280, 0xd29c, 0xd2b8, 0xd2b9, 0xd2bc, 0xd2bf, 0xd2c0, 0xd2c8,
    0xd2f0, 0xd2f1, 0xd300, 0xd305, 0xd30c, 0xd30e, 0xd310, 0xd314, 0xd31d, 0xd328, 0xd329, 0xd32c,
    0xd33d, 0xd37c, 0xd37d, 0xd398, 0xd39c, 0xd3b4, 0xd3b8, 0xd3bc, 0xd3c9, 0xd3d0, 0xd3ec, 0xd3ed,
    0xd3f0, 0xd45c, 0xd478, 0xd479, 0xd480, 0xd488, 0xd48d, 0xd4e8, 0xd504, 0xd50c, 0xd514, 0xd53c,
    0xd53d, 0xd544, 0xd54f, 0xd551, 0xd558, 0xd559, 0xd55c, 0xd560, 0xd568, 0xd569, 0xd56d, 0xd574,
    0xd575, 0xd578, 0xd584, 0xd587, 0xd589, 0xd5a5, 0xd5c8, 0xd5cc, 0xd5d8, 0xd5e4, 0xd5ec, 0xd600,
    0xd604, 0xd608, 0xd611, 0xd615, 0xd61c, 0xd638, 0xd639, 0xd63c, 0xd640, 0xd648, 0xd649, 0xd64d,
    0xd654, 0xd655, 0xd658, 0xd65c, 0xd669, 0xd68c, 0xd68d, 0xd69f, 0xd6a1, 0xd6a8, 0xd6c4, 0xd6c8,
    0xd6cc, 0xd6d4, 0xd6e8, 0xd718, 0xd734, 0xd749, 0xd750, 0xd751, 0xd754, 0xd758, 0xd759, 0xd761,
    0xd765, 0xd769, 0xd76c, 0xd770, 0xd788, 0xd798,
};
const unsigned short CaptchaGen::kJapaneseHirakana[72] = {
    0x3042, 0x3044, 0x3046, 0x3048, 0x304a, 0x304b, 0x304c, 0x304d, 0x304e, 0x304f, 0x3050, 0x3051,
    0x3052, 0x3053, 0x3054, 0x3055, 0x3056, 0x3057, 0x3058, 0x3059, 0x305a, 0x305b, 0x305c, 0x305d,
    0x305e, 0x305f, 0x3060, 0x3061, 0x3062, 0x3064, 0x3065, 0x3066, 0x3067, 0x3068, 0x3069, 0x306a,
    0x306b, 0x306c, 0x306d, 0x306e, 0x306f, 0x3070, 0x3071, 0x3072, 0x3073, 0x3074, 0x3075, 0x3076,
    0x3077, 0x3078, 0x3079, 0x307a, 0x307b, 0x307c, 0x307d, 0x307e, 0x307f, 0x3080, 0x3081, 0x3082,
    0x3084, 0x3086, 0x3088, 0x3089, 0x308a, 0x308b, 0x308c, 0x308d, 0x308f, 0x3090, 0x3092, 0x3093,
};
const unsigned short CaptchaGen::kTaiwanAlphabet[26] = {
    0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c,
    0x004d, 0x004e, 0x004f, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
    0x0059, 0x005a,
};
const unsigned int CaptchaGen::kKoreanGeneralCount = 978;    // VA 0x08b2c444
const unsigned int CaptchaGen::kTaiwanAlphabetCount = 26;    // VA 0x08b2c534
const unsigned int CaptchaGen::kJapaneseHirakanaCount = 72;  // VA 0x08b2c4f0

// CaptchaGen 实现（G7 验证码；ORIG 走 FreeType，本环境无 FreeType，按算法等价实现）
CaptchaGen::ImageOption::ImageOption()
{
    m_field0 = 100;
    m_field4 = 3;
    m_field8 = 2;
    m_fieldc = 0.8f;
    m_field10 = 1.3f;
    m_field14 = 0.0f;
    m_field18 = 0.5f;
    m_field1c = 8;
    m_field20 = 0.9f;
    m_field24 = 0.1f;
    m_field28 = 0.12f;
    m_field2c = 2.0f;
    m_field30 = 2.5f;
}

CaptchaGen::CaptchaGen()
{
    m_library = 0;
    m_rng.seed(Tick::nowAsInternalValue());
}

CaptchaGen::~CaptchaGen()
{
    for (size_t i = 0; i < m_faces.size(); ++i)
    {
        free(m_faces[i]);
    }
}

bool CaptchaGen::addFont(const char* font, int size)
{
    // ORIG：FT_New_Face -> FT_Set_Pixel_Sizes -> FT_Select_Charmap(UNICODE)，
    // 任一失败返回 false；成功后把 face 句柄存入 m_faces。
    // 本环境无 FreeType：校验字体文件可读，并把字体名句柄存入 m_faces。
    FILE* f = fopen(font, "rb");
    if (!f)
    {
        return false;
    }
    fclose(f);
    char* handle = (char*)malloc(strlen(font) + 1);
    if (!handle)
    {
        return false;
    }
    strcpy(handle, font);
    m_faces.push_back(handle);
    return true;
}

void CaptchaGen::generateText(CountryCode code, int len, unsigned short* out)
{
    const unsigned short* table = 0;
    unsigned int count = 0;
    if (code == CountryCode::korea)
    {
        table = kKoreanGeneral;
        count = sizeof(kKoreanGeneral) / sizeof(kKoreanGeneral[0]);
    }
    else if (code == CountryCode::japan)
    {
        table = kJapaneseHirakana;
        count = sizeof(kJapaneseHirakana) / sizeof(kJapaneseHirakana[0]);
    }
    else if (code == CountryCode::taiwan)
    {
        table = kTaiwanAlphabet;
        count = sizeof(kTaiwanAlphabet) / sizeof(kTaiwanAlphabet[0]);
    }
    else
    {
        return;
    }
    for (int i = 0; i < len; ++i)
    {
        out[i] = table[m_rng() % count];
    }
}

float CaptchaGen::getFloat(XorRand4& rng, float lo, float hi)
{
    unsigned int v = rng() % 0xffffu;
    return (float)v * (1.0f / 65536.0f) * (hi - lo) + lo;
}

void CaptchaGen::generateImage(int width, int height, const unsigned short* text, int len,
                               const ImageOption& opt, GreyImage* image)
{
    if (m_faces.empty())
    {
        return;
    }
    image->init(width, height, 0);

    // 噪点圆（ORIG 第一段循环）
    for (int i = 0; i < opt.m_field0; ++i)
    {
        int cx = m_rng() % (width - 10) + 5;
        int cy = m_rng() % (height - 10) + 5;
        int r = m_rng() % opt.m_field4 + 1;
        for (int dy = -r; dy <= r; ++dy)
        {
            for (int dx = -r; dx <= r; ++dx)
            {
                int v = ((dx * dx + dy * dy) * 0xff) / (r * r);
                if (v < 0xff)
                {
                    image->setAlphaAsMax(cx + dx, cy + dy,
                                         (unsigned char)((0xff - v) / r));
                }
            }
        }
    }

    // 干扰线（ORIG 第二段循环）
    for (int i = 0; i < opt.m_field8; ++i)
    {
        int x1 = m_rng() % 10 + 1;
        int y1 = m_rng() % height;
        int x2 = (width - m_rng() % 10) - 2;
        int y2 = m_rng() % height;
        drawLine(image, x1, y1, x2, y2);
    }

    // 字符渲染（ORIG 第三段循环：随机面/旋转/缩放 -> FT 栅格 -> 粘贴）
    int penX = 10;
    int penY = 2;
    for (int i = 0; i < len; ++i)
    {
        unsigned int faceIdx = m_rng() % m_faces.size();
        (void)faceIdx;
        (void)getFloat(m_rng, opt.m_fieldc, opt.m_field10);   // 旋角
        (void)getFloat(m_rng, opt.m_fieldc, opt.m_field10);
        (void)getFloat(m_rng, opt.m_field14, opt.m_field18);  // 缩放
        (void)getFloat(m_rng, opt.m_field14, opt.m_field18);
        (void)(m_rng() & 2);                                   // 镜像位
        (void)(m_rng() & 2);

        unsigned char glyph[8 * 8];
        int gw = 8;
        int gh = 8;
        renderGlyph(text[i], glyph, gw, gh);

        int yOff = m_rng() % opt.m_field1c;
        int cols = width - penX;
        if (cols > gw)
        {
            cols = gw;
        }
        for (int row = 0; row < gh; ++row)
        {
            int yy = penY + row + yOff;
            if (yy < 0 || yy >= height)
            {
                continue;
            }
            for (int c = 0; c < cols; ++c)
            {
                if (glyph[row * gw + c] != 0)
                {
                    image->setAlphaAsMax(penX + c, yy, glyph[row * gw + c]);
                }
            }
        }
        penX += (int)(opt.m_field20 * (float)gw);
    }

    // 波形扭曲（ORIG 第四段：cos/sin 16.16 定点偏移 + 双线性采样）
    GreyImage tmp;
    tmp.init(width, height, 0);
    float freq = getFloat(m_rng, opt.m_field24, opt.m_field28);
    float amp = getFloat(m_rng, opt.m_field2c, opt.m_field30);
    std::vector<int> cosTbl(width);
    std::vector<int> sinTbl(height);
    for (int x = 0; x < width; ++x)
    {
        cosTbl[x] = (int)(65536.0f * cosf((float)x * freq) * amp);
    }
    for (int y = 0; y < height; ++y)
    {
        sinTbl[y] = (int)(65536.0f * sinf((float)y * freq) * amp);
    }
    for (int x = 0; x < width; ++x)
    {
        for (int y = 0; y < height; ++y)
        {
            tmp.setAlpha(x, y, (unsigned char)image->sampleAlpha(
                x * 65536 + sinTbl[y], y * 65536 + cosTbl[x]));
        }
    }
    memcpy(image->getImage(), tmp.getImage(), (size_t)width * height);
}

// s_capchaDataManager（ORIG 0x941f7a0；namespace GlobalData 成员）定义；
// 类型/方法见 CCapchaDataManager.h。
WongWork::CCapchaDataManager GlobalData::s_capchaDataManager;
void WongWork::CCapchaDataManager::push(const MSG_UDP_CAPCHA_CERTIFY& msg)
{
    // ORIG 0x08299f3a：push_back；超过 0x28 条则 pop_front
    m_queue.push_back(msg);
    if (m_queue.size() > 0x28)
    {
        m_queue.pop_front();
    }
}

bool WongWork::CCapchaDataManager::isEmpty()
{
    // ORIG W 0x086977fa：deque::empty。
    return m_queue.empty();
}

MSG_UDP_CAPCHA_CERTIFY* WongWork::CCapchaDataManager::peekData()
{
    // ORIG T 0x08299f74：at(get_rand_int(size()))——随机取一条。
    return &m_queue.at(static_cast<unsigned int>(
        get_rand_int(static_cast<int>(m_queue.size()))));
}

// ============================================================================
// 依赖类 stub 实现（GlobalData.cpp 自含声明的占位类；G2/G4/G6/G7/G8 批次接管）
// ============================================================================

DBThread::DBThread() : m_bLogDB(false)
{
    // ORIG 0x083ff5f2：先置 +0x0c=false，再隐式构造 CQueryCounter@+0x10。
}
void DBThread::setLogDB(bool logDB)
{
    // ORIG 0x083ff6c8：置标志后经成员回读；循环内经 isLogDB() 判定。
    m_bLogDB = logDB;
    m_queryCounter.setLogDB(m_bLogDB);
    for (int i = 0; i <= 0x10; ++i)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)i,
                                                      (ENUM_SERVER_GROUP)0);
        if (isLogDB())
        {
            if (i == 4)
            {
                db->SetQueryCounter(&m_queryCounter);
            }
        }
        else if (i != 4)
        {
            db->SetQueryCounter(&m_queryCounter);
        }
    }
}
bool DBThread::isLogDB()
{
    return m_bLogDB;
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x083ff786：new DBTask（0xd54）后进入 DB 事件循环；
// HandleEvents 处理 DB_Q/LOGDB_Q（isLogDB 决定 2/4），CMessageCounter(20000)
// 按 600s 周期 WriteLog；循环条件 !GlobalData::s_server_stop。
// ---------------------------------------------------------------------------
void DBThread::dispatch(void* arg)
{
    (void)arg;
    DBTask* task = new DBTask;
    if (task == 0)
    {
        std::cerr << "[DBThread::dispatch] Memory allocation fail!" << std::endl;
        OS_API::Exit(0);
    }

    int queueIdx = 2;
    if (isLogDB())
    {
        queueIdx = 4;
    }

    message_counter::CMessageCounter msgCounter(20000);
    int curSec = 0;
    int lastWriteSec = 0;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    int signal = 0;

    while (!GlobalData::s_server_stop)
    {
        curSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        signal = 0;
        if (!task->HandleEvents((MsgQueueMgr::QUEUE_IDX)queueIdx, signal))
        {
            break;
        }
        if (signal != 0)
        {
            msgCounter.IncreaseMessage(
                (unsigned int)signal, (unsigned char)0, 0,
                interval->GetTimeInterval());
            if (curSec - lastWriteSec > 600)
            {
                lastWriteSec = curSec;
                msgCounter.WriteLog();
            }
        }
    }
    message_counter::CTimeInterval::Destory(interval);
    delete task;
}
DispatchThread::DispatchThread() {}  // 基类 ThreadInterface 隐式构造（ORIG 0x847e268）
NetworkThread::NetworkThread() {}    // ORIG 0x857d554
UdpThread::UdpThread() {}            // ORIG 0x857c354
// g_bUdp_TH_start —— ORIG BSS 0x943e240；dispatch 置 1 标记 UDP 线程已启动。
bool g_bUdp_TH_start = false;

// ---------------------------------------------------------------------------
// CPacketCounter<1000,10240> —— ORIG 由本 TU（NetworkThread.cpp）实例化
// （C1 0x0857f4dc / D1 0x0857f55a / IncrementPacketCount 0x0857f560 /
// Reset 0x0857f92e，均为 W 弱符号；布局按 Reset/C1 反汇编偏移恢复）。
// ---------------------------------------------------------------------------
template<int Lo, int Hi>
class CPacketCounter
{
public:
    CPacketCounter(char* dir, char* name);
    ~CPacketCounter();
    void Reset();
    void IncrementPacketCount(int id);
    int m_totalCount;                 // +0x00
    int m_startTime;                  // +0x04
    unsigned int m_packetCount[0x2418];  // +0x08（id 域 0x3e8..0x27ff）
    int m_packetTime[0x2418];         // +0x9068
    char m_packetFlag[0x2418];        // +0x120c8
    int m_packetTotal[0x2418];        // +0x144e8
    char m_path[0x100];               // +0x1d548
    bool m_flagInit;                  // +0x1d648
    char m_flag2;                     // +0x1d649
};

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::CPacketCounter(char* dir, char* name)
{
    Reset();
    m_startTime = (int)time(0);
    if (dir)
    {
        sprintf(m_path, "./log/%s/%s", dir, name);
    }
    else
    {
        sprintf(m_path, "./log/%s", name);
    }
    m_flagInit = true;
}

template<int Lo, int Hi>
CPacketCounter<Lo, Hi>::~CPacketCounter() {}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::Reset()
{
    for (int i = 0; i <= 0x2417; ++i)
    {
        m_packetCount[i] = 0;
        m_packetTotal[i] = 0;
        m_packetTime[i] = 0;
        m_packetFlag[i] = 0;
    }
    m_totalCount = 0;
    m_flag2 = 0;
}

template<int Lo, int Hi>
void CPacketCounter<Lo, Hi>::IncrementPacketCount(int id)
{
    if (id > 0x27ff) return;
    if (id < 0x3e8) return;
    if (!m_flagInit && m_packetCount[id - 0x3e8] > 10) return;
    ++m_packetCount[id - 0x3e8];
}

// ---------------------------------------------------------------------------
// _bindUDPSocket —— ORIG 0x0857c3be：socket/bind/SetNonBlock，失败日志后 exit。
// ---------------------------------------------------------------------------
int UdpThread::_bindUDPSocket(unsigned short port)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr addr;
    memset(&addr, 0, 0x10);
    addr.sa_family = AF_INET;
    *(unsigned short*)&addr.sa_data[0] = htons(port);
    *(unsigned int*)&addr.sa_data[2] = htonl(0);
    if (bind(fd, &addr, 0x10) < 0)
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x84,
                              "[ERROR]UDP Thread Socket Binding ERROR");
        int err = *__errno_location();
        if (err == 0x62)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x9e,
                                  "Port %d for receiving UDP is in use\n",
                                  (int)(unsigned short)htons(port));
        }
        else if (err == 0x63)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa2,
                                  "Cannot assign requested address\n");
        }
        else if (err != 0)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa6,
                                  "Could not bind UDP receive port. Error= %d , strerror = %s\n",
                                  err, strerror(err));
        }
        exit(-1);
    }
    if (!CNetwork<4096, 450000>::SetNonBlock(fd))
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xb0,
                              "Fail to SetNonBlock UdpThread.");
        exit(-1);
    }
    return fd;
}

// ---------------------------------------------------------------------------
// _processUDPData —— ORIG 0x0857c5a4：AddUdpLog → 静态计数 → dispatchPacket；
// 未识别包按首字节分流（0 互斥数据 / 1 换装队列 / 2 忽略 / 3 拍卖停止）。
// ---------------------------------------------------------------------------
void UdpThread::_processUDPData(char* data, int len)
{
    unsigned short uVar1 = *(unsigned short*)data;
    CPacketTracer* tracer = CPacketTracerInstance();
    tracer->AddUdpLog(3, (int)uVar1);

    static CPacketCounter<1000, 10240> packet_counter(
        G_CEnvironment()->get_file_name(), "InterDispatcher");
    packet_counter.IncrementPacketCount((int)*(unsigned short*)data);

    if (!CDispatchServerPacket::dispatchPacket(data))
    {
        char cVar2 = *data;
        if (cVar2 == '\x01')
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xf8);
            CStreamGuard guard(stream, true);
            **guard << 0x9f;
            **guard << -1;
            guard->put_binary(data + 2, len);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else if (cVar2 < '\x02')
        {
            if (cVar2 == '\0')
            {
                char* local_24 = data;
                Stream* stream = GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xe8);
                CStreamGuard guard(stream, true);
                **guard << 0x3d;
                **guard << -1;
                guard->put_binary(local_24, 0x52);
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
                return;
            }
        }
        else
        {
            if (cVar2 == '\x02')
            {
                return;
            }
            if (cVar2 == '\x03')
            {
                GlobalData::s_auction_proxy->SetRunning(false);
                GlobalData::s_cera_auction_proxy->SetRunning(false);
                char* local_20 = data;
                Stream* stream = GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0x126);
                CStreamGuard guard(stream, true);
                **guard << 0x3d;
                **guard << -1;
                guard->put_binary(local_20, 0x52);
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
                return;
            }
        }
        if (*(unsigned short*)data < 1000)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x112,
                                  "ABNORMAL UDP PACKET [%d]", (int)*data);
        }
    }
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG 0x0857c9de：bind UDP 口后 select 轮询 recv，转入
// _processUDPData；循环条件 !GlobalData::s_server_stop（0x940bd64）。
// ---------------------------------------------------------------------------
void UdpThread::dispatch(void* arg)
{
    (void)arg;
    int i;
    char buf[0x1800];
    fd_set readfds;
    int maxfd;
    struct timeval timeout;
    int sockets[1];
    int recvLen;
    int nfds;

    sockets[0] = _bindUDPSocket((unsigned short)G_CEnvironment()->get_udp_port());
    g_bUdp_TH_start = true;
    while (!GlobalData::s_server_stop)
    {
        memset(buf, 0, sizeof(buf));
        FD_ZERO(&readfds);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        maxfd = 0;
        for (i = 0; i <= 0; ++i)
        {
            FD_SET(sockets[i], &readfds);
            maxfd = std::max(maxfd, sockets[i]);
        }
        maxfd += 1;
        nfds = select(maxfd, &readfds, NULL, NULL, &timeout);
        if (nfds < 0)
        {
            continue;
        }
        for (i = 0; i <= 0; ++i)
        {
            if (FD_ISSET(sockets[i], &readfds))
            {
                recvLen = recv(sockets[i], buf, sizeof(buf), 0);
                if (recvLen > 0)
                {
                    _processUDPData(buf, recvLen);
                }
            }
        }
    }
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0847e2d2：主分发循环。TimerEntry 取定时器消息 →
// TimerDispatcher::dispatch；DISPATCH_Q 取 Stream → 按流头 code 分流
// PacketDispatcher（客户端包）/ InterDispatcher（服务器互连包）；池归还按
// stream->m_poolType（0/1/2 = 默认/小/大流池）。CMessageCounter(10000) 与
// CUserPacketCounter 600s 周期落盘。pUserPacketCounter_ 指向本栈实例。
// ---------------------------------------------------------------------------
void DispatchThread::dispatch(void* arg)
{
    (void)arg;
    TimerEntry entry;
    unsigned char typeByte = 0;      // 首字节（客户端包分流码）
    unsigned short sizeWord = 0;     // +1 起的 ushort
    message_counter::CMessageCounter msgCounter(10000);
    int lastWriteSec = 0;
    int curSec = 0;
    message_counter::CUserPacketCounter userCounter(
        G_CEnvironment()->get_file_name(), "UserPacketCounter");
    PacketDispatcher::pUserPacketCounter_ = &userCounter;
    message_counter::CTimeInterval* interval =
        message_counter::CTimeInterval::Create();
    int code = 0;   // 流头读出的分发码
    int idx = 0;    // 流头读出的用户 id

    while (!GlobalData::s_server_stop)
    {
        GlobalData::s_systemTime_.update();
        if (G_TimerQueue()->GetTimerMess(entry))
        {
            interval->SetLastTime();
            G_TimerDispatcher()->dispatch(entry);
            msgCounter.IncreaseMessage(
                0, (unsigned char)0,
                (unsigned short)((short)entry.msg + 30000),
                interval->GetTimeInterval());
        }

        Stream* stream = GlobalData::s_msgq_mgr->get(MsgQueueMgr::DISPATCH_Q);
        if (stream == 0)
        {
            OS_API::uSleep(100);
            continue;
        }
        int length = stream->length();
        int streamType = stream->m_poolType;
        *stream >> code;
        *stream >> idx;
        curSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        if (code == 0)
        {
            typeByte = (unsigned char)*stream->get_buffer();
            sizeWord = *(unsigned short*)(stream->get_buffer() + 1);
            G_PacketDispatcher()->dispatch(
                idx, stream->get_buffer(), stream->length());
        }
        else
        {
            typeByte = 0;
            sizeWord = 0;
            G_InterDispatcher()->dispatch(
                code, idx, stream->get_buffer(), stream->length());
        }
        msgCounter.IncreaseMessage(
            (unsigned int)code, typeByte, sizeWord,
            interval->GetTimeInterval());
        if (curSec - lastWriteSec > 600)
        {
            lastWriteSec = curSec;
            msgCounter.WriteLog();
            userCounter.WriteLog();
            cMyTrace trace(__PRETTY_FUNCTION__, 0x8f, 5);
            trace("timer_queue_count(%u)", G_TimerQueue()->GetQueueSize());
            G_TimerQueue()->write_timer_queue_log();
        }
        switch (streamType)
        {
        case 1:
            if (0xfff < length)
            {
                cMyTrace t(__PRETTY_FUNCTION__, 0x9e, 5);
                t("Small Stream Size over code(%d), idx(%d), size(%u)",
                  code, idx, length);
            }
            GlobalData::s_small_stream_pool->Free(stream);
            break;
        case 2:
            if (0x3ffff < length)
            {
                cMyTrace t(__PRETTY_FUNCTION__, 0xb2, 5);
                t("Big Stream Size over code(%d), idx(%d), size(%u)",
                  code, idx, length);
            }
            GlobalData::s_big_stream_pool->Free(stream);
            break;
        default:
            if (0x3fff < length)
            {
                cMyTrace t(__PRETTY_FUNCTION__, 0xa8, 5);
                t("Default Stream Size over code(%d), idx(%d), size(%u)",
                  code, idx, length);
            }
            GlobalData::s_stream_pool->Free(stream);
            break;
        }
    }
    message_counter::CTimeInterval::Destory(interval);
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857d5be：网络收包线程。消费 AcceptThread 入队的
// s_accept_que（加锁逐个 SetEpoll），随后 epoll_wait(600, 100ms) 处理
// EPOLLIN(1)/EPOLLOUT(4)/错误掩码(0x18)；sendSize>0x6ddd0(450000) 判定
// 发送缓冲过大 → DisConnSig(6)。
// ---------------------------------------------------------------------------
void NetworkThread::dispatch(void* arg)
{
    (void)arg;
    EpollHandler* epoll = G_EpollHandler();
    epoll_event* events = epoll->GetEpollEvents();
    int epollfd = G_EpollHandler()->GetEpollFD();

    while (true)
    {
        if (GlobalData::s_server_stop)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2fe, 0);
            trace("RecvThread Terminate");
            return;
        }
        *__errno_location() = 0;
        OS_API::Sleep(10);

        if (!GlobalData::s_accept_que.empty())
        {
            GlobalData::s_accept_lock.lock();
            while (!GlobalData::s_accept_que.empty())
            {
                CUser* user = GlobalData::s_accept_que.front();
                int err = G_EpollHandler()->SetEpoll(
                    user, user->m_network.get_socket(), false);
                if (err != 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x2c4,
                        "(m_id: %s) G_EpollHandler()->SetEpoll(pUser->get_socket(%d)) %d(%s)",
                        NumberToString(user->get_acc_id(), 0),
                        user->m_network.get_socket(), err, strerror(err));
                }
                GlobalData::s_accept_que.pop();
            }
            GlobalData::s_accept_lock.unlock();
        }

        int nfds = epoll_wait(epollfd, events, 600, 100);
        if (nfds == 0)
        {
            continue;
        }
        if (nfds < 0 && errno != EINTR && errno != 0)
        {
            return;
        }
        for (int i = 0; i < nfds; ++i)
        {
            CUser* user = (CUser*)events[i].data.ptr;
            if (user == 0)
            {
                continue;
            }
            if (events[i].events & 1)
            {
                user->Recv();
            }
            if (user->m_network.m_sendSize != 0 && (events[i].events & 4))
            {
                if (user->m_network.m_sendSize <= 0x6ddd0)
                {
                    user->m_network.send_packet();
                }
                else
                {
                    user->DisConnSig((DISCONN_SIG)6, false, 0);
                }
            }
            if (events[i].events & 0x18)
            {
                user->DisConnSig((DISCONN_SIG)9, false, 0);
            }
        }
    }
}
MonitorTcpThread::MonitorTcpThread() {}  // ORIG 0x857d8f0
AcceptThread::AcceptThread()
{
    // ORIG 0x0857cbe8：置 +0x0c 标志为 false。
    m_acceptFlag = false;
}
// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857d95a：监控代理线程。monitor/guild 代理数组 +
// pcroom/community/auction/cera/hades 五个代理 socket 构建 fd_set，
// select(1s) 轮询 OnTcpRecv/OnTcpSend；断线按 60s 周期重连（auction 系
// InitTcp(true) + 虚 Disconnect + SendRegistPacket；community 仅整合 PvP
// 频道时处理）。
// ---------------------------------------------------------------------------
void MonitorTcpThread::dispatch(void* arg)
{
    (void)arg;
    fd_set readfds;
    fd_set writefds;
    socklen_t soLen[4];
    struct timeval timeout;
    int soError = 0;
    int ret = 0;
    int lastReconnectPcroom = 0;
    int lastReconnectCommunity = 0;
    int lastReconnectAuction = 0;
    int lastReconnectCera = 0;
    int pcroomSocket = 0;
    int communitySocket = 0;
    int auctionSocket = 0;
    int ceraSocket = 0;
    int hadesSocket = 0;
    int proxyCount = 0;

    while (true)
    {
        do
        {
            if (GlobalData::s_server_stop)
            {
                return;
            }
            OS_API::Sleep(1);
            pcroomSocket =
                GlobalData::s_pcroom_proxy->GetTcpHandler()->get_socket();
            communitySocket =
                GlobalData::s_community_proxy->GetTcpHandler()->get_socket();
            auctionSocket =
                GlobalData::s_auction_proxy->GetTcpHandler()->get_socket();
            ceraSocket =
                GlobalData::s_cera_auction_proxy->GetTcpHandler()->get_socket();
            hadesSocket =
                GlobalData::s_hades_proxy->GetTcpHandler()->get_socket();

            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            int maxfd = 0;
            proxyCount =
                GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize();
            for (int i = 0; i < proxyCount; ++i)
            {
                CMonitorServerProxy* mon =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i);
                int s = mon->GetTcpHandler()->get_socket();
                if (s != -1)
                {
                    FD_SET(s, &readfds);
                    FD_SET(s, &writefds);
                    maxfd = std::max(maxfd, s);
                }
                CGuildServerProxy* guild =
                    GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i);
                s = guild->GetTcpHandler()->get_socket();
                if (s != -1)
                {
                    FD_SET(s, &readfds);
                    FD_SET(s, &writefds);
                    maxfd = std::max(maxfd, s);
                }
            }
            if (pcroomSocket != -1)
            {
                FD_SET(pcroomSocket, &readfds);
                FD_SET(pcroomSocket, &writefds);
                maxfd = std::max(maxfd, pcroomSocket);
            }
            if (communitySocket != -1)
            {
                FD_SET(communitySocket, &readfds);
                FD_SET(communitySocket, &writefds);
                maxfd = std::max(maxfd, communitySocket);
            }
            if (auctionSocket != -1)
            {
                FD_SET(auctionSocket, &readfds);
                FD_SET(auctionSocket, &writefds);
                maxfd = std::max(maxfd, auctionSocket);
            }
            if (ceraSocket != -1)
            {
                FD_SET(ceraSocket, &readfds);
                FD_SET(ceraSocket, &writefds);
                maxfd = std::max(maxfd, ceraSocket);
            }
            if (hadesSocket != -1)
            {
                FD_SET(hadesSocket, &readfds);
                FD_SET(hadesSocket, &writefds);
                maxfd = std::max(maxfd, hadesSocket);
            }
            maxfd += 1;
            ret = select(maxfd, &readfds, &writefds, 0, &timeout);
        } while (ret < 0);

        proxyCount =
            GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize();
        for (int i = 0; i < proxyCount; ++i)
        {
            CMonitorServerProxy* mon =
                GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i);
            int s = mon->GetTcpHandler()->get_socket();
            if (s != -1 && FD_ISSET(s, &readfds))
            {
                if (mon->OnTcpRecv() != 1)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x449,
                        "Monitor::OnTcpRecv == false!!!");
                    CStreamGuard guard(
                        GlobalData::s_stream_pool->Acquire(
                            "NetworkThread.cpp", 0x44c),
                        true);
                    **guard << 0x15f;
                    **guard << -1;
                    **guard << 0xc9;
                    GlobalData::s_msgq_mgr->put(
                        MsgQueueMgr::DISPATCH_Q, guard);
                }
            }
            if (s != -1 && FD_ISSET(s, &writefds))
            {
                int sendRet = mon->OnTcpSend();
                if (sendRet < 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x45e,
                        "Monitor::OnTcpSend == false!!!remain_size(%d)",
                        sendRet);
                    CStreamGuard guard(
                        GlobalData::s_stream_pool->Acquire(
                            "NetworkThread.cpp", 0x460),
                        true);
                    **guard << 0x15f;
                    **guard << -1;
                    **guard << 0xc9;
                    GlobalData::s_msgq_mgr->put(
                        MsgQueueMgr::DISPATCH_Q, guard);
                }
            }
            CGuildServerProxy* guild =
                GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i);
            s = guild->GetTcpHandler()->get_socket();
            if (s != -1 && FD_ISSET(s, &readfds))
            {
                if (guild->OnTcpRecv() != 1)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x46f,
                        "Guild::OnTcpRecv == false!!!");
                    CStreamGuard guard(
                        GlobalData::s_stream_pool->Acquire(
                            "NetworkThread.cpp", 0x471),
                        true);
                    **guard << 0x15f;
                    **guard << -1;
                    **guard << 0xcb;
                    GlobalData::s_msgq_mgr->put(
                        MsgQueueMgr::DISPATCH_Q, guard);
                }
            }
            if (s != -1 && FD_ISSET(s, &writefds))
            {
                int sendRet = guild->OnTcpSend();
                if (sendRet < 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x480,
                        "Guild::OnTcpSend == false!!!remain_size(%d)",
                        sendRet);
                    CStreamGuard guard(
                        GlobalData::s_stream_pool->Acquire(
                            "NetworkThread.cpp", 0x481),
                        true);
                    **guard << 0x15f;
                    **guard << -1;
                    **guard << 0xcb;
                    GlobalData::s_msgq_mgr->put(
                        MsgQueueMgr::DISPATCH_Q, guard);
                }
            }
        }

        // HADES 代理
        if (hadesSocket != -1 &&
            GlobalData::s_hades_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET(hadesSocket, &readfds))
        {
            if (!GlobalData::s_hades_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x493,
                    "HADES::OnTcpRecv == false!!!");
            }
        }
        if (hadesSocket != -1 && FD_ISSET(hadesSocket, &writefds))
        {
            int sendRet = GlobalData::s_hades_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x49d,
                    "HADES::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }

        // PCROOM 代理
        if (pcroomSocket != -1 &&
            GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET(pcroomSocket, &readfds))
        {
            if (!GlobalData::s_pcroom_proxy->OnTcpRecv())
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4e0,
                    "PCROOM::OnTcpRecv == false!!!");
            }
        }
        if (pcroomSocket != -1 && FD_ISSET(pcroomSocket, &writefds))
        {
            if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[3] = 4;
                ret = getsockopt(pcroomSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[3]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_pcroom_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
            }
            int sendRet = GlobalData::s_pcroom_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4f9,
                    "PCROOM::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }
        int pcroomState = GlobalData::s_pcroom_proxy->GetSocketState();
        if (pcroomState == ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                GlobalData::s_pcroom_proxy->InitTcp();
                GlobalData::s_pcroom_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (pcroomState == ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                GlobalData::s_pcroom_proxy->Disconnect();
            }
        }

        // COMMUNITY 代理（仅整合 PvP 频道时处理）
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            if (communitySocket != -1 &&
                GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTED &&
                FD_ISSET(communitySocket, &readfds))
            {
                if (!GlobalData::s_community_proxy->OnTcpRecv())
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x521,
                        "COMMUNITY::OnTcpRecv == false!!!");
                }
            }
            if (communitySocket != -1 &&
                FD_ISSET(communitySocket, &writefds))
            {
                if (GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTING)
                {
                    soError = 0;
                    soLen[2] = 4;
                    ret = getsockopt(communitySocket, SOL_SOCKET, SO_ERROR,
                                     &soError, &soLen[2]);
                    if (soError == 0 && ret == 0)
                    {
                        GlobalData::s_community_proxy->SetSocketState(
                            ENUM_SOCKET_STATE_CONNECTED);
                    }
                }
                int sendRet =
                    GlobalData::s_community_proxy->OnTcpSend();
                if (sendRet < 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x537,
                        "COMMUNITY::OnTcpSend == false!!!remain_size(%d)",
                        sendRet);
                }
            }
            int commState =
                GlobalData::s_community_proxy->GetSocketState();
            if (commState == ENUM_SOCKET_STATE_DISCONNECTED)
            {
                GlobalData::s_community_proxy->InitTcp();
                GlobalData::s_community_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCommunity =
                    GlobalData::s_systemTime_.getCurSec();
            }
            else if (commState == ENUM_SOCKET_STATE_CONNECTING)
            {
                if (GlobalData::s_systemTime_.getCurSec() -
                        lastReconnectCommunity >
                    0x3c)
                {
                    lastReconnectCommunity =
                        GlobalData::s_systemTime_.getCurSec();
                    GlobalData::s_community_proxy->Disconnect();
                }
            }
        }

        // AUCTION 代理
        if (auctionSocket != -1 &&
            GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET(auctionSocket, &readfds))
        {
            if (!GlobalData::s_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x734,
                    "AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (auctionSocket != -1 && FD_ISSET(auctionSocket, &writefds))
        {
            if (GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[1] = 4;
                ret = getsockopt(auctionSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[1]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_auction_proxy->SendRegistPacket();
            }
            int sendRet = GlobalData::s_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x74c,
                    "AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        int auctionState =
            GlobalData::s_auction_proxy->GetSocketState();
        if (auctionState == ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x754, 0);
                trace("AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_auction_proxy->InitTcp(true);
                GlobalData::s_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (auctionState == ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x761, 0);
                trace("AUCTION - Disconnect to Auction Server(Timeout for connect trying)");
                GlobalData::s_auction_proxy->Disconnect();
            }
        }

        // CERA AUCTION 代理
        if (ceraSocket != -1 &&
            GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET(ceraSocket, &readfds))
        {
            if (!GlobalData::s_cera_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x76c,
                    "Cera AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (ceraSocket != -1 && FD_ISSET(ceraSocket, &writefds))
        {
            if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[0] = 4;
                ret = getsockopt(ceraSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[0]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_cera_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_cera_auction_proxy->SendRegistPacket();
            }
            int sendRet =
                GlobalData::s_cera_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x784,
                    "Cera AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        int ceraState =
            GlobalData::s_cera_auction_proxy->GetSocketState();
        if (ceraState == ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x78c, 0);
                trace("Cera AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_cera_auction_proxy->InitTcp(true);
                GlobalData::s_cera_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (ceraState == ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x799, 0);
                trace("Cera AUCTION - Disconnect to Cera Auction Server(Timeout for connect trying)");
                GlobalData::s_cera_auction_proxy->Disconnect();
            }
        }
    }
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857cc5a：accept 线程。socket/bind/listen/SetNonBlock
// 后进入 select(1s) accept 循环：按队列余量（m_acceptFlag 水位）与
// 用户池用量（Size-Remain、m_maxUserNum）拒绝/放行连接；放行时
// CGameManager::createUser → InitOnAccept → s_accept_que 入队（NetworkThread
// 消费）。IsProhibitConnectState / 监控 IP / LOCK STATE 分流拒绝包。
// ---------------------------------------------------------------------------
void AcceptThread::dispatch(void* arg)
{
    (void)arg;
    int listenSocket = -1;
    int sockOpt = 60000;
    unsigned short port = (unsigned short)G_CEnvironment()->get_tcp_port();
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        LogManager::logFormat(
            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x18e,
            "[AcceptThread::dispatch] Fail to Create ListenSocket.");
    }
    else
    {
        struct sockaddr addr;
        std::memset(&addr, 0, 0x10);
        addr.sa_family = AF_INET;
        *(unsigned short*)&addr.sa_data[0] = htons(port);
        *(unsigned int*)&addr.sa_data[2] = htonl(0);
        int setRet = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR,
                                &sockOpt, 4);
        if (setRet < 0)
        {
            LogManager::logFormat(
                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x19b,
                "[Error] listen socket setsockopt error");
        }
        else
        {
            if (bind(listenSocket, &addr, 0x10) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a1,
                    "Fail to bind Listen - %s", strerror(*__errno_location()));
            }
            else if (listen(listenSocket, 0x400) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a7,
                    "Fail to Create AcceptThread.");
            }
            else if (!CNetwork<4096, 450000>::SetNonBlock(listenSocket))
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1ae,
                    "Fail to SetNonBlock AcceptThread.");
            }
            else
            {
                // ---- select/accept 循环 ----
                while (true)
                {
                    if (G_CEnvironment()->get_suspend_state())
                    {
                        continue;
                    }
                    socklen_t addrLen = 0x10;
                    fd_set readfds;
                    FD_ZERO(&readfds);
                    FD_SET(listenSocket, &readfds);
                    struct timeval timeout;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;
                    int selRet = select(listenSocket + 1, &readfds, 0, 0,
                                        &timeout);
                    if (selRet >= 0 &&
                        FD_ISSET(listenSocket, &readfds))
                    {
                        struct sockaddr_in peerAddr;
                        int conn = accept(listenSocket,
                                          (struct sockaddr*)&peerAddr,
                                          &addrLen);
                        if (conn < 0)
                        {
                            LogManager::logFormat(
                                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1d4,
                                "Accept Socket[%d] Error(%s)", conn,
                                strerror(*__errno_location()));
                        }
                        else
                        {
                            char* peerIp = inet_ntoa(peerAddr.sin_addr);
                            if (strcmp(
                                    G_CEnvironment()->get_system_monitoring_ip(),
                                    peerIp) != 0)
                            {
                                CEnvironment* env = G_CEnvironment();
                                if (env->m_serverEnvir.m_acceptLockDBQueueCount != 0)
                                {
                                    int remain = (int)GlobalData::s_msgq_mgr->remain(
                                        MsgQueueMgr::DB_Q);
                                    if (m_acceptFlag == 0 &&
                                        remain >=
                                            env->m_serverEnvir.m_acceptLockDBQueueCount)
                                    {
                                        m_acceptFlag = true;
                                    }
                                    else if (m_acceptFlag &&
                                             remain <
                                                 env->m_serverEnvir.m_acceptUnlockDBQueueCount)
                                    {
                                        m_acceptFlag = false;
                                    }
                                }
                                int used = G_CGameManager()->userPoolSize() -
                                           G_CGameManager()->userPoolRemain();
                                if (m_acceptFlag || used >= env->m_serverEnvir.m_maxUserNum)
                                {
                                    char* lockIp = inet_ntoa(peerAddr.sin_addr);
                                    if (strcmp("192.168.30.146", lockIp) != 0 &&
                                        env->m_serverEnvir.m_maxUserNum + 5 <= used)
                                    {
                                        PacketGuard guard;
                                        guard.put_header(1, 1);
                                        guard.put_byte(0);
                                        guard.put_byte(4);
                                        guard.finalize(true);
                                        write(conn, guard.get_packet(0),
                                              guard.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x211, 0);
                                        trace("[ACCEPT] LOCK STATE(Curr Session : %d)",
                                              used);
                                        continue;
                                    }
                                    if (GameserverDemonManagerInstance()
                                            ->IsProhibitConnectState())
                                    {
                                        PacketBuf pkt;
                                        pkt.put_header(1, 1);
                                        pkt.put_byte(0);
                                        pkt.put_byte(4);
                                        pkt.finalize(true);
                                        write(conn, pkt.get_packet(0), pkt.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x230, 0);
                                        trace("[ACCEPT DENY] ProhibitUserConnect Signal STATE(Curr Session : %d)",
                                              used);
                                    }
                                    else
                                    {
                                        CUser* user = G_CGameManager()->createUser();
                                        if (user == 0)
                                        {
                                            PacketGuard guard;
                                            guard.put_header(1, 1);
                                            guard.put_byte(0);
                                            guard.put_byte(4);
                                            guard.finalize(true);
                                            write(conn, guard.get_packet(0),
                                                  guard.get_len());
                                            close(conn);
                                            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 0);
                                            trace("[ACCEPT] fail create user(Curr Session : %d)",
                                                  used);
                                        }
                                        else
                                        {
                                            user->InitOnAccept(conn);
                                            user->set_public_ip_address(
                                                (int)peerAddr.sin_addr.s_addr);
                                            GlobalData::s_accept_lock.lock();
                                            GlobalData::s_accept_que.push(user);
                                            GlobalData::s_accept_lock.unlock();
                                        }
                                    }
                                }
                            }
                            else
                            {
                                close(conn);
                            }
                        }
                    }
                }
            }
        }
        close(listenSocket);
    }
}

// ---- GlobalData::Init 直接 new 的基础对象 ----
// CTimeGate ctor 现由 CTimeGate.cpp 提供（ORIG 0x8284512，继承 IObject）。
// [dedup] Arad_ServerStateManager ctor 权威定义在 Arad_ServerStateManager.cpp

// ORIG PacketPool::PacketPool() 0x0828fc76：DynamicPool(10)，容量与步长均为10，
// 检查回调清零；这是 GlobalData::Init 使用的真实默认池，而非空构造。
PacketPool::PacketPool()
{
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(10);
    m_pool->SetPoolSize(10, 10);
    m_pool->m_pCheckCallback = 0;
}

PacketPool::PacketPool(int poolSize, int allocSize)
{
    // ORIG 0x828fcfa：置 m_threadID=0；new DynamicPool(poolSize)；
    // SetPoolSize(poolSize, allocSize)；m_pCheckCallback=0。
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(poolSize);
    m_pool->SetPoolSize(poolSize, allocSize);
    m_pool->m_pCheckCallback = 0;
}
PacketPool::~PacketPool()
{
    // ORIG 0x828fd82：if(m_pool){ delete m_pool; m_pool=0; }。
    if (m_pool)
    {
        delete m_pool;
        m_pool = 0;
    }
}
pthread_t PacketPool::getThreadID() { return m_threadID; }
void PacketPool::setThreadID(unsigned long tid) { m_threadID = tid; }
PacketBuf* PacketPool::Acquire()
{
    // ORIG 0x828fdba：Acquire 失败时 logFormat（App.cpp:0x275）后返回 0；
    // 成功路径调用 packet->clear() 后返回。
    PacketBuf* packet = m_pool->Acquire();
    if (packet == 0)
    {
        LogManager::logFormat(
            1, "App.cpp", __PRETTY_FUNCTION__, 0x275,
            "[PacketPool alloc fail] curthreadid(%08x), poolthreadid(%08x), alloc_cnt(%d), size(%d)",
            pthread_self(), getThreadID(), m_pool->Remain(), m_pool->Size());
        return 0;
    }
    packet->clear();
    return packet;
}
void PacketPool::Free(PacketBuf* packet)
{
    // ORIG 0x858e2ae：无空判，直接转调 DynamicPool::Free。
    m_pool->Free(packet);
}

// ---- CServerProxyMgr<CMonitorServerProxy>/<CGuildServerProxy>/<CStatisticServerProxy> ----
// 由 CServerProxyMgr.h 真实模板在下方 new/delete 处隐式实例化
// （ORIG 弱符号 0x82b314e/0x82b3212/0x82b3296/0x82b3b8e 等：+0x10 内嵌代理 +
// 字段清零 + Init；dtor 释放动态索引/代理数组）。CStatisticServerProxy 完整
// 类型来自 CStatisticServerProxy.h（经 CServerProxyMgr.h 模板），不再显式特化。

// CServerProxyMgr 三特化的 GetServerProxyArraySize（仅 Monitor）与
// GetServerProxyArrayAt（Monitor/Guild）在 ORIG 为弱符号实例（W 0x0857f8c2 /
// 0x0857f8ce / 0x0857f8fe），重建无调用点无法隐式实例化，故在此显式实例化
// 补齐（模板语义已与反汇编核对：ArraySize 读 m_count；ArrayAt 越界返 0，
// 元素步长 = sizeof(T)，Monitor 0x30e0c / Guild 0x30e24）。GetStartIndex/
// GetEndIndex/GetNextIndex 由 CGameManager.cpp 等消费 TU 直接成员调用隐式
// 实例化（非 asm 桥，旧注释已过时）。
template int CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize();
template CMonitorServerProxy* CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArrayAt(int);
template CGuildServerProxy* CServerProxyMgr<CGuildServerProxy>::GetServerProxyArrayAt(int);


// CPCRoomServerProxy 仅在本 TU 定义（无独立源文件）。ORIG T 0x084717dc：
// CNetwork 基类构造 + string m_ip(ip)（初始化列表）；函数体写 m_tcpPort=port
// 再清 m_socketState（汇编：string 构造后 mov port→0x30dfc、movl $0→0x30e00）。
CPCRoomServerProxy::CPCRoomServerProxy(char* ip, int port)
    : m_ip(ip)
{
    m_tcpPort = port;
    m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
}

// ORIG T 0x08471874：成员自动析构（string D1 → CNetwork D1，声明反向序）。
CPCRoomServerProxy::~CPCRoomServerProxy()
{
}

// ORIG T 0x084718bc：!InitTcp() 返 0，否则返 1（P22 bool 取反形态）。
bool CPCRoomServerProxy::Init()
{
    if (!InitTcp()) return 0;
    return 1;
}

// ORIG T 0x084718e2：ip/port 有效且 m_sock<0 时 CreateConnectionSocket；
// 失败 cMyTrace 报错返 0；m_sock>=0 跳过连接，尾部汇合 return 1。
bool CPCRoomServerProxy::InitTcp()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_tcp.m_sock < 0)
    {
        register int ret;
        if ((ret = CNetwork<100000, 100000>::CreateConnectionSocket(
                 m_tcp.m_sock, m_ip.c_str(), m_tcpPort)) != 0)
        {
            cMyTrace(__FUNCTION__, 0x116, 0)
                ("CPCRoomServerProxy CreateConnectionSocket() error(%d:%d:%s)",
                 ret, errno, strerror(errno));
            return 0;
        }
    }
    return 1;
}

// ORIG T 0x084719de：连接有效（ip 非空且端口非 0）时 disconnect + 清 socketState。
void CPCRoomServerProxy::Disconnect()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return;
    }
    else
    {
        m_tcp.disconnect();
        m_socketState = ENUM_SOCKET_STATE_DISCONNECTED;
    }
}

// ORIG T 0x08471a32：socketState==CONNECTED(2) 时经 m_tcp.send_packet 下发；
// 否则 cMyTrace 报 disconnected。
void CPCRoomServerProxy::SendTcpPacket(char* data, int len)
{
    if (m_socketState != ENUM_SOCKET_STATE_CONNECTED)
    {
        cMyTrace trace(__FUNCTION__, 0x138, 0);
        trace("cannot send packet because PCRoom Server is disconnected.");
        return;
    }
    m_tcp.send_packet(data, len);
}

// ORIG T 0x08471a9a：recv_packet()>0 → Parsing 失败则 Disconnect 返 0；
// <0 → logFormat 后 Disconnect 返 0。
bool CPCRoomServerProxy::OnTcpRecv()
{
    int recvLen = m_tcp.recv_packet();
    if (recvLen > 0)
    {
        if (!Parsing(recvLen))
        {
            Disconnect();
            return 0;
        }
        return 1;
    }
    if (recvLen < 0)
    {
        LogManager::logFormat(1, "DF_PCRoomServerProxy.cpp",
                              __FUNCTION__, 0x155,
                              "PCRoom Server maybe Die!!!");
        Disconnect();
        return 0;
    }
    return 1;
}

// ORIG T 0x08471b32：m_tcp.send_packet()<0 → 报错+Disconnect 返 -1；
// 否则返 m_recvOffset（+0x18）。
int CPCRoomServerProxy::OnTcpSend()
{
    if (m_tcp.send_packet() < 0)
    {
        cMyTrace(__FUNCTION__, 0x16d, 0)
            ("PCRoom OnSend() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        Disconnect();
        return -1;
    }
    return m_tcp.m_recvOffset;
}

// ORIG T 0x08471bd0：errno!=0 → 报错返 0；否则返 1。
bool CPCRoomServerProxy::OnTcpError()
{
    if (errno != 0)
    {
        cMyTrace(__FUNCTION__, 0x17f, 0)
            ("PCRoomServer OnError() Socket[%d] Error(%s)",
             GetTcpHandler()->get_socket(), strerror(errno));
        return 0;
    }
    return 1;
}

// ORIG T 0x08471c5a：转发 Parsing_ServerPacket（返回 bool，OnTcpRecv 取反检验）。
bool CPCRoomServerProxy::Parsing(int recvLen)
{
    return m_tcp.Parsing_ServerPacket(recvLen);
}
// P119：ORIG W 弱符号（0x0857f444/0x0857f456/0x08471e08），inline 内联不发射
// 独立符号 → 本属主 TU 外联定义。GetSocketState 返回 int（ORIG mov 0x30e00,%eax）。
void CPCRoomServerProxy::SetSocketState(ENUM_SOCKET_STATE state)
{
    m_socketState = state;
}
int CPCRoomServerProxy::GetSocketState()
{
    return m_socketState;
}
CNetwork<100000, 100000>* CPCRoomServerProxy::GetTcpHandler()
{
    return &m_tcp;
}
CmmChannelProxy::CmmChannelProxy(std::string ip, int port)
    : m_ip(ip), m_port(port)
{
    // ORIG 0x081060f4：string 拷贝 + port + CUdpHandler@+0x10 构造（成员自动）。
}
bool CmmChannelProxy::Init()
{
    // ORIG 0x0810614a：CUdpHandler::InitClientSocket；成功则清 0x1c 与 0x80000 缓冲。
    int r = m_udp.InitClientSocket();
    if (r != -1)
    {
        m_field1c = 0;
        memset(m_buf, 0, 0x80000);
    }
    return r != -1;
}

// ---- WongWork ----
// CGMAccounts：ORIG isGM 0x08109346 —— 在 m_gmList 中查找 {mid, type=3}。
WongWork::CGMAccounts::CGMAccounts()
{
}
WongWork::CGMAccounts::~CGMAccounts()
{
}
bool WongWork::CGMAccounts::isGM(unsigned int mid)
{
    WongWork::CGMAccounts::stGMInfo_t key(mid, 3);
    std::list<WongWork::CGMAccounts::stGMInfo_t>::iterator it =
        std::find(m_gmList.begin(), m_gmList.end(), key);
    return it != m_gmList.end();
}
void WongWork::CGMAccounts::removeGM(unsigned int mid, unsigned int param)
{
    // ORIG 0x081093fc：空实现（返回 0）。保留签名以匹配 _ZN8WongWork11CGMAccounts8removeGMEjj。
    (void)mid;
    (void)param;
}
WongWork::CCeraShop::CCeraShop()
{
    // ORIG 0x83202b8：new SpecialItemHandler(0x74) 存 +0；+4 置 0。
    m_handler = new SpecialItemHandler(this);
    m_originalItemIdx = 0;
}
WongWork::CCeraShop::~CCeraShop()
{
    // ORIG 0x8320314：仅 delete m_handler（不置空）。
    delete m_handler;
}
WongWork::CAutoPunishRuleBlackIPMgr::CAutoPunishRuleBlackIPMgr() {}
void WongWork::CAutoPunishRuleBlackIPMgr::reset()
{
    // ORIG 0x82a5a74：m_map.clear()。
    m_map.clear();
}
WongWork::CAutoPunishRuleHackTypeMgr::CAutoPunishRuleHackTypeMgr()
{
    // ORIG 0x082a5b1a：构造 4 个容器（multimap<ENUM_HACKTYPE,HackTypeInfo_t>
    // / multimap<u16,HackIPCounter_t> / multimap<u16,HackFullIPCounter_t>
    // / map<int,stHacktypeExceptionInfo_t>），随后向 m_exceptionMap 插入 18 个
    // 异常类型条目（key, {m_cnt, m_flag}）。+0x18 的 12 字节未初始化。
    m_exceptionMap.insert(std::make_pair(0x83, stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xca, stHacktypeExceptionInfo_t(0x28, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xcd, stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xce, stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xd0, stHacktypeExceptionInfo_t(0xc8, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xd1, stHacktypeExceptionInfo_t(0x96, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xd3, stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xd7, stHacktypeExceptionInfo_t(0x32, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(0xd8, stHacktypeExceptionInfo_t(0xc8, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(0x131, stHacktypeExceptionInfo_t(0x1e, 0x0)));
    m_exceptionMap.insert(std::make_pair(0x193, stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(0x19b, stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(0x19c, stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(0x340, stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xdc, stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(0x86, stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(0xe0, stHacktypeExceptionInfo_t(0x00, 0x14)));
    m_exceptionMap.insert(std::make_pair(0xe1, stHacktypeExceptionInfo_t(0x00, 0x32)));
}
void WongWork::CAutoPunishRuleHackTypeMgr::reset()
{
    // ORIG 0x82a5a9c：m_hackTypeMap.clear(); +0x18/+0x1c/+0x20 清零。
    m_hackTypeMap.clear();
    m_totalTradeType = 0;
    m_totalTradeValue = 0;
}
// ORIG 0x82a6526：仅构造 multimap 成员（无其它标量初始化）。
WongWork::CBlackIPMonitor::CBlackIPMonitor()
{
}
void WongWork::CBlackIPMonitor::reset()
{
    // ORIG 0x82a6512：multimap::clear()。
    m_monitorMap.clear();
}
WongWork::stBlackIPMonitorEntry_t::stBlackIPMonitorEntry_t() {}
WongWork::stBlackIPMonitorEntry_t::stBlackIPMonitorEntry_t(
    ENUM_BLACKIPMONITOR_PUNISH_TYPE punishType, unsigned int idCnt,
    long startTime, long endTime)
{
    // ORIG 0x84eba2c：逐字段写 +0/+4/+8/+0xc。
    m_punishType = punishType;
    m_idCnt = idCnt;
    m_startTime = startTime;
    m_endTime = endTime;
}
void WongWork::CBlackIPMonitor::insertNewIP(
    unsigned int ip, ENUM_BLACKIPMONITOR_PUNISH_TYPE type,
    unsigned int idCnt, long startTime, long endTime)
{
    // ORIG 0x84eba54：构造 entry 并 multimap::insert(make_pair(ip, entry))。
    m_monitorMap.insert(std::make_pair(
        ip, stBlackIPMonitorEntry_t(type, idCnt, startTime, endTime)));
}
char WongWork::CBlackIPMonitor::deleteIP(char const* ipStr, unsigned int type)
{
    // ORIG 0x84ebace：ip 为空返回 0；inet_addr 转 key；遍历 [lower_bound, upper_bound)
    // 删除 entry.m_punishType == type 的项。
    if (!ipStr)
        return 0;
    unsigned int key = static_cast<unsigned int>(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator it =
        m_monitorMap.lower_bound(key);
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator end =
        m_monitorMap.upper_bound(key);
    while (it != end)
    {
        if (static_cast<unsigned int>(it->second.m_punishType) == type)
        {
            // ORIG 0x84ebace：命中即 erase 并返回 1，未命中返回 0。
            m_monitorMap.erase(it);
            return 1;
        }
        ++it;
    }
    return 0;
}
unsigned int WongWork::CBlackIPMonitor::getPunishType(char const* ipStr)
{
    // ORIG 0x84ebba6：ip 为空返回 0；遍历 [lower_bound, upper_bound) 累 OR
    // entry.m_punishType。
    if (!ipStr)
        return 0;
    unsigned int key = static_cast<unsigned int>(inet_addr(ipStr));
    unsigned int result = 0;
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(key);
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(key);
    for (; it != end; ++it)
    {
        result |= static_cast<unsigned int>(it->second.m_punishType);
    }
    return result;
}
long WongWork::CBlackIPMonitor::GetEndTime(
    char const* ipStr, ENUM_BLACKIPMONITOR_PUNISH_TYPE type)
{
    // ORIG 0x84ebc8c：ip 为空返回 0；遍历 [lower_bound, upper_bound) 返回
    // entry.m_punishType == type 的 m_endTime，否则 0。
    if (!ipStr)
        return 0;
    unsigned int key = static_cast<unsigned int>(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(key);
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(key);
    for (; it != end; ++it)
    {
        if (it->second.m_punishType == type)
        {
            return it->second.m_endTime;
        }
    }
    return 0;
}
// ORIG 0x093ac220 数据表：31 行 × 24 列 SSO 密钥串（提取自 df_game_r .data）。
#include "GlobalData_SSO_keys.inc"

WongWork::CSimpleSSO::CSimpleSSO()
{
    // ORIG 0x082a7260（81 insn）：m_keys[31][24] 数组成员自动构造
    // （行主序 31×24 CRijndael，列步长 0x47c、行步长 0x6ba0，覆盖 0xd0860），
    // 由编译器按 ORIG 同形生成（含异常清理 vtable 槽 0 虚析构）；尾部 m_mysql=0。
    m_mysql = 0;
}

WongWork::CSimpleSSO::~CSimpleSSO()
{
    // ORIG 0x082a7352：数组成员自动析构（自表尾 0xd0860 逆序步进 0x47c，
    // 共 744 个 CRijndael），由编译器生成，与 ORIG 同形。
}

void WongWork::CSimpleSSO::init(MySQL* mysql)
{
    // ORIG 0x086027fc：保存 DB 句柄；逐块（行*0x6ba0 + 列*0x47c）以
    // strKeyTable_[row*24+col] 为密钥（keyLen=0x20）、CRijndael::sm_chain0 为
    // 初始链，调用 CRijndael::Initialize(…,0x10,0x10,0,0)。每块按 ORIG 步长
    // 0x47c 访问；当前 CRijndael::Initialize 写入范围 0x18..0x43c，块内安全。
    m_mysql = mysql;
    for (int row = 0; row < 31; ++row)
    {
        for (int col = 0; col < 24; ++col)
        {
            CRijndael* r = &m_keys[row][col];
            r->Initialize(strKeyTable_[row * 24 + col], 0x20, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
        }
    }
}

// W7_BuildHarmonize 去重：CIPGHelper::{Initialize,Finalize} 唯一 owner 为
// W6_IPGDeps.cpp（ORIG 0x08100b3e/0x08100c92 完整转写，Finalize 先转发
// m_mgr->Finalize()）。此处原重复定义已移除。
WongWork::IPG::CIPGHelper::CIPGHelper()
{
    // ORIG 0x082a7190：IIPGHelperInterface 基类（vptr 自动）。
    m_mgr = 0;
}
WongWork::IPG::CIPGHelper::~CIPGHelper()
{
    // ORIG 0x082a71b6：仅设 vptr，不释放 m_mgr（非拥有指针）。
}

// ---- 全局业务对象 ----
ItemVendingMachine::ItemVendingMachine()
{
    // ORIG 0x0854c565：构造 CCeraShop@+0 + 购物积分 map@+8（成员自动构造）。
}
ItemVendingMachine::~ItemVendingMachine()
{
    // ORIG 0x0854c42a：析构 CeraShopPurcahseCountBonus@+8 + CCeraShop@+0。
    // map 成员自动析构；CCeraShop 为 stub 区（推断：无真实析构语义）。
}
int ItemVendingMachine::BuyItem(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    // ORIG 0x0854c4d0：仅委托 ProcessBuyRequestEx(this, user, msg) 后返回 1。
    ProcessBuyRequestEx(user, msg);
    return 1;
}


// ============================================================================
// CheckPurcahseBonusItem —— ORIG 0x08179ed8 (T, sz 0x30)
// _ZN18ItemVendingMachine22CheckPurcahseBonusItemEm
// 报告 ItemVendingMachine/CheckPurcahseBonusItem.md：仅取 itemIdx 低 32 位作
// key 查 m_bonus（this+0x08，ARAD::CeraShopPurcahseCountBonus）.CheckBonusItem，
// 命中→true。Report C 体即 `CheckBonusItem((int)itemIdx)`。
// ============================================================================
bool ItemVendingMachine::CheckPurcahseBonusItem(unsigned long itemIdx)
{
    return m_bonus.CheckBonusItem((int)itemIdx);
}

// ============================================================================
// ProcessPurcahseBonusItem —— ORIG 0x08179f08 (T)
// _ZN18ItemVendingMachine24ProcessPurcahseBonusItemEP5CUsermit
// 纯委托：m_bonus（ARAD::CeraShopPurcahseCountBonus，this+0x08）.
// GiveBonusItem(user, itemIdx, count, threshold)（ORIG 0x081abf44，已定义
// 于 Arad_ItemBonus.cpp）。报告 ProcessPurcahseBonusItem.md 全文转写。
// ============================================================================
void ItemVendingMachine::ProcessPurcahseBonusItem(CUser* user, unsigned long itemIdx,
                                                  int count, unsigned short threshold)
{
    m_bonus.GiveBonusItem(user, itemIdx, count, threshold);
}

// ============================================================================
// ProcessIPGUserData —— ORIG 0x0854c4f6 (T)
// _ZN18ItemVendingMachine17ProcessIPGUserDataEP5CUserPN8WongWork3IPG8SIPGDataES6_P6Stream
// 纯委托：按原参序转发 ProcessIPGUserDataJpnEx（ORIG 0x081783ea，同签名；
// JpnEx 定义属 IPG 计费应答族后续批次）。报告 ProcessIPGUserData.md 全文转写。
// ============================================================================
void ItemVendingMachine::ProcessIPGUserData(CUser* user,
                                            WongWork::IPG::SIPGData* reqData,
                                            WongWork::IPG::SIPGData* resData,
                                            Stream* stream)
{
    ProcessIPGUserDataJpnEx(user, reqData, resData, stream);
}
// DB 请求类最小声明（定义：DB_UpdateGiftCount.cpp / DB_UpdateCeraShopPurchaseCount.cpp；
// mangled 与 ORIG 一致：makeRequestEji / makeRequestEjmii）。
class DB_UpdateGiftCount
{
public:
    static void makeRequest(unsigned int accId, int count);
};
class DB_UpdateCeraShopPurchaseCount
{
public:
    static void makeRequest(unsigned int accId, unsigned long itemUid, int count, int param4);
};
class DB_SavePuUserCheck
{
public:
    static void makeRequest(unsigned int accId);   // ORIG T 0x0844c098（W8LT_DBTask.cpp 定义）
};

// ============================================================================
// IPG 计费应答族 —— LinkE2.JpnExImpl 批次（ORIG localjapan/AppendVendingMachine.cpp）。
// 覆盖：ProcessIPGUserDataJpnEx(0x081783ea) / ProcessIPG_ResultInput(0x0817866c) /
//   ProcessIPG_ResultQuery(0x081784d4) / ProcessIPG_ResultOutput(0x08178676)。
// 报告：docs/class_func_reports/ItemVendingMachine/{同名}.md，逐指令转写。
//
// SIPGData（TaiwanCash.h，0x2dc pad 权威布局）字段偏移实证（ORIG 汇编）：
//   +0x1c  应答状态 int（4=Query / 0=Input / 2=Output）
//   +0x69/+0x75/+0x81/+0x8d/+0x99/+0xa5  cera 数值项（atoi 目标，char[0xc] 步进）
//   +0xb1  返回码（atoi 目标）   +0xb5  cera point（atoi 目标）
//   +0x28/+0x3c/+0x69/+0x177  trace/log 文本槽；+0x7d/+0x177  itemName/accName
//   +0x173  结果返回码（atoi 目标）；+0x1e0/+0x1e4 计费 64 位 uid
//   （TaiwanCash.h SIPGData 属主批次未具名化，本 TU 以偏移视图访问【ORIG 实证】。）
// ============================================================================
namespace
{

inline int& ipg_field1c(WongWork::IPG::SIPGData* d) { return *(int*)((char*)d + 0x1c); }
inline char* ipg_str(WongWork::IPG::SIPGData* d, int off) { return (char*)d + off; }

const char* const kPrettyJpnEx =
    "void ItemVendingMachine::ProcessIPGUserDataJpnEx(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";
const char* const kPrettyResultQuery =
    "void ItemVendingMachine::ProcessIPG_ResultQuery(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";
const char* const kPrettyResultOutput =
    "void ItemVendingMachine::ProcessIPG_ResultOutput(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)";

}  // namespace

// 商城 bonus 事件：权威声明见 CCeraShopBonusItemEvent.h（ORIG C1 0x0810a2dc /
// getBonusRate W 0x0817a2e2；定义归 CCeraShopAddRestrict.cpp）。

// ----------------------------------------------------------------------------
// ProcessIPGUserDataJpnEx —— ORIG 0x081783ea（0xe9 字节）。
// user 判空后按 resData+0x1c 分发；未匹配状态走 LogManager 告警。
// ----------------------------------------------------------------------------
void ItemVendingMachine::ProcessIPGUserDataJpnEx(CUser* user,
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    if (user == 0)
        return;

    int status = ipg_field1c(resData);
    if (status == 4)
    {
        ProcessIPG_ResultQuery(user, reqData, resData, stream);
    }
    else if (status == 0)
    {
        ProcessIPG_ResultInput(user, reqData, resData, stream);
    }
    else if (status == 2)
    {
        ProcessIPG_ResultOutput(user, reqData, resData, stream);
    }
    else
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyJpnEx,
                              0x482, "Unhandled reply from BModule is %d", status);
    }
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultInput —— ORIG 0x0817866c（0x9 字节：push/mov/cmp 0xc(%ebp)/pop/ret，
// 判空无副作用，函数体忠实为空）。
// ----------------------------------------------------------------------------
void ItemVendingMachine::ProcessIPG_ResultInput(CUser* user,
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    (void)(user == 0);
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultQuery —— ORIG 0x081784d4（报告全文转写）。
// ret==0：六项 cera 求和 SetCera；+0xb5 SetCeraPoint；CeraPoint 日志；SendCashData(false)。
// ret!=0：SendCashData(true) 后 IPG_QUERY 日志。
// ----------------------------------------------------------------------------
void ItemVendingMachine::ProcessIPG_ResultQuery(CUser* user,
                                                WongWork::IPG::SIPGData* reqData,
                                                WongWork::IPG::SIPGData* resData,
                                                Stream* stream)
{
    if (user == 0)
        return;

    PacketGuard guard;
    int retCode = atoi(ipg_str(resData, 0xb1));
    if (retCode == 0)
    {
        int total = atoi(ipg_str(resData, 0x99)) + atoi(ipg_str(resData, 0xa5)) +
                    atoi(ipg_str(resData, 0x81)) + atoi(ipg_str(resData, 0x75)) +
                    atoi(ipg_str(resData, 0x8d)) + atoi(ipg_str(resData, 0x69));
        user->SetCera((unsigned int)total);
        unsigned int point = (unsigned int)atoi(ipg_str(resData, 0xb5));
        user->SetCeraPoint(point);
        // ORIG cUserHistoryLog@user+0x79700 ::CeraPoint(ENUM 0, total)
                user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)0,
                                     (unsigned int)total);
        user->SendCashData(false);
    }
    else
    {
        user->SendCashData(true);
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultQuery,
                              0x4dd, "IPG_QUERY %d %d", user->get_acc_id(), retCode);
    }
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultOutput —— ORIG 0x08178676（0x12a8 字节，报告全文转写）。
// ----------------------------------------------------------------------------
void ItemVendingMachine::ProcessIPG_ResultOutput(CUser* user,
                                                 WongWork::IPG::SIPGData* reqData,
                                                 WongWork::IPG::SIPGData* resData,
                                                 Stream* stream)
{
    // ORIG 函数内 static：__cxa_guard + Arad_IPGProcess(this)/Arad_MileageProcess(this)
    static Arad_IPGProcess IPGProcess(this);
    static Arad_MileageProcess mMileageProcess(this);

    stCeraShopIPGRequest_t* req = stream->GetOutBuffer<stCeraShopIPGRequest_t>();

    // ORIG 局部快照（-0x74..-0x38，全部来自 req 具名字段）
    int groupIdx = req->m_groupIdx;              // local_78
    int seq = req->m_seq;                        // local_74
    int itemCount = req->m_itemCount;            // local_70
    int featuredIdx = req->m_featuredIdx;        // local_6c
    unsigned int avatarField = req->m_avatarField;   // local_68
    unsigned int productNo = req->m_productNo;   // local_64（itemIdx 64 位读的低 32 位）
    unsigned long itemIdx = (unsigned long)req->m_itemIdx;  // local_60（LP32 ulong 4B）
    unsigned short stackCount = req->m_stackCount;   // local_5c
    unsigned char avatarType0 = req->m_avatarType0;  // local_5a
    unsigned char avatarType1 = req->m_avatarType1;  // local_59
    unsigned int ceraPrice = req->m_ceraPrice;       // local_58
    unsigned int winPoint = req->m_winPoint;         // local_54
    unsigned int goldPrice = req->m_goldPrice;       // local_50
    unsigned int curSec = req->m_curSec;             // local_4c
    char buyFlag = (char)req->m_buyFlag;             // local_45
    int characNo = req->m_characNo;                  // local_44
    unsigned short buytype = req->m_buytype;         // local_3e
    unsigned int accId = req->m_accId;               // local_3c

    std::vector<Inven_Item> items;                   // local_c0
    WongWork::CCeraShop::stCeraShopItemParam_t param;  // local_d8（ORIG ctor 0x0817a244）

    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_subCount; ++i)
    {
        int first;
        unsigned char second;
        memcpy(&first, &req->m_subEntries[i][0], 4);      // ORIG make_pair(buf+0x48+8i, ...)
        memcpy(&second, &req->m_subEntries[i][4], 1);
        param.m_pairs.push_back(std::make_pair(first, (int)second));
    }
    for (int i = 0; i < (int)(unsigned int)(unsigned char)req->m_optCount; ++i)
    {
        tagPackageItemSelection sel;
        memcpy(&sel, &req->m_optEntries[i][0], 8);        // ORIG push_back(buf+0x164+8i)
        param.m_sel.push_back(sel);
    }

    int extraInt = 0;   // local_dc：stream >> int（购买附加参数）
    *stream >> extraInt;

    int retCode = atoi(ipg_str(resData, 0x173));   // local_e0
    if ((retCode != 0) || user == 0 || user->getCurCharacR() == 0)
    {
        // 错误路（ORIG 0x8178923..0x8178a5e）
        if (user == 0 || user->getCurCharacR() == 0)
        {
            cMyTrace trace(kPrettyResultOutput, 0x59d, 5);
            trace("There's abnormal transaction that the client doesn't exist, "
                  "client would be %s(m_id: %d) and the Goodis is %d, ret_code = %d ret_msg = %s",
                  ipg_str(resData, 0x3c), atoi(ipg_str(resData, 0x28)),
                  atoi(ipg_str(resData, 0x69)), retCode, ipg_str(resData, 0x177));
        }
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)0x19, groupIdx,
                                    productNo, seq, itemCount, buyFlag);
        return;   // ORIG 0x81798d7 清理后 ret
    }

    // 成功路：retCode=0；creature 结果槽（local_e4）初值 -1；local_38 初值 -1
    retCode = 0;
    int creatureResult = -1;   // local_e4
    int groupFinal = groupIdx; // local_78 复用（case 10 可能改写为 7）

    switch (groupIdx)
    {
    case 0:
        IPGProcess.CashShopModeAvatar(user, itemIdx, productNo, avatarField,
                                      (char)req->m_avatarType0, (char)req->m_avatarType1,
                                      ipg_str(reqData, 0x7d), retCode, accId, characNo,
                                      buyFlag);
        break;
    case 1:
        IPGProcess.CashShopModeCoin(user, stackCount, accId, characNo, buyFlag);
        break;
    case 2:
    case 7:
    case 9:
        IPGProcess.CashShopModeItem(user, itemIdx, stackCount, featuredIdx, (long)curSec,
                                    items, param, retCode, accId, characNo, buyFlag);
        break;
    case 3:
        IPGProcess.CashShopModeCreature(user, itemIdx, stackCount, creatureResult, retCode,
                                        accId, characNo, buyFlag);
        break;
    case 10:
    {
        m_ceraShop._processAdditionalItem(user, itemIdx, stackCount, avatarType0,
                                          avatarType1, items, param, 0);
        groupFinal = -1;
        CItem* item = G_CDataManager()->find_item((int)itemIdx);
        if (item != 0 && item->isCerashopPackageItem())
            groupFinal = 7;
        if (groupFinal != 7)
            items.clear();

        CItemLimitEditionMgr* mgr = G_CDataManager()->getItemLimitEditionMgr();
        CItemLimitEdition* limit = mgr->getItemInfo(productNo);
        if (limit == 0)
            throw (ENUM_ERROR)0x11;   // ORIG __cxa_throw(0x11, ENUM_ERROR)

        bool isSpecial = false;
        int outA = 0, outB = 0;   // ORIG local_100[2]/local_100[1]
        if (limit->getFeaturedIdx() == 0)
        {
            if (m_ceraShop.IsSpecialItem(limit->getRelateditem(), outA, outB, isSpecial))
                isSpecial = true;
        }
        int restrictCode = 0;
        if (isSpecial)
            restrictCode = G_CDataManager()->getItemList()->GetRestrictCode(
                limit->getRelateditem());
        if (restrictCode != 0)
        {
            m_ceraShop.SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                       GlobalData::s_systemTime_.getCurSec(), 0);
        }
        // ORIG case 10 尾直接落入 default 的 Invalid Group Index 日志（0x8178f97 jmp
        // 链与跳转表目标核对为 fall-through 前置 0x8179xxx 段）【按 Ghidra case 排列】。
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultOutput,
                              0x653, "Invalid Group Index [%d]", groupFinal);
        break;
    }
    case 0xb:
    {
        int failCode = mMileageProcess.BuyMileageItem(
            user, productNo, itemIdx, stackCount, featuredIdx, (long)curSec, avatarField,
            (char)req->m_avatarType0, (char)req->m_avatarType1, extraInt, itemCount, seq,
            ipg_str(reqData, 0x7d), param);
        if (failCode != 0)
            mMileageProcess.BuyFail(user, (short)failCode, productNo, seq, itemCount);
        return;   // ORIG LAB_081798d7 清理后 ret
    }
    default:
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultOutput,
                              0x653, "Invalid Group Index [%d]", groupFinal);
        break;
    }

    if (retCode != 0)
    {
        m_ceraShop._ProcessIPGError(user, reqData, resData, (unsigned char)retCode, groupFinal,
                                    productNo, seq, itemCount, buyFlag);
        cMyTrace trace(kPrettyResultOutput, 0x65b, 0);
        trace("[%s][%d]", kPrettyResultOutput, 0x65b);
        return;   // ORIG LAB_081798d7
    }

    if (groupFinal == 7)
    {
        ARAD::Singleton<HeroMissionEvent>::Get()->setMissionValue(
            user, (HeroMissionCondition::MissionNo::T)1, 7);
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (buytype == 1)
    {
        invenW->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCeraPoint(ceraPrice);
        user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)3,
                                     ceraPrice);
    }
    else if (buytype == 0)
    {
        GlobalData::s_pIPGHelper->IPGOutputConfirm(reqData, resData);   // ORIG vptr+0x28 slot10
        invenW->use_money((int)goldPrice, (eMoneySubReason)0, true);
        user->UseWinPoint((int)winPoint, (eWPSubReason)2);
        user->UseCera(ceraPrice);
        user->addUsedCera(ceraPrice);
    }

    mMileageProcess.GiveMileage(user, (int)productNo, (int)ceraPrice);

    if (CheckPurcahseBonusItem(itemIdx))
    {
        DB_UpdateCeraShopPurchaseCount::makeRequest(user->get_acc_id(), itemIdx,
                                                    productNo, user->GetUID());
    }
    CerashopAddRestrict::Manager::UpdateBuyableRestrictItem(user, productNo, 1);

    if (featuredIdx != 0 && !user->IsRestrictedGoods(featuredIdx))
    {
        user->SetRestrictedGoods(featuredIdx);
        m_ceraShop.SaveFeaturedIdx(user, featuredIdx, (int)curSec, 0);
    }

    unsigned long bonusItemIdx = (unsigned long)-1;   // local_100[0]（ORIG 低 4B 写 0xffffffff）
    unsigned int bonusCount = 0;   // local_104

    PacketGuard response;
    response.put_header(1, 0x43);
    response.put_byte(1);
    response.put_byte((int)buyFlag);
    response.put_int(groupFinal);
    response.put_int((int)productNo);
    response.put_int(seq);
    response.put_int(itemCount);
    response.put_int(creatureResult);
    response.put_short((int)items.size());
    for (std::vector<Inven_Item>::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        response.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        response.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    response.put_short(0);

    if (buyFlag == (char)1 && accId != 0)
    {
        DB_UpdateGiftCount::makeRequest(accId, 1);
    }

    if (itemCount == seq)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.clear();
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        const CInventory* invenR = user->getCurCharacInvenR();
        invenR->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        invenR->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);

        CEventBase* bonusEvent = GlobalData::s_event_manager->GetRepeatEvent(0x11);
        if (bonusEvent->IsEventing(0))
        {
            bonusEvent = GlobalData::s_event_manager->GetRepeatEvent(0x11);
            int bonusRate = static_cast<CCeraShopBonusItemEvent*>(bonusEvent)->getBonusRate();
            if (get_rand_int(1000) <= bonusRate)
            {
                unsigned int usedCera = user->getUsedCera();
                G_CDataManager()->getItemList()->getCeraShopBonusItem((int)usedCera,
                                                                      bonusItemIdx, bonusCount);
                if ((int)bonusItemIdx != -1)
                {
                    std::vector<Inven_Item> bonusItems;
                    m_ceraShop._processAdditionalItem(user, bonusItemIdx,
                                                      (unsigned short)bonusCount, 0, 0xff,
                                                      bonusItems, param, 0);
                }
            }
        }
    }

    response.put_int(bonusItemIdx);
    response.put_int((int)bonusCount);
    response.finalize(true);
    user->Send(response);

    GlobalData::s_BreakAwaySys->updateUsedCera(user->get_acc_id(), ceraPrice);

    unsigned long long billUid =
        (unsigned long long)*(unsigned int*)((char*)resData + 0x1e0) |
        ((unsigned long long)*(unsigned int*)((char*)resData + 0x1e4) << 32);
    cUserHistoryLog* history = &user->m_historyLog;
    if (buytype == 1)
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      ipg_str(resData, 0x7d), ipg_str(resData, 0x177), -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount, ipg_str(resData, 0x7d),
                                 buyFlag, (CeraShopPaymentType::T)1, billUid);
    }
    else
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      ipg_str(reqData, 0x7d), ipg_str(reqData, 0x177), -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount, ipg_str(reqData, 0x7d),
                                 buyFlag, (CeraShopPaymentType::T)0, billUid);
    }
}

// ----------------------------------------------------------------------------
// BillingRecord —— ORIG 0x0817991e（0x58f 字节，报告 BillingRecord.md + objdump 转写）。
// 打包 0x10e 字节计费记录（SIG 结构以文件内局部缓冲按 ORIG 偏移写入）后
// put_binary/入 DB 队列；PuUser 标志异常时补 DB_SavePuUserCheck。
// ----------------------------------------------------------------------------
void ItemVendingMachine::BillingRecord(CUser* user, int a3, int a4, int a5,
                                       const char* itemName, const char* accName,
                                       int priceLabel, unsigned long long uid,
                                       char payFlag)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp",
                                                        0x730);
    CStreamGuard guard(stream, true);
    guard << (int)0xb0;              // SIG opcode
    guard << user->GetUID();

    // 0x10e 字节记录（B=基址；偏移为 ORIG 汇编实证）
    char rec[0x10e];
    memset(rec, 0, sizeof(rec));
    strcpy(rec + 0x00, accName);                                  // B+0x00（strcpy 实证）
    *(int*)(rec + 0x11) = 1;
    *(const char**)(rec + 0x15) = itemName;
    *(int*)(rec + 0x19) = 0;
    *(int*)(rec + 0x1d) = 1;
    *(int*)(rec + 0x21) = a3;
    *(int*)(rec + 0x25) = a4;
    *(unsigned int*)(rec + 0x29) = user->get_acc_id();
    *(int*)(rec + 0x2d) = user->getCurCharacNo();
    strcpy(rec + 0x31, user->get_acc_name());                     // B+0x31（char[0x19]）
    *(int*)(rec + 0x4a) = user->get_charac_job();

    // SSN 拆分 → 生日串（B+0x52 char[11]）与世纪类型（B+0x4e）
    const char* ssn = user->GetSsnString();
    char ssnPart[0xe];
    memset(ssnPart, 0, sizeof(ssnPart));
    if (ssn != 0)
        strncpy(ssnPart, ssn, 0xe);
    // ORIG 汇编：yy/mm/dd 为 char[3]（memset 3 字节保证 %s 终止符），cent 为 char[2]。
    char yy[3], mm[3], dd[3];
    memset(yy, 0, 3); memset(mm, 0, 3); memset(dd, 0, 3);
    // 汇编 strncpy 布局：yy←ssn+0(2)、mm←ssn+2(2)、dd←ssn+4(2)、cent←ssn+6(1)
    memcpy(yy, ssnPart + 0, 2);
    memcpy(mm, ssnPart + 2, 2);
    memcpy(dd, ssnPart + 4, 2);
    char centBuf[2];
    memset(centBuf, 0, 2);
    centBuf[0] = ssnPart[6];
    int cent = atoi(centBuf);
    char birth[0xb];
    memset(birth, 0, sizeof(birth));
    if (cent == 2)
    {
        sprintf(birth, "19%s-%s-%s", yy, mm, dd);
        *(int*)(rec + 0x4e) = 2;
    }
    else if (cent == 1)
    {
        sprintf(birth, "19%s-%s-%s", yy, mm, dd);
        *(int*)(rec + 0x4e) = 1;
    }
    else if (cent == 3)
    {
        sprintf(birth, "20%s-%s-%s", yy, mm, dd);
        *(int*)(rec + 0x4e) = 1;
    }
    else if (cent == 4)
    {
        sprintf(birth, "20%s-%s-%s", yy, mm, dd);
        *(int*)(rec + 0x4e) = 2;
    }

    *(int*)(rec + 0x5d) = 1;
    *(int*)(rec + 0x61) = 0;
    *(int*)(rec + 0xc9) = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    *(int*)(rec + 0xcd) = user->getCurCharacGrowType();
    *(int*)(rec + 0xd1) = user->get_charac_level();
    *(int*)(rec + 0xd5) = *(int*)(rec + 0xc9);
    *(int*)(rec + 0xd9) = 0;
    *(int*)(rec + 0xdd) = 0;
    *(int*)(rec + 0xe1) = *(int*)(rec + 0x19);
    *(int*)(rec + 0xe5) = 0;
    *(int*)(rec + 0xe9) = priceLabel;
    // strcmp(accName,"mileage item"/"cera point")——ORIG 0x8179d49 源=0x20(%ebp) 实证
    if (strcmp(accName, "mileage item") == 0)
        *(int*)(rec + 0xed) = 2;
    else if (strcmp(accName, "cera point") == 0)
        *(int*)(rec + 0xed) = 3;
    else
        *(int*)(rec + 0xed) = 1;

    char ip[0x10];
    memset(ip, 0, sizeof(ip));
    if (user->m_network.GetPeerIP2(ip, 0x10) == 0)
        memcpy(ip, "NONE", 5);
    memcpy(rec + 0xf1, ip, 0x10);
    *(unsigned int*)(rec + 0x101) = (unsigned int)(uid >> 32);
    *(unsigned int*)(rec + 0x105) = (unsigned int)uid;      // ORIG -0x150 = 0x28(%ebp)
    *(unsigned int*)(rec + 0x109) = (unsigned int)(uid >> 32);  // ORIG -0x14c
    rec[0x10d] = payFlag;                                    // ORIG -0x154

    guard.put_binary(rec, 0x10e);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);

    // PuUser 标志（ORIG xor 1 判定：GetPuUser()==1 时补存）
    if (user->GetPuUser())
        DB_SavePuUserCheck::makeRequest(user->get_acc_id());
}
// ============================================================================
// ItemVendingMachine 私有校验/串化 helper —— ORIG 成员（mangled 见
// docs/class_func_reports/ItemVendingMachine/README.md）
// ============================================================================
namespace
{
const char* const kPrettyGetItemString =
    "void ItemVendingMachine::_GetItemString(const CCeraShopGoods*, char*, size_t)";
const char* const kPrettyProcessBuy =
    "bool ItemVendingMachine::ProcessBuyRequestEx(CUser*, const MSG_BUY_CERASHOP_ITEM&)";

// WongWork::CGenUniqueNo::GenUniqueNo(char*) —— ORIG _ZN8WongWork12CGenUniqueNo11GenUniqueNoEPc。
// CGenUniqueNo.cpp 提供 C++ 定义（经 CGenUniqueNo.h），但 CInventory.h 以截断 stub 重声明
// 该类（缺 GenUniqueNo）；本 TU 经 CInventory.h 纳入该 stub 类，故以 asm 别名引用 ORIG
// 符号，避免类重定义冲突（与 CCeraShop_Special.cpp DB_SaveFeaturedGoodsIdx 先例一致）。
char* GenUniqueNo_asm(char* out) asm("_ZN8WongWork12CGenUniqueNo11GenUniqueNoEPc");
}

// _CheckEnableGiftCondition —— ORIG 0x08176722 (T)
bool ItemVendingMachine::_CheckEnableGiftCondition(const CCeraShopGoods* goods,
                                                   CUser* user, char c)
{
    if (user->IsGameMasterMode())
        return true;
    if (goods->GetSticker() == 99)
        return false;
    if (c == 1 && goods->isGiftDisallowance_())
        return false;
    return true;
}

// _CheckEnableItemCondition —— ORIG 0x0817697c (T)
bool ItemVendingMachine::_CheckEnableItemCondition(const CCeraShopGoods* goods,
                                                   const CItem*& item,
                                                   std::vector<std::pair<int, int> >&)
{
    int groupIdx = goods->GetGroupIdx();
    if (groupIdx != 1)
    {
        if (groupIdx == 11 && goods->GetSubGroupIndex() == 1)
            return true;
        item = goods->GetRelatedItem();
        if (item == 0)
            return false;
    }
    return true;
}

// _CheckEnableStateCondition —— ORIG 0x081768b0 (T)：恒真。
bool ItemVendingMachine::_CheckEnableStateCondition(const CCeraShopGoods*, CUser*)
{
    return true;
}

// _CheckEnableAvatarCondition —— ORIG 0x08176794 (T)
// ORIG 代码形态（Fix18_E 实证）：GetRelatedItem 两次调用、IsAvatarItem 前
// 无判空、out 经 stAvatarTypeSelect_t::init() 清零、四字段各自下标访问。
bool ItemVendingMachine::_CheckEnableAvatarCondition(const CCeraShopGoods* goods,
                                                     char c, unsigned char uc,
                                                     stAvatarTypeSelect_t& out)
{
    CItem* related = goods->GetRelatedItem();
    if (!related->IsAvatarItem())
        return false;
    out.init();
    related = goods->GetRelatedItem();
    CEquipItem* equip = static_cast<CEquipItem*>(related);
    void* vec = equip->getAvatarTypeSelect();
    std::vector<stAvatarTypeSelect_t>& select =
        *static_cast<std::vector<stAvatarTypeSelect_t>*>(vec);
    int idx = (signed char)c;
    if (select.empty() || idx >= (int)select.size())
        return false;
    if (uc >= 100)
        return false;
    out.m_fieldc = select[idx].m_fieldc;
    out.m_field4 = select[idx].m_field4;
    out.m_field8 = select[idx].m_field8;
    out.m_field0 = select[idx].m_field0;
    return true;
}

// _GetItemString —— ORIG 0x0817645a (T)
void ItemVendingMachine::_GetItemString(const CCeraShopGoods* goods, char* buf,
                                        unsigned int len)
{
    int itemIdx = goods->GetItemIdx();
    int stackCount = goods->GetStackCount();
    switch (itemIdx)
    {
    case 1:    OS_API::snprintf(buf, len, "Continue Coin %d", stackCount); return;
    case 0xca: OS_API::snprintf(buf, len, "Mystic Coin %d", stackCount); return;
    case 8:    OS_API::snprintf(buf, len, "Hands of Remy %d", stackCount); return;
    case 0xf:  OS_API::snprintf(buf, len, "Kaleido Box %d", stackCount); return;
    case 0x43d:OS_API::snprintf(buf, len, "Manaade %d", stackCount); return;
    case 0xe:  OS_API::snprintf(buf, len, "Golden Wax Candle %d", stackCount); return;
    case 0xd8: OS_API::snprintf(buf, len, "Mystic Cube %d", stackCount); return;
    default: break;
    }
    CItem* related = goods->GetRelatedItem();
    if (related == 0)
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyGetItemString,
                              0x18d, "CashShop Relation Item is NULL [ITEM:%d]", itemIdx);
        return;
    }
    if (related->is_stackable() && stackCount >= 2)
        OS_API::snprintf(buf, len, "%s Count %d", related->GetItemName(), stackCount);
    else
        strncpy(buf, related->GetItemName(), len);
}

// ============================================================================
// ProcessBuyRequestEx —— ORIG 0x081769f6 (T)
// ============================================================================
int ItemVendingMachine::ProcessBuyRequestEx(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    if (user == 0)
    {
        cMyTrace tr(kPrettyProcessBuy, 0x290, 0);
        tr("There is no User in ItemVendingMachine");
        return 0;
    }

    const char* body = reinterpret_cast<const char*>(&msg);
    unsigned int itemCount = (unsigned char)body[0x36];
    const unsigned int MAX_ENTRIES = 0x23;
    const unsigned int STRIDE = 0x1cf;
    if (itemCount > MAX_ENTRIES)
        itemCount = MAX_ENTRIES;

    CCeraShopGoods* goods[MAX_ENTRIES];
    std::memset(goods, 0, sizeof(goods));
    std::vector<std::pair<int, int> > pairs;
    const CItem* relatedItemRef = 0;   // _CheckEnableItemCondition 输出（仅在 groupIdx 非 1/11 写入）

    int groupIdx = -1;
    int totalCera = 0, totalGold = 0, totalWin = 0, totalMileage = 0;

    user->resetUsedCera();

    // ---- 第一遍：逐条目校验、总价累计 ----
    for (unsigned int i = 0; i < itemCount; ++i)
    {
        const char* entryBase = body + i * STRIDE;
        int productNo = *(const int*)(body + 0x39 + i * STRIDE);
        unsigned char slotByte = (unsigned char)body[0x37 + i * STRIDE];
        unsigned char tagByte = (unsigned char)body[0x38 + i * STRIDE];
        (void)slotByte; (void)tagByte;

        goods[i] = G_CDataManager()->getItemList()->FindGoods(productNo);
        if (goods[i] == 0)
        {
            m_ceraShop.ProcessError(user, 0x15, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }
        groupIdx = goods[i]->GetGroupIdx();

        if (!_CheckEnableGiftCondition(goods[i], user, (char)body[0xf]))
        {
            m_ceraShop.ProcessError(user, 8, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }

        // IsBuyableRestrictItem(user, productNo, 1, i)：1=可购买（取反判不可购）。
        if (!CerashopAddRestrict::Manager::IsBuyableRestrictItem(user, productNo, 1, i))
        {
            m_ceraShop.ProcessError(user, 0x71, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }

        if (!_CheckEnableItemCondition(goods[i], relatedItemRef, pairs))
        {
            m_ceraShop.ProcessError(user, 0x11, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }

        if (!_CheckEnableStateCondition(goods[i], user))
        {
            m_ceraShop.ProcessError(user, 7, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }

        int buyingCode = 0;
        if (!m_ceraShop._CheckBuyingCondition(user, goods[i], productNo,
                                              (unsigned char)body[0x37 + i * STRIDE],
                                              (unsigned short)(unsigned char)body[0x38 + i * STRIDE],
                                              (char)body[0xf], buyingCode))
        {
            m_ceraShop.ProcessError(user, (unsigned short)buyingCode, groupIdx, productNo,
                                    i + 1, itemCount, (char)body[0xf]);
            goods[i] = 0;
            continue;
        }

        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[i],
                                             (char)body[0x38 + i * STRIDE],
                                             (unsigned char)body[0x37 + i * STRIDE],
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx, productNo, i + 1, itemCount,
                                        (char)body[0xf]);
                goods[i] = 0;
                continue;
            }
            totalCera += avatarSel.m_fieldc;
            totalGold += avatarSel.m_field8;
            totalWin += avatarSel.m_field4;
        }
        else
        {
            totalCera += goods[i]->GetCeraPrice();
            totalGold += goods[i]->GetGoldPrice();
            totalWin += goods[i]->GetWinPoint();
        }
        totalMileage += goods[i]->GetMileagePrice();

        if (*(const short*)(body + 0xd) == 1 && goods[i]->isCeraPointDisallowance_())
        {
            m_ceraShop.ProcessError(user, 0x138b, groupIdx, productNo, i + 1, itemCount,
                                    (char)body[0xf]);
            goods[i] = 0;
            continue;
        }
    }

    // ---- 总校验 ----
    if (!m_ceraShop._CheckValidCount((const CCeraShopGoods**)goods))
    {
        m_ceraShop.ProcessError(user, 1, -1, *(const int*)(body + 0x39), 1, 1,
                                (char)body[0xf]);
        return 1;
    }

    // ---- 总价资金校验 ----
    int errCode = 0;
    if (user->GetCera() < totalCera) errCode = 0xb;
    else if (user->getCurCharacMoney() < totalGold) errCode = 10;
    else if (user->GetWinPoint() < totalWin) errCode = 0xc;
    else if (user->GetMileage() < totalMileage) errCode = 0x3ed;
    if (*(const short*)(body + 0xd) == 1)
    {
        errCode = 0;
        if (user->GetCeraPoint() < (unsigned int)totalCera)
            errCode = 0x138a;
    }
    if (errCode != 0)
    {
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34b,
            "[ERROR] Not enough UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
            user->getCurCharacName(), totalCera, totalGold, totalWin, totalMileage);
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x34c,
            " / UCera[%d] UGold[%d] UWP[%d] AMile[%d]\n",
            user->GetCera(), user->getCurCharacInvenR()->get_money(),
            user->GetWinPoint(), user->GetMileage());
        m_ceraShop.ProcessError(user, (unsigned short)errCode, -1,
                                *(const int*)(body + 0x39), 1, 1, (char)body[0xf]);
        return 1;
    }

    // ---- 第二遍：逐条资金校验、扣减与 IPG 派发 ----
    int curGold = user->getCurCharacMoney();
    int curWin = user->GetWinPoint();
    int curMileage = user->GetMileage();
    int curSec = GlobalData::s_systemTime_.getCurSec();

    for (unsigned int j = 0; j < itemCount; ++j)
    {
        if (goods[j] == 0)
            continue;
        groupIdx = goods[j]->GetGroupIdx();

        int ceraPrice = 0, goldPrice = 0, winPrice = 0, avatarField = 0;
        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[j],
                                             (char)body[0x38 + j * STRIDE],
                                             (unsigned char)body[0x37 + j * STRIDE],
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        *(const int*)(body + 0x39 + j * STRIDE),
                                        j + 1, itemCount, (char)body[0xf]);
                goods[j] = 0;
                continue;
            }
            ceraPrice = avatarSel.m_fieldc;
            goldPrice = avatarSel.m_field8;
            winPrice = avatarSel.m_field4;
            avatarField = avatarSel.m_field0;
        }
        else
        {
            ceraPrice = goods[j]->GetCeraPrice();
            goldPrice = goods[j]->GetGoldPrice();
            winPrice = goods[j]->GetWinPoint();
        }
        int mileagePrice = goods[j]->GetMileagePrice();
        int productNo = *(const int*)(body + 0x39 + j * STRIDE);

        int perErr = 0;
        if (user->GetCera() < (unsigned int)ceraPrice) perErr = 0xb;
        else if (user->getCurCharacInvenR()->get_money() < goldPrice) perErr = 10;
        else if (user->GetWinPoint() < (unsigned int)winPrice) perErr = 0xc;
        if (*(const short*)(body + 0xd) == 1)
        {
            perErr = 0;
            if (user->GetCeraPoint() < (unsigned int)ceraPrice)
                perErr = 0x138a;
        }
        if (curGold < goldPrice) perErr = 10;
        if (curWin < winPrice) perErr = 0xc;
        if (user->GetMileage() < mileagePrice) perErr = 0x3ed;

        if (perErr != 0)
        {
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b3,
                "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] UserID[%s] ACera[%d] AGold[%d] AWP[%d] AMile[%d]",
                user->getCurCharacName(), ceraPrice, goldPrice, winPrice, mileagePrice);
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b4,
                " / UCera[%d] UGold[%d] UWP[%d] UMile[%d]\n",
                user->GetCera(), user->getCurCharacInvenR()->get_money(),
                user->GetWinPoint(), user->GetMileage());
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b9,
                "Error Goods No : %d", productNo);
            m_ceraShop.ProcessError(user, (unsigned short)perErr, groupIdx, productNo,
                                    j + 1, itemCount, (char)body[0xf]);
            goods[j] = 0;
            continue;
        }

        // 扣减（clamp 到 0）
        curGold = (curGold - goldPrice) > 0 ? (curGold - goldPrice) : 0;
        curWin = (curWin - winPrice) > 0 ? (curWin - winPrice) : 0;
        curMileage = (curMileage - mileagePrice) > 0 ? (curMileage - mileagePrice) : 0;

        if (!user->IsRestrictedGoods(goods[j]->GetFeaturedIdx()))
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire(
                "localjapan/AppendVendingMachine.cpp", 0x3cb);
            CStreamGuard guard(stream, false);
            CStreamGuard* g = guard.operator->();
            stCeraShopIPGRequest_t* req = g->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo = productNo;
            req->m_groupIdx = goods[j]->GetGroupIdx();
            req->m_stackCount = (unsigned short)goods[j]->GetStackCount();
            req->m_ceraPrice = (unsigned int)ceraPrice;
            req->m_goldPrice = (unsigned int)goldPrice;
            req->m_winPoint = (unsigned int)winPrice;
            req->m_itemIdx = goods[j]->GetItemIdx();
            req->m_avatarType0 = (unsigned char)body[0x37 + j * STRIDE];
            req->m_avatarType1 = (unsigned char)body[0x38 + j * STRIDE];
            req->m_avatarField = (unsigned int)avatarField;
            req->m_featuredIdx = goods[j]->GetFeaturedIdx();
            req->m_itemCount = itemCount;
            req->m_seq = j + 1;
            req->m_curSec = curSec;
            req->m_buyFlag = (unsigned char)body[0xf];
            req->m_accId = *(const unsigned int*)(body + 0x2e);
            req->m_characNo = *(const int*)(body + 0x32);
            req->m_buytype = *(const unsigned short*)(body + 0xd);
            req->m_subCount = (unsigned char)body[0x3d + j * STRIDE];
            for (unsigned int k = 0; k < req->m_subCount && k < MAX_ENTRIES; ++k)
            {
                // 子项数组：+0x3e 起每项 5 字节 { int idx; uchar val; }
                int idx = *(const int*)(body + 0x3e + k * 5 + j * STRIDE);
                unsigned char val = (unsigned char)body[0x42 + k * 5 + j * STRIDE];
                std::memcpy(req->m_subEntries[k], &idx, sizeof(int));
                req->m_subEntries[k][4] = val;
            }
            req->m_optCount = (unsigned char)body[0xed + j * STRIDE];
            for (unsigned int k = 0; k < req->m_optCount && k < MAX_ENTRIES; ++k)
            {
                // 选项数组：+0x3e.. 每项 8 字节 { ulong itemIdx; ushort a; ushort b; }
                const char* optBase = body + 0x3e + (k + 0x16) * 8 + j * STRIDE;
                unsigned long itemIdx = *(const unsigned long*)(optBase);
                unsigned short a = *(const unsigned short*)(optBase + 4);
                unsigned short b = *(const unsigned short*)(optBase + 6);
                unsigned char* opt = req->m_optEntries[k];
                // 选项项布局：{ ulong itemIdx(4, at +0); ushort a(2, at +4); ushort b(2, at +6) }
                //（dest 为 (k+0x2c)*8 + {4,8,10}，即 m_optEntries[k] 的 +0/+4/+6）。
                std::memcpy(opt, &itemIdx, sizeof(unsigned long));
                std::memcpy(opt + 4, &a, sizeof(unsigned short));
                std::memcpy(opt + 6, &b, sizeof(unsigned short));
            }

            guard.operator*()->operator<<(mileagePrice);
            Stream* theStream = guard.Get();

            char itemStr[0x400];
            char ceraPriceStr[0x24];
            char productStr[8];
            char uniqueStr[0x15];
            std::memset(itemStr, 0, sizeof(itemStr));
            std::memset(ceraPriceStr, 0, sizeof(ceraPriceStr));
            std::memset(productStr, 0, sizeof(productStr));
            std::memset(uniqueStr, 0, sizeof(uniqueStr));
            OS_API::snprintf(productStr, sizeof(productStr), "%d", productNo);
            OS_API::snprintf(ceraPriceStr, sizeof(ceraPriceStr), "%d", ceraPrice);
            _GetItemString(goods[j], itemStr, 0x400);
            char* uniqueNo = GenUniqueNo_asm(uniqueStr);

            typedef void (*IPGInputFn)(void*, CUser*, const char*, const char*,
                                       const char*, const char*, const char*, void*);
            IPGInputFn fn = 0;
            if (ceraPrice == 0)
                fn = (IPGInputFn)(*(void**)(*(char**)GlobalData::s_pIPGHelper + 0x18));
            else if (*(const short*)(body + 0xd) == 1)
                fn = (IPGInputFn)(*(void**)(*(char**)GlobalData::s_pIPGHelper + 0x10));
            else
                fn = (IPGInputFn)(*(void**)(*(char**)GlobalData::s_pIPGHelper + 0x14));
            if (fn)
                fn(GlobalData::s_pIPGHelper, user, productStr, ceraPriceStr, uniqueNo,
                   itemStr, user->getCurCharacName(), theStream);
            guard.~CStreamGuard();
        }
        else
        {
            m_ceraShop.ProcessError(user, 0x14, groupIdx, productNo,
                                    j + 1, itemCount, (char)body[0xf]);
            goods[j] = 0;
        }
    }

    return 1;
}
Secu_DataControl::Secu_DataControl() {}  // ORIG 0x82aa6ba：空体（纯 ret，无成员初始化）

// encryptAES_ 定义在 MouseRegisterEJ.cpp（保留方，0x148 更近 ORIG 0x14d）；此 TU 仅调用。
char* encryptAES_(CRijndael* cipher, char* inData, char* outHex);

// Secu_DataControl::UpdateSecurityServiceState —— ORIG T 0x08286e40
// [0x08286e40,0x08287081]。service==0：SecurityCard 已激活→init；
// service==1：CPad 已激活→若已认证则 flag=1，否则将 msg 截 8 字节 AES 加密后
// setFailCnt(0)+setPassword；任一分支生效则发送 0x159 通知包（service/flag）。
void Secu_DataControl::UpdateSecurityServiceState(CUser* user, unsigned char service,
                                                  unsigned char flag, char* msg)
{
    if (user == 0)
    {
        return;
    }
    bool done = false;
    if (service == 0)
    {
        WongWork::CSecurityCard* card = user->getSecurityCard();
        if (card->isActivate())
        {
            card = user->getSecurityCard();
            card->init();
            done = true;
        }
    }
    else if (service == 1)
    {
        Sanicova::CPad* pad = user->getPad();
        if (pad->isActivate())
        {
            pad = user->getPad();
            if (pad->isCertified())
            {
                flag = 1;
            }
            else
            {
                char plain[36];
                char enc[36];
                memset(plain, 0, sizeof(plain));
                memset(enc, 0, sizeof(enc));
                strncpy(plain, msg, 8);
                cMyTrace trace(
                    __PRETTY_FUNCTION__,
                    0xa0, 0);
                trace("[Secu_Web_Conn] (%s)  ", msg);
                encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, enc);
                pad = user->getPad();
                pad->setFailCnt(0);
                pad = user->getPad();
                pad->setPassword(enc);
                done = true;
            }
        }
    }
    if (done)
    {
        PacketGuard guard;
        guard.put_header(0, 0x159);
        guard.put_byte(service);
        guard.put_byte(flag);
        guard.finalize(true);
        user->Send(guard);
    }
}

bool CDBConnector::GetConnInfo(DBGroupMap& groupMap, DBInfoMap& infoMap)
{
    // ORIG 0x080ec9ca：遍历 groupMap，逐组 QueryConnInfo(idx, group, infoMap)；
    // 任一失败返回 false，全部成功返回 true。
    for (DBGroupMap::iterator it = groupMap.begin(); it != groupMap.end(); ++it)
    {
        if (!QueryConnInfo(it->first, it->second, infoMap))
        {
            return false;
        }
    }
    return true;
}

bool CDBConnector::Connect(DBInfoMap infoMap)
{
    // ORIG 0x080ec760：按值遍历连接表；idx==0/5 跳过；cMyTrace 记录
    // "Open(type:%2d, %s/%d/%s/%s)"（.rodata 0x8b356a4）；Open 失败即 false。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        ENUM_DB_HANDLE_IDX idx = it->first;
        STDBConnInfo& info = it->second;
        if (!(idx == 0 || idx == (ENUM_DB_HANDLE_IDX)5))
        {
            cMyTrace t("CDBConnector::Connect", 0x4e, 1);
            t("Open(type:%2d, %s/%d/%s/%s)", (int)idx, info.m_host,
              (int)info.m_port, info.m_db, info.m_user);
            if (!Open(idx, (ENUM_SERVER_GROUP)0, info))
            {
                return false;
            }
        }
    }
    return true;
}

bool CDBConnector::ConnectWithServerGroup(DBInfoMap infoMap)
{
    // ORIG 0x080ec8f8：遍历连接表；idx==0 跳过；以 +0x168 处 server group
    // 为组参数调 Open；失败即 false。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        ENUM_DB_HANDLE_IDX idx = it->first;
        if (idx != (ENUM_DB_HANDLE_IDX)0)
        {
            STDBConnInfo& info = it->second;
            ENUM_SERVER_GROUP group =
                *(ENUM_SERVER_GROUP*)(reinterpret_cast<char*>(&info) + 0x160);  // ORIG +0x168-8（map 节点值基），推断映射
            if (!Open(idx, group, info))
            {
                return false;
            }
        }
    }
    return true;
}

// ---- CDBConnector ----
CDBConnector::CDBConnector(DBMgr* dbMgr)
    : m_dbMgr(dbMgr)   // ORIG C1 0x080ec5fc：先存 DBMgr*，再默认构造成员 STDBConnInfo。
{
}
CDBConnector::~CDBConnector() {}  // TODO(后续批次)
bool CDBConnector::Init(STDBConnInfo& info)
{
    // ORIG 0x080ec620：拷贝 0x5a dword（0x168 字节）到 this+4；OpenMaster()。
    memcpy(&m_connInfo, &info, sizeof(STDBConnInfo));
    return OpenMaster();
}
bool CDBConnector::OpenMaster()
{
    // ORIG 0x080ec67c：Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, this+4)
    // 结果取反返回。
    return !Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, m_connInfo);
}
void CDBConnector::CloseMaster()
{
    // ORIG 0x080ec6bc：GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0)。
    GlobalData::s_db_mgr->Close((ENUM_DB_HANDLE_IDX)0);
}
void CDBConnector::Close()
{
    // ORIG 0x080ec668：转发 CloseMaster()。
    CloseMaster();
}
bool CDBConnector::Open(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                        STDBConnInfo& info)
{
    // ORIG 0x080ec6da：info 首字节为 0（host 空）→ 直接成功；否则按 ORIG 字段
    // 映射调 DBMgr::Open（host=+0x2d, port=+0x14, db=+0x18, user=+0x42,
    // passwd=+0x00——按反汇编忠实传递）。
    if (info.m_host[0] == 0)
    {
        return true;
    }
    return GlobalData::s_db_mgr->Open(idx, info.m_pass, (unsigned int)info.m_port,
                                      info.m_user, info.m_db, info.m_host, group);
}
// ---- CDBConnector::QueryConnInfo ORIG @ 0x080eca6c（U3ShopDb 批次） ----
// 用主 DB 句柄(0,0) 查 db_connect 表，逐列填 STDBConnInfo 后放入 outMap[idx]。
// 列序：db_name(0x1f→m_db) / db_ip(0x11→m_host) / db_port(→m_port) /
//       db_userid(0x15→m_user) / db_passwd（先清零 60 字节，0x3c→+0x2d 起）。
// 密文经 CEnvironment::DecryptValue 解出（ORIG 解密结果未回写结构，仅校验）。
bool CDBConnector::QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                                 DBInfoMap& outMap)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db ||
        !db->set_query("seLect db_name,db_ip,db_port,db_userid,db_passwd from "
                       "db_connect where db_server_group = %d and db_type = %d",
                       (int)group, (int)idx))
        return false;
    if (!db->exec(true) || !db->fetch())
        return false;
    if (db->get_n_rows() != 1)
        return false;

    STDBConnInfo info;
    char passCipher[0x3c];
    char passPlain[0x15];
    if (!db->get_str(0, info.m_db, 0x1f))
        return false;
    if (!db->get_str(1, info.m_host, 0x11))
        return false;
    if (!db->get_int(2, info.m_port))
        return false;
    if (!db->get_str(3, info.m_user, 0x15))
        return false;
    std::memset(info.m_pass, 0, 60);
    if (!db->get_str(4, passCipher, 0x3c))
        return false;
    if (!G_CEnvironment()->DecryptValue(passCipher, passPlain))
        return false;
    // ORIG 0x80eccb8：rep movsd 0x5a dword（0x168 字节）拷入 outMap[idx]。
    outMap[idx] = info;
    return true;
}
// sync_script::CSyncScript 权威定义见 sync_script.h（SetEnable/IsEnable 内联 +
// GetInstanceSyncScript 单例访问器）；本 TU 不再提供本地镜像。

// ---- 事件/GM/统计 ----
// CEventManager 构造/析构已移入 CEventManager.cpp（独立 TU）。
CGM_Manager::CGM_Manager()
{
    // ORIG 0x082990d0：map 构造（成员自动）+ rwLock 构造 + 存 DB 句柄(6) +
    // LoadGmListFromDb()。

    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}
CGM_Manager::~CGM_Manager()
{
    // ORIG 0x08299159 附近：rwLock 析构 + map 自动析构。

}
bool CGM_Manager::LoadGmListFromDb()
{
    // ORIG 0x08299530：查询 gm_manifest(m_id, level)，写锁保护下填充 map。
    m_db->set_query("seLect m_id, level from gm_manifest");
    char ok = m_db->exec(true);
    if (ok != 1)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1579, 0);
        t("Failed to get GM list from DB.");
        return false;
    }
    m_lock.wrlock();
    int rows = m_db->get_n_rows();
    int result = 1;
    for (int i = 0; i < rows; ++i)
    {
        m_db->fetch();
        unsigned int mId = 0;
        int level = 0;
        if (m_db->get_int(0, *(int*)&mId) != 1)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1588, 0);
            t("Failed to get GM's m_id from DB.");
            result = 0;
            break;
        }
        if (m_db->get_int(1, level) != 1)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x158e, 0);
            t("Failed to get GM's lv from DB.");
            result = 0;
            break;
        }
        m_gmMap[mId].m_level = level;
    }
    m_lock.unlock();
    return result == 1;
}
CStatisticMgr::CStatisticMgr()
{
    // ORIG 0x82aa89e：两个统计子对象（map 成员自动构造）。
}

// ORIG T 0x0860ecc6 IncreaseLevelDungeonPlay(level, playCount)：
// level ∉ [1,200] → 仅 trace(type=0)；否则 find 命中 → 引用缓存后
// m_playCount += playCount、++m_userCount；未命中 → 先写 playCount 再置
// userCount=1，insert。
void CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(int level, int playCount)
{
    if (level < 1 || level > 200)
    {
        cMyTrace tr(__PRETTY_FUNCTION__,
                    0x550, 0);
        tr("LEVEL_DUNGEON_PLAY_STATISTIC wrong level(%d)", level);
        return;
    }
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.find(level);
    if (it == m_levelPlayMap.end())
    {
        stDungeonPlayInfo info;
        info.m_playCount = playCount;
        info.m_userCount = 1;
        m_levelPlayMap.insert(std::make_pair(level, info));
    }
    else
    {
        stDungeonPlayInfo& info = it->second;
        info.m_playCount += playCount;
        info.m_userCount += 1;
    }
}

// ---- 后续批次业务模块 ----
break_away_prevent::CBreakAwayPreventSystem::CBreakAwayPreventSystem()
{
    // ORIG 0x0831c3e0：SIG 基类构造；map 构造；5×STRewardDataKey 构造；
    // memset(+0x30b, 0, 0x1f)；map.clear()。基类/Key 内容未还原（推断：
    // 以清零等价，容器真实构造）。
    // ORIG 0x0831c3e0：m_keys 仅构造 4 项（mov $0x4 循环 STRewardDataKeyC1，
    // 每项 3 字节）；第 5 槽（+0x308..+0x30a）保持垃圾，不清零。
    memset(m_base, 0, sizeof(m_base));
    memset(m_keys, 0, 4 * sizeof(STRewardDataKey));
    memset(m_tail, 0, sizeof(m_tail));
    m_rewardMap.clear();
}

// ORIG T 0x0831c486 [0x0831c486, 0x0831c49d]：仅析构 +0x2e4 的
// m_rewardMap（std::map<STRewardDataKey,STRewardDataList>），成员自动析构等价。
break_away_prevent::CBreakAwayPreventSystem::~CBreakAwayPreventSystem()
{
}
// ORIG T 0x0831c49e（loadDbData.md 全量转写）：拷贝 SIG 基类区 0x2e4，
// 清 map 后按条目组装：条目 stride 0x18，key=+0x10/+0x11/+0x12 三字节，
// value=+0x14..+0x24 五个 int（STRewardData）。key 已存在 → list push_back；
// 否则新建 list 插入 map。同时维护 m_keys（5 项）。
void break_away_prevent::CBreakAwayPreventSystem::loadDbData(
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data)
{
    memcpy(m_base, data, 0x2e4);
    m_rewardMap.clear();

    int keyCount = 0;
    int count = data->m_field2e0;
    for (int i = 0; i < count; ++i)
    {
        const char* entry = (const char*)(data) + i * 0x18;
        STRewardDataKey key;
        key.m_data[0] = entry[0x11];
        key.m_data[1] = entry[0x12];
        key.m_data[2] = entry[0x10];

        STRewardData reward;
        reward.m_field0 = *(int*)(entry + 0x14);
        reward.m_field4 = *(int*)(entry + 0x18);
        reward.m_field8 = *(int*)(entry + 0x1c);
        reward.m_fieldc = *(int*)(entry + 0x20);
        reward.m_field10 = *(int*)(entry + 0x24);

        std::map<STRewardDataKey, STRewardDataList>::iterator it =
            m_rewardMap.find(key);
        if (it != m_rewardMap.end())
        {
            it->second.m_list.push_back(reward);
        }
        else
        {
            STRewardDataList list;
            list.m_list.push_back(reward);
            m_rewardMap.insert(std::make_pair(key, list));
        }

        if (keyCount < 5)
        {
            m_keys[keyCount].m_data[0] = key.m_data[0];
            m_keys[keyCount].m_data[1] = key.m_data[1];
            m_keys[keyCount].m_data[2] = key.m_data[2];
            ++keyCount;
        }
    }
}
expert_job::CExpertJobFactory::CExpertJobFactory()
{
    // ORIG 0x0849f68c：两个 StaticPool 成员构造。
}
expert_job::CExpertJobFactory::~CExpertJobFactory()
{
    // ORIG 0x0849f6d2：两个 StaticPool 成员析构。
}
expert_job::CExpertJobMgr::CExpertJobMgr()
{
    // ORIG 0x0849e102：new(0x60, nothrow) CExpertJobFactory，失败 printf 后继续。
    m_factory = new (std::nothrow) CExpertJobFactory;
    if (m_factory == 0)
    {
        printf("memory error func(%s) line(%d)",
               __PRETTY_FUNCTION__, 0x57f);
    }
}
expert_job::CExpertJobMgr::~CExpertJobMgr()
{
    // ORIG 0x0849e1ac：delete factory。
    delete m_factory;
        m_factory = 0;
}
// CVillageObjectMgr ctor/dtor 现由 CVillageObjectMgr.cpp 提供
//（ORIG 0x86b6f30 默认构造 std::map；dtor 0x86b6f44 调用 map dtor）。
// CVillageMonsterMgr ctor/dtor 现由 CVillageMonsterMgr.cpp 提供（LinkVillageSolo）。
village_attacked::CRevengeDungeon::CRevengeDungeon()
{
    // ORIG 0x82aa6a4：+0 字节清零 + +0x04 int 清零。
    m_isOpen = false;
    m_field04 = 0;
}
bool village_attacked::CRevengeDungeon::IsOpenRevengeDungeon()  // ORIG 0x8234ff6
{
    return m_isOpen;
}
int village_attacked::CRevengeDungeon::GetCloseTime()  // ORIG W 0x085bff32
{
    return m_field04;
}
void village_attacked::CRevengeDungeon::OpenRevengeDungeon()   // ORIG W 0x0847dd1e
{
    // movb $0x1,(%eax)：置 +0 字节为 1。
    m_isOpen = true;
}
void village_attacked::CRevengeDungeon::CloseRevengeDungeon()  // ORIG W 0x0847dd2a
{
    // movb $0x0,(%eax)：置 +0 字节为 0。
    m_isOpen = false;
}
void village_attacked::CRevengeDungeon::SetCloseTime(unsigned int sec)  // ORIG W 0x0847dd36
{
    // mov %edx,0x4(%eax)：arg 写 +0x04 int。
    m_field04 = sec;
}
InstanceRentalSystem::InstanceRentalSystem()
{
    // ORIG 0x08270014：new RentalSystemImpl(0x18)；注册 PcRoom(1)/Dimension(2)。
    m_impl = new RentalSystemImpl;
    registerRentalMachine(1, new RentalMachinePcRoom);
    registerRentalMachine(2, new RentalMachineDimension);
}
InstanceRentalSystem::~InstanceRentalSystem()
{
    // ORIG 0x08270110：遍历机器表虚析构；释放 impl。
    if (m_impl)
    {
        for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
             it != m_impl->m_machines.end(); ++it)
        {
            delete it->second;
        }
        m_impl->m_machines.clear();
        delete m_impl;
        m_impl = 0;
    }
}
void InstanceRentalSystem::registerRentalMachine(int idx, IRentalMachine* m)
{
    m_impl->m_machines[idx] = m;
}

RentalMachinePcRoom::RentalMachinePcRoom() : m_impl(new RentalMachinePcRoomImpl)
{
    // ORIG 0x82703c0：IRentalMachine 基类 + vptr；new Impl(0x11c) 存 auto_ptr@+4；
    // initLoggingEnvironment()。
    initLoggingEnvironment();
}
void RentalMachinePcRoom::initLoggingEnvironment()
{
    // ORIG 0x8270886：snprintf(impl, 0x103, "./log/%s/dw2_RentalItems", 文件名)。
    snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
             G_CEnvironment()->get_file_name());
}

void RentalMachineDimension::initLoggingEnvironment()
{
    // ORIG 0x826efdc：同 PcRoom 日志路径（.rodata 0x8bf90c2 同串）。
    snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
             G_CEnvironment()->get_file_name());
}

// ---- IRentalMachine 槽实现（PcRoom / Dimension 同构，逐报告转写）----

// PcRoom::Ready（ORIG 0x0827073e）：尾调 checkAllRentalItemScript()。
bool RentalMachinePcRoom::Ready()
{
    return checkAllRentalItemScript();
}
// Dimension::Ready（ORIG 0x0826ee94）：同构。
bool RentalMachineDimension::Ready()
{
    return checkAllRentalItemScript();
}

// PcRoom::CheckRentableItem（ORIG 0x08270752）：
// return m_impl->m_set.find(itemIdx) != m_impl->m_set.end()。
bool RentalMachinePcRoom::CheckRentableItem(unsigned int itemIdx)
{
    // ORIG 0x08270752：auto_ptr 解引用后 m_set.find(itemIdx) != end()。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return it != m_impl->m_set.end();
}
// Dimension::CheckRentableItem（ORIG 0x0826eea8）：同构。
bool RentalMachineDimension::CheckRentableItem(unsigned int itemIdx)
{
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return it != m_impl->m_set.end();
}

// ---- InstanceRentalSystem 查询/分发（ORIG 报告 InstanceRentalSystem/*.md）----

// getMachine（ORIG T 0x08270348）：map find，未命中返回 0。
IRentalMachine* InstanceRentalSystem::getMachine(int idx)
{
    std::map<int, IRentalMachine*>::const_iterator it = m_impl->m_machines.find(idx);
    return it != m_impl->m_machines.end() ? it->second : 0;
}

// Ready（ORIG T 0x082701dc）：遍历机器表；任一为空或其槽3 Ready() 返回非 1
// 即返回 false，全部就绪（含空表）返回 true。
bool InstanceRentalSystem::Ready()
{
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        IRentalMachine* machine = it->second;
        if (machine == 0 || !machine->Ready())
        {
            return false;
        }
    }
    return true;
}

// Rent 分发（ORIG T 0x08270288）：按 RentInput.m_machineKey 取机器并经槽2
// 调用；无机器时 ORIG 直接返回 1。
int InstanceRentalSystem::Rent(CUser& user, const RentInput& input,
                               RentOutput& output)
{
    IRentalMachine* machine = getMachine(input.m_machineKey);
    if (machine == 0)
    {
        return true;
    }
    return machine->Rent(user, input, output);
}

CDailyScheduleManager::CDailyScheduleManager() {}
CDailyScheduleManager::~CDailyScheduleManager() {}
online_preliminary::CTeamFactory::CTeamFactory()
{
    // ORIG 0x08588e2e：StaticPool 成员构造（new CNode[600]）。
}
online_preliminary::CTeamFactory::~CTeamFactory()
{
    // ORIG 0x08588e42：StaticPool 成员析构。
}
online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()
{
    // ORIG 0x0858860c：+0x00=0；map@+0x04 由编译器构造；new(0x30, nothrow)
    // CTeamFactory；失败 logFormat 后继续。
    m_factory = 0;
    m_factory = new (std::nothrow) CTeamFactory;
    if (m_factory == 0)
    {
        LogManager::logFormat(1, "OnlinePreliminaryTeam.cpp",
                              __PRETTY_FUNCTION__,
                              0xd9, "memory error func(%s) line(%d)",
                              "online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()",
                              0xd9);
    }
}
// ORIG T 0x085886fe：m_factory 非空时 delete（CTeamFactory D1 + operator delete）；
// m_teamMap@+0x04 由编译器析构（ORIG 尾调 map D1）。
online_preliminary::COnlinePreliminaryTeamMgr::~COnlinePreliminaryTeamMgr()
{
    delete m_factory;
}
// ORIG T 0x08588be4（OnLoadTeamInfo.md 全量转写）：data 首 4 字节为队伍 key，
// 命中 m_teamMap 时委托 loadDataPreliminaryTeamMatchList(data)。
void online_preliminary::COnlinePreliminaryTeamMgr::OnLoadTeamInfo(
    CUser* user, char* data)
{
    std::map<int, COnlinePreliminaryTeam*>::iterator it =
        m_teamMap.find(*(int*)data);
    if (it != m_teamMap.end())
    {
        it->second->loadDataPreliminaryTeamMatchList(data);
    }
}
// game_master::CCommandMap / CGameMasterMgr 真实实现见 GmCore.cpp 与 GmRegister.cpp
//（ORIG CCommandMap ctor 0x082a7f64、add(int) 0x082a7c76、add(string) 0x082a7cdc、
// Mgr ctor 注册链 0x082a8a28）。

// ---- IBusiness / CBusinessImpl（报告 docs/class_func_reports/CBusinessImpl/）----

IBusiness::IBusiness()
{
    // ORIG W 0x082a7c24：仅设 vptr（编译器生成）。
}

CBusinessImpl::CBusinessImpl()
{
    // ORIG 0x082a7c32：call IBusiness ctor + 设 vptr（编译器生成序列）。
}

CBusinessImpl::~CBusinessImpl()
{
    // ORIG 0x082a7c4e：仅重置 vptr（编译器生成序列）。
}

void CBusinessImpl::SendBuffertoClient(int a, int b, int c, unsigned int d,
                                       unsigned char* data, short len)
{
    (void)b;
    (void)c;
    // ORIG 0x080de240：按 stream 长档位取池；<<0x252 <<-1；
    // put_binary({int a, uint d, short len}, 0xc)；put_binary(data, len)。
    const int size = (int)len + 0x14;
    CStreamGuard guard(true);
    if (size < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("BusinessImpl.cpp", 0x3a));
    }
    else if (0x3fff < size)
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("BusinessImpl.cpp", 0x42));
    }
    else
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x3e));
    }
    **guard << 0x252;
    **guard << -1;
    struct
    {
        int field00;
        unsigned int field04;
        int field08;
    } hdr;
    hdr.field00 = a;
    hdr.field04 = d;
    hdr.field08 = (unsigned short)(len);   /* movzwl %ax */
    guard->put_binary(&hdr, 0xc);
    guard->put_binary(data, (int)len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

void CBusinessImpl::PunishClient(int a, int b, unsigned int c, EPUNISHMODE mode,
                                 tagAntibotPunishInfo* info, char* reason)
{
    (void)a;
    (void)b;
    (void)reason;
    // ORIG 0x080de3ee：StreamPool 取流；CStreamGuard(stream,true)；<<0x253 <<-1；
    // put_binary({uint c; EPUNISHMODE mode(byte); info 前 8 字节}, 0x10)。
    Stream* s = GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x59);
    CStreamGuard guard(s, true);
    **guard << 0x253;
    **guard << -1;
    struct
    {
        unsigned int field00;
        EPUNISHMODE field04;
        int field08;
        int field0c;
    } body;
    body.field00 = c;
    body.field04 = mode;
    const int* infoWords = reinterpret_cast<const int*>(info);
    body.field08 = infoWords[0];
    body.field0c = infoWords[1];
    guard->put_binary(&body, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

void CBusinessImpl::ProcessProtectMsg(tagTSafeModePkg* pkg)
{
    // ORIG 0x080de4ec：StreamPool 取流；<<0x254 <<-1；put_binary(pkg, 0x34)。
    Stream* s = GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x88);
    CStreamGuard guard(s, true);
    **guard << 0x254;
    **guard << -1;
    guard->put_binary(pkg, 0x34);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ---- DB 请求（ORIG 模式：Acquire 流 → CStreamGuard → <<msgId → <<-1 → 入 DB 队列）----
void DB_ReloadAutoPunishRuleBackIP::makeRequest()
{
    // ORIG 0x08432e80 同型（DBThread.cpp 0x7a8e）；本函数 msgId=0x108（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7077), true);
    **guard << 0x108;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_ReloadAutoPunishRuleHackType::makeRequest()
{
    // msgId=0x109（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x70e2), true);
    **guard << 0x109;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_LoadCleanPadPoint::makeRequest()
{
    // ORIG 0x0842efa0：DBThread.cpp 0x73d8；msgId=0x127；第二字段 -1。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73d8), true);
    **guard << 0x127;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_BlackIPMonitor::makeRequest()
{
    // ORIG 0x08432e80：DBThread.cpp 0x7a8e；msgId=0x152；第二字段 -1。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7a8e), true);
    **guard << 0x152;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- 杂项对象 ----
StackableAction::Action::Action()
{
    // ORIG 0x0827f6d8：两个 map 构造（成员自动）+ init_function_type()。
    init_function_type();
}
int StackableAction::Action::proc_bingo(input_param& p)
{
    // ORIG 0x0827fb14：BingoEvent(0x6a) IsEventing(0) 检查后转发 reqBingoMark；
    // 未开启返回 0x13。
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
    if (!ev->IsEventing(0))
    {
        return 0x13;
    }
    // BingoEvent 完整类型已建模（BingoEvent.h 权威头，OrigBingoEvt 批次）；
    // ORIG 0x0827fb14 直调 _ZN10BingoEvent12reqBingoMarkERN15StackableAction11input_paramE。
    BingoEvent* bingoEvent = static_cast<BingoEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x6a));
    if (bingoEvent == 0)
    {
        return 0x13;
    }
    return bingoEvent->reqBingoMark(p);
}
int StackableAction::Action::proc_addFatigue(input_param& p)
{
    // ORIG 0x0827fbd6：user/item 判空→0x13；item+0x3a0 的 vector<int> 取第 0 项
    // 与当前疲劳比较；疲劳不足返回 0x5f；否则扣减并 SendFatigue。
    if (p.m_user == 0)
    {
        return 0x13;
    }
    if (p.m_item == 0)
    {
        return 0x13;
    }
    // ORIG item+0x3a0 = CStackableItem::m_vec3a0（运行时 item 为 stackable）
    std::vector<int>* costs = &static_cast<CStackableItem*>(p.m_item)->m_vec3a0;
    if (costs->empty())
    {
        return 4;
    }
    int need = std::max(0, (*costs)[0]);
    int cur = p.m_user->getCurCharacFatigue();
    if ((int)(cur & 0xffff) < need)
    {
        return 0x5f;
    }
    int after = (cur & 0xffff) - need;
    int lower = 0;
    p.m_user->setCurCharacFatigue(std::max(after, lower));
    p.m_user->SendFatigue();
    return 0;
}
int StackableAction::Action::proc_lotteryCount(input_param& p)
{
    // ORIG 0x0827fb86：user/item 判空→0x13；否则 giveLotteryCountItem(
    // item->get_index(), false)，返回 0。
    if (p.m_user == 0)
    {
        return 0x13;
    }
    if (p.m_item == 0)
    {
        return 0x13;
    }
    p.m_user->giveLotteryCountItem(p.m_item->get_index(), false);
    return 0;
}

void StackableAction::Action::init_function_type()
{
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34/0x36/0x37）。
    // 与 ORIG 一致用 insert 方式（非 operator[]）。
    using boost::arg;
    m_map2.insert(std::make_pair(TYPE_BINGO,
        boost::function<int(input_param&)>(
            boost::bind(&Action::proc_bingo, this, arg<1>()))));
    m_map2.insert(std::make_pair(TYPE_LOTTERY_COUNT,
        boost::function<int(input_param&)>(
            boost::bind(&Action::proc_lotteryCount, this, arg<1>()))));
    m_map2.insert(std::make_pair(TYPE_ADD_FATIGUE,
        boost::function<int(input_param&)>(
            boost::bind(&Action::proc_addFatigue, this, arg<1>()))));
}

expert_extraction::CExtraction::CExtraction()
{
    // ORIG 0x084a2938：vptr + new CMTRand 并以 time(0) seed。
    unsigned long t = (unsigned long)time(0);
    m_rand = new CMTRand;
    m_rand->seed(t);
}
expert_extraction::CExtraction::~CExtraction()
{
    // ORIG 0x084a2938：仅 delete m_rand（判空），不置零。
    delete m_rand;
}
expert_extraction::CAlchemistExtraction::CAlchemistExtraction() {}
expert_extraction::CAlchemistExtraction::~CAlchemistExtraction() {}
expert_extraction::CEnchanterExtraction::CEnchanterExtraction() {}
expert_extraction::CEnchanterExtraction::~CEnchanterExtraction() {}
expert_extraction::CDollControllerExtraction::CDollControllerExtraction() {}
expert_extraction::CDollControllerExtraction::~CDollControllerExtraction() {}
expert_extraction::CExtractionMgr::CExtractionMgr()
{
    // ORIG 0x084a302e：new 三个子提取器对象，依次存 +0/+4/+8。
    m_slots[0] = new CAlchemistExtraction;
    m_slots[1] = new CEnchanterExtraction;
    m_slots[2] = new CDollControllerExtraction;
}
expert_extraction::CExtractionMgr::~CExtractionMgr()
{
    // ORIG 0x84a311a：for i∈[0,2] if(m_slots[i]) delete m_slots[i];（不置空）。
    for (int i = 0; i < 3; ++i)
    {
        if (m_slots[i])
        {
            delete m_slots[i];
        }
    }
}
// ---- Teleport / NPC_Teleport / User_Teleport（报告 Teleport/、NPC_Teleport/、User_Teleport/）----

STCheckRuleData::STCheckRuleData() {}
// ORIG W 0x0822d78e：仅清零 +0x00 的 int（m_field00），不触碰 m_tp 子对象。
STTeleportData::STTeleportData()
{
    m_field00 = 0;
}

Teleport::Teleport()
{
    // ORIG 0x08283c30：设 vptr；stPartyTelePort::reset(this+4)；*(this+0x10)=0。
    m_tp.reset();
    m_endCoolTime = 0;
}
Teleport::~Teleport()
{
    // ORIG D1 0x082843b0：重置 vptr，无成员析构（POD）。
}
int Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x08283d82：转发 check_rule(data.m_user)。
    return check_rule(data.m_user);
}
void Teleport::set_party_teleport(stPartyTelePort const& tp)
{
    // ORIG 0x08283d98：拷贝 stPartyTelePort 前 10 字节（8 字节 + short）。
    // 块拷贝视图：成员地址 + 宽度视图（含 pad 字节），无裸偏移。
    *reinterpret_cast<int*>(&m_tp.m_state) = *reinterpret_cast<const int*>(&tp.m_state);
    *reinterpret_cast<int*>(&m_tp.m_posx) = *reinterpret_cast<const int*>(&tp.m_posx);
    *reinterpret_cast<short*>(&m_tp.m_direction) =
        *reinterpret_cast<const short*>(&tp.m_direction);
}
stPartyTelePort& Teleport::get_party_teleport()
{
    // ORIG 0x08283db6：返回 this+4。
    return m_tp;
}
void Teleport::set_end_cool_time(CUser* user)
{
    // ORIG 0x08283dc2：endCoolTime=aura_option(2)；now=getCurSec；
    // endCoolTime = (now < endCoolTime ? endCoolTime - now : 0)。
    if (user != 0)
    {
        m_endCoolTime = user->get_aura_avatar_option_value(2);
        int now = GlobalData::s_systemTime_.getCurSec();
        if (now < m_endCoolTime)
        {
            m_endCoolTime = m_endCoolTime - now;
        }
        else
        {
            m_endCoolTime = 0;
        }
    }
}
int Teleport::get_end_cool_time()
{
    // ORIG 0x08283e24：返回 this+0x10。
    return m_endCoolTime;
}
void Teleport::update_next_end_cool_time(CUser* user)
{
    // ORIG 0x08283e30：is_equip_aura_avatar('..', &remain) 取剩余；
    // set_aura_avatar_option_value(2, getCurSec()+remain)；UpdateAuraAvatarOption。
    if (user != 0)
    {
        int remain = 0;
        user->is_equip_aura_avatar(2, remain);
        int v = GlobalData::s_systemTime_.getCurSec() + remain;
        user->set_aura_avatar_option_value(2, v);
        int cur = user->get_aura_avatar_option_value(2);
        user->UpdateAuraAvatarOption(2, cur);
    }
}
void Teleport::send_teleport_effect(CUser* user, int type)
{
    // ORIG 0x08283eba：PacketGuard put_header(0,0x17b)/put_byte(type)/put_byte(1)
    // /put_short(GetUID)/finalize(true)；按 vill/area send_to_area。
    if (user != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x17b);
        packet.put_byte((int)(char)type);
        packet.put_byte(1);
        packet.put_short(user->GetUID());
        packet.finalize(true);
        G_GameWorld()->send_to_area(
            (int)user->getCurCharacVill(), user->get_area(false), packet);
    }
}
bool Teleport::move_area(CUser* user)
{
    // ORIG 0x08283fb8：effect(1) → GameWorld::move_area(...) → effect(2)+
    // update_next_end_cool_time。m_tp 字段直读（stPartyTelePort getters 归属
    // CPartyTelePort 域未在本 TU）。
    if (user == 0)
    {
        return false;
    }
    send_teleport_effect(user, 1);
    const bool ok = G_GameWorld()->move_area(
        user, (int)(unsigned char)m_tp.m_village, (int)(unsigned char)m_tp.m_areaIndex,
        (int)m_tp.m_posx, (int)m_tp.m_posy,
        (int)(unsigned char)m_tp.m_direction, true, 2, 0, 0);
    if (ok)
    {
        send_teleport_effect(user, 2);
        update_next_end_cool_time(user);
    }
    return ok;
}
NPC_Teleport::NPC_Teleport()
{
    // ORIG 0x082840da：call Teleport ctor + 设 vptr（编译器生成序列）。
}
NPC_Teleport::~NPC_Teleport() {}

int Teleport::check_rule(CUser* user)
{
    // ORIG 0x08283c5a：状态/渠道/vill/aura 冷却校验链（报告 Teleport/check_rule.md）。
    if (user == 0)
    {
        return 1;
    }
    int result;
    if (user->get_state() != 3)
    {
        result = 0x13;
    }
    else
    {
        bool pvpChannel = G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsPvPSkilTreeChannel();
        if (pvpChannel)
        {
            result = 0x13;
        }
        else
        {
            char vill = user->getCurCharacVill();
            if (vill == 7)
            {
                result = 0xbe;
            }
            else
            {
                int remain = 0;
                if (!user->isGMUser())
                {
                    int equipped =
                        (int)user->is_equip_aura_avatar(2, remain);
                    if (equipped > 0)
                    {
                        return equipped;
                    }
                }
                int auraEnd = user->get_aura_avatar_option_value(2);
                bool expired =
                    (!user->isGMUser()) &&
                    (auraEnd > GlobalData::s_systemTime_.getCurSec());
                if (expired)
                {
                    result = 0x16;
                }
                else
                {
                    result = 0;
                }
            }
        }
    }
    return result;
}


int NPC_Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x082840f6：r = Teleport::check_rule(data)；r==0 → 返回 0。
    int r = Teleport::check_rule(data);
    if (r != 0)
    {
        return r;
    }
    return 0;
}
void NPC_Teleport::set_teleport_data(STTeleportData const& data)
{
    // ORIG 0x08284124：set_party_teleport(data.m_tp)。
    set_party_teleport(*reinterpret_cast<const stPartyTelePort*>(
        reinterpret_cast<const char*>(&data) + 4));
}

User_Teleport::User_Teleport()
{
    // ORIG 0x08284142：call Teleport ctor + 设 vptr（编译器生成序列）。
}
User_Teleport::~User_Teleport() {}
int User_Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x0828415e：单一 result 变量贯穿（基类链 + find_from_world 链式校验）。
    int result;
    result = Teleport::check_rule(data);
    if (result == 0)
    {
        CUser* target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
        if (target == 0)
        {
            result = 0x15;
        }
        else
        {
            int state = target->get_state();
            if (state == 3)
            {
                char vill = target->getCurCharacVill();
                bool special = vill == 7 || vill == 8 || vill == 10;
                if (special)
                {
                    result = 4;
                }
                else
                {
                    int areaIdx = target->get_area(false);
                    char hacked =
                        CHackLog_InvalidAreaMove::checkSeriaRoom((int)vill, areaIdx);
                    bool seriaOk = hacked != 0;
                    if (seriaOk)
                    {
                        result = 4;
                    }
                    else
                    {
                        int assault = target->GetAssaultPlace();
                        if (assault != 0)
                        {
                            result = 0x9c;
                        }
                        else
                        {
                            CParty* party =
                                static_cast<CParty*>(target->GetParty());
                            if (party != 0)
                            {
                                CPartyTelePort* tp = static_cast<CPartyTelePort*>(
                                    party->GetPartyTelePort());
                                if (tp->get_teleport_state() != -1)
                                {
                                    return 0x9e;
                                }
                            }
                            result = 0;
                        }
                    }
                }
            }
            else
            {
                result = 4;
            }
        }
    }
    return result;
}
void User_Teleport::set_teleport_data(STTeleportData const& data)
{
    // ORIG 0x082842b8：find_from_world(data.field00)；命中则取 vill/area/pos/
    // direction 组装局部 stPartyTelePort 后 set_party_teleport。
    CUser* target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target != 0)
    {
        // ORIG 0x082842b8：局部 stPartyTelePort 不 reset（未设字段为栈垃圾），
        // 仅填 5 字段后 set_party_teleport——保持 ORIG 原样，不额外清零。
        stPartyTelePort tp;
        tp.m_village = (unsigned char)target->getCurCharacVill();
        tp.m_areaIndex = (unsigned char)target->get_area(false);
        tp.m_posx = (short)target->get_posX();
        tp.m_posy = (short)target->get_posY();
        tp.m_direction = (unsigned char)target->get_direction();
        set_party_teleport(tp);
    }
}
void* random_option::GetRandomOption()
{
    // ORIG T 0x085f0eb0：返回 &m_randomOption（CDataManager+0x4e10，objdump 实证）。
    return &G_CDataManager()->m_randomOption;
}
random_option::CRandomOptionItemHandle::CRandomOptionItemHandle()
{
    // ORIG 0x085f0d64：new[0x7f28] 13×CMTRand，逐元素 seed(time(0))；
    // 字段清零；+0x14 = GetRandomOption()+0x148 的指针。
    m_rand = new CMTRand[13];
    unsigned long t = (unsigned long)time(0);
    for (int i = 0; i < 13; ++i)
    {
        m_rand[i].seed(t);
    }
    m_field08 = 0;
    m_disabled = 0;
    m_field10 = 0;
    // ORIG 0x85f0e2d..0x85f0e3b：m_optionSet = GetRandomOption() + 0x148
    // （m_randomOption 内嵌 provider 对象基址 m_providerNormal148；虚槽0/1 见 RandomOptionScript.h）。
    m_optionSet = (char*)GetRandomOption() + 0x148;
}
random_option::CRandomOptionItemHandle::~CRandomOptionItemHandle()
{
    // ORIG 0x085f0e46（推断）：释放 CMTRand 数组。
    delete[] m_rand;
    m_rand = 0;
}

void CHumanCertifier::init()
{
    // ORIG 0x082a73cb：遍历 0..0x63 号人证字符串表（.data 0x936e2a0，64 项
    // char*），以字符串为键向 static map<string, vector<int>>
    // humanCertifyTable_（0x93f5758）插入下标。
    // 表指针原始值为 0x0814af00+2i（指向 .text，运行时不可用），重建工程无法
    // 还原内容，故以空串占位：循环结构/符号保持真实，表内容缺失（推断）。
    for (int i = 0; i <= 0x63; ++i)
    {
        static const char* const emptyStr[64] = {0};
        const char* s = emptyStr[i];
        if (s == 0 || *s == 0)
        {
            continue;
        }
        humanCertifyTable_[std::string(s)].push_back(i);
    }
}

void* CreateAntiServerObj(const char* path)
{
    // ORIG W 0x082a6539：path + "libantisvrimport.so" → dlopen(2) →
    // dlsym("GetAntiBotInterfaceAll") → 无参调用并返回结果。
    char buf[0x118];
    memset(buf, 0, 0x41);   // ORIG 0x082a654f：仅清前 0x41 字节
    strncat(buf, path, strlen(path));
    strncat(buf, "libantisvrimport.so", 0x14);
    void* handle = dlopen(buf, RTLD_NOW);
    const char* err = dlerror();
    if (err != 0)
    {
        printf("Open Dll %s failed, %s\n", buf, err);
        return 0;
    }
    void* fn = dlsym(handle, "GetAntiBotInterfaceAll");
    err = dlerror();
    if (err != 0)
    {
        dlclose(handle);
        printf("dlsym GetAntiBotInterfaceAll failed, %s\n", err);
        return 0;
    }
    return ((void* (*)())fn)();
}

// ---- CDBEnviroment ----
// ORIG W 0x082aa8e4：CEnvironment::CEnvironment() + m_dbType(+0x3e0)=0。
CDBEnviroment::CDBEnviroment()
    : m_dbType((ENUM_SERVER_GROUP)0)
{
}

// ORIG W 0x082aa90e：vptr 复位 + CEnvironment::~CEnvironment()。
CDBEnviroment::~CDBEnviroment()
{
}

// ---- CDBEnviroment::parse ORIG @0x0829d8e8（GameDbType 环境脚本解析）----
// strtok(buf, "=") 取两 token 并 trim；首 token 为 "game_db_type" 时按第二
// token 文字映射 m_dbType（cain=1/diregie=2/siroco=3/prey=4/casillas=5/
// hilder=6/ruke=7/seria=8/anton=9/roxy=0xa/bakal=0xb/ozma=0xc/khazan=0xd/
// first1=0x63/first2=0x64/starter_server=0x11），并把空 map 存入
// G_CEnvironment()->m_serverGroupMap[m_dbType]；未知值 cMyTrace(0x2032,5)
// "Unknown value (%s)" 返 0。否则在 m_serverGroupMap[m_dbType] 中查找，
// 命中则 make_pair(p1,p2) insert。恒返 1。
// m_serverGroupMap 为 CEnvironment private，用 +0x50 偏移组件视图访问（P1）。
struct CDBEnvServerGroupMapView
{
    char m_pad[0x50];
    std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > m_map;
};

bool CDBEnviroment::parse(char* buf)
{
    const char* sep = "=";
    char* p1 = strtok(buf, sep);
    char* p2 = strtok(0, sep);
    p1 = trim(p1);
    p2 = trim(p2);
    if (strcmp(p1, "game_db_type") == 0)
    {
        if (strcmp(p2, "cain") == 0)
            m_dbType = (ENUM_SERVER_GROUP)1;
        else if (strcmp(p2, "diregie") == 0)
            m_dbType = (ENUM_SERVER_GROUP)2;
        else if (strcmp(p2, "siroco") == 0)
            m_dbType = (ENUM_SERVER_GROUP)3;
        else if (strcmp(p2, "prey") == 0)
            m_dbType = (ENUM_SERVER_GROUP)4;
        else if (strcmp(p2, "casillas") == 0)
            m_dbType = (ENUM_SERVER_GROUP)5;
        else if (strcmp(p2, "hilder") == 0)
            m_dbType = (ENUM_SERVER_GROUP)6;
        else if (strcmp(p2, "ruke") == 0)
            m_dbType = (ENUM_SERVER_GROUP)7;
        else if (strcmp(p2, "seria") == 0)
            m_dbType = (ENUM_SERVER_GROUP)8;
        else if (strcmp(p2, "anton") == 0)
            m_dbType = (ENUM_SERVER_GROUP)9;
        else if (strcmp(p2, "roxy") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xa;
        else if (strcmp(p2, "bakal") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xb;
        else if (strcmp(p2, "ozma") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xc;
        else if (strcmp(p2, "khazan") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0xd;
        else if (strcmp(p2, "first1") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x63;
        else if (strcmp(p2, "first2") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x64;
        else if (strcmp(p2, "starter_server") == 0)
            m_dbType = (ENUM_SERVER_GROUP)0x11;
        else
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2032, 5);
            t("Unknown value (%s)", p2);
            return 0;
        }
        std::map<std::string, std::string> tmp;
        ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map[m_dbType] = tmp;
    }
    else
    {
        CDBEnvServerGroupMapView* env = (CDBEnvServerGroupMapView*)G_CEnvironment();
        std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> >::iterator it =
            env->m_map.find(m_dbType);
        if (it != env->m_map.end())
        {
            it->second.insert(std::make_pair(p1, p2));
        }
    }
    return 1;
}

// ============================================================================
// 静态成员定义（平凡/指针；对象型成员由全局 TU 定义，避免单 TU 静态初始化）
// ============================================================================

bool GlobalData::s_server_stop = false;
std::map<std::string, std::vector<int> > CHumanCertifier::humanCertifyTable_;  // ORIG B 0x93f5758
CSystemTime GlobalData::s_systemTime_;
CaptchaGen GlobalData::cg_;
MsgQueueMgr* GlobalData::s_msgq_mgr = 0;
StreamPool* GlobalData::s_stream_pool = 0;
PacketPool* GlobalData::s_packet_pool_list[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
BigStreamPool* GlobalData::s_big_stream_pool = 0;
SmallStreamPool* GlobalData::s_small_stream_pool = 0;
DBMgr* GlobalData::s_db_mgr = 0;
DBThread* GlobalData::s_db_thr = 0;
DBThread* GlobalData::s_db_thr_for_logDB = 0;
DispatchThread* GlobalData::s_dispatch_thr = 0;
NetworkThread* GlobalData::s_net_thr = 0;
UdpThread* GlobalData::s_udp_thr = 0;
AcceptThread* GlobalData::s_accept_thr = 0;
MonitorTcpThread* GlobalData::s_monitor_tcp_thr = 0;
CServerProxyMgr<CMonitorServerProxy>* GlobalData::s_monitor_proxy_mgr = 0;
CServerProxyMgr<CGuildServerProxy>* GlobalData::s_guild_proxy_mgr = 0;
CServerProxyMgr<CStatisticServerProxy>* GlobalData::s_statistic_proxy_mgr = 0;
CHadesServerProxy* GlobalData::s_hades_proxy = 0;
CDoubleConnCheckServerProxy* GlobalData::s_double_check_proxy = 0;
// 对象型成员（ORIG 地址：s_accept_lock @0940bdcc (0x18)、s_accept_que @0940be00 (0x28)）；
// 定义置于本 TU（GlobalData.h 注释的"全局 TU"即本文件，s_systemTime_/cg_ 同区）。
Mutex GlobalData::s_accept_lock;
std::queue<CUser*, std::deque<CUser*> > GlobalData::s_accept_que;
CSchoolServerProxy* GlobalData::s_school_proxy = 0;
CPCRoomServerProxy* GlobalData::s_pcroom_proxy = 0;
CCommunityServerProxy* GlobalData::s_community_proxy = 0;
CAuctionServerProxy* GlobalData::s_auction_proxy = 0;
CCeraAuctionServerProxy* GlobalData::s_cera_auction_proxy = 0;
CPowerManager* GlobalData::s_power_manager = 0;
secretshop::CSecretShop* GlobalData::s_secret_shop = 0;
CFatigueBatteryHandle* GlobalData::s_fatigueBatteryHandle_ = 0;
CmmChannelProxy* GlobalData::s_channel_proxy = 0;
CRijndael* GlobalData::s_pcryptRijndael_CharacView_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_Password_ = 0;
CRijndael* GlobalData::s_pcryptRijndael_MousePassword_ = 0;
Cipher* GlobalData::s_secu_cipher_keystring_manager_[0x4e20] = {0};
CTEA* GlobalData::s_pcryptTEA_MousePassword_ = 0;
CTEA* GlobalData::s_pcryptTEA_SecurityCard_ = 0;
SPremiumInfo g_SPremiumInfo[0x6a];             // 09483a40；全局 C 名（无命名空间，ORIG B）；静态初始化逐元素构造（ORIG _GLOBAL__I_g_SPremiumInfo 0x86b080a）
WongWork::CSimpleSSO* GlobalData::s_psimpleSSO = 0;
WongWork::CSecurityCardCenter* GlobalData::s_securityCardCenter = 0;
WongWork::CGMAccounts* GlobalData::s_pGMAccounts_ = 0;
CEventManager* GlobalData::s_event_manager = 0;
WongWork::IPG::CIPGHelper* GlobalData::s_pIPGHelper = 0;
InstanceRentalSystem* GlobalData::s_rentalSystem = 0;
RefPvpGrade* GlobalData::s_ref_pvp_grade = 0;
WongWork::CLogGameChannel* GlobalData::s_pLogGameChannel = 0;
WongWork::CCeraShop* GlobalData::s_pCeraShop = 0;
CDBConnector* GlobalData::s_db_connector = 0;
void* GlobalData::s_pCompoundAvatar = 0;
void* GlobalData::s_pCompoundBindCube = 0;
void* GlobalData::s_pTowerOfDespairMgr = 0;
Event_Upgrade_Jar* GlobalData::s_pEvent_UpGrade_Jar = 0;
CGM_Manager* GlobalData::s_GM_Manager = 0;
ItemVendingMachine* GlobalData::s_pItemVendingMachine = 0;
CStatisticMgr* GlobalData::s_statisticMgr = 0;
expert_job::CExpertJobMgr* GlobalData::s_ExpertJobMgr = 0;
break_away_prevent::CBreakAwayPreventSystem* GlobalData::s_BreakAwaySys = 0;
Gm_List* GlobalData::gmList = 0;
village_object::CVillageObjectMgr* GlobalData::s_villageObjectMgr = 0;
game_master::CGameMasterMgr* GlobalData::s_GameMasterMgr = 0;
village_attacked::CVillageMonsterMgr* GlobalData::s_villageMonsterMgr = 0;
village_attacked::CRevengeDungeon* GlobalData::s_revengeDungeonMgr = 0;
CDailyScheduleManager* GlobalData::s_DailyScheduleManager = 0;
online_preliminary::COnlinePreliminaryTeamMgr* GlobalData::s_onlinePreliminaryTeamMgr = 0;
Secu_DataControl* GlobalData::s_pSecuDataControl = 0;
CSecu_ProtectionField* GlobalData::s_pSecuProtectionField = 0;
WongWork::CAutoPunishRuleBlackIPMgr* GlobalData::g_autoPunishRuleBlackIPMgr = 0;
WongWork::CAutoPunishRuleHackTypeMgr* GlobalData::g_autoPunishRuleHackTypeMgr = 0;
WongWork::CBlackIPMonitor* GlobalData::g_blackIPMonitor = 0;
unsigned int GlobalData::g_IPMonitorLastDBAccessTime = 0;
CBusinessImpl* GlobalData::s_antibotBusinessImpl_ = 0;
void* GlobalData::s_antibotChecker_ = 0;
CItemAmplifier* GlobalData::s_itemAmplifier_ = 0;
CGlobalEffectManager* GlobalData::s_globalEffectManager_ = 0;
ARAD::Arad_ServerStateManager* GlobalData::s_serverStateManager_ = 0;
CTimeGate* GlobalData::s_timeGate_ = 0;
unsigned int GlobalData::g_GoldControlDateFlag = 0;
StackableAction::Action* GlobalData::g_Actions = 0;
expert_extraction::CExtractionMgr* GlobalData::g_ExtractionMgr = 0;
void* GlobalData::g_pTeleport[2] = {0, 0};
random_option::CRandomOptionItemHandle* GlobalData::g_randomOptionHandle = 0;
void* GlobalData::s_pAvatarRoulette = 0;

// ============================================================================
// GlobalData::setCurrentPacketPool
// ============================================================================

bool GlobalData::setCurrentPacketPool(THREAD_TYPE type, pthread_t threadID)
{
    if (type < 0 || type > 8)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2190,
                              "[PacketPool setCurrentThreadID err] pool_type(%d), thread_id(%08x)",
                              type, threadID);
        return false;
    }

    if (pthread_equal(0, s_packet_pool_list[type]->getThreadID()) == 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x2195,
                              "[PacketPool already ThreadID setting] pool_type(%d), thread_id(%08x) alreadyID(%08x)",
                              type, threadID, s_packet_pool_list[type]->getThreadID());
        return false;
    }

    s_packet_pool_list[type]->setThreadID(threadID);
    return true;
}

// ============================================================================
// GlobalData::InitCaptcha
// ============================================================================

bool GlobalData::InitCaptcha()
{
    if (!cg_.addFont("captcha_font/SeoulHangangL.ttf", 0x24))
    {
        std::cerr << "[GlobalData::Init] cg.loadFont fail" << std::endl;
        return false;
    }

    for (int i = 0; i <= 0x27; ++i)
    {
        GenerateCaptchaData();
    }
    return true;
}

// ============================================================================
// GlobalData::GenerateCaptchaData
// ============================================================================

void GlobalData::GenerateCaptchaData()
{
    GreyImage image;
    MSG_UDP_CAPCHA_CERTIFY msg;

    memset(&msg, 0, 0x339e);
    cg_.generateText(CountryCode::taiwan, 5, (unsigned short*)&msg);
    CaptchaGen::ImageOption opt;
    opt.m_field8 = 0;
    cg_.generateImage(0xdc, 0x3c, (const unsigned short*)&msg, 5, opt, &image);
    memcpy(msg.m_image, image.getImage(), image.getWidth() * image.getHeight());
    msg.m_size = image.getWidth() * image.getHeight();
    s_capchaDataManager.push(msg);
}

// ============================================================================
// GlobalData::Fini
// ============================================================================

void GlobalData::Fini()
{
    delete s_ref_pvp_grade;
    if (s_accept_thr)
    {
        s_accept_thr->stop();
    }
    if (s_monitor_proxy_mgr)
    {
        delete s_monitor_proxy_mgr;
        s_monitor_proxy_mgr = 0;
    }
    if (s_guild_proxy_mgr)
    {
        delete s_guild_proxy_mgr;
        s_guild_proxy_mgr = 0;
    }
    if (s_statistic_proxy_mgr)
    {
        delete s_statistic_proxy_mgr;
        s_statistic_proxy_mgr = 0;
    }
    if (s_udp_thr)
    {
        s_udp_thr->stop();
    }
    if (s_net_thr)
    {
        s_net_thr->stop();
    }
    if (s_dispatch_thr)
    {
        s_dispatch_thr->stop();
    }
    if (s_db_thr_for_logDB)
    {
        s_db_thr_for_logDB->stop();
    }
    if (s_db_thr)
    {
        s_db_thr->stop();
    }
    if (s_monitor_tcp_thr)
    {
        s_monitor_tcp_thr->stop();
    }
    delete s_db_mgr;
    delete s_stream_pool;
    delete s_small_stream_pool;
    delete s_big_stream_pool;
    delete s_msgq_mgr;
    delete s_pItemVendingMachine;
    CSyncSlangFilter::FreeInstance();
    delete s_pcryptRijndael_CharacView_;
    delete s_psimpleSSO;
    delete s_pGMAccounts_;
    delete s_GM_Manager;
    delete s_antibotBusinessImpl_;
    delete s_BreakAwaySys;
    delete s_ExpertJobMgr;
    delete s_villageObjectMgr;
    delete s_villageMonsterMgr;
    delete s_revengeDungeonMgr;
    delete s_DailyScheduleManager;
    delete s_onlinePreliminaryTeamMgr;
    delete s_auction_proxy;
    delete s_cera_auction_proxy;
    delete s_db_connector;
    delete s_rentalSystem;
    for (int i = 0; i < 9; ++i)
    {
        if (s_packet_pool_list[i])
        {
            delete s_packet_pool_list[i];
            s_packet_pool_list[i] = 0;
        }
    }
}

// ============================================================================
// GlobalData::initTournamentDB
// ============================================================================

bool GlobalData::initTournamentDB(const char* fileName)
{
    typedef std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > GroupNameMap;
    typedef std::map<ENUM_SERVER_GROUP, DBInfoMap> GroupDBMap;

    if (G_GameWorld()->GetChannelType() != 7 &&
        !G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return true;
    }

    CDBEnviroment dbEnv;
    dbEnv.set_file_name(fileName);
    if (!dbEnv.load())
    {
        return false;
    }
    if (!s_db_connector)
    {
        return false;
    }

    GroupNameMap::iterator it =
        (*(GroupNameMap*)((char*)G_CEnvironment() + 0x50)).begin();
    while (it != (*(GroupNameMap*)((char*)G_CEnvironment() + 0x50)).end())
    {
        ENUM_SERVER_GROUP group = it->first;
        DBGroupMap groupMap;
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)2, group));
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)3, group));
        DBInfoMap& info = (*(GroupDBMap*)((char*)G_CEnvironment() + 0x80))[group];
        if (!s_db_connector->GetConnInfo(groupMap, info))
        {
            return false;
        }
        if (!s_db_connector->ConnectWithServerGroup(DBInfoMap(info)))
        {
            return false;
        }
        ++it;
    }
    // ORIG 0x829e040：cMyTrace 第一参即 __PRETTY_FUNCTION__
    // （rodata _ZZN10GlobalData16initTournamentDBEPKcE19__PRETTY_FUNCTION__ @08c1b100）。
    cMyTrace t(__PRETTY_FUNCTION__, 0x20aa, 0);
    t("This is Tournament PvP channel");
    s_db_mgr->activateGlobalDB();
    return true;
}

// ============================================================================
// GlobalData::Init
// ============================================================================

bool GlobalData::Init()
{
    s_server_stop = false;
    s_systemTime_.update();

    s_itemAmplifier_ = new CItemAmplifier;
    s_globalEffectManager_ = new CGlobalEffectManager;

    s_timeGate_ = new (std::nothrow) CTimeGate;
    if (!s_timeGate_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate s_timeGate_ memory" << std::endl;
        return false;
    }

    s_serverStateManager_ = new ARAD::Arad_ServerStateManager;

    s_msgq_mgr = new MsgQueueMgr;
    if (!s_msgq_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate msg queue memory" << std::endl;
        return false;
    }

    s_stream_pool = new StreamPool;
    if (!s_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate stream pool memory" << std::endl;
        return false;
    }

    for (int i = 0; i < 9; ++i)
    {
        s_packet_pool_list[i] = new PacketPool;
        if (!s_packet_pool_list[i])
        {
            std::cerr << "[GlobalData::Init] can't allocate packet pool memory" << std::endl;
            return false;
        }
    }
    setCurrentPacketPool(THREAD_TYPE_ACCEPT, pthread_self());

    s_small_stream_pool = new SmallStreamPool;
    if (!s_small_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate small stream pool memory" << std::endl;
        return false;
    }

    s_big_stream_pool = new BigStreamPool;
    if (!s_big_stream_pool)
    {
        std::cerr << "[GlobalData::Init] can't allocate big stream pool memory" << std::endl;
        return false;
    }

    s_db_mgr = new DBMgr;
    if (!s_db_mgr)
    {
        std::cerr << "[GlobalData::Init] can't allocate db manager memory" << std::endl;
        return false;
    }

    s_db_thr = new DBThread;
    if (!s_db_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory" << std::endl;
        return false;
    }
    s_db_thr->setLogDB(false);

    s_db_thr_for_logDB = new DBThread;
    if (!s_db_thr_for_logDB)
    {
        std::cerr << "[GlobalData::Init] Can't allocate db thread memory, s_db_thr_for_logDB"
                  << std::endl;
        return false;
    }
    s_db_thr_for_logDB->setLogDB(true);

    s_dispatch_thr = new DispatchThread;
    if (!s_dispatch_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate dispath thread memory" << std::endl;
        return false;
    }

    s_net_thr = new NetworkThread;
    if (!s_net_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate network thread memory" << std::endl;
        return false;
    }

    s_udp_thr = new UdpThread;
    if (!s_udp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate udp thread memory" << std::endl;
        return false;
    }

    s_monitor_tcp_thr = new (std::nothrow) MonitorTcpThread;
    if (!s_monitor_tcp_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor tcp thread memory" << std::endl;
        return false;
    }

    s_monitor_proxy_mgr = new (std::nothrow) CServerProxyMgr<CMonitorServerProxy>;
    if (!s_monitor_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate monitor server proxy memory" << std::endl;
        return false;
    }

    s_guild_proxy_mgr = new (std::nothrow) CServerProxyMgr<CGuildServerProxy>;
    if (!s_guild_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate guild server proxy memory" << std::endl;
        return false;
    }

    s_statistic_proxy_mgr = new (std::nothrow) CServerProxyMgr<CStatisticServerProxy>;
    if (!s_statistic_proxy_mgr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate statistic server proxy memory" << std::endl;
        return false;
    }

    char* hadesIP = G_CEnvironment()->get_udp_ip_hades();
    int hadesTCP = G_CEnvironment()->get_tcp_port_hades();
    int hadesUDP = G_CEnvironment()->get_udp_port_hades();
    s_hades_proxy = new (std::nothrow) CHadesServerProxy(hadesIP, hadesTCP, hadesUDP);
    if (!s_hades_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Hades server proxy memory" << std::endl;
        return false;
    }
    if (!s_hades_proxy->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (Hades server proxy)" << std::endl;
        return false;
    }

    {
        std::string doubleCheckIP(G_CEnvironment()->get_udp_ip_doublecheck());
        s_double_check_proxy = new (std::nothrow)
            CDoubleConnCheckServerProxy(doubleCheckIP,
                                        G_CEnvironment()->get_udp_port_doublecheck());
        if (!s_double_check_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate double conn check server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_double_check_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (double conn check server proxy)"
                      << std::endl;
            return false;
        }
    }

    if (*G_CEnvironment()->get_school_server_ip() != '\0' &&
        G_CEnvironment()->get_school_server_port() >= 1)
    {
        std::string schoolIP(G_CEnvironment()->get_school_server_ip());
        s_school_proxy = new (std::nothrow)
            CSchoolServerProxy(schoolIP, G_CEnvironment()->get_school_server_port());
        if (!s_school_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate school server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_school_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (school server proxy)" << std::endl;
            return false;
        }
    }

    s_pcroom_proxy = new (std::nothrow)
        CPCRoomServerProxy(G_CEnvironment()->get_pcroom_server_ip(),
                           G_CEnvironment()->get_pcroom_server_port());
    if (!s_pcroom_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pcroom server proxy memory" << std::endl;
        return false;
    }

    s_community_proxy = new (std::nothrow)
        CCommunityServerProxy(G_CEnvironment()->get_community_server_ip(),
                              G_CEnvironment()->get_community_server_port());
    if (!s_community_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate community server proxy memory"
                  << std::endl;
        return false;
    }

    s_auction_proxy = new (std::nothrow)
        CAuctionServerProxy(G_CEnvironment()->get_auction_server_ip(),
                            G_CEnvironment()->get_auction_server_port(), "Auction Server");
    if (!s_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate auction server proxy memory" << std::endl;
        return false;
    }

    s_cera_auction_proxy = new (std::nothrow)
        CCeraAuctionServerProxy(G_CEnvironment()->get_cera_auction_server_ip(),
                                G_CEnvironment()->get_cera_auction_server_port(),
                                "Cera Auction Server");
    if (!s_cera_auction_proxy)
    {
        std::cerr << "[GlobalData::Init] Can't allocate cera auction server proxy memory"
                  << std::endl;
        return false;
    }

    s_power_manager = new (std::nothrow) CPowerManager;
    if (!s_power_manager)
    {
        std::cerr << "[GlobalData::Init] Can't allocate power manager memory" << std::endl;
        return false;
    }
    if (!s_power_manager->Init())
    {
        std::cerr << "[GlobalData::Init] Fail Init (power manager memory)" << std::endl;
        return false;
    }
    CPowerManager::LoadPowerWarInfo();
    CPowerManager::LoadRankerInfo();

    s_fatigueBatteryHandle_ = new (std::nothrow) CFatigueBatteryHandle;
    if (!s_fatigueBatteryHandle_)
    {
        std::cerr << "[GlobalData::Init] Can't allocate Fatigue Battery Handle memory"
                  << std::endl;
        return false;
    }

    s_secret_shop = new (std::nothrow) secretshop::CSecretShop;
    if (!s_secret_shop)
    {
        std::cerr << "[GlobalData::Init] Can't allocate SecretShop Handle memory" << std::endl;
        return false;
    }

    {
        std::string channelIP(G_CEnvironment()->get_udp_ip_channel());
        s_channel_proxy = new (std::nothrow)
            CmmChannelProxy(channelIP, G_CEnvironment()->get_udp_port_channel());
        if (!s_channel_proxy)
        {
            std::cerr << "[GlobalData::Init] Can't allocate channel server proxy memory"
                      << std::endl;
            return false;
        }
        if (!s_channel_proxy->Init())
        {
            std::cerr << "[GlobalData::Init] Fail Init (channel server proxy)" << std::endl;
            return false;
        }
    }

    s_pcryptRijndael_CharacView_ = new CRijndael;
    s_pcryptRijndael_CharacView_->Initialize("wongyaofejsvk007", 0x20,
                                             CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_Password_ = new CRijndael;
    s_pcryptRijndael_Password_->Initialize("wongyaofneople00", 0x20,
                                           CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    s_pcryptRijndael_MousePassword_ = new CRijndael;
    s_pcryptRijndael_MousePassword_->Initialize("ejsvk@@key4mouse", 0x20,
                                                CRijndael::sm_chain0, 0x10, 0x10, 0, 0);

    s_pcryptTEA_MousePassword_ = new CTEA;
    s_pcryptTEA_MousePassword_->Initialize("tjsghTlWkdajtwla", 0x10, "", 0, 0);
    s_pcryptTEA_SecurityCard_ = new CTEA;
    s_pcryptTEA_SecurityCard_->Initialize("ejsvk@@@secucard", 0x10, "", 0, 0);

    s_pLogGameChannel = new (std::nothrow) WongWork::CLogGameChannel;
    s_pLogGameChannel->StartLogTimer();

    s_pCeraShop = new (std::nothrow) WongWork::CCeraShop;
    s_pItemVendingMachine = new (std::nothrow) ItemVendingMachine;

    s_pSecuDataControl = new Secu_DataControl;
    s_pSecuProtectionField = new CSecu_ProtectionField;

    g_autoPunishRuleBlackIPMgr = new WongWork::CAutoPunishRuleBlackIPMgr;
    g_autoPunishRuleBlackIPMgr->reset();
    g_autoPunishRuleHackTypeMgr = new WongWork::CAutoPunishRuleHackTypeMgr;
    g_autoPunishRuleHackTypeMgr->reset();
    g_blackIPMonitor = new WongWork::CBlackIPMonitor;
    g_blackIPMonitor->reset();

    s_accept_thr = new AcceptThread;
    if (!s_accept_thr)
    {
        std::cerr << "[GlobalData::Init] Can't allocate accept thread memory" << std::endl;
        return false;
    }

    s_ref_pvp_grade = new RefPvpGrade;
    if (!s_ref_pvp_grade)
    {
        std::cerr << "[GlobalData::Init] Can't allocate pvp grade ref" << std::endl;
        return false;
    }

    s_db_connector = new (std::nothrow) CDBConnector(s_db_mgr);
    if (!s_db_connector)
    {
        return false;
    }

    DBInfoMap& masterDbInfo =
        *(DBInfoMap*)((char*)G_CEnvironment() + 0x68);
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;

    {
        DBGroupMap groupMap;
        for (int i = 1; i < 0xf; ++i)
        {
            groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)i, (ENUM_SERVER_GROUP)i));
        }
        int idx1 = 0;
        STDBConnInfo& db1 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx1];
        int idx2 = 0;
        STDBConnInfo& db2 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx2];
        int idx3 = 0;
        STDBConnInfo& db3 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx3];
        unsigned int masterPort = *(unsigned int*)((char*)&db3 + 0x14);
        int idx4 = 0;
        STDBConnInfo& db4 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx4];
        (void)serverGroup;
        cMyTrace t1(__PRETTY_FUNCTION__, 0x1b0c, 9);
        t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ", &db4, masterPort,
           (char*)&db2 + 0x18, (char*)&db1 + 0x42);
        int idx0 = 0;
        STDBConnInfo& db0 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx0];
        if (!s_db_connector->Init(db0))
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1b0f, 9);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3(__PRETTY_FUNCTION__, 0x1b12, 9);
        t3("Success");
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1b14, 9);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5(__PRETTY_FUNCTION__, 0x1b17, 9);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6(__PRETTY_FUNCTION__, 0x1b1a, 9);
        t6("Success");
        cMyTrace t7(__PRETTY_FUNCTION__, 0x1b1c, 9);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8(__PRETTY_FUNCTION__, 0x1b1f, 9);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9(__PRETTY_FUNCTION__, 0x1b22, 9);
        t9("\t- All DB Open Success");
    }

    s_psimpleSSO = new WongWork::CSimpleSSO;
    s_psimpleSSO->init(s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0));

    s_securityCardCenter = new WongWork::CSecurityCardCenter;
    s_securityCardCenter->init();

    s_pIPGHelper = new (std::nothrow) WongWork::IPG::CIPGHelper;
    if (s_pIPGHelper->Initialize(1, 5000) < 0)
    {
        puts("Fail to init IPG Module");
        delete s_pIPGHelper;
        return false;
    }

    CHumanCertifier::init();

    s_pGMAccounts_ = new WongWork::CGMAccounts;

    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x1d4d, 9);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1d50, 9);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3(__PRETTY_FUNCTION__, 0x1d55, 9);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1d58, 9);
        t4("Success");
    }

    bool syncEnable = false;
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        syncEnable = true;
    }
    sync_script::GetInstanceSyncScript()->SetEnable(syncEnable);
    if (sync_script::GetInstanceSyncScript()->IsEnable())
    {
        int webIdx = 5;
        STDBConnInfo& web1 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx];
        int webIdx2 = 5;
        STDBConnInfo& web2 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx2];
        int webIdx3 = 5;
        STDBConnInfo& web3 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx3];
        int webIdx4 = 5;
        STDBConnInfo& web4 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx4];
        unsigned int webPort = *(unsigned int*)((char*)&web4 + 0x14);
        int webIdx5 = 5;
        STDBConnInfo& web5 =
            (*(DBInfoMap*)((char*)G_CEnvironment() + 0x68))[(ENUM_DB_HANDLE_IDX)webIdx5];
        if (!s_db_mgr->Open((ENUM_DB_HANDLE_IDX)5, (char*)&web5, webPort,
                            (char*)&web3 + 0x42, (char*)&web2 + 0x18, (char*)&web1 + 0x2d,
                            (ENUM_SERVER_GROUP)0))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1d8d, 9);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d90, 9);
        t("\t- Success WEB DB OPEN");
    }

    bool slangFail;
    if (CSyncSlangFilter::GetInstance()->AddSlangListName() == 1)
    {
        if (CSyncSlangFilter::GetInstance()->AddSlangList() != 1)
        {
            slangFail = true;
        }
        else
        {
            slangFail = false;
        }
    }
    else
    {
        slangFail = true;
    }

    if (slangFail)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d9f, 9);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1da8, 9);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t(__PRETTY_FUNCTION__, 0x1dc6, 9);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dcd, 9);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dd6, 9);
        t("Can't allocate memory for a CGM_Manager");
        return false;
    }

    {
        int keyLen = G_Cipher()->GetTotalKeyLength();
        unsigned char key[1024];
        for (int i = 0; i < keyLen; ++i)
        {
            key[i] = (unsigned char)rand();
        }
        G_Cipher()->Initialize(key, keyLen);
    }

    CStatisticMgr* statMgr =
        (CStatisticMgr*)operator new(0x30, std::nothrow);
    if (statMgr)
    {
        int* q = (int*)statMgr;
        for (int i = 0xc; i != 0; --i)
        {
            *q = 0;
            q += 4;
        }
        new (statMgr) CStatisticMgr();
    }
    s_statisticMgr = statMgr;
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1df7, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dfe, 9);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e05, 9);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e0c, 9);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e13, 9);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e1b, 9);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e24, 9);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e2d, 9);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e36, 9);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e47, 9);
        t("Can't allocate memory for a CGameMasterMgr");
        return false;
    }

    TimerUpdatePvPGrade::registNextTimer(s_systemTime_.getCurSec());
    DB_ReloadAutoPunishRuleBackIP::makeRequest();
    DB_ReloadAutoPunishRuleHackType::makeRequest();
    DB_LoadCleanPadPoint::makeRequest();
    DB_BlackIPMonitor::makeRequest();

    s_antibotBusinessImpl_ = new CBusinessImpl;
    s_antibotChecker_ = CreateAntiServerObj("./");
    if (!s_antibotChecker_)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eb0, 5);
        t("AntiBot create fail");
        return false;
    }

    {
        char seq[4] = {0, 0, 0, 0};
        // ORIG 0x8471ebd：G_CEnvironment()->getProcessSequence() 真实读环境进程序列号
        sprintf(seq, "%d", G_CEnvironment()->getProcessSequence());
        typedef void (*AntibotInitFn)(void*, void*, char*);
        (*(AntibotInitFn*)s_antibotChecker_)(s_antibotChecker_, s_antibotBusinessImpl_, seq);
    }

    CStatisticMgr* statMgr2 =
        (CStatisticMgr*)operator new(0x30, std::nothrow);
    if (statMgr2)
    {
        int* q = (int*)statMgr2;
        for (int i = 0xc; i != 0; --i)
        {
            *q = 0;
            q += 4;
        }
        new (statMgr2) CStatisticMgr();
    }
    s_statisticMgr = statMgr2;
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ebf, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ed9, 9);
        t("Can't allocate memory for a StackableAction::Action()");
        return false;
    }

    {
        Stream* s = s_stream_pool->Acquire("App.cpp", 0x1ee0);
        CStreamGuard guard(s, true);
        **guard << 0x277;
        **guard << -1;
        s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    if (InitCaptcha())
    {
        Timer_GenerateCaptchaData::RegistNextTimer(3);
        g_ExtractionMgr = new (std::nothrow) expert_extraction::CExtractionMgr;
        if (!g_ExtractionMgr)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1ef5, 9);
            t("Can't allocate memory for a CExtractionMng");
            return false;
        }

        g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
        if (!g_pTeleport[0])
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1f02, 9);
            t("Can't allocate memory for a NPC_Teleport()");
            return false;
        }

        g_pTeleport[1] = new (std::nothrow) User_Teleport;
        if (!g_pTeleport[1])
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1f09, 9);
            t("Can't allocate memory for a User_Teleport()");
            return false;
        }

        g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
        if (!g_randomOptionHandle)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1f12, 9);
            t("Can't allocate memory for a CRandomOptionItemHandle");
            return false;
        }

        ServiceRestrictManager* srm = ARAD::Singleton<ServiceRestrictManager>::Get();
        if (srm->initialize() == 1)
        {
            ARAD::Singleton<ServiceRestrictManager>::Get()->savePolicy();
            ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
            RestrictGeolocation* geo = ARAD::Singleton<RestrictGeolocation>::Get();
            if (geo->initGeolocation() == 1)
            {
                return true;
            }
            cMyTrace t(__PRETTY_FUNCTION__, 0x1f2a, 9);
            t("Can't initialize GeoIP");
            return false;
        }
        else
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1f1f, 9);
            t("Can't allocate memory for a ServiceRestrictManager");
            return false;
        }
    }
    else
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eea, 9);
        t("captcha data initialization failed");
        return false;
    }
}

// ###########################################################################
// # 租赁域（RandomRentalA2 批次）：pc_room 自由函数 + 两台租赁机器本体 +      #
// # InstanceRentalSystem::{RentalInfo,DeletedRentItem} C1 + CUser 别名       #
// # 全部逐报告转写；无报告者标注"报告缺失，objdump 直转"。                    #
// ###########################################################################

// ---- InstanceRentalSystem::RentalInfo C1 —— W 0x082719f6 ------------------
// （报告缺失，objdump 直转：清 +0 字/+2 半/+4 int/+8..+b 四字节，+0xc=-1）
InstanceRentalSystem::RentalInfo::RentalInfo()
{
    m_flag0 = 0;
    m_slot = 0;
    m_itemIndex = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_inEquip = 0;
    m_fieldB = 0;
    m_rentItemId = -1;
}

// ---- InstanceRentalSystem::DeletedRentItem C1 —— W 0x08271a3a -------------
// （报告缺失，objdump 直转：清 +0 字节/+2 半字/+4 int）
InstanceRentalSystem::DeletedRentItem::DeletedRentItem()
{
    m_bEquip = 0;
    m_slot = 0;
    m_itemId = 0;
}
// ---- InstanceRentalSystem::RentInput::RentInElem C1 —— W 0x0822f02e ------
// （objdump 直转：逐字节清 +0..+3，+4=-1；无帧无栈，每字段重读 this）
InstanceRentalSystem::RentInput::RentInElem::RentInElem()
{
    m_sectionIdx = 0;
    m_itemIdx = 0;
    m_bEquip = 0;
    m_type = 0;
    m_rentItemId = -1;
}

// ---- InstanceRentalSystem::RentInput C1 —— W 0x0822f058 ------------------
// （objdump 直转：vector C1 后清 +0xc int、+0x10/+0x11 两字节）
InstanceRentalSystem::RentInput::RentInput()
{
    m_machineKey = 0;
    m_logFlag = 0;
    m_field11 = 0;
}

// ---- CUser::GetCurrentCharacRentalInfo —— T 0x08677166 --------------------
// （GetCurrentCharacRentalInfo.md：尾调小写版 getCurrentCharacRentalInfo）
bool CUser::GetCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    return getCurrentCharacRentalInfo(out, param);
}

// makeFairPVPEquipmentPackageKey —— T 0x088945ad（InitScriptProducers_2.cpp 定义；
// DataManagerInitDepends.h 与本 TU 本地类冲突，故此处仅前向声明）
int makeFairPVPEquipmentPackageKey(int job, int idx);

namespace pc_room
{

// ---- IsPcRoomUser —— T 0x08270fea（IsPcRoomUser.md）------------------------
bool IsPcRoomUser(CUser& user)
{
    return user.GetPCRoomNo() != 0;
}

// ---- CheckRentableEnvironment —— T 0x08271002（同名报告）-------------------
bool CheckRentableEnvironment(CUser& /*user*/)
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}

// ---- CheckRentableEnvironmentAtChannelType —— T 0x08271032（同名报告）------
bool CheckRentableEnvironmentAtChannelType(CUser& /*user*/)
{
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return false;
    }
    return true;
}

// ---- GetSelectionIndex —— T 0x082711f7（同名报告全文转写）------------------
bool GetSelectionIndex(CUser& user, int& outIndex, int type)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }

    // 角色数据 +0x27 short 选择索引；isAffectedPremium(0x16) 时 +5；
    // type==2 用未加成原值（ORIG 重读）。
    int selIdx = (int)(*reinterpret_cast<const short*>(
        reinterpret_cast<const char*>(cur) + 0x27));   // [推断] 具名字段待补
    if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)0x16))
    {
        selIdx += 5;
    }
    if (type == 2)
    {
        selIdx = (int)(*reinterpret_cast<const short*>(
            reinterpret_cast<const char*>(cur) + 0x27));
    }

    int chosen = selIdx > 0 ? selIdx : 0;              // std::max(0, idx)

    EventRewardSection* section = 0;
    if (type == 1)
    {
        section = G_CDataManager()->m_pcBangRentar.getSectionData(chosen);
    }
    else if (type == 2)
    {
        section =
            G_CDataManager()->m_dimensionAct.m_rewardData.getSectionData(chosen);
    }
    if (section == 0)
    {
        return false;
    }
    outIndex = (int)section->m_field0;                 // 段首字节
    return true;
}

// ---- GetCharacterJob —— T 0x082712a6（同名报告全文转写）--------------------
bool GetCharacterJob(CUser& user, char& outJob)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }
    outJob = cur->m_field26;  // job 字节（+0x26）
    return true;
}

}  // namespace pc_room

// ---- PcBangItemRentarData::getSectionData —— T 0x08915518（同名报告）-------
EventRewardSection* PcBangItemRentarData::getSectionData(int idx)
{
    // rbegin→rend：返回首个 m_field1（段上界字节）<= idx 的段。
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}

// ---- DimensionActivationRewardData 三查询（同名报告全文转写）---------------
EventRewardSection* DimensionActivationRewardData::getSectionData(int idx)
{
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}

std::vector<std::vector<EventRewardItemInfo> >*
DimensionActivationRewardData::getRewardSelectItemList(int packageKey)
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it == m_mapC.end())
    {
        return 0;
    }
    return &it->second;
}

std::vector<std::vector<EventRewardItemInfo> >*
DimensionActivationRewardData::getRewardGiveItemList(int job, int growType)
{
    std::pair<int, int> key;
    key.first = job;
    key.second = growType;
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_map24.find(key);
    if (it == m_map24.end())
    {
        return 0;
    }
    return &it->second;
}

std::vector<std::vector<EventRewardItemInfo> >*
PcBangItemRentarData::getRewardSelectionList(int packageKey)
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it == m_mapC.end())
    {
        return 0;
    }
    return &it->second;
}

namespace pc_room
{

// ---- GetRewardItemSelectionList —— T 0x082712da（同名报告全文转写）--------
std::vector<std::vector<EventRewardItemInfo> >*
GetRewardItemSelectionList(int packageKey, register int type)
{
    if (type == 1)
    {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getRewardSelectionList(packageKey);
    }
    else if (type == 2)
    {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getRewardSelectItemList(packageKey);
    }
    return 0;
}

// ---- GetRewardSection —— T 0x08271338（同名报告全文转写）------------------
EventRewardSection* GetRewardSection(int idx, register int type)
{
    if (type == 1)
    {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getSectionData(idx);
    }
    else if (type == 2)
    {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getSectionData(idx);
    }
    return 0;
}

// ---- GetRentItemInfo —— T 0x08271396（同名报告全文转写）--------------------
bool GetRentItemInfo(CUser& user, char sectionIdx, char seqIdx,
                     EventRewardItemInfo& out, int type)
{
    int selectionIdx = 0;
    if (!GetSelectionIndex(user, selectionIdx, type))
    {
        return false;
    }
    char job = 0;
    if (!GetCharacterJob(user, job))
    {
        return false;
    }

    int packageKey =
        makeFairPVPEquipmentPackageKey((int)job, selectionIdx);

    std::vector<std::vector<EventRewardItemInfo> >* list = 0;
    if (type == 1)
    {
        list = G_CDataManager()->m_pcBangRentar.getRewardSelectionList(packageKey);
    }
    else if (type == 2)
    {
        list = G_CDataManager()
                   ->m_dimensionAct.m_rewardData.getRewardSelectItemList(
                       packageKey);
    }
    if (list == 0)
    {
        return false;
    }

    if (sectionIdx < 0 || (int)list->size() <= sectionIdx)
    {
        return false;
    }
    std::vector<EventRewardItemInfo>& inner = list->at((std::size_t)sectionIdx);
    if (seqIdx < 0 || (int)inner.size() <= seqIdx)
    {
        return false;
    }
    out = inner[(std::size_t)seqIdx];
    return true;
}

// ---- CheckInventoryEquipmentEmptyCount —— T 0x08271062（同名报告全文转写）--
bool CheckInventoryEquipmentEmptyCount(CUser& user, int need)
{
    return user.getCurCharacInvenR()->check_empty_count(
        (Inven_Item::ITEM_TYPE)1, need);
}

// ---- CheckInventoryEmptyCount —— T 0x0827108c（同名报告全文转写）-----------
bool CheckInventoryEmptyCount(CUser& user, int need)
{
    const CInventory* inv = user.getCurCharacInvenR();
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)1, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)2, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)3, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)10, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)9, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)5, need))
    {
        return true;
    }
    if (!inv->check_empty_count((Inven_Item::ITEM_TYPE)6, need))
    {
        return true;
    }
    if (!(inv->GetRemainCapacity((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) <
          need))
    {
        return true;
    }
    return false;
}

// ---- DeleteAllRentalItemInSameSectionForNewRent —— T 0x08271807 ------------
void DeleteAllRentalItemInSameSectionForNewRent(
    CUser& user,
    std::vector<InstanceRentalSystem::DeletedRentItem>& deleted, int rentalType)
{
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (!user.GetCurrentCharacRentalInfo(rentalList, rentalType))
    {
        return;
    }

    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        const int itemId = it->m_itemIndex;   // RentalInfo+0x04

        int slot = 0;
        bool isEquip = false;
        CInventory* invR =
            const_cast<CInventory*>(user.getCurCharacInvenR());
        if (!invR->CheckExistItemInInventoryOrEquipment(itemId, slot, isEquip))
        {
            continue;
        }

        CInventory* invW = user.getCurCharacInvenW();
        invW->ResetSlot(isEquip ? 0 : 1, slot);

        InstanceRentalSystem::DeletedRentItem entry;
        entry.m_bEquip = isEquip ? 1 : 0;
        entry.m_slot = (unsigned short)slot;
        entry.m_itemId = itemId;
        deleted.push_back(entry);
    }
}

// ---- ProcessEquipItem —— T 0x082714c5（同名报告全文转写；委托转写核对）-----
// 与 ProcessInsertItem 仅差 SetUpgrade/rentId 写入顺序、装备位插入与标记字节。
int ProcessEquipItem(CUser& user,
                     const InstanceRentalSystem::RentInElem& inElem,
                     const EventRewardItemInfo& reward,
                     InstanceRentalSystem::RentOutput& out, int reason)
{
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    const int savedRentId = inElem.m_rentItemId;

    InstanceRentalSystem::RentalInfo ri;
    item.SetUpgrade((unsigned char)reward.m_short8);  // 装备路径：先 rentId 后升级
    if (savedRentId != -1)
    {
        item.m_addInfo2 = savedRentId;                // +0x07
    }

    const short slot = (short)user.getCurCharacInvenW()->insertItemIntoEquipment(
        item, (eItemAddReason)reason, true, false);

    ri.m_flag0 = 3;                                   // 装备租借标记
    ri.m_inEquip = 1;
    ri.m_slot = (unsigned short)slot;
    ri.m_itemIndex = reward.m_field0;
    ri.m_field8 = (unsigned char)reward.m_field0;
    ri.m_field9 = (unsigned char)(reward.m_field0 >> 8);
    ri.m_fieldB = (unsigned char)(reward.m_field0 >> 24);
    ri.m_rentItemId = savedRentId;

    out.m_infos.push_back(ri);
    return 0;
}

// ---- ProcessInsertItem —— T 0x08271666（同名报告全文转写）------------------
int ProcessInsertItem(CUser& user,
                      const InstanceRentalSystem::RentInElem& inElem,
                      const EventRewardItemInfo& reward,
                      InstanceRentalSystem::RentOutput& out, int reason)
{
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    item.SetUpgrade((unsigned char)reward.m_short8);  // 插入路径：先升级后 rentId
    const int savedRentId = inElem.m_rentItemId;
    if (savedRentId != -1)
    {
        item.m_addInfo2 = savedRentId;                // +0x07
    }

    InstanceRentalSystem::RentalInfo ri;
    const short slot = (short)user.getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)reason, true, false);

    ri.m_flag0 = 0;                                   // 背包租借标记
    ri.m_inEquip = 0;
    ri.m_slot = (unsigned short)slot;
    ri.m_itemIndex = reward.m_field0;
    ri.m_field8 = (unsigned char)reward.m_field0;
    ri.m_field9 = (unsigned char)(reward.m_field0 >> 8);
    ri.m_fieldB = (unsigned char)(reward.m_field0 >> 24);
    ri.m_rentItemId = savedRentId;

    out.m_infos.push_back(ri);
    return 0;
}

}  // namespace pc_room

// ===========================================================================
// RentalMachinePcRoom / RentalMachineDimension 本体
// ===========================================================================

// ---- PcRoom::log —— T 0x082707c0（同名报告全文转写）------------------------
void RentalMachinePcRoom::log(CUser& user, int itemId)
{
    char serverInfo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverInfo = (char)user.GetServerGroup();
    }
    else
    {
        serverInfo = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    const int charNo = user.get_charac_no(-1);
    const unsigned int accId = user.get_acc_id();

    CMyFileLog flog("void RentalMachinePcRoom::log(CUser&, int)", 0x12e);
    flog(m_impl->m_data,
         "[RENT] m_id(%d) char_no(%d) server_info(%d) item_id(%d)", accId,
         charNo, (int)serverInfo, itemId);
}

// ---- Dimension::log —— T 0x0826ef16（同名报告全文转写）---------------------
void RentalMachineDimension::log(CUser& user, int itemId)
{
    char serverInfo = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverInfo = (char)user.GetServerGroup();
    }
    else
    {
        serverInfo = (char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    const int charNo = user.get_charac_no(-1);
    const unsigned int accId = user.get_acc_id();

    CMyFileLog flog("void RentalMachineDimension::log(CUser&, int)", 0x71);
    flog(m_impl->m_data,
         "[RENT] Dimension m_id(%d) char_no(%d) server_info(%d) item_id(%d)",
         accId, charNo, (int)serverInfo, itemId);
}

// ---- Dimension::checkRentalLevel —— T 0x0826f7e4（同名报告）----------------
bool RentalMachineDimension::checkRentalLevel(int level)
{
    if (0x40 < level)
    {
        return true;
    }
    return false;
}

// ---- Dimension::checkRentalGrowJob —— T 0x0826f7fc（同名报告）--------------
bool RentalMachineDimension::checkRentalGrowJob(int growType)
{
    if (growType != 0)
    {
        return true;
    }
    return false;
}

// ---- Dimension::GetRewardItemGiveList —— T 0x0826f814（同名报告：透传）-----
int RentalMachineDimension::GetRewardItemGiveList(int job, int growType)
{
    return (int)G_CDataManager()
        ->m_dimensionAct.m_rewardData.getRewardGiveItemList(job, growType);
}

// ---- Dimension::GetRentItemInfo —— T 0x0826f842（同名报告全文转写）---------
int RentalMachineDimension::GetRentItemInfo(
    CUser& user, const InstanceRentalSystem::RentInElem& inElem,
    EventRewardItemInfo& out)
{
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(user, selectionIdx, 2))
    {
        return 0;
    }
    char job = 0;
    if (!pc_room::GetCharacterJob(user, job))
    {
        return 0;
    }

    const int packageKey =
        makeFairPVPEquipmentPackageKey((int)(signed char)job, selectionIdx);

    std::vector<std::vector<EventRewardItemInfo> >* list = 0;
    if ((int)inElem.m_type == 1)
    {
        const int growType =
            (int)(signed char)user.getCurCharFirstGrowType();
        list = (std::vector<std::vector<EventRewardItemInfo> >*)
            GetRewardItemGiveList((int)(signed char)job, growType);
    }
    else
    {
        // ORIG 0x0826f900：pc_room::GetRewardItemSelectionList(packageKey, type)
        list = pc_room::GetRewardItemSelectionList(packageKey,
                                                   (int)inElem.m_type);
    }
    if (list == 0)
    {
        return 0;
    }

    const int s0 = (int)(signed char)inElem.m_sectionIdx;
    if (s0 < 0 || (int)list->size() <= s0)
    {
        return 0;
    }
    std::vector<EventRewardItemInfo>& inner = list->at((std::size_t)s0);

    const int s1 = (int)(signed char)inElem.m_itemIdx;
    if (s1 < 0 || (int)inner.size() <= s1)
    {
        return 0;
    }
    out = inner[(std::size_t)s1];
    return 1;
}

// ---- PcRoom::checkAllRentalItemScript —— T 0x082708cc（同名报告全文转写）---
bool RentalMachinePcRoom::checkAllRentalItemScript()
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >& table =
        G_CDataManager()->m_pcBangRentar.m_mapC;

    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             outer = table.begin();
         outer != table.end(); ++outer)
    {
        std::vector<std::vector<EventRewardItemInfo> >& sections =
            outer->second;
        for (std::size_t s = 0; s < sections.size(); ++s)
        {
            std::vector<EventRewardItemInfo>& items = sections[s];
            for (std::size_t i = 0; i < items.size(); ++i)
            {
                const unsigned int itemId = (unsigned)items[i].m_field0;
                CItem* item = G_CDataManager()->find_item((int)itemId);
                if (item == 0)
                {
                    std::cerr << "[RentalSystem::Ready()] Can`t find item["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleAmplify() != 1)     // vt+0x34 槽13
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "amplify upgrade) is not checked. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleCharacCargo() != 1)  // vt+0x40 槽16
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "charac cargo) is not checked. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleDisjoint() != 1)     // vt+0x38 槽14
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "disjoint) is not checked. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleGift() != 1)         // vt+0x3c 槽15
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "gift) is not checked. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->IsImpossibleUpgrade() != 1)      // vt+0x30 槽12
                {
                    std::cerr << "[RentalSystem::Ready()] Impossible contents("
                                 "upgrade) is not checked. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                if (item->GetAttachType() != 2)
                {
                    std::cerr << "[RentalSystem::Ready()] AtttachType is not "
                                 "trade delete. ["
                              << itemId << "]" << std::endl;
                    return false;
                }
                m_impl->m_set.insert(itemId);
            }
        }
    }
    return true;
}

// ---- Dimension::checkSelectionListScript —— T 0x0826f14e -------------------
// （报告全文转写；五谓词次序同 PcRoom，cerr 前缀为 RentalDimensionSystem）
int RentalMachineDimension::checkSelectionListScript(
    const std::vector<std::vector<EventRewardItemInfo> >& selectionList)
{
    for (std::size_t s = 0; s < selectionList.size(); ++s)
    {
        const std::vector<EventRewardItemInfo>& items = selectionList[s];
        for (std::size_t i = 0; i < items.size(); ++i)
        {
            const unsigned int itemId = (unsigned)items[i].m_field0;
            CItem* item = G_CDataManager()->find_item((int)itemId);
            if (item == 0)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Can`t find item["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleAmplify() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(amplify upgrade) is not checked. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleCharacCargo() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(charac cargo) is not checked. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleDisjoint() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(disjoint) is not checked. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleGift() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(gift) is not checked. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleUpgrade() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(upgrade) is not checked. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            if (item->GetAttachType() != 2)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] AtttachType is "
                             "not trade delete. ["
                          << itemId << "]" << std::endl;
                return 0;
            }
            m_impl->m_set.insert(itemId);
        }
    }
    return 1;
}

// ---- Dimension::checkAllRentalItemScript —— T 0x0826f022（同名报告转写）----
bool RentalMachineDimension::checkAllRentalItemScript()
{
    CDataManager* dm = G_CDataManager();
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >& selectMap =
        dm->m_dimensionAct.m_rewardData.m_mapC;      // +0x4b40
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = selectMap.begin();
         it != selectMap.end(); ++it)
    {
        if (checkSelectionListScript(it->second) != 1)
        {
            return false;
        }
    }
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > >& giveMap =
        dm->m_dimensionAct.m_rewardData.m_map24;     // +0x4b58
    for (std::map<std::pair<int, int>,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = giveMap.begin();
         it != giveMap.end(); ++it)
    {
        if (checkSelectionListScript(it->second) != 1)
        {
            return false;
        }
    }
    return true;
}

// ---- PcRoom::CheckupAllRentalItems —— T 0x08270d1e（同名报告全文转写）------
int RentalMachinePcRoom::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    for (std::size_t i = 0; i < input.m_elems.size(); ++i)
    {
        const InstanceRentalSystem::RentInElem& elem = input.m_elems[i];
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);                    // ORIG 虚表槽 +0x08

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) >= 0)
            {
                ++inventoryNeed;
            }
            else
            {
                return 0xcb;
            }
        }
    }
    return pc_room::CheckInventoryEmptyCount(user, inventoryNeed) ? 0 : 0xcb;
}

// ---- Dimension::CheckupAllRentalItems —— T 0x0826f52c（同名报告全文转写）---
int RentalMachineDimension::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    for (std::size_t i = 0; i < input.m_elems.size(); ++i)
    {
        const InstanceRentalSystem::RentInElem& elem = input.m_elems[i];
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            return 1;
        }
        CItem* script = G_CDataManager()->find_item(info.m_field0);
        if (script == 0)
        {
            return 1;
        }

        Inven_Item probe;
        probe.m_addInfo = info.m_field0;
        script->make_item(probe);

        if (elem.m_bEquip != 0)
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoEquipment(probe) < 0)
            {
                return 0xcb;
            }
        }
        else
        {
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) >= 0)
            {
                ++inventoryNeed;
            }
            else
            {
                return 0xcb;
            }
        }
    }
    return pc_room::CheckInventoryEquipmentEmptyCount(user, inventoryNeed) ? 0
                                                                          : 0xcb;
}

// ---- PcRoom::Rent —— T 0x082704e6（Rent.md 全文转写）------------------------
int RentalMachinePcRoom::Rent(CUser& user,
                              const InstanceRentalSystem::RentInput& input,
                              InstanceRentalSystem::RentOutput& output)
{
    // ORIG: (!IsPcRoomUser || input+0x11 字节==1) → 7。RentInput 建模中 +0x10
    // 为 m_logFlag，+0x11 为其填充位（未具名）；按报告条件组合转写。
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&input);
    if (!pc_room::IsPcRoomUser(user) || raw[0x11] == 1)
    {
        return 7;   // ENUM_ERROR 7
    }
    if (!pc_room::CheckRentableEnvironment(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        1);
    for (std::size_t i = 0; i < input.m_elems.size(); ++i)
    {
        const InstanceRentalSystem::RentInElem& elem = input.m_elems[i];
        EventRewardItemInfo info;
        if (!pc_room::GetRentItemInfo(user, (char)elem.m_sectionIdx,
                                      (char)elem.m_itemIdx, info, 1))
        {
            LogManager::logFormat(
                1, "RentalSystem.cpp",
                "virtual ENUM_ERROR RentalMachinePcRoom::Rent(CUser&, const "
                "InstanceRentalSystem::RentInput&, "
                "InstanceRentalSystem::RentOutput&)",
                0xf8,
                "Rent error. GetRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        if (elem.m_bEquip == 0)
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3d);
        }
        else
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3d);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}

// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
int RentalMachineDimension::Rent(CUser& user,
                                 const InstanceRentalSystem::RentInput& input,
                                 InstanceRentalSystem::RentOutput& output)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&input);
    if (!pc_room::CheckRentableEnvironmentAtChannelType(user))
    {
        return 7;   // ENUM_ERROR 7
    }
    if (!checkRentalLevel(user.get_charac_level()))
    {
        return 14;  // ENUM_ERROR 0xe（14）
    }
    if (!checkRentalGrowJob((int)(signed char)user.getCurCharFirstGrowType()))
    {
        return 254;   // ENUM_ERROR 0xfe（254）
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err;
    }

    pc_room::DeleteAllRentalItemInSameSectionForNewRent(user, output.m_deleted,
                                                        2);
    for (std::size_t i = 0; i < input.m_elems.size(); ++i)
    {
        const InstanceRentalSystem::RentInElem& elem = input.m_elems[i];
        EventRewardItemInfo info;
        if (!GetRentItemInfo(user, elem, info))
        {
            LogManager::logFormat(
                1, "RentalMachineDimension.cpp",
                "virtual ENUM_ERROR RentalMachineDimension::Rent(CUser&, const "
                "InstanceRentalSystem::RentInput&, "
                "InstanceRentalSystem::RentOutput&)",
                0x43,
                "Rent error. GetDimensionRentItemInfo return false. "
                "scriptSectionIndex(%d), scriptSeqIndex(%d)",
                (int)(signed char)elem.m_sectionIdx,
                (int)(signed char)elem.m_itemIdx);
            return 1;   // ENUM_ERROR 1
        }
        if (elem.m_bEquip == 0)
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3e);
        }
        else
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3e);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}

// ORIG _ZN20InstanceRentalSystem17CheckRentableItemEij T 0x082702dc【objdump】：
// machine = getMachine(type)；machine==0 → 返 1（视为可用）；否则尾调虚槽
// vptr+0x10 CheckRentableItem(itemIdx)。（OnlyOrig_CreatureJob.EJEnchanter 批补）
bool InstanceRentalSystem::CheckRentableItem(int type, unsigned int itemIdx)
{
    IRentalMachine* machine = getMachine(type);
    if (machine == 0)
        return true;
    return machine->CheckRentableItem(itemIdx);
}

// ORIG _Z18get_seed_from_datev T 0x086b1d25：year*365 + mon*30 + mday
// （ORIG 以 mon*32-mon 形态实现 *30；反汇编 0x086b1d53 读 tm_mon(+0x10)）。
int get_seed_from_date()
{
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm v;
    localtime_r(&t, &v);
    return v.tm_year * 365 + v.tm_mon * 30 + v.tm_mday;
}

// ============================================================================
// GlobalInstance 显式实例化（弱符号，COMDAT 合并；与 ORIG nm 的 W/V 一致）
//   EncryptTool: inst_ptr 0x848e1d3 / create 0x848e1e5 / m_p V 0x943d9a0
//   Cipher:      inst_ptr 0x82ab9da / create 0x82b6577 / m_p V 0x941f8a4
//   CDataManager/InterDispatcher/TimerDispatcher/Store/GameWorld: inst_ptr 等
//   ORIG 均 W/u（弱/外部引用），此处显式实例化补链接符号，不影响各权威 TU。
// ============================================================================
template class GlobalInstance<EncryptTool>;
template class GlobalInstance<CDataManager>;
template class GlobalInstance<InterDispatcher>;
template class GlobalInstance<TimerDispatcher>;
template class GlobalInstance<Store>;
template class GlobalInstance<Cipher>;
template class GlobalInstance<GameWorld>;
// ---- 全局单例访问器（ORIG W：G_TimerDispatcher 0x0847e8a4 /
// G_InterDispatcher 0x0847e8b7，GlobalInstance inst_ptr 形态，同 G_TimerQueue）----
TimerDispatcher* G_TimerDispatcher()
{
    return GlobalInstance<TimerDispatcher>::inst_ptr();
}

InterDispatcher* G_InterDispatcher()
{
    return GlobalInstance<InterDispatcher>::inst_ptr();
}

#include "LogManager.h"
// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。

#include "GlobalData.h"
#include "CCapchaDataManager.h"
#include "CExpertJob.h"      // expert_job::CExpertJobMgr/CExpertJobFactory 权威   // WongWork::CCapchaDataManager / MSG_UDP_CAPCHA_CERTIFY 权威
#include "GameMasterCmd.h"        // game_master::CGameMasterMgr（占位骨架已删，实现在 GmCore/GmRegister）
#include "CItemList.h"             // get_rand_int

#include <iostream>
#include <new>
#include <stdio.h>
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
#include "Arad_DataManager.h"  // ARAD::Singleton 唯一权威定义
#include "CInventory.h"              // CSecu_ProtectionField 权威定义（CInventory.h）
#include "CMyFileLog.h"              // 租赁 log（RandomRentalA2 批次）
#include "CUser.h"    // StackableAction proc_* 需要 CUser 完整类型
#include "CSimpleSSO.h"  // WongWork::CSimpleSSO 权威定义（原局部类已收敛）
#include "CItem.h"    // StackableAction proc_* 需要 CItem 完整类型
#include "CLogGameChannel.h"  // WongWork::CLogGameChannel 权威定义
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

#include "CCommunityServerProxy.h"
#include "CSchoolServerProxy.h"

// StaticPool/expert_job::CEnchanter/CDisjointer 权威声明已迁 CExpertJob.h
//（含 CExpertJobFactory/CExpertJobMgr），本 TU 经 include 获取。

#include "CEnvironment.h"
#include "GameTypes.h"
// PacketGuard 完整类型经 CInventory.h（权威定义点）；此处不再重复 include。
#include "CRijndael.h"
#include "Cipher.h"
#include "BaseServerProxy.h"
#include "CHadesServerProxy.h"
#include "CaptchaGenData.h"
#include "CQueryCounter.h"
#include "ThreadInterface.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "ServiceRestrictManager.h"  // 权威定义（本地类已迁移）
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "CItemAmplifier.h"
#include "CPowerManager.h"   // 权威 CPowerManager 类（替代下方本地 stub，解除 ODR）

// ============================================================================
// 依赖类最小 stub（符号/布局属后续批次；本 TU 只用于还原 GlobalData 函数）
// ============================================================================

typedef std::map<ENUM_DB_HANDLE_IDX, ENUM_SERVER_GROUP> DBGroupMap;
typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DBInfoMap;

namespace ARAD
{
class Arad_ServerStateManager
{
public:
    Arad_ServerStateManager();
private:
    // ORIG ctor 0x81a922c：bool@+0=0；Mutex@+4；map<ENUM_GLOBAL_EFFECT_ARAD,
    // AradServerStateMessage>@+0x1c（key 枚举未建模，以 int 等价，推断）。
    bool m_active;   // +0x00
    Mutex m_lock;    // +0x04（0x18 字节）
    std::map<int, AradServerStateMessage> m_stateMap;  // +0x1c
};
}

class PacketPool;

class DBThread : public ThreadInterface
{
public:
    DBThread();
    void setLogDB(bool isLogDB);
    bool isLogDB();  // ORIG T 0x083ff77a：读 +0x0c
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
};

class NetworkThread : public ThreadInterface
{
public:
    NetworkThread();
};

class UdpThread : public ThreadInterface
{
public:
    UdpThread();
};

class MonitorTcpThread : public ThreadInterface
{
public:
    MonitorTcpThread();
};

class AcceptThread : public ThreadInterface
{
public:
    AcceptThread();
private:
    // ORIG ctor 0x0857cbe8：置 +0x0c 字节为 0 → 基类 0xc 之后一个 bool，总 0x10。
    bool m_acceptFlag;  // +0x0c
};

template <class T> class CServerProxyMgr;

class CDoubleConnCheckServerProxy
{
public:
    CDoubleConnCheckServerProxy(std::string ip, int port);
    bool Init();
private:
    char m_pad[0x10];
};



class CPCRoomServerProxy
{
public:
    CPCRoomServerProxy(char* ip, int port);
private:
    // ORIG ctor 0x084717dc：CNetwork@+0x00 + string@+0x30df8。
    CNetwork<100000, 100000> m_tcp;  // +0x00
    std::string m_ip;                // +0x30df8
};



class CAuctionServerProxy
{
public:
    CAuctionServerProxy(char* ip, int port, char* name);
    ~CAuctionServerProxy();
private:
    char m_pad[0x30e10];
};

class CCeraAuctionServerProxy
{
public:
    CCeraAuctionServerProxy(char* ip, int port, char* name);
    ~CCeraAuctionServerProxy();
private:
    char m_pad[0x30e10];
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
        *reinterpret_cast<int*>(reinterpret_cast<char*>(env) + 0x198);
    *reinterpret_cast<int*>(pkt + 0x0f) = env->get_server_group();
    *reinterpret_cast<int*>(pkt + 0x13) =
        *reinterpret_cast<int*>(reinterpret_cast<char*>(env) + 0x1b0); // Server_Envir.m_channelNo（推断）
    *reinterpret_cast<int*>(pkt + 0x17) =
        *reinterpret_cast<int*>(reinterpret_cast<char*>(env) + 0x3c0);
    *reinterpret_cast<int*>(pkt + 0x1b) = static_cast<int>(
        reinterpret_cast<std::map<unsigned short, CUser*>*>(
            reinterpret_cast<char*>(G_GameWorld()) + 0x134)
            ->size());                                 // m_UserInWorld
    std::strcpy(pkt + 0x1f, reinterpret_cast<char*>(env) + 0x1b4); // Server_Envir.m_serverIP
    *reinterpret_cast<int*>(pkt + 0x2f) =
        *reinterpret_cast<int*>(reinterpret_cast<char*>(env) + 0x1c8);

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
        cMyTrace trace("int CmmChannelProxy::SendPacket(char*, int)", 200, 0);
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
        cMyTrace trace("int CmmChannelProxy::RecvPacket()", 0x114, 0);
        trace("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
              fd, e, strerror(e));
        return -1;
    }
    if (n == 0)
    {
        cMyTrace trace("int CmmChannelProxy::RecvPacket()", 0x11a, 0);
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
            "bool CmmChannelProxy::SendCheckChannelScript()", 0x143, 9,
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
            "bool CmmChannelProxy::SendCheckChannelScript()", 0x188, 9,
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

namespace WongWork
{
// CLogGameChannel 权威定义见 CLogGameChannel.h（§7 合并）。

class CAutoPunishRuleBlackIPMgr
{
public:
    CAutoPunishRuleBlackIPMgr();
    void reset();

    // ORIG 嵌套类型（reset/C1 的 multimap 符号实证）。
    struct stIPInfo_t { char m_pad[0x18]; };

private:
    std::multimap<unsigned long, stIPInfo_t> m_map;  // +0x00
};
// ---- 反挂机/反作弊规则表相关类型（ORIG namespace WongWork）----
#ifndef DNF_GAME_WONGWORK_HACKTYPE_TYPES
#define DNF_GAME_WONGWORK_HACKTYPE_TYPES
struct HackTypeInfo_t { char m_pad[0x10]; };  // 仅容器实例化（布局待细化）
// 与 CUser.h 258 行一致（本 TU 不 include CUser.h，故本地定义防 ODR 冲突）。
struct HackIPCounter_t
{
    unsigned short m_a;    // +0x00
    unsigned short m_b;    // +0x02
    char m_ip[0xd];        // +0x04（ctor: memset 0xd + strncpy）
    unsigned int m_cnt;    // +0x14
};                         // sizeof 0x18
struct HackFullIPCounter_t { char m_pad[0x18]; };  // 仅容器实例化（布局待细化）
struct stHacktypeExceptionInfo_t
{
    unsigned short m_cnt;   // +0x00（ctor 第一参数）
    unsigned short m_flag;  // +0x02（ctor 第二参数）
    stHacktypeExceptionInfo_t(short cnt, short flag)
        : m_cnt(cnt), m_flag(flag)
    {
    }
};  // 4 字节

#endif

class CAutoPunishRuleHackTypeMgr
{
public:
    CAutoPunishRuleHackTypeMgr();
    void reset();
private:
    // ORIG 布局（ctor 反汇编 0x82a5b1a）：四个 STL 容器，总 sizeof 0x70
    std::multimap<ENUM_HACKTYPE, HackTypeInfo_t> m_hackTypeMap;         // +0x00
    int m_field18;                                                      // +0x18
    int m_field1c;                                                      // +0x1c
    int m_field20;                                                      // +0x20
    std::multimap<unsigned short, HackIPCounter_t> m_ipCounterMap;      // +0x24
    std::multimap<unsigned short, HackFullIPCounter_t> m_fullIPCounterMap;  // +0x40
    std::map<int, stHacktypeExceptionInfo_t> m_exceptionMap;            // +0x58
};

// WongWork::CSimpleSSO 权威定义已收敛到 CSimpleSSO.h（顶部 include）；
// 成员函数 ctor/dtor/init 在本 TU，其余方法在 WongWorkSSO_A.cpp。

class CSecurityCardCenter
{
public:
    CSecurityCardCenter();
    void init();
private:
    char m_pad[0x488];
};

// CIPGHelper 保留本 TU（ORIG 方法集属 GlobalData 域）；基类 IIPGHelperInterface
// 与成员 CIPGMgr 用 TaiwanCash.h 权威定义（WongWork::IPG，include 见文件头部）。
namespace IPG
{

class CIPGHelper : public IIPGHelperInterface
{
public:
    CIPGHelper();
    ~CIPGHelper();
    int Initialize(unsigned int a, unsigned int b);
    void Finalize();  // ORIG T 0x08100c92（Initialize 复位用）
private:
    // ORIG 布局：+0x00 vptr；+0x04 CIPGMgr*；+0x08..0x20 常量；+0x24 服务器名。
    CIPGMgr* m_mgr;        // +0x04
    unsigned int m_c8;     // +0x08
    unsigned int m_cc;     // +0x0c
    unsigned int m_c10;    // +0x10
    unsigned int m_c14;    // +0x14
    unsigned int m_c18;    // +0x18
    unsigned int m_c1c;    // +0x1c
    unsigned int m_c20;    // +0x20
    char m_serverName[0x14];  // +0x24
    unsigned int m_c38;    // +0x38
    unsigned int m_c3c;    // +0x3c
    unsigned int m_c40;    // +0x40
    unsigned int m_c44;    // +0x44
    unsigned int m_c48;    // +0x48
    unsigned int m_c4c;    // +0x4c
    unsigned int m_c50;    // +0x50
};
}
}

// ItemVendingMachine 权威声明 + ARAD::PurcahseBonus / CeraShopPurcahseCountBonus
// 见 ItemVendingMachine.h；此处不再重复定义（避免 ODR）。
#include "ItemVendingMachine.h"

class Secu_DataControl
{
public:
    Secu_DataControl();
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

namespace break_away_prevent
{
// SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER 基类（ORIG ctor 0x0831d2b0：置字段）。
struct STRewardDataKey
{
    char m_data[3];
    bool operator<(const STRewardDataKey& o) const
    {
        return memcmp(m_data, o.m_data, 3) < 0;
    }
};
// 奖励条目（loadDbData/selectReward：+0x00..+0x10 五个 4 字节字段，stride 0x18）。
struct STRewardData
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
    int m_fieldc;   // +0x0c
    int m_field10;  // +0x10
};
struct STRewardDataList
{
    std::list<STRewardData> m_list;
};

class CBreakAwayPreventSystem
{
public:
    CBreakAwayPreventSystem();
    ~CBreakAwayPreventSystem();
    // ORIG T 0x0831c49e：拷贝基类区（0x2e4）后按条目填充
    // m_rewardMap（key=+0x10..+0x12 三字节 / value=5×int）与 m_keys[]。
    void loadDbData(SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data);
private:
    // ORIG：基类区 0x2e4 + map + 5×STRewardDataKey(+0x2fc) + memset(+0x30b,0x1f)。
    char m_base[0x2e4];
    std::map<STRewardDataKey, STRewardDataList> m_rewardMap;  // +0x2e4
    STRewardDataKey m_keys[5];                                 // +0x2fc
    char m_tail[0x1f];                                         // +0x30b
};
}

// CExpertJobFactory/CExpertJobMgr 权威声明已迁 CExpertJob.h（含 StaticPool 双池），
// 本 TU 仅保留其成员函数定义（下方）。

namespace village_attacked
{
// 本地最小视图（权威布局见 CVillageMonsterMgr.h；本 TU 仅需 C1/D1 的容器
// 实例化形态，Zone/CVillageMonsterArea 以同布局本地类型参与 map 模板实例）。
struct Zone
{
    unsigned char m_a;
    unsigned char m_b;
};
inline bool operator<(const Zone& x, const Zone& y)
{
    if (x.m_a != y.m_a)
        return x.m_a < y.m_a;
    return x.m_b < y.m_b;
}
struct CVillageMonsterArea { char m_pad[0x3c]; };  // ORIG sizeof 0x3c

class CVillageMonsterMgr
{
public:
    CVillageMonsterMgr();
    ~CVillageMonsterMgr();
private:
    int m_field00;                                 // +0x00（ctor 体清零）
    int m_field04;                                 // +0x04（ctor 体清零）
    std::map<Zone, CVillageMonsterArea> m_areas;   // +0x08（0x18）
    int m_field20;                                 // +0x20（ctor 体清零）
};

}

// RentalMachinePcRoomImpl（ORIG ctor 0x8271a80：set<unsigned>@0x104 构造 +
// memset 前 0x104；总 0x11c）。
struct RentalMachinePcRoomImpl
{
    RentalMachinePcRoomImpl() { memset(m_data, 0, sizeof(m_data)); }
    char m_data[0x104];            // +0x00（日志路径等，未还原字段区）
    std::set<unsigned int> m_set;  // +0x104
};  // 0x11c
class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom();
    virtual ~RentalMachinePcRoom() {}

    // 槽2 Rent：ORIG T 0x082704e6（0x258 字节 pc_room 租赁事务链，本体待
    // pc_room 域批次 —— 报告 RentalMachinePcRoom/Rent.md）。声明占位保 vtable 序。
    virtual bool Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
                      InstanceRentalSystem::RentOutput& output);
    // 槽3 Ready：ORIG 0x0827073e = checkAllRentalItemScript() 尾调。
    virtual bool Ready();
    // 槽4 CheckRentableItem：ORIG 0x08270752 = m_impl->m_set.find(idx)!=end。
    virtual bool CheckRentableItem(unsigned int itemIdx);

private:
    bool checkAllRentalItemScript();   // ORIG T 0x082708cc（本批实现）
    void initLoggingEnvironment();     // ORIG T 0x8270886
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x08270d1e
    void log(CUser& user, int itemId);                                      // T 0x082707c0
    RentalMachinePcRoomImpl* m_impl;   // +0x04（ORIG auto_ptr）
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
    virtual bool Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
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


// _DailySchedule 本地布局副本（权威定义 CDailyScheduleManager.h，0x24 字节；
// 本 TU 不 include 该头，仅按同布局参与 vector 模板实例化）。
struct _DailySchedule
{
    int m_eventId;              // +0x00
    int m_type;                 // +0x04
    int m_idx;                  // +0x08
    int m_startSec;             // +0x0c
    int m_endSec;               // +0x10
    int m_wday;                 // +0x14
    unsigned char m_startHour;  // +0x18
    unsigned char m_startMin;   // +0x19
    unsigned char m_startSec2;  // +0x1a
    unsigned char m_endHour;    // +0x1b
    unsigned char m_endMin;     // +0x1c
    unsigned char m_endSec2;    // +0x1d
    short m_pad1e;              // +0x1e
    int m_state;                // +0x20
};  // 0x24

class CDailyScheduleManager
{
public:
    CDailyScheduleManager();
    ~CDailyScheduleManager();
private:
    // 权威布局见 CDailyScheduleManager.h；本 TU 镜像仅供 C1/D1 产出
    // vector<_DailySchedule> 构造/析构调用（+0xd8）。
    _DailySchedule m_pad024[6];               // +0x000..0x0d7（6×0x24）
    std::vector<_DailySchedule> m_schedules;  // +0xd8
};

namespace online_preliminary
{
// COnlinePreliminaryTeam 权威定义见 CUserCharacInfo.h（经 CUser.h 引入），此处不再重复。

// CTeamFactory（ORIG ctor 0x08588e2e 直接调 StaticPool<COnlinePreliminaryTeam,600> ctor）。
class CTeamFactory
{
public:
    CTeamFactory();
    ~CTeamFactory();
    StaticPool<COnlinePreliminaryTeam, 600> m_pool;  // +0x00（0x30）
};

class COnlinePreliminaryTeamMgr
{
public:
    COnlinePreliminaryTeamMgr();
    ~COnlinePreliminaryTeamMgr();
    // ORIG T 0x08588be4：data 首 4 字节为 key，命中 m_teamMap 时
    // 委托 COnlinePreliminaryTeam::loadDataPreliminaryTeamMatchList(data)。
    void OnLoadTeamInfo(CUser* user, char* data);
private:
    CTeamFactory* m_factory;                        // +0x00
    std::map<int, COnlinePreliminaryTeam*> m_teamMap;  // +0x04
};
}

// game_master 命令体系完整声明见 GameMasterCmd.h（GmCore/GmRegister 等 TU 实现）。

class TimerUpdatePvPGrade
{
public:
    static void registNextTimer(long long sec);
};

class Timer_GenerateCaptchaData
{
public:
    static void RegistNextTimer(int sec);
};

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
namespace StackableAction
{
// ORIG：输入参数结构（+0x08 CUser*、+0x0c CItem* 为 proc_* 实际使用字段）。
struct input_param
{
    char m_pad0[8];    // +0x00（未还原）
    CUser* m_user;     // +0x08
    CItem* m_item;     // +0x0c
};

class Action
{
public:
    Action();
    int proc_bingo(input_param& p);        // ORIG T 0x0827fb14
    int proc_addFatigue(input_param& p);   // ORIG T 0x0827fbd6
    int proc_lotteryCount(input_param& p); // ORIG T 0x0827fb86
    void init_function_type();             // ORIG T 0x0827f7c8
    // ORIG 布局：boost::noncopyable 基类 + 两个 map<int, boost::function>。
    std::map<int, boost::function<int(input_param&)> > m_map1;  // +0x00
    std::map<int, boost::function<int(input_param&)> > m_map2;  // +0x18
};
}


namespace expert_extraction
{
// ORIG 0x084a2938：vptr + CMTRand*（new 0x9c8 + seed(time)）。
class CExtraction
{
public:
    CExtraction();
    virtual ~CExtraction();
    CMTRand* m_rand;  // +0x04
};

class CAlchemistExtraction : public CExtraction
{
public:
    CAlchemistExtraction();
    virtual ~CAlchemistExtraction();
};
class CEnchanterExtraction : public CExtraction
{
public:
    CEnchanterExtraction();
    virtual ~CEnchanterExtraction();
};

class CDollControllerExtraction : public CExtraction
{
public:
    CDollControllerExtraction();
    virtual ~CDollControllerExtraction();
};

class CExtractionMgr
{
public:
    CExtractionMgr();
    ~CExtractionMgr();
private:
    // ORIG D1 0x84a311a 以 (CExtraction**) 索引循环析构 → 成员为数组。
    CExtraction* m_slots[3];                  // +0x00/+0x04/+0x08
};
}


namespace random_option
{
void* GetRandomOption();  // ORIG T 0x085f0eb0：&G_CDataManager()->+0x148

}


class RestrictGeolocation
{
public:
    bool initGeolocation();
    void clearAllowCountry();  // ORIG T 0x08170b16（支撑）
    bool loadData();           // ORIG T 0x08170fec（支撑）
    RestrictGeolocation();   // ORIG T 0x0817097c
    ~RestrictGeolocation();  // ORIG T 0x081709fe
    void closeGeolocation(); // ORIG T 0x08170a88
private:
    void* m_geoip;                     // +0x00 GeoIP*
    // ORIG 成员为 boost::unordered_set<std::string>（addAllowCountry/clearAllowCountry）；
    // 此处以 std::set 做语义等价（推断）。
    std::set<std::string> m_allowCountry;
};
class CHumanCertifier
{
public:
    static void init();
    // ORIG B 0x93f5758：map<string, vector<int>> 人证类型表。
    static std::map<std::string, std::vector<int> > humanCertifyTable_;
};
class CParty
{
public:
    void* GetPartyTelePort();   // ORIG T（权威 CParty.h:317）
};

class GameWorld
{
public:
    bool IsPVPChannel() const;                       // 权威 GameWorld.h:121
    bool IsPvPSkilTreeChannel() const;               // 权威 GameWorld.h:125
    int GetChannelType() const;
    bool IsIntegratedPvPBaseChannel() const;
    CUser* find_from_world(unsigned short uniqueId);   // ORIG T 0x086c4b9c（权威 GameWorld.h:145）
    void send_to_area(int vill, int area, PacketGuard& packet);  // 权威 GameWorld.h:163
    bool move_area(CUser* user, int vill, int area, int x, int y, int z,
                   bool flag, int param, int prevVill, int prevArea);  // 权威 GameWorld.h:197
};

GameWorld* G_GameWorld();

class CDBEnviroment : public CEnvironment
{
public:
    CDBEnviroment();
    ~CDBEnviroment();
private:
    char m_pad[8];   // ORIG：CEnvironment + 0x3e0 处成员/填充（0x3e8）
};

// ---- Captcha ----
struct CountryCode
{
    unsigned char idx_;           // +0x00（1 字节）
    static const CountryCode none;    // 09501a20
    static const CountryCode global;  // 09501a21
    static const CountryCode korea;   // 09501a22
    static const CountryCode japan;   // 09501a23
    static const CountryCode china;   // 09501a24
    static const CountryCode taiwan;  // 09501a25
    static const CountryCode usa;     // 09501a26

    bool operator==(CountryCode other) const;   // 0x08ad332e（按值，ORIG mangling eqES_）
};

const CountryCode CountryCode::none   = {0};
const CountryCode CountryCode::global = {1};
const CountryCode CountryCode::korea  = {2};
const CountryCode CountryCode::japan  = {3};
const CountryCode CountryCode::china  = {4};
const CountryCode CountryCode::taiwan = {5};
const CountryCode CountryCode::usa    = {6};

bool CountryCode::operator==(CountryCode other) const
{
    return idx_ == other.idx_;
}

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
        return kXorRandMsTable0[p[0]] ^ kXorRandMsTable1[p[1]] ^
               kXorRandMsTable2[p[2]] ^ kXorRandMsTable3[p[3]];
    }
    unsigned int operator()() { return getUInt32(); }
private:
    unsigned int m_counter;
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
private:
    static float getFloat(XorRand4& rng, float lo, float hi);
    void* m_library;             // +0x00（ORIG FT_Library；本还原中为句柄占位）
    std::vector<void*> m_faces;  // +0x04
    XorRand4 m_rng;              // +0x10
};

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
    m_rng.seed((unsigned int)time(0));
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
DispatchThread::DispatchThread() {}  // 基类 ThreadInterface 隐式构造（ORIG 0x847e268）
NetworkThread::NetworkThread() {}    // ORIG 0x857d554
UdpThread::UdpThread() {}            // ORIG 0x857c354
MonitorTcpThread::MonitorTcpThread() {}  // ORIG 0x857d8f0
AcceptThread::AcceptThread()
{
    // ORIG 0x0857cbe8：置 +0x0c 标志为 false。
    m_acceptFlag = false;
}

// ---- GlobalData::Init 直接 new 的基础对象 ----
// CTimeGate ctor 现由 CTimeGate.cpp 提供（ORIG 0x8284512，继承 IObject）。
ARAD::Arad_ServerStateManager::Arad_ServerStateManager()
    : m_active(false)
{
    // ORIG 0x81a922c：bool@0=0；Mutex@+4；map@+0x1c（成员自动构造）。
}

PacketPool::PacketPool()
{
    // ORIG 0x828fc76：先置 m_threadID=0；new DynamicPool(10)；SetPoolSize(10,10)；
    // m_pCheckCallback=0。
    m_threadID = 0;
    m_pool = new DynamicPool<PacketBuf>(10);
    m_pool->SetPoolSize(10, 10);
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
            1, "App.cpp", "PacketBuf* PacketPool::Acquire()", 0x275,
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

// CServerProxyMgr<CMonitorServerProxy> 的 GetStartIndex/GetEndIndex/GetNextIndex
// 仅被 CGameManager.cpp 经 sub_CServerProxyMgrMonitor_* asm 桥接引用（ORIG
// 符号 _ZN15CServerProxyMgrI19CMonitorServerProxyE13GetStartIndexEv 等），
// 无法在调用 TU 隐式实例化，故在此显式实例化（对齐 CStatisticServerProxy.cpp 模式）。
template int CServerProxyMgr<CMonitorServerProxy>::GetStartIndex();
template int CServerProxyMgr<CMonitorServerProxy>::GetEndIndex();
template int CServerProxyMgr<CMonitorServerProxy>::GetNextIndex(int&);


// CPCRoomServerProxy 仅在本 TU 定义（无独立源文件），保留其唯一构造函数。
CPCRoomServerProxy::CPCRoomServerProxy(char* ip, int port)
    : m_ip(ip ? ip : "")
{
    (void)port;
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
    WongWork::stGMInfo_t key(mid, 3);
    std::list<WongWork::stGMInfo_t>::iterator it =
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
    m_field18 = 0;
    m_field1c = 0;
    m_field20 = 0;
}
WongWork::CBlackIPMonitor::CBlackIPMonitor()
{
    // ORIG 0x82a6526：构造 std::multimap<unsigned int, stBlackIPMonitorEntry_t>（成员隐式）。
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
void WongWork::CBlackIPMonitor::deleteIP(char const* ipStr, unsigned int type)
{
    // ORIG 0x84ebace：ip 为空返回 0；inet_addr 转 key；遍历 [lower_bound, upper_bound)
    // 删除 entry.m_punishType == type 的项。
    if (!ipStr)
        return;
    unsigned int key = static_cast<unsigned int>(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator it =
        m_monitorMap.lower_bound(key);
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator end =
        m_monitorMap.upper_bound(key);
    while (it != end)
    {
        if (static_cast<unsigned int>(it->second.m_punishType) == type)
        {
            m_monitorMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
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
    // ORIG 0x082a7260：31 行 × 24 列默认构造 CRijndael（清零 + 置位）；
    // 尾部 +0xd0860 的 MySQL* 置 0。当前 CRijndael 在 0x47c 步长下 ctor
    // 会写越界（m_temp3 区），故按 ORIG 语义做等价清零（推断）。
    memset(m_keyData, 0, sizeof(m_keyData));
    m_mysql = 0;
}

WongWork::CSimpleSSO::~CSimpleSSO() {}  // ORIG 0x082a7352：逆序析构 744 个 CRijndael（dtor 为空）

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
            CRijndael* r = reinterpret_cast<CRijndael*>(m_keyData + col * 0x47c + row * 0x6ba0);
            r->Initialize(strKeyTable_[row * 24 + col], 0x20, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
        }
    }
}
WongWork::CSecurityCardCenter::CSecurityCardCenter()
{
    // ORIG 0x082a6654：构造 CRijndael @0x0（0x47c 块）+ list<uchar> @0x47c。
    // 当前 CRijndael 为 0x4a0（ctor 越界写 m_temp3 区），故对 0x47c 块做
    // 等价清零（推断）；list 必须真实构造（push_back 依赖其自环节点）。
    memset(m_pad, 0, 0x47c);
    new (m_pad + 0x47c) std::list<unsigned char>();
}

void WongWork::CSecurityCardCenter::init()
{
    // ORIG 0x085fe05e：以固定密钥初始化本对象头部 CRijndael；把 42 字符
    // characterSet 全部 push 进 +0x47c 的 list<uchar>；+0x484 置 1。
    static const unsigned char characterSet[] =
        "+<>?@#$&ABCDEFGHIJKLMNPQRSTUVWXYZ123456789";  // ORIG .data 0x93ac1e0（42 字符+'\0'）
    reinterpret_cast<CRijndael*>(m_pad)->Initialize(
        "n6;fw;vjfs0fuguf", 0x10, CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
    std::list<unsigned char>& charList =
        *reinterpret_cast<std::list<unsigned char>*>(m_pad + 0x47c);
    for (int i = 0; i < 42; ++i)
    {
        charList.push_back(characterSet[i]);
    }
    *reinterpret_cast<unsigned short*>(m_pad + 0x484) = 1;
}

void WongWork::IPG::CIPGHelper::Finalize()
{
    // ORIG 0x08100c92：释放 CIPGMgr（推断：delete + 置空）。
    if (m_mgr)
    {
        delete m_mgr;
        m_mgr = 0;
    }
}

int WongWork::IPG::CIPGHelper::Initialize(unsigned int a, unsigned int b)
{
    // ORIG 0x08100b3e：已初始化则先 Finalize；写入常量表；取环境服务器名
    // （CEnvironment+0x2b8，空串→-1）strncpy 0x14；再写常量组；new CIPGMgr
    // 并转发 Initialize(a,b)。
    if (m_mgr)
    {
        Finalize();
    }
    m_c8 = 0x2b0;
    m_cc = 0x1cb;
    m_c10 = 0x1fc;
    m_c14 = 0x1d4;
    m_c18 = 0x99;
    m_c1c = 0x1d4;
    m_c20 = 0x90;
    const char* envName = reinterpret_cast<const char*>(
        reinterpret_cast<char*>(G_CEnvironment()) + 0x2b8);  // 未建模字段（推断）
    if (envName[0] == 0)
    {
        return -1;
    }
    strncpy(m_serverName, envName, sizeof(m_serverName));
    m_serverName[sizeof(m_serverName) - 1] = 0;
    m_c38 = 0x5a3c;
    m_c3c = 0x5aa0;
    m_c40 = 0x5b04;
    m_c44 = 0x5b68;
    m_c48 = 0x5bcc;
    m_c4c = 0x232e;
    m_c50 = 0;
    m_mgr = new CIPGMgr(this);
    return m_mgr->Initialize(a, b);
}
WongWork::IPG::CIPGHelper::CIPGHelper()
{
    // ORIG 0x082a7190：IIPGHelperInterface 基类（vptr 自动）。
    m_mgr = 0;
    memset(m_serverName, 0, sizeof(m_serverName));
}
WongWork::IPG::CIPGHelper::~CIPGHelper()
{
    delete m_mgr;
    m_mgr = 0;
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
bool ItemVendingMachine::_CheckEnableAvatarCondition(const CCeraShopGoods* goods,
                                                     char c, unsigned char uc,
                                                     stAvatarTypeSelect_t& out)
{
    CItem* related = goods->GetRelatedItem();
    if (related == 0 || !related->IsAvatarItem())
        return false;
    std::memset(&out, 0, sizeof(out));   // 等价 stAvatarTypeSelect_t::init()
    CEquipItem* equip = static_cast<CEquipItem*>(related);
    void* vec = equip->getAvatarTypeSelect();
    std::vector<stAvatarTypeSelect_t>& select =
        *static_cast<std::vector<stAvatarTypeSelect_t>*>(vec);
    int idx = (signed char)c;
    if (select.empty() || idx >= (int)select.size())
        return false;
    if (uc >= 100)
        return false;
    const stAvatarTypeSelect_t& src = select[idx];
    out.m_fieldc = src.m_fieldc;
    out.m_field4 = src.m_field4;
    out.m_field8 = src.m_field8;
    out.m_field0 = src.m_field0;
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
    if (!m_ceraShop._CheckValidCount(goods))
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
    // ORIG 0x080ec67c：Open(0, 0, this+4)。
    return Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, m_connInfo);
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
    std::memset(reinterpret_cast<char*>(&info) + 0x2d, 0, 60);
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
        cMyTrace t("bool CGM_Manager::LoadGmListFromDb()", 0x1579, 0);
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
            cMyTrace t("bool CGM_Manager::LoadGmListFromDb()", 0x1588, 0);
            t("Failed to get GM's m_id from DB.");
            result = 0;
            break;
        }
        if (m_db->get_int(1, level) != 1)
        {
            cMyTrace t("bool CGM_Manager::LoadGmListFromDb()", 0x158e, 0);
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
        cMyTrace tr("void CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(int, int)",
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
    memset(m_base, 0, sizeof(m_base));
    memset(m_keys, 0, sizeof(m_keys));
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
    int count = *(int*)((char*)data + 0x2e0);
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
               "expert_job::CExpertJobMgr::CExpertJobMgr()", 0x57f);
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
    if (m_impl)
    {
        m_impl->m_machines[idx] = m;
    }
}

RentalMachinePcRoom::RentalMachinePcRoom()
{
    // ORIG 0x82703c0：IRentalMachine 基类 + vptr；new Impl(0x11c) 存 auto_ptr@+4；
    // initLoggingEnvironment()。
    m_impl = new RentalMachinePcRoomImpl;
    initLoggingEnvironment();
}
void RentalMachinePcRoom::initLoggingEnvironment()
{
    // ORIG 0x8270886：snprintf(impl, 0x103, "./log/%s/dw2_RentalItems", 文件名)。
    if (m_impl == 0)
    {
        return;
    }
    snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
             G_CEnvironment()->get_file_name());
}

RentalMachineDimension::RentalMachineDimension()
{
    // ORIG 0x826eafc：与 PcRoom 同构（vptr 表不同）。
    m_impl = new RentalMachineDimensionImpl;
    initLoggingEnvironment();
}
void RentalMachineDimension::initLoggingEnvironment()
{
    // ORIG 0x826efdc：同 PcRoom 日志路径（.rodata 0x8bf90c2 同串）。
    if (m_impl == 0)
    {
        return;
    }
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
bool InstanceRentalSystem::Rent(CUser& user, const RentInput& input,
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
                              "online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()",
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
    Stream* s = GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x75);
    CStreamGuard guard(s, true);
    **guard << 0x254;
    **guard << -1;
    guard->put_binary(pkg, 0x34);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}

// ---- 定时器/DB 请求/反外挂 ----
void TimerUpdatePvPGrade::registNextTimer(long long) {}  // TODO(后续批次)
void Timer_GenerateCaptchaData::RegistNextTimer(int sec)
{
    // ORIG 0x863b2ba：G_TimerQueue()->InsertTimer((OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
    // sec, 0, 0)。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
                                sec, 0, 0);
}
// ---- DB 请求（ORIG 模式：Acquire 流 → CStreamGuard → <<msgId → <<-1 → 入 DB 队列）----
void DB_ReloadAutoPunishRuleBackIP::makeRequest()
{
    // ORIG 0x08432e80 同型（DBThread.cpp 0x7a8e）；本函数 msgId=0x108（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73d8), true);
    **guard << 0x108;
    **guard << -1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void DB_ReloadAutoPunishRuleHackType::makeRequest()
{
    // msgId=0x109（报告 makeRequest.md）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x73d8), true);
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
    if (ev == 0 || !ev->IsEventing(0))
    {
        return 0x13;
    }
    ev = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
    if (ev == 0)
    {
        return 0x13;
    }
    // BingoEvent 完整类型未建模（CBingo/shared_ptr 依赖缺失）。按 ORIG 0x080c9ff6
    // （docs/class_func_reports/BingoEvent/reqBingoMark.md）内联还原可证实前缀检查；
    // 随机标记路径依赖 CBingo::getBingoRandomVaule/calNewValue，未建模时按「无法标记」
    // 返回 0x13（推断）。内联定义避免产生 ORIG 不存在的独立链接符号。
    struct BingoEventView
    {
        int reqBingoMark(StackableAction::input_param& p)
        {
            if (p.m_user == 0 || p.m_item == 0)
            {
                return 0x13;
            }
            unsigned char* bd = (unsigned char*)p.m_user->getBingoData();
            if ((int)(signed char)bd[4] >= 3)
            {
                return 7;
            }
            int no = (*(std::vector<int>*)((char*)p.m_item + 0x3a0))[0];
            if (no < 1 || no > 0x32)
            {
                return 0x17;
            }
            return 0x13; // 推断：CBingo 标记链路未建模
        }
    };
    return BingoEventView().reqBingoMark(p);
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
    std::vector<int>* costs = reinterpret_cast<std::vector<int>*>(
        reinterpret_cast<char*>(p.m_item) + 0x3a0);  // CItem 未建模字段（推断）
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
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34 等）。
    // proc 实现未还原，注册关系保留（推断：key 集合不完整）。
    using boost::arg;
    m_map2[0x34] = boost::function<int(input_param&)>(
        boost::bind(&Action::proc_bingo, this, arg<1>()));
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
    delete m_rand;
    m_rand = 0;
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
STTeleportData::STTeleportData() {}

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
    *reinterpret_cast<int*>(&m_tp) = *reinterpret_cast<const int*>(&tp);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&m_tp) + 4) =
        *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&tp) + 4);
    *reinterpret_cast<short*>(reinterpret_cast<char*>(&m_tp) + 8) =
        *reinterpret_cast<const short*>(reinterpret_cast<const char*>(&tp) + 8);
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
        stPartyTelePort tp;
        tp.m_state = data.m_field00 >> 16;                 /* 【推断】ORIG 未回读该字段 */
        (void)tp.m_state;
        tp.reset();
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
    // ORIG T 0x085f0eb0：返回 G_CDataManager()->+0x148（RandomOptionScript 成员）。
    return reinterpret_cast<char*>(G_CDataManager()) + 0x148;  // 推断：偏移直访
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
    // （script 内嵌 provider 对象基址；虚槽0/1 见 RandomOptionScript.h）。
    m_optionSet = reinterpret_cast<char*>(GetRandomOption()) + 0x148;
}
random_option::CRandomOptionItemHandle::~CRandomOptionItemHandle()
{
    // ORIG 0x085f0e46（推断）：释放 CMTRand 数组。
    delete[] m_rand;
    m_rand = 0;
}

// ---- 限制服务/地理/人证 ----
RestrictGeolocation::RestrictGeolocation()
{
    // ORIG 0x0817097c：ARAD::Singleton 基类构造；m_geoip=0；unordered_set 构造
    // （此处以 std::set 语义等价，成员自动构造）。
    m_geoip = 0;
}

RestrictGeolocation::~RestrictGeolocation()
{
    // ORIG 0x081709fe：closeGeolocation() + 集合析构（成员自动）。
    closeGeolocation();
}

void RestrictGeolocation::closeGeolocation()
{
    // ORIG 0x08170a88：GeoIP_delete(m_geoip)。工程未链 libGeoIP，经 dlsym
    // 动态查找（与 initGeolocation 的 GeoIP_new 同模式，推断）。
    typedef void (*GeoIPDeleteFn)(void*);
    GeoIPDeleteFn geoipDelete = (GeoIPDeleteFn)dlsym(RTLD_DEFAULT, "GeoIP_delete");
    if (geoipDelete)
    {
        geoipDelete(m_geoip);
    }
    m_geoip = 0;
}

bool RestrictGeolocation::initGeolocation()
{
    // ORIG 0x08170a46：m_geoip 为空则 GeoIP_new(0)；失败返回 0；随后 loadData()。
    if (m_geoip == 0)
    {
        // ORIG 直接链接 libGeoIP 的 GeoIP_new；工程未链 GeoIP，故经 dlsym 动态
        // 查找（推断：运行时行为与 ORIG 一致——无 GeoIP 时返回 0）。
        typedef void* (*GeoIPNewFn)(int);
        GeoIPNewFn geoipNew = (GeoIPNewFn)dlsym(RTLD_DEFAULT, "GeoIP_new");
        m_geoip = geoipNew ? geoipNew(0) : 0;
        if (m_geoip == 0)
        {
            return 0;
        }
    }
    loadData();
    return 1;
}

void RestrictGeolocation::clearAllowCountry()
{
    // ORIG 0x08170b16：清空 boost::unordered_set<string>（推断：set 等价）。
    m_allowCountry.clear();
}

bool RestrictGeolocation::loadData()
{
    // ORIG 0x08170fec：Acquire 流 → internal_stream(1, 0xffffffff) → clearAllowCountry
    // → 入 DB 队列。internal_stream 包类型按 loadPolicy 同型推断为 1。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "ServiceRestrictManager.cpp", 0x1c5), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(1), 0xffffffff);
    clearAllowCountry();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
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
    memset(buf, 0, sizeof(buf));
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
CDBEnviroment::CDBEnviroment() : CEnvironment()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CDBEnviroment::~CDBEnviroment() {}  // TODO(后续批次)

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
SPremiumInfo GlobalData::g_SPremiumInfo[0x6a];   // 09483a40；静态初始化逐元素构造（ORIG _GLOBAL__I_g_SPremiumInfo 0x86b080a）
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
int GlobalData::g_GoldControlDateFlag = 0;
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
    cMyTrace t("static bool GlobalData::initTournamentDB(const char*)", 0x20aa, 0);
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

    // Event Upgrade Jar（ORIG 于 CDataManager::init 中 new + LoadScript）。
    s_pEvent_UpGrade_Jar = new Event_Upgrade_Jar;
    s_pEvent_UpGrade_Jar->LoadScript("", "Etc/EventUpgradeJar.etc");

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
    int serverGroup = *(int*)((char*)G_CEnvironment() + 0x378);

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
        cMyTrace t1("static bool GlobalData::Init()", 0x1b0c, 9);
        t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ", &db4, masterPort,
           (char*)&db2 + 0x18, (char*)&db1 + 0x42);
        int idx0 = 0;
        STDBConnInfo& db0 = masterDbInfo[(ENUM_DB_HANDLE_IDX)idx0];
        if (!s_db_connector->Init(db0))
        {
            cMyTrace t2("static bool GlobalData::Init()", 0x1b0f, 9);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3("static bool GlobalData::Init()", 0x1b12, 9);
        t3("Success");
        cMyTrace t4("static bool GlobalData::Init()", 0x1b14, 9);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5("static bool GlobalData::Init()", 0x1b17, 9);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6("static bool GlobalData::Init()", 0x1b1a, 9);
        t6("Success");
        cMyTrace t7("static bool GlobalData::Init()", 0x1b1c, 9);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8("static bool GlobalData::Init()", 0x1b1f, 9);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9("static bool GlobalData::Init()", 0x1b22, 9);
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
        cMyTrace t1("static bool GlobalData::Init()", 0x1d4d, 9);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2("static bool GlobalData::Init()", 0x1d50, 9);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3("static bool GlobalData::Init()", 0x1d55, 9);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4("static bool GlobalData::Init()", 0x1d58, 9);
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
            cMyTrace t("static bool GlobalData::Init()", 0x1d8d, 9);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t("static bool GlobalData::Init()", 0x1d90, 9);
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
        cMyTrace t("static bool GlobalData::Init()", 0x1d9f, 9);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1da8, 9);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t("static bool GlobalData::Init()", 0x1dc6, 9);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dcd, 9);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dd6, 9);
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
        cMyTrace t("static bool GlobalData::Init()", 0x1df7, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1dfe, 9);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e05, 9);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e0c, 9);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e13, 9);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e1b, 9);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e24, 9);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e2d, 9);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e36, 9);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1e47, 9);
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
        cMyTrace t("static bool GlobalData::Init()", 0x1eb0, 5);
        t("AntiBot create fail");
        return false;
    }

    {
        char seq[4] = {0, 0, 0, 0};
        struct CEnvProcSeq
        {
            int getProcessSequence() const
            {
                return 0;  // TODO(后续批次)：ORIG 读取环境对象的进程序列号
            }
        };
        sprintf(seq, "%d", ((const CEnvProcSeq*)G_CEnvironment())->getProcessSequence());
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
        cMyTrace t("static bool GlobalData::Init()", 0x1ebf, 9);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1ed9, 9);
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
            cMyTrace t("static bool GlobalData::Init()", 0x1ef5, 9);
            t("Can't allocate memory for a CExtractionMng");
            return false;
        }

        g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
        if (!g_pTeleport[0])
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f02, 9);
            t("Can't allocate memory for a NPC_Teleport()");
            return false;
        }

        g_pTeleport[1] = new (std::nothrow) User_Teleport;
        if (!g_pTeleport[1])
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f09, 9);
            t("Can't allocate memory for a User_Teleport()");
            return false;
        }

        g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
        if (!g_randomOptionHandle)
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f12, 9);
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
            cMyTrace t("static bool GlobalData::Init()", 0x1f2a, 9);
            t("Can't initialize GeoIP");
            return false;
        }
        else
        {
            cMyTrace t("static bool GlobalData::Init()", 0x1f1f, 9);
            t("Can't allocate memory for a ServiceRestrictManager");
            return false;
        }
    }
    else
    {
        cMyTrace t("static bool GlobalData::Init()", 0x1eea, 9);
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
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_map24.find(std::make_pair(job, growType));
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
        serverInfo = (char)*reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(G_CEnvironment()) + 0x378);  // [裸偏移] CEnvironment+0x378（log 报告直转；具名字段待补）
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
        serverInfo = (char)*reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(G_CEnvironment()) + 0x378);  // [裸偏移] CEnvironment+0x378（log 报告直转；具名字段待补）
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
        list =
            G_CDataManager()->m_dimensionAct.m_rewardData.getRewardSelectItemList(
                packageKey);
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
bool RentalMachinePcRoom::Rent(CUser& user,
                               const InstanceRentalSystem::RentInput& input,
                               InstanceRentalSystem::RentOutput& output)
{
    // ORIG: (!IsPcRoomUser || input+0x11 字节==1) → 7。RentInput 建模中 +0x10
    // 为 m_logFlag，+0x11 为其填充位（未具名）；按报告条件组合转写。
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&input);
    if (!pc_room::IsPcRoomUser(user) || raw[0x11] == 1)
    {
        return true;   // ENUM_ERROR 7
    }
    if (!pc_room::CheckRentableEnvironment(user))
    {
        return true;   // ENUM_ERROR 7
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err != 0;
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
            return true;   // ENUM_ERROR 1
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
    return false;   // 0=成功
}

// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
bool RentalMachineDimension::Rent(CUser& user,
                                  const InstanceRentalSystem::RentInput& input,
                                  InstanceRentalSystem::RentOutput& output)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&input);
    if (!pc_room::CheckRentableEnvironmentAtChannelType(user))
    {
        return true;   // ENUM_ERROR 7
    }
    if (!checkRentalLevel(user.get_charac_level()))
    {
        return false;  // ENUM_ERROR 0xe（14）
    }
    if (!checkRentalGrowJob((int)(signed char)user.getCurCharFirstGrowType()))
    {
        return true;   // ENUM_ERROR 0xfe（254）
    }
    int err = CheckupAllRentalItems(user, input);
    if (err != 0)
    {
        return err != 0;
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
            return true;   // ENUM_ERROR 1
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
    return false;   // 0=成功
}

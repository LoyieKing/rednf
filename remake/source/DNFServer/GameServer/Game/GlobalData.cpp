#include "LogManager.h"
// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。

#include "GlobalData.h"

#include <iostream>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <algorithm>
#include <map>
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

// ============================================================================
// StaticPool<T,N> —— 与 CGameManager.h 相同模板声明（成员定义在
// CGameManager.cpp 显式实例化，_ZN10StaticPoolI…E* 符号由此链接）。
// ============================================================================
template <class T, int N>
class StaticPool
{
public:
    struct CNode
    {
        T data;        // +0x00
        int magic;     // +sizeof(T)，恒 0x8f21
        char bUse;     // +sizeof(T)+4
    };

    StaticPool();
    ~StaticPool();

    T* Acquire();
    T* Get(int index);
    void Free(T* p);
    int GetIndex(T* p);
    int GetLastErrorCode() const { return m_nErrorCode; }
    static int Size() { return N; }
    int Remain() { return (int)m_Queue.size(); }

private:
    CNode* m_pNode;              // +0x00
    std::queue<CNode*> m_Queue;  // +0x04
    int m_nErrorCode;            // +0x2c
};

// 池化类型最小声明（与 CGameManager.h / CUserCharacInfo.h 同名同布局）。
namespace expert_job
{
class CEnchanter
{
public:
    CEnchanter() {}
    ~CEnchanter() {}
private:
    char m_pad[0x100];
};

class CDisjointer
{
public:
    CDisjointer() {}
    ~CDisjointer() {}
private:
    char m_pad[0x100];
};
}

#include "CEnvironment.h"
#include "GameTypes.h"
#include "PacketGuard.h"
#include "CRijndael.h"
#include "Cipher.h"
#include "BaseServerProxy.h"
#include "CHadesServerProxy.h"
#include "CaptchaGenData.h"
#include "CQueryCounter.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
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

class Thread
{
public:
    virtual ~Thread();
    virtual void Stop();
};

class CGlobalEffectManager
{
public:
    CGlobalEffectManager();
private:
    char m_pad[0x18];
};

class CTimeGate
{
public:
    CTimeGate();
private:
    char m_pad[0x1c];
};

namespace ARAD
{
class Arad_ServerStateManager
{
public:
    Arad_ServerStateManager();
private:
    char m_pad[0x34];
};
}

class PacketPool;

class DBThread : public Thread
{
public:
    DBThread();
    void setLogDB(bool isLogDB);
    bool isLogDB();  // ORIG T 0x083ff77a：读 +0x0c
private:
    // ORIG 布局：Thread(vptr) +0x00；+0x0c bool m_bLogDB；+0x10 CQueryCounter。
    char m_pad[8];                 // +0x04
    bool m_bLogDB;                 // +0x0c
    char m_pad2[3];                // +0x0d
    CQueryCounter m_queryCounter;  // +0x10（sizeof 0x3c → 总 0x4c）
};

class DispatchThread : public Thread
{
public:
    DispatchThread();
private:
    char m_pad[0xc - 4];
};

class NetworkThread : public Thread
{
public:
    NetworkThread();
private:
    char m_pad[0xc - 4];
};

class UdpThread : public Thread
{
public:
    UdpThread();
private:
    char m_pad[0xc - 4];
};

class MonitorTcpThread : public Thread
{
public:
    MonitorTcpThread();
private:
    char m_pad[0xc - 4];
};

class AcceptThread : public Thread
{
public:
    AcceptThread();
private:
    char m_pad[0x10 - 4];
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

class CSchoolServerProxy
{
public:
    CSchoolServerProxy(std::string ip, int port);
    bool Init();
private:
    std::string m_ip;    // +0x00
    int m_port;          // +0x04
    CUdpHandler m_udp;   // +0x08
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

class CCommunityServerProxy
{
public:
    CCommunityServerProxy(char* ip, int port);
private:
    // ORIG ctor 0x0846c5c8：CNetwork@+0x00 + string@+0x30df8。
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


class CFatigueBatteryHandle
{
public:
    CFatigueBatteryHandle();
private:
    char m_pad[1];
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

namespace WongWork
{
class CLogGameChannel
{
public:
    CLogGameChannel();
    void StartLogTimer();
private:
    char m_pad[0x10];
};

class CCeraShop
{
public:
    CCeraShop();
private:
    char m_pad[8];
};

class CAutoPunishRuleBlackIPMgr
{
public:
    CAutoPunishRuleBlackIPMgr();
    void reset();
private:
    char m_pad[0x18];
};

// ---- 反挂机/反作弊规则表相关类型（ORIG namespace WongWork；ctor 需实例化容器）----
#ifndef DNF_GAME_WONGWORK_HACKTYPE_TYPES
#define DNF_GAME_WONGWORK_HACKTYPE_TYPES
// 与 CUser.h 258 行一致（本 TU 不 include CUser.h，故本地定义防 ODR 冲突）。
enum ENUM_HACKTYPE { ENUM_HACKTYPE_0 = 0 };
struct HackTypeInfo_t { char m_pad[0x10]; };  // 仅容器实例化（布局待细化）
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
    stHacktypeExceptionInfo_t() {}
    stHacktypeExceptionInfo_t(short cnt, short flag)
        : m_cnt((unsigned short)cnt), m_flag((unsigned short)flag)
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
    char m_pad[0xc];                                                    // +0x18（未初始化区）
    std::multimap<unsigned short, HackIPCounter_t> m_ipCounterMap;      // +0x24
    std::multimap<unsigned short, HackFullIPCounter_t> m_fullIPCounterMap;  // +0x40
    std::map<int, stHacktypeExceptionInfo_t> m_exceptionMap;            // +0x58
};

class CBlackIPMonitor
{
public:
    CBlackIPMonitor();
    void reset();
private:
    char m_pad[0x18];
};

class CSimpleSSO
{
public:
    CSimpleSSO();
    ~CSimpleSSO();
    void init(MySQL* mysql);

    // ORIG 0x093ac220（.data）：31*24 个 16 字节 SSO 会话密钥串指针
    static const char* const strKeyTable_[31 * 24];

private:
    // ORIG 布局：CRijndael m_keyTable[31][24]（每块 0x47c，共 0xd0860 字节）
    // + MySQL* m_mysql（+0xd0860）。CRijndael 本实现为 0x4a0（尾部多 0x24 临时
    // 缓冲），故以字节区保留 ORIG 步长，init 逐块调用 Initialize（写入范围
    // 0x18..0x43c，块内安全）。（推断）
    char m_keyData[0xd0860];
    MySQL* m_mysql;
};

class CSecurityCardCenter
{
public:
    CSecurityCardCenter();
    void init();
private:
    char m_pad[0x488];
};

namespace IPG
{
// IIPGHelperInterface（ORIG 0x082a7182：设置 vptr）。最小声明（推断）。
class IIPGHelperInterface
{
public:
    virtual ~IIPGHelperInterface() {}
};

// CIPGMgr（ORIG 0xf0 字节；方法未还原，推断最小实现）。
class CIPGMgr
{
public:
    CIPGMgr(IIPGHelperInterface* iface) { m_iface = iface; }
    int Initialize(unsigned int a, unsigned int b)
    {
        (void)a; (void)b;
        return 0;  // 推断：IPG 管理初始化未还原
    }
    IIPGHelperInterface* m_iface;  // +0x00
    char m_pad[0xec];
};

class CIPGHelper : public IIPGHelperInterface
{
public:
    CIPGHelper();
    ~CIPGHelper();
    int Initialize(unsigned int a, unsigned int b);
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

class ItemVendingMachine
{
public:
    ItemVendingMachine();
    ~ItemVendingMachine();
private:
    // ORIG 布局：+0x00 WongWork::CCeraShop（SpecialItemHandler 等子对象），
    // +0x08 map<int, vector<ARAD::PurcahseBonus>>。CCeraShop 未建模，
    // 以 8 字节 stub 区保留（推断）；购物积分表以 int 元素等价（推断）。
    char m_ceraShop[8];                       // +0x00
    std::map<int, std::vector<int> > m_bonusMap;  // +0x08
};

class Secu_DataControl
{
public:
    Secu_DataControl();
private:
    char m_pad[1];
};

class CSecu_ProtectionField
{
public:
    CSecu_ProtectionField();
private:
    char m_pad[0x18];
};

class RefPvpGrade
{
public:
    RefPvpGrade();
    ~RefPvpGrade();
private:
    char m_pad[0x14c];
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
    bool OpenMaster();  // ORIG T 0x080ec67c（支撑）
    bool Open(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
              STDBConnInfo& info);  // ORIG T 0x080ec6da（支撑）
private:
    DBMgr* m_dbMgr;            // +0x00
    STDBConnInfo m_connInfo;   // +0x04（0x168 字节）
};

namespace sync_script
{
class CSyncScript
{
public:
    static CSyncScript* GetInstanceSyncScript();
    static void SetEnable(bool enable);
    static bool IsEnable();
};
}

class CSyncSlangFilter
{
public:
    static CSyncSlangFilter* GetInstance();   // ORIG W 0x08235001
    static bool AddSlangList();               // ORIG T 0x0862dccc
    static bool AddSlangListName();           // ORIG T 0x0862dec6
    bool HasSlang(std::string& name);         // ORIG T 0x0862e622
    bool HasSlangName(std::string& name);     // ORIG T 0x0862e16e
    static void FreeInstance();               // ORIG W 0x082a7147
    void AddSlang(const std::string& str);    // ORIG T 0x0862e72a（支撑）
    void AddSlangName(const std::string& str);  // ORIG T 0x0862e276（支撑）

private:
    CSyncSlangFilter();                       // ORIG T 0x0862dba4
    ~CSyncSlangFilter();                      // ORIG T 0x0862dc36

    // ORIG 布局：+0x00/+0x04 两个 IMPL*（SlangNode 字典树根），+0x08 rwLock(0x20)，
    // sizeof 0x28。ORIG 用逐字符字典树（SlangNode::AddChild）存储敏感词；
    // 此处以 std::set<std::string> 做语义等价（推断：字典树→红黑树，查询/插入
    // 语义一致），类为 TU 局部类型，不影响外部 ABI。
    std::set<std::string> m_slangSet;         // 敏感词（slang_list 表）
    std::set<std::string> m_slangNameSet;     // 敏感名（slang_list_name 表）
    pthread_mutex_t m_lock;                   // 读写互斥（推断：替代 rwLock）
};

class CGM_Manager;  // 完整定义见 CGM_Manager.h（真实布局）

class CStatisticMgr
{
public:
    CStatisticMgr();
private:
    char m_pad[0x30];
};

namespace break_away_prevent
{
// SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER 基类（ORIG ctor 0x0831d2b0：置字段）。
// STRewardDataKey（ORIG 3 字节/项 ×5）；STRewardDataList 未还原（推断）。
struct STRewardDataKey
{
    char m_data[3];
    bool operator<(const STRewardDataKey& o) const
    {
        return memcmp(m_data, o.m_data, 3) < 0;
    }
};
struct STRewardDataList
{
    char m_pad[4];
};

class CBreakAwayPreventSystem
{
public:
    CBreakAwayPreventSystem();
    ~CBreakAwayPreventSystem();
private:
    // ORIG：基类区 0x2e4 + map + 5×STRewardDataKey(+0x2fc) + memset(+0x30b,0x1f)。
    char m_base[0x2e4];
    std::map<STRewardDataKey, STRewardDataList> m_rewardMap;  // +0x2e4
    STRewardDataKey m_keys[5];                                 // +0x2fc
    char m_tail[0x1f];                                         // +0x30b
};
}

namespace expert_job
{
// CExpertJobFactory（ORIG W 0x0849f68c）：两个池，sizeof 0x60。
class CExpertJobFactory
{
public:
    CExpertJobFactory();
    ~CExpertJobFactory();
    StaticPool<CEnchanter, 300> m_enchanterPool;    // +0x00
    StaticPool<CDisjointer, 600> m_disjointerPool;  // +0x30
};

class CExpertJobMgr
{
public:
    CExpertJobMgr();
    ~CExpertJobMgr();
private:
    CExpertJobFactory* m_factory;  // +0x00
};
}

namespace village_object
{
class CVillageObjectMgr
{
public:
    CVillageObjectMgr();
    ~CVillageObjectMgr();
private:
    char m_pad[0x18];
};
}

namespace village_attacked
{
class CVillageMonsterMgr
{
public:
    CVillageMonsterMgr();
    ~CVillageMonsterMgr();
private:
    char m_pad[0x24];
};

}

// IRentalMachine 抽象（ORIG 虚表 vtable[1]=dtor）。
class IRentalMachine
{
public:
    virtual ~IRentalMachine() {}
};
class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom() {}
    virtual ~RentalMachinePcRoom() {}
};
class RentalMachineDimension : public IRentalMachine
{
public:
    RentalMachineDimension() {}
    virtual ~RentalMachineDimension() {}
};

class InstanceRentalSystem
{
public:
    InstanceRentalSystem();
    ~InstanceRentalSystem();
    void registerRentalMachine(int idx, IRentalMachine* m);  // ORIG T 0x08270320
private:
    // ORIG：+0x00 auto_ptr<RentalSystemImpl>；impl 含 map<int, IRentalMachine*>。
    struct RentalSystemImpl
    {
        std::map<int, IRentalMachine*> m_machines;
    };
    RentalSystemImpl* m_impl;  // +0x00
};

class CDailyScheduleManager
{
public:
    CDailyScheduleManager();
    ~CDailyScheduleManager();
private:
    char m_pad[0xe4];
};

namespace online_preliminary
{
// 与 CUserCharacInfo.h 同名声明的空类（StaticPool 节点布局需要 1 字节 T）。
class COnlinePreliminaryTeam
{
public:
    int GetTeamId() const;
    const char* GetTeamName() const;
};

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
private:
    CTeamFactory* m_factory;                        // +0x00
    std::map<int, COnlinePreliminaryTeam*> m_teamMap;  // +0x04
};
}

namespace game_master
{
// game_master 命令体系（ORIG T 符号众多，完整还原超本任务范围）。
// 本 TU 以最小类型保留 CGameMasterMgr 的 ORIG 布局与构造序列（推断）。
class IGameMaster
{
public:
    virtual ~IGameMaster() {}
};
class CCommand
{
public:
    virtual ~CCommand() {}
};
class CCommandMap
{
public:
    CCommandMap();
    void insert(int id, const char* name, CCommand* cmd);
    std::map<int, CCommand*> m_cmdById;        // +0x00
    std::map<std::string, CCommand*> m_cmdByName;  // +0x18
};
class CSeniorGameMaster : public IGameMaster
{
public:
    CSeniorGameMaster(IGameMaster* master) { m_master = master; }
    IGameMaster* m_master;  // +0x00
};
class CJuniorGameMaster : public IGameMaster
{
public:
    CJuniorGameMaster(IGameMaster* master) { m_master = master; }
    IGameMaster* m_master;  // +0x00
};
class CMoveCharacCmd : public CCommand
{
public:
    CMoveCharacCmd() { memset(m_pad, 0, sizeof(m_pad)); }
    char m_pad[0xc];
};

class CGameMasterMgr
{
public:
    CGameMasterMgr();
private:
    CSeniorGameMaster* m_senior;  // +0x00
    CJuniorGameMaster* m_junior;  // +0x04
    CCommandMap m_cmdMap;         // +0x08
};
}

class CBusinessImpl
{
public:
    CBusinessImpl();
    ~CBusinessImpl();
private:
    char m_pad[4];
};

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
    return *(char*)GetLightServerFlag();
}

namespace StackableAction
{
// ORIG：输入参数结构（内容未还原，推断为最小占位）。
struct input_param
{
    char m_pad[4];
};

class Action
{
public:
    Action();
    int proc_bingo(input_param& p);        // ORIG T 0x0827fb40（推断实现）
    int proc_addFatigue(input_param& p);   // ORIG T（推断实现）
    int proc_lotteryCount(input_param& p); // ORIG T（推断实现）
    void init_function_type();             // ORIG T 0x0827f7c8
    // ORIG 布局：boost::noncopyable 基类 + 两个 map<int, boost::function>。
    std::map<int, boost::function<int(input_param&)> > m_map1;  // +0x00
    std::map<int, boost::function<int(input_param&)> > m_map2;  // +0x18
};
}

class CMTRand;  // 全局类（PvPTypes.h 同布局），定义见 random_option 区

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
    CAlchemistExtraction* m_alchemist;        // +0x00
    CEnchanterExtraction* m_enchanter;        // +0x04
    CDollControllerExtraction* m_doll;        // +0x08
};
}

class NPC_Teleport
{
public:
    NPC_Teleport();
private:
    char m_pad[0x14];
};

class User_Teleport
{
public:
    User_Teleport();
private:
    char m_pad[0x14];
};

// 与 PvPTypes.h 同布局（0x9c8）；符号由 CMTRand.cpp 提供（全局类）。
class CMTRand
{
public:
    CMTRand();
    void seed(unsigned long s);
private:
    char m_pad[0x9c8];
};

namespace random_option
{
void* GetRandomOption();  // ORIG T 0x085f0eb0：&G_CDataManager()->+0x148

class CRandomOptionItemHandle
{
public:
    CRandomOptionItemHandle();
    virtual ~CRandomOptionItemHandle();
private:
    // ORIG 布局：+0x00 vptr；+0x04 CMTRand*（13×0x9c8）；+0x08 int；+0x0c byte；
    // +0x10 int；+0x14 void*（RandomOption 对象）。
    CMTRand* m_rand;    // +0x04
    int m_field8;       // +0x08
    char m_fieldc;      // +0x0c
    int m_field10;      // +0x10
    void* m_field14;    // +0x14
};
}

namespace ARAD
{
template <class T> class Singleton
{
public:
    static T* Get();
};

template <class T> T* Singleton<T>::Get()
{
    static T inst;  // TODO(后续批次)：单例对象真实构造/销毁由对应类批次接管
    return &inst;
}
}

class RestrictGeolocation
{
public:
    bool initGeolocation();
    void clearAllowCountry();  // ORIG T 0x08170b16（支撑）
    bool loadData();           // ORIG T 0x08170fec（支撑）
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

class GameWorld
{
public:
    int GetChannelType() const;
    bool IsIntegratedPvPBaseChannel() const;
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
        unsigned int v = m_counter;
        m_counter = v + 1;
        return kXorRandMsTable0[v & 0xff] ^ kXorRandMsTable1[(v >> 8) & 0xff] ^
               kXorRandMsTable2[(v >> 16) & 0xff] ^ kXorRandMsTable3[(v >> 24) & 0xff];
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

#pragma pack(push, 2)
struct MSG_UDP_CAPCHA_CERTIFY
{
    unsigned short m_text[5];                        // +0x00
    int m_size;                                      // +0x0a
    unsigned char m_image[0x339e - 0xe];             // +0x0e
};
#pragma pack(pop)

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

namespace WongWork
{
class CCapchaDataManager
{
public:
    void push(const MSG_UDP_CAPCHA_CERTIFY& msg);
private:
    std::deque<MSG_UDP_CAPCHA_CERTIFY> m_queue;   // ORIG：deque（0x28 字节）
};
}

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

// ============================================================================
// 依赖类 stub 实现（GlobalData.cpp 自含声明的占位类；G2/G4/G6/G7/G8 批次接管）
// ============================================================================

// ---- Thread / 线程族 ----
Thread::~Thread() {}  // TODO(后续批次)
void Thread::Stop() {}  // TODO(后续批次)

DBThread::DBThread()
{
    // ORIG 0x083ff5f2：ThreadInterface 基类 + CQueryCounter 构造（成员隐式构造）。
    m_bLogDB = false;
}
void DBThread::setLogDB(bool isLogDB)
{
    // ORIG 0x083ff6c8：置 +0x0c 标志；CQueryCounter::setLogDB；
    // 遍历 0..0x10 号 DB 句柄：非 logDB 时 4 号句柄不挂查询计数，
    // logDB 时仅 4 号句柄挂查询计数。
    m_bLogDB = isLogDB;
    m_queryCounter.setLogDB(isLogDB);
    for (int i = 0; i <= 0x10; ++i)
    {
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)i,
                                                      (ENUM_SERVER_GROUP)0);
        if (isLogDB)
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
DispatchThread::DispatchThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
NetworkThread::NetworkThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
UdpThread::UdpThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
MonitorTcpThread::MonitorTcpThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
AcceptThread::AcceptThread() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)

// ---- GlobalData::Init 直接 new 的基础对象 ----
CGlobalEffectManager::CGlobalEffectManager() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CTimeGate::CTimeGate() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
ARAD::Arad_ServerStateManager::Arad_ServerStateManager()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)

PacketPool::PacketPool() : m_pool(new DynamicPool<PacketBuf>(0)), m_threadID(0) {}
PacketPool::~PacketPool() { delete m_pool; }
pthread_t PacketPool::getThreadID() { return m_threadID; }
void PacketPool::setThreadID(unsigned long tid) { m_threadID = tid; }
PacketBuf* PacketPool::Acquire()
{
    return m_pool ? m_pool->Acquire() : 0;
}
void PacketPool::Free(PacketBuf* packet)
{
    if (m_pool)
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

// ---- ServerProxy（BaseServerProxy/CHadesServerProxy 属 E1 批次，见
//      BaseServerProxy.cpp/CHadesServerProxy.cpp）----
CSchoolServerProxy::CSchoolServerProxy(std::string ip, int port)
    : m_ip(ip), m_port(port)
{
    // ORIG 0x08471e14：string 拷贝 + port + CUdpHandler 构造（成员自动）。
}
bool CSchoolServerProxy::Init()
{
    // ORIG 0x08471e7a：CUdpHandler@+8 的 InitClientSocket，失败 log。
    int r = m_udp.InitClientSocket();
    if (r == -1)
    {
        LogManager::logFormat(1, "DF_SchoolServerProxy.cpp",
                              "bool CSchoolServerProxy::Init()", 0x3c,
                              "[CSchoolServerProxy::Init] Can't init udp client socket\n");
    }
    return r != -1;
}
CPCRoomServerProxy::CPCRoomServerProxy(char* ip, int port)
    : m_ip(ip ? ip : "")
{
    // ORIG 0x084717dc：CNetwork ctor + string(ip)@+0x30df8；port 不在 ctor 使用。
    (void)port;
}
CCommunityServerProxy::CCommunityServerProxy(char* ip, int port)
    : m_ip(ip ? ip : "")
{
    // ORIG 0x0846c5c8：CNetwork ctor + string(ip)@+0x30df8；port 不在 ctor 使用。
    (void)port;
}

CFatigueBatteryHandle::CFatigueBatteryHandle() { m_pad[0] = 0; }  // TODO(后续批次)
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
WongWork::CLogGameChannel::CLogGameChannel()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CLogGameChannel::StartLogTimer() {}  // TODO(后续批次)
WongWork::CCeraShop::CCeraShop() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
WongWork::CAutoPunishRuleBlackIPMgr::CAutoPunishRuleBlackIPMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CAutoPunishRuleBlackIPMgr::reset() {}  // TODO(后续批次)
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
void WongWork::CAutoPunishRuleHackTypeMgr::reset() {}  // TODO(后续批次)
WongWork::CBlackIPMonitor::CBlackIPMonitor()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
void WongWork::CBlackIPMonitor::reset() {}  // TODO(后续批次)
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
        charList.push_back(characterSet[i]);
    *reinterpret_cast<unsigned short*>(m_pad + 0x484) = 1;
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
int WongWork::IPG::CIPGHelper::Initialize(unsigned int, unsigned int)
{
    return 0;  // TODO(后续批次)
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
Secu_DataControl::Secu_DataControl() { m_pad[0] = 0; }  // TODO(后续批次)
CSecu_ProtectionField::CSecu_ProtectionField()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
RefPvpGrade::RefPvpGrade() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
RefPvpGrade::~RefPvpGrade() {}  // TODO(后续批次)

// ---- CDBConnector ----
CDBConnector::CDBConnector(DBMgr* dbMgr)
{
    // ORIG 0x080ec5fc：存 DBMgr*；STDBConnInfo 成员构造。
    m_dbMgr = dbMgr;
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
bool CDBConnector::GetConnInfo(DBGroupMap&, DBInfoMap&) { return true; }  // TODO(后续批次)
bool CDBConnector::Connect(DBInfoMap) { return true; }  // TODO(后续批次)
bool CDBConnector::ConnectWithServerGroup(DBInfoMap) { return true; }  // TODO(后续批次)

// ---- sync_script ----
namespace sync_script
{
CSyncScript* CSyncScript::GetInstanceSyncScript()
{
    static CSyncScript inst;  // TODO(后续批次)
    return &inst;
}
void CSyncScript::SetEnable(bool) {}  // TODO(后续批次)
bool CSyncScript::IsEnable() { return false; }  // TODO(后续批次)
}

// ---- CSyncSlangFilter（GetInstance/HasSlang/HasSlangName 同时服务 CDataManager）----
// GetInstance/FreeInstance 用 ORIG 全局 instance（0x9473894）单例；AddSlangList/
// AddSlangListName 按 ORIG 反汇编从 DB 表读取并逐条 AddSlang/AddSlangName。
static CSyncSlangFilter* s_slangFilterInstance = 0;  // 对应 ORIG .bss 0x9473894

CSyncSlangFilter* CSyncSlangFilter::GetInstance()
{
    // ORIG W 0x08235001：if (instance == 0) instance = new CSyncSlangFilter;
    if (s_slangFilterInstance == 0)
    {
        s_slangFilterInstance = new CSyncSlangFilter;
    }
    return s_slangFilterInstance;
}

void CSyncSlangFilter::FreeInstance()
{
    // ORIG W 0x082a7147：if (instance) { delete instance; instance = 0; }
    if (s_slangFilterInstance)
    {
        delete s_slangFilterInstance;
        s_slangFilterInstance = 0;
    }
}

bool CSyncSlangFilter::AddSlangList()
{
    // ORIG T 0x0862dccc：查询 slang_list 表，逐行 fetch → AddSlang(buf)。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slang from slang_list");
    if (db->exec(true) != 1)
    {
        return false;
    }
    int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        if (db->fetch() != 1)
        {
            return false;
        }
        char buf[0x33];
        memset(buf, 0, sizeof(buf));
        if (db->get_str(0, buf, 0x33) != 1)
        {
            cMyTrace t("bool CSyncSlangFilter::AddSlangList()", 0xbc, 5);
            t("CSyncSlangFilter::AddSlangList");
            return false;
        }
        GetInstance()->AddSlang(std::string(buf));
    }
    return true;
}

bool CSyncSlangFilter::AddSlangListName()
{
    // ORIG T 0x0862dec6：查询 slang_list_name 表，逐行 fetch → AddSlangName(buf)。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slang from slang_list_name");
    if (db->exec(true) != 1)
    {
        return false;
    }
    int rows = db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        if (db->fetch() != 1)
        {
            return false;
        }
        char buf[0x33];
        memset(buf, 0, sizeof(buf));
        if (db->get_str(0, buf, 0x33) != 1)
        {
            cMyTrace t("bool CSyncSlangFilter::AddSlangListName()", 0xd9, 5);
            t("CSyncSlangFilter::AddSlangList");
            return false;
        }
        GetInstance()->AddSlangName(std::string(buf));
    }
    return true;
}

CSyncSlangFilter::CSyncSlangFilter()
{
    // ORIG T 0x0862dba4：new 两个 IMPL + 构造 rwLock@+8。此处构造两个集合与互斥锁。
    pthread_mutex_init(&m_lock, 0);
}

CSyncSlangFilter::~CSyncSlangFilter()
{
    // ORIG T 0x0862dc36：销毁 IMPL×2 + rwLock。
    pthread_mutex_destroy(&m_lock);
}

void CSyncSlangFilter::AddSlang(const std::string& str)
{
    // ORIG T 0x0862e72a：以 writeGuard 保护，逐字符 SlangNode::AddChild 建树。
    // 推断：集合插入语义等价。
    pthread_mutex_lock(&m_lock);
    m_slangSet.insert(str);
    pthread_mutex_unlock(&m_lock);
}

void CSyncSlangFilter::AddSlangName(const std::string& str)
{
    // ORIG T 0x0862e276：同上，写入名字树。
    pthread_mutex_lock(&m_lock);
    m_slangNameSet.insert(str);
    pthread_mutex_unlock(&m_lock);
}

bool CSyncSlangFilter::HasSlang(std::string& name)
{
    // ORIG T 0x0862e622：对每个起点 i，match(substr(i, size-i)) 判定后缀是否命中字典树。
    // 推断：set 后缀查找语义等价。
    pthread_mutex_lock(&m_lock);
    bool hit = false;
    for (unsigned int i = 0; i < name.size(); ++i)
    {
        if (m_slangSet.find(name.substr(i)) != m_slangSet.end())
        {
            hit = true;
            break;
        }
    }
    pthread_mutex_unlock(&m_lock);
    return hit;
}

bool CSyncSlangFilter::HasSlangName(std::string& name)
{
    pthread_mutex_lock(&m_lock);
    bool hit = false;
    for (unsigned int i = 0; i < name.size(); ++i)
    {
        if (m_slangNameSet.find(name.substr(i)) != m_slangNameSet.end())
        {
            hit = true;
            break;
        }
    }
    pthread_mutex_unlock(&m_lock);
    return hit;
}

// ---- 事件/GM/统计 ----
// CEventManager 构造/析构已移入 CEventManager.cpp（独立 TU）。
Gm_List::Gm_List() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CGM_Manager::CGM_Manager()
{
    // ORIG 0x082990d0：map 构造（成员自动）+ rwLock 构造 + 存 DB 句柄(6) +
    // LoadGmListFromDb()。
    pthread_rwlock_init(&m_lock, 0);
    m_db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6,
                                             (ENUM_SERVER_GROUP)0);
    LoadGmListFromDb();
}
CGM_Manager::~CGM_Manager()
{
    // ORIG 0x08299159 附近：rwLock 析构 + map 自动析构。
    pthread_rwlock_destroy(&m_lock);
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
    pthread_rwlock_wrlock(&m_lock);
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
    pthread_rwlock_unlock(&m_lock);
    return result == 1;
}
CStatisticMgr::CStatisticMgr() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)

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
break_away_prevent::CBreakAwayPreventSystem::~CBreakAwayPreventSystem()
{
    // ORIG 0x0831d548 区域：map 析构（成员自动）+ 基类析构（推断）。
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
village_object::CVillageObjectMgr::CVillageObjectMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
village_object::CVillageObjectMgr::~CVillageObjectMgr() {}  // TODO(后续批次)
village_attacked::CVillageMonsterMgr::CVillageMonsterMgr()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
village_attacked::CVillageMonsterMgr::~CVillageMonsterMgr() {}  // TODO(后续批次)
village_attacked::CRevengeDungeon::CRevengeDungeon()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
bool village_attacked::CRevengeDungeon::IsOpenRevengeDungeon()  // ORIG 0x8234ff6：读 +0x0 字节
{
    return m_pad[0] != 0;
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
    // ORIG T 0x08270320：m_impl->m_machines[idx] = m。
    if (m_impl)
    {
        m_impl->m_machines[idx] = m;
    }
}
CDailyScheduleManager::CDailyScheduleManager()
{
    memset(m_pad, 0, sizeof(m_pad));
}  // TODO(后续批次)
CDailyScheduleManager::~CDailyScheduleManager() {}  // TODO(后续批次)
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
online_preliminary::COnlinePreliminaryTeamMgr::~COnlinePreliminaryTeamMgr()
{
    // ORIG 0x085886fe：delete factory；map 由编译器析构。
    delete m_factory;
    m_factory = 0;
}
game_master::CCommandMap::CCommandMap()
{
    // ORIG 0x082a7f64：map<int,CCommand*> + map<string,CCommand*> 构造（成员自动）。
}
void game_master::CCommandMap::insert(int id, const char* name, CCommand* cmd)
{
    // ORIG 注册：按 id 与命令名双索引（推断简化；ORIG 直接操作 map）。
    m_cmdById[id] = cmd;
    if (name)
    {
        m_cmdByName[name] = cmd;
    }
}
game_master::CGameMasterMgr::CGameMasterMgr()
{
    // ORIG 0x082a8a28（1610 insn）：构造 CCommandMap@+0x08；new CSeniorGameMaster/
    // CJuniorGameMaster；随后注册大量 GM 命令（CCommandMap::insert(id, name, cmd)）。
    // 命令对象类型（CMoveCharacCmd 等 40+ 类）未还原，此处保留构造序列与
    // 核心命令注册（推断：命令注册表不完整）。
    m_senior = new CSeniorGameMaster(0);
    m_junior = new CJuniorGameMaster(m_senior);
    CCommandMap& map = m_cmdMap;
    map.insert(1, "move", new CMoveCharacCmd);
}
CBusinessImpl::CBusinessImpl() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
CBusinessImpl::~CBusinessImpl() {}  // TODO(后续批次)

// ---- 定时器/DB 请求/反外挂 ----
void TimerUpdatePvPGrade::registNextTimer(long long) {}  // TODO(后续批次)
void Timer_GenerateCaptchaData::RegistNextTimer(int) {}  // TODO(后续批次)
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
    (void)p;
    return 0;  // 推断：Bingo 商城动作未还原
}
int StackableAction::Action::proc_addFatigue(input_param& p)
{
    (void)p;
    return 0;  // 推断：疲劳动作未还原
}
int StackableAction::Action::proc_lotteryCount(input_param& p)
{
    (void)p;
    return 0;  // 推断：抽奖计数动作未还原
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
    // ORIG 0x084a302e：new 3 个子提取器对象。
    m_alchemist = new CAlchemistExtraction;
    m_enchanter = new CEnchanterExtraction;
    m_doll = new CDollControllerExtraction;
}
expert_extraction::CExtractionMgr::~CExtractionMgr()
{
    // ORIG 0x084a311a：逐个 delete（虚析构释放）。
    delete m_alchemist;
    m_alchemist = 0;
    delete m_enchanter;
    m_enchanter = 0;
    delete m_doll;
    m_doll = 0;
}
NPC_Teleport::NPC_Teleport() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
User_Teleport::User_Teleport() { memset(m_pad, 0, sizeof(m_pad)); }  // TODO(后续批次)
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
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = GetRandomOption();
}
random_option::CRandomOptionItemHandle::~CRandomOptionItemHandle()
{
    // ORIG 0x085f0e46（推断）：释放 CMTRand 数组。
    delete[] m_rand;
    m_rand = 0;
}

// ---- 限制服务/地理/人证 ----
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

// ---- ServiceRestrictManager（ORIG 0x0816e272 ctor / 0x0816e2b2 initialize /
//      0x0816e7d6 loadPolicy / 0x0816e8ce savePolicy）----
// 依赖策略类（NullPolicy/AuthMobileRestrictPolicy/CreateCharacterRestrictPolicy/
// SecurityRestrictPolicy/InterfaceServicePolicy）与 Taiwan 内部包
// （SigLoadRestrictPolicy/SigSaveRestrictPolicy）。策略类未建模，本 TU 以最小
// 内部类型 + map 布局还原 initialize 的注册语义（推断：策略行为未还原）。
namespace
{
struct InterfaceServicePolicyBase
{
    virtual ~InterfaceServicePolicyBase() {}
    virtual int getCode() = 0;
    virtual void save(Taiwan::SigLoadRestrictPolicy*) = 0;
};

struct NullPolicy : InterfaceServicePolicyBase
{
    virtual int getCode() { return 0; }
    virtual void save(Taiwan::SigLoadRestrictPolicy*) {}
};
struct AuthMobileRestrictPolicy : InterfaceServicePolicyBase
{
    virtual int getCode() { return 1; }
    virtual void save(Taiwan::SigLoadRestrictPolicy*) {}
};
struct CreateCharacterRestrictPolicy : InterfaceServicePolicyBase
{
    virtual int getCode() { return 2; }
    virtual void save(Taiwan::SigLoadRestrictPolicy*) {}
};
struct SecurityRestrictPolicy : InterfaceServicePolicyBase
{
    virtual int getCode() { return 3; }
    virtual void save(Taiwan::SigLoadRestrictPolicy*) {}
};
}

class CDataManager;
CDataManager* G_CDataManager();

class ServiceRestrictManager
{
public:
    bool initialize();
    void savePolicy();
    void loadPolicy();
    // ORIG 布局：map<RestrictCategory::Enum, InterfaceServicePolicy*> + 2 标志字节。
    std::map<int, InterfaceServicePolicyBase*> m_policyMap;
    char m_flag18;   // +0x18（ORIG ctor 置 0）
    char m_flag19;   // +0x19（channel_no==1 时置 1）
};

bool ServiceRestrictManager::initialize()
{
    // ORIG 0x0816e2b2：channel_no==1 → 置 0x19=1；创建 4 个策略对象按 getCode()
    // 注册进 map（失败 log 并返回 0）。
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        m_flag19 = 1;
    }
    m_policyMap[0] = new NullPolicy;
    m_policyMap[1] = new AuthMobileRestrictPolicy;
    m_policyMap[2] = new CreateCharacterRestrictPolicy;
    m_policyMap[3] = new SecurityRestrictPolicy;
    if (m_policyMap[0] == 0 || m_policyMap[1] == 0 ||
        m_policyMap[2] == 0 || m_policyMap[3] == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              "bool ServiceRestrictManager::initialize()", 0x169,
                              "[Taiwan, Restrict] Fail create NullPolicy.");
        return false;
    }
    return true;
}

void ServiceRestrictManager::savePolicy()
{
    // ORIG 0x0816e8ce：0x19==1 时遍历 map，每条策略经 SigSaveRestrictPolicy
    // 流（internal_stream(2,-1)）序列化后入 DB 队列。策略 save() 为虚调用，
    // 当前策略类为空实现（推断：策略数据未还原）。
    if (m_flag19 != 1)
    {
        return;
    }
    for (std::map<int, InterfaceServicePolicyBase*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1de), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace t("void ServiceRestrictManager::savePolicy()", 0x1e3, 5);
            t("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
            return;
        }
        it->second->save(sig);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

void ServiceRestrictManager::loadPolicy()
{
    // ORIG 0x0816e7d6：SigLoadRestrictPolicy 请求（internal_stream(1,-1)）入 DB 队列。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/ServiceRestrictManager.cpp", 0x1c5), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(1), 0xffffffff);
    Taiwan::SigLoadRestrictPolicy* sig =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              "void ServiceRestrictManager::loadPolicy()", 0x1cc,
                              "[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
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
char GlobalData::g_SPremiumInfo[0x7740];
WongWork::CSimpleSSO* GlobalData::s_psimpleSSO = 0;
WongWork::CSecurityCardCenter* GlobalData::s_securityCardCenter = 0;
WongWork::CGMAccounts* GlobalData::s_pGMAccounts_ = 0;
CEventManager* GlobalData::s_event_manager = 0;
WongWork::IPG::CIPGHelper* GlobalData::s_pIPGHelper = 0;
void* GlobalData::s_rentalSystem = 0;
RefPvpGrade* GlobalData::s_ref_pvp_grade = 0;
WongWork::CLogGameChannel* GlobalData::s_pLogGameChannel = 0;
WongWork::CCeraShop* GlobalData::s_pCeraShop = 0;
CDBConnector* GlobalData::s_db_connector = 0;
void* GlobalData::s_pCompoundAvatar = 0;
void* GlobalData::s_pCompoundBindCube = 0;
void* GlobalData::s_pTowerOfDespairMgr = 0;
void* GlobalData::s_pEvent_UpGrade_Jar = 0;
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
        s_accept_thr->Stop();
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
        s_udp_thr->Stop();
    }
    if (s_net_thr)
    {
        s_net_thr->Stop();
    }
    if (s_dispatch_thr)
    {
        s_dispatch_thr->Stop();
    }
    if (s_db_thr_for_logDB)
    {
        s_db_thr_for_logDB->Stop();
    }
    if (s_db_thr)
    {
        s_db_thr->Stop();
    }
    if (s_monitor_tcp_thr)
    {
        s_monitor_tcp_thr->Stop();
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
    delete (InstanceRentalSystem*)s_rentalSystem;
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
    sync_script::CSyncScript::SetEnable(syncEnable);
    if (sync_script::CSyncScript::IsEnable())
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
    if (CSyncSlangFilter::AddSlangListName() == 1)
    {
        if (CSyncSlangFilter::AddSlangList() != 1)
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

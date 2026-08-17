#ifndef GAME_CENVIRONMENT_H_
#define GAME_CENVIRONMENT_H_

// df_game_r CEnvironment（服务器配置解析与访问）还原（2026-08-16）。
// 依据 ORIG 汇编/rodata：全部偏移、vtable、函数形态与 df_game_r 一致。

#include <map>
#include <string>
#include <vector>
#include <utility>

#include "GameTypes.h"

// ---- 全局枚举（df_game_r 全局命名空间）----
enum ENUM_SERVER_GROUP { E_SERVER_GROUP_NONE = 0 };
enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_0 = 0, DB_HANDLE_1, DB_HANDLE_2, DB_HANDLE_3, DB_HANDLE_4,
    DB_HANDLE_5, DB_HANDLE_6, DB_HANDLE_7, DB_HANDLE_8, DB_HANDLE_9,
    DB_HANDLE_A, DB_HANDLE_B, DB_HANDLE_C, DB_HANDLE_D, DB_HANDLE_E,
    DB_HANDLE_F, DB_HANDLE_10
};
enum eStartArgumentType_t { eStartArgumentType_NONE = 0 };

// ---- 全局 CTEA（与 DBMW 侧同源；本 TU 只做声明，实现属于加密库 TU）----
class IMethod
{
public:
    IMethod();
    virtual ~IMethod();
    virtual void Signature(char* szSignature) = 0;
    virtual int Encrypt(const char* szIn, char* szOut, size_t iLength) = 0;
    virtual int Decrypt(const char* szIn, char* szOut, size_t iLength) = 0;
    virtual void ResetChain() = 0;

    // G7 分组模式辅助（实现属加密库 TU：CRijndael.cpp）
    void Xor(char* buff, const char* chain);
    void SetMode(int iMode);

    bool m_bInit;      // +0x04
    int m_blockSize;   // +0x08
    int m_keyLength;   // +0x0c
    int m_mode;        // +0x10
    int m_padding;     // +0x14
};

class CTEA : public IMethod
{
public:
    CTEA();
    virtual ~CTEA();
    virtual void Signature(char* szSignature);
    virtual int Encrypt(const char* szIn, char* szOut, size_t iLength);
    virtual int Decrypt(const char* szIn, char* szOut, size_t iLength);
    virtual void ResetChain();
    int Initialize(const char* key, int keyLen, const char* iv, int ivLen, int mode);
    static const char sm_chain0[8];

    unsigned char m_key[0x10];    // +0x18
    unsigned char m_iv[0x10];     // +0x28
    unsigned char m_chain[8];     // +0x38
    unsigned char m_chain2[8];    // +0x40
};

// ---- less_TCHAR（ORIG map 比较器；实现属于 STL/ServerLib TU）----
struct less_TCHAR
{
    bool operator()(const char* const& x, const char* const& y) const;
};

// ---- SpinLock（ORIG 全局类；本 TU 只做声明，enter/leave 属于 ServerLib TU）----
class SpinLock : public Uncopyable
{
public:
    SpinLock();
    ~SpinLock();
    void enter();
    void leave();
    bool isTaken();
    bool tryEnter();

private:
    volatile int m_lock;   // +0x00
};

// ---- SK_IP：stun 服务器 IP 列表元素（16 字节）----
struct SK_IP
{
    char m_ip[0x10];
};

// ---- STDBConnInfo：DB 连接信息（0x168 字节）----
struct STDBConnInfo
{
    STDBConnInfo();

    char m_host[0x11];      // +0x00
    int m_port;             // +0x14
    char m_user[0x15];      // +0x18
    char m_pass[0x15];      // +0x2d
    char m_db[0x1f];        // +0x42
    char m_pad61[0x100];    // +0x61
    int m_tail;             // +0x164
};

// ---- Server_Envir：服务器环境配置主体（0x22c 字节，位于 CEnvironment +0x198）----
class Server_Envir
{
public:
    Server_Envir();
    ~Server_Envir();

    // 配置字段（偏移相对 Server_Envir 基址）
    int m_gcNo;                        // +0x00
    int m_smallPoolSize;               // +0x04
    int m_defaultPoolSize;             // +0x08
    int m_bigPoolSize;                 // +0x0c
    int m_serverType;                  // +0x10
    int m_processSequence;             // +0x14
    int m_channelNo;                   // +0x18
    char m_serverIP[0x10];             // +0x1c
    unsigned int m_serverAddr;         // +0x2c
    int m_tcpPort;                     // +0x30
    int m_udpPort;                     // +0x34
    char m_udpIPMonitor[0x10];         // +0x38
    int m_udpPortMonitor;              // +0x48
    char m_udpIPGuild[0x10];           // +0x4c
    int m_udpPortGuild;                // +0x5c
    int m_tcpPortMonitor;              // +0x60
    int m_tcpPortGuild;                // +0x64
    char m_udpIPHades[0x10];           // +0x68
    int m_udpPortHades;                // +0x78
    int m_tcpPortHades;                // +0x7c
    char m_udpIPStatistic[0x10];       // +0x80
    int m_udpPortStatistic;            // +0x90
    char m_udpIPDoubleCheck[0x10];     // +0x94
    int m_udpPortDoubleCheck;          // +0xa4
    char m_udpIPChannel[0x10];         // +0xa8
    int m_udpPortChannel;              // +0xb8（udp/tcp 共用）
    int m_padBC;                       // +0xbc（未访问）
    char m_exchangeServerIP[0x10];     // +0xc0
    int m_exchangeServerPort;          // +0xd0
    char m_schoolServerIP[0x10];       // +0xd4
    int m_schoolServerPort;            // +0xe4
    char m_pcroomServerIP[0x10];       // +0xe8
    int m_pcroomServerPort;            // +0xf8
    char m_communityServerIP[0x10];    // +0xfc
    int m_communityServerPort;         // +0x10c
    char m_systemMonitoringIP[0x10];   // +0x110
    char m_ipgIP[0x10];                // +0x120
    char m_auctionServerIP[0x10];      // +0x130
    int m_auctionServerPort;           // +0x140
    char m_ceraAuctionServerIP[0x10];  // +0x144
    int m_ceraAuctionServerPort;       // +0x154
    int m_acceptLockDBQueueCount;      // +0x158
    int m_acceptUnlockDBQueueCount;    // +0x15c
    std::string m_baseScriptDirectory; // +0x160
    std::vector<std::string> m_priorityScriptDir;  // +0x164
    char m_scriptpacks;                // +0x170
    int m_channelType;                 // +0x174
    char m_validateScript;             // +0x178
    std::string m_garenaPublickey;     // +0x17c
    char m_llsKey[0x2d];               // +0x180
    char m_relayIP[0x10];              // +0x1ad
    int m_relayTCPPort;                // +0x1c0
    int m_relayUDPPort;                // +0x1c4
    std::vector<SK_IP> m_stunIP;       // +0x1c8
    std::vector<int> m_stunPort;       // +0x1d4
    int m_serverGroup;                 // +0x1e0
    int m_fatigueTime;                 // +0x1e4
    int m_avatarTime;                  // +0x1e8
    char m_channelName[0x1e];          // +0x1ec
    char m_fileName[0x1e];             // +0x20a
    int m_maxUserNum;                  // +0x228
};

// ---- CEnvironment（全局命名空间，与 df_game_r 一致）----
class CEnvironment
{
public:
    CEnvironment();
    ~CEnvironment();

    virtual bool parse(char* line);

    bool load();
    bool fgetline(FILE* fp, char* buf);
    bool check_comment(char* line);
    bool DecryptValue(const char* value, char* dst);
    bool IsEnableValue(const char* value);
    void adjust_base_script_directory();
    bool parse_se_event_db(char* line, char* value);

    char* get_file_name();
    void set_file_name(const char* name);
    char* get_server_ip();
    int get_tcp_port();
    int get_udp_port();
    int get_channel_no();
    int get_gc_no();
    int get_gc_no_hardcode(ENUM_SERVER_GROUP group);
    char* get_channel_name();
    void set_channel_name(char* name);
    int get_server_group();
    bool get_suspend_state();
    void set_suspend_state(bool state);
    eStartArgumentType_t get_operation_type();
    void set_operation_type(eStartArgumentType_t type);

    char* get_udp_ip_monitor();
    int get_udp_port_monitor();
    char* get_udp_ip_guild();
    int get_udp_port_guild();
    int get_tcp_port_guild();
    int get_tcp_port_monitor();
    char* get_udp_ip_hades();
    int get_udp_port_hades();
    int get_tcp_port_hades();
    char* get_udp_ip_statistic();
    int get_udp_port_statistic();
    char* get_udp_ip_doublecheck();
    int get_udp_port_doublecheck();
    char* get_udp_ip_channel();
    int get_udp_port_channel();
    int get_tcp_port_channel();
    char* get_exchange_server_ip();
    int get_exchange_server_port();
    char* get_school_server_ip();
    int get_school_server_port();
    char* get_pcroom_server_ip();
    int get_pcroom_server_port();
    char* get_community_server_ip();
    int get_community_server_port();
    char* get_auction_server_ip();
    int get_auction_server_port();
    char* get_cera_auction_server_ip();
    int get_cera_auction_server_port();
    char* get_system_monitoring_ip();

    void lock();
    void unlock();

private:
    friend class StreamPool;       // 三个池构造函数读取 m_serverEnvir 池大小（ORIG 直访偏移）
    friend class SmallStreamPool;
    friend class BigStreamPool;
    CTEA m_cipher;                              // +0x04（0x48 字节）
    eStartArgumentType_t m_operationType;       // +0x4c
    std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > m_serverGroupMap;   // +0x50
    std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> m_dbConnInfo;                             // +0x68
    std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> > m_dbConnInfoByGroup;  // +0x80
    char m_dbTblFile[0x100];                    // +0x98
    Server_Envir m_serverEnvir;                 // +0x198（0x22c 字节）
    Mutex m_lock;                               // +0x3c4（0x18 字节）
    bool m_suspendState;                        // +0x3dc
};

// ---- GlobalInstance（df_game_r 全局命名空间模板：inst_ptr/create/inst/destroy）----
template <class T>
class GlobalInstance
{
public:
    static T* m_p;

    static T* inst_ptr()
    {
        create();
        return m_p;
    }

    static T* inst()
    {
        create();
        return m_p;
    }

    static void create()
    {
        if (m_p == NULL)
        {
            if (m_p == NULL)
            {
                m_p = new T;
            }
        }
        else
        {
            return;  // ORIG：外层 if/else + return，产出 jne→nop/jne→出口 形态
        }
    }

    static void destroy()
    {
        if (m_p)
        {
            delete m_p;
            m_p = NULL;
        }
    }
};

template <class T>
T* GlobalInstance<T>::m_p = NULL;

// ---- trim（ORIG 全局函数，实现属于 ServerCommon/Library TU）----
void trim(char* str);

// ---- 单例访问器 ----
CEnvironment* G_CEnvironment();

#endif  // GAME_CENVIRONMENT_H_

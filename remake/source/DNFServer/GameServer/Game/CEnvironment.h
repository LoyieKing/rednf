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
    virtual int ResetChain() = 0;

    // G7 分组模式辅助（实现属加密库 TU：CRijndael.cpp）
    void Xor(char* buff, const char* chain);
    // ORIG _ZN7IMethod7SetModeEi T @0x080b436e（0x39）：**返回 int 错误码**
    // （m_bInit!=1 ⇒ 0x70000005；mode 越界 [0,2] ⇒ 0x70000003；否则写 m_mode
    //  +0x10 后返 0x6fffffff）。旧 void 建模漏了返回值透传链（CNCryptoTea/
    // Rijndael::SetMode 的 ORIG 体直接以本函数 eax 为返回）。
    int SetMode(int iMode);
    // ORIG _ZN7IMethod11BytesToWordEPKhRj / _ZN7IMethod11WordToBytesEjPh
    // （CTEA EncryptBlock/DecryptBlock 字节↔字组转换；实现属 ServerLib/Method.cpp:145/157。
    //  Main 对齐权威形：**static**——ServerLib Method.h:49 即 static，非 static 声明使
    //  CTEA 调用点多出 this 装载压栈（.text+0x283 前多一条 mov），且与定义侧签名不一致。）
    static void BytesToWord(unsigned char const* src, unsigned int& dst);
    static void WordToBytes(unsigned int src, unsigned char* dst);
    // ORIG 非虚成员（_ZN7IMethod12GetBlockSizeEPj T 0x080b4416 /
    //   _ZN7IMethod3PadEPciPj T 0x080b44a6；定义在 DBMW/Method.cpp）。
    int GetBlockSize(unsigned int* outError);
    int Pad(char* data, int len, unsigned int* outError);
    // ORIG 非虚成员（真缺口 4 条，报告 docs/class_func_reports/IMethod/*.md）：
    //   _ZN7IMethod10SetPaddingEi  T 0x080b43aa（0x3b）
    //   _ZN7IMethod12GetKeyLengthEPj T 0x080b43e6（0x2f）
    //   _ZN7IMethod7GetModeEPj     T 0x080b4446（0x2f）
    //   _ZN7IMethod10GetPaddingEPj T 0x080b4476（0x2f）
    // 定义落 CTEA.cpp（同 IMethod::BytesToWord/WordToBytes 所在 TU）。
    // 非虚成员函数不入布局 ⇒ sizeof/成员偏移零变化。
    int SetPadding(int iPadding);
    int GetKeyLength(unsigned int* outError);
    int GetMode(unsigned int* outError);
    int GetPadding(unsigned int* outError);
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
    virtual int ResetChain();
    int Initialize(const char* key, int keyLen, const char* iv, int ivLen, int mode);
    // ORIG _ZN4CTEA12EncryptBlockEPKhPh/_ZN4CTEA12DecryptBlockEPKhPh（TEA 分组核心）
    int EncryptBlock(unsigned char const* src, unsigned char* dst);
    int DecryptBlock(unsigned char const* src, unsigned char* dst);
    static const char sm_chain0[8];

    unsigned char m_key[0x10];    // +0x18
    unsigned int m_iv[4];         // +0x28（同布局 0x10B；uint[4] 下标供 BytesToWord 实参 lea 一次成址）
    unsigned char m_chain[8];     // +0x38
    unsigned char m_chain2[8];    // +0x40
};

// ---- less_TCHAR（ORIG map 比较器；实现属于 STL/ServerLib TU）----
struct less_TCHAR
{
    // ORIG mangled _ZNK10less_TCHARclEPKcS1_（按值参数）。
    bool operator()(const char* x, const char* y) const;
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
    unsigned int m_port;    // +0x14（ORIG QueryConnInfo 走 get_int EiRj unsigned 重载）
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
    // ORIG parse case 0x435 @0x082910f3..f8 为 RHS-first+edx 中转（P211 符号性异型）
    // ⇒ 字段与 inet_addr() 的 unsigned 返回值异型；探针 /tmp/ceprobe/p.o 三形实测：
    // unsigned int/unsigned long ⇒ LHS-first，int ⇒ RHS-first+中转。4 字节，布局不变。
    int m_serverAddr;                  // +0x2c
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
    // bool 实证（§8.3 字段化）：ORIG init.md +0x007d 区 `movzbl 0x308(%eax),%eax;
    // movzbl %al,%esi` 传 InitRDARScript 的 bool 形参，无 test/setne 规范化 ⇒ 字段
    // 本身即 bool（char 会多 2 条，P9/P22/P32）；消费者仅 =0/=1 赋值，指令同形。
    bool m_scriptpacks;                // +0x170
    int m_channelType;                 // +0x174
    bool m_validateScript;             // +0x178（bool：ORIG movzbl 0x310(%eax)+test+je 形态）
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
    int getProcessSequence() const;                        // ORIG W 0x08234516（读 m_serverEnvir.m_processSequence）
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

    // [b28d-Main·检查点18] 删除自造 accessor get_db_conn_info()：ORIG 无该符号；
    //   -O0 下头内 inline accessor 被调用时仍发射弱符号（实测 only-NEW 有
    //   `_ZN12CEnvironment16get_db_conn_infoEv`）⇒ 调用点改直读 m_dbConnInfo。

    void lock();
    void unlock();

private:
    friend class StreamPool;
    friend class SmallStreamPool;
    friend class BigStreamPool;
    friend bool Init(int, char**);              // GameMain.cpp（ORIG 直访 +0x50）
    CTEA m_cipher;                              // +0x04（0x48 字节）
    eStartArgumentType_t m_operationType;       // +0x4c
    // 偏移清理（2026-09-04 GlobalData 批次）：ORIG GlobalData::initTournamentDB
    // 直访本表（G_CEnvironment()+0x50，见 GlobalData.cpp 站点注释），置 public
    // 以具名直访替代裸偏移；声明位置不变（+0x50），仅访问级别调整（先例 m_dbConnInfo）。
public:
    std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > m_serverGroupMap;   // +0x50
public:
    // ORIG GlobalData::Init 经 G_CEnvironment()+0x68 逐站点直访该表（每 operator[]
    // 站点独立取全局，非缓存引用），故置 public 以具名直访 m_dbConnInfo，忠实匹配调用序。
    // 声明位置不变（+0x68），仅访问级别调整。
    std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> m_dbConnInfo;                             // +0x68
private:
public:
    // 同上：ORIG initTournamentDB 直访 +0x80（GroupDBMap operator[] 写站点），
    // 声明位置不变（+0x80），仅访问级别调整。
    std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> > m_dbConnInfoByGroup;  // +0x80
private:
    char m_dbTblFile[0x100];                    // +0x98
public:
    // 服务器环境布局由 ORIG +0x198 固定；必须声明在 m_dbTblFile 之后（声明于
    // public 区起始会使其落在 +0x04，导致 GameWorld 等协议组装代码偏移全部错误）。
    Server_Envir m_serverEnvir;                 // +0x198（0x22c 字节）
private:
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
                // ORIG（Store 实证 0x082711xx）：trivial T 时 value-init 清零、
                // 非 trivial T 与 new T 同形。
                m_p = new T();
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
char* trim(char* str);
void trim(std::string& s);   // ORIG _Z4trimRSs @0x0829f068（game_master 命令体系用）

// ---- 单例访问器 ----
CEnvironment* G_CEnvironment();

#endif  // GAME_CENVIRONMENT_H_

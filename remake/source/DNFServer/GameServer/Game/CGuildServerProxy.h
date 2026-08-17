#ifndef GAME_CGUILDSERVERPROXY_H_
#define GAME_CGUILDSERVERPROXY_H_

// ============================================================================
// df_game_r 还原 —— CGuildServerProxy（公会服务器客户端代理，D4/E2 批次）。
// 参照 docs/class_func_reports/CGuildServerProxy.md + df_game_r 反汇编。
// 布局（sizeof 0x30e34，CServerProxyMgr<CGuildServerProxy> 内嵌于 +0x10）：
//   +0x00 std::string m_ip
//   +0x04 int m_udpPort
//   +0x08 std::map<unsigned int, STGuildProxy> m_guildInfoMap（代理侧公会缓存）
//   +0x20 int m_tcpPort
//   +0x24 CNetwork<100000,100000> m_tcp
//   +0x30e1c CUdpHandler m_udp
// 内联函数（默认 ctor / Init(string,int,int) / dtor / GetTcpHandler）为 ORIG
// 弱符号（0x82a6a96 / 0x82a6b3a / 0x82b31be / 0x846fc8e），与本头定义逐字对应。
// STGuildProxy（26 字节：name[0x16] + lev + powerSide + agitFlag + pad）的
// 默认 ctor 属独立 TU（ORIG 弱符号 0x82344de），此处只声明。
// ============================================================================

#include <map>
#include <string>

#include "CNetwork.h"
#include "CUdpHandler.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Packet_Add_Guild_Fund.h"

// CDataManager 完整定义仅 .cpp 需要（SendGuildExpLimit 直访字段），
// 头文件只做前向声明，避免把 CItemList.h 等大依赖带入调用方 TU。
class CDataManager;
CDataManager* G_CDataManager();

// ---- STGuildProxy：代理侧公会缓存条目（sizeof 0x1a）----
struct STGuildProxy
{
    STGuildProxy();                        // 0x082344de W（独立 TU）
    STGuildProxy(const char* name, unsigned char lev,
                 unsigned char powerSide); // 0x084ed19a W（独立 TU）

    char m_guildName[0x16];   // +0x00
    char m_pad16;             // +0x16（默认 ctor memset 覆盖，无语义证据）
    unsigned char m_level;    // +0x17
    unsigned char m_powerSide;// +0x18
    unsigned char m_agitFlag; // +0x19
};

// ---- 外部类型（权威定义在 CUser.h / CUserCharacInfo.h，此处仅前向）----
class CUserCharacInfo;
class CUser;

class CItem;

// 事件参数（StartEvent(Word_Param) 按值传入；低 ushort 为持续分钟数）。
struct Word_Param
{
    unsigned short m_duration;  // +0x00
    unsigned short m_field2;    // +0x02
};

class PacketGuard;
class CDelivery;
class DeliveryMsg;

// CEventBase（ORIG vtable 0x08b4bae0）：0/1 = 虚析构，2/3 = StartEvent()/
// EndEvent() 纯虚，4 = StartEvent(Word_Param)（基类弱实现），5..13 =
// StartAction/AppendInfo/GetAddInfo/changeCharacName/dailyresetData/
// deliveryStart/deliveryEnd/delivery/IsEventing。ORIG SendPowerWarPoint 与
// CGuildWarEvent 均经 vptr+0x34（slot 13 = IsEventing(CUser*)）判活。
class CEventBase
{
public:
    virtual ~CEventBase();
    virtual void StartEvent() = 0;              // vtable+0x10
    virtual void EndEvent() = 0;                // vtable+0x14
    virtual void StartEvent(Word_Param param);  // vtable+0x18
    virtual void StartAction();                 // vtable+0x1c
    virtual void AppendInfo(PacketGuard& packet);  // vtable+0x20
    virtual int GetAddInfo();                   // vtable+0x24
    virtual void changeCharacName(CUser* user, int a, char* name, int b);
                                                // vtable+0x28
    virtual void dailyresetData();              // vtable+0x2c
    virtual void deliveryStart(CDelivery* delivery);  // vtable+0x30
    virtual void deliveryEnd(CDelivery* delivery);    // vtable+0x34
    virtual void delivery(DeliveryMsg& msg);    // vtable+0x38
    virtual bool IsEventing(CUser* user) const; // vtable+0x3c
    void SetEventFlag(bool flag);               // 非虚 0x080c84e2
};

class CEventManager
{
public:
    CEventManager();                       // 0x08114ce4 T（独立 TU）
    CEventBase* GetRepeatEvent(int type);  // 0x08115998
private:
    char m_pad[0x29c];
};

namespace WongWork
{
class CGMAccounts
{
public:
    CGMAccounts();                         // 0x082a73a4 W（独立 TU）
    ~CGMAccounts();                        // 0x082a73b8 W（独立 TU）
    bool isGM(unsigned int mid);           // 0x08109346
private:
    char m_pad[8];
};
}

class Gm_List
{
public:
    Gm_List();                            // 0x08299912 T（独立 TU）
    void SendGMList_To_UpperServer(int param);  // 0x08299d54
private:
    char m_pad[4];
};

class CGuildServerProxy
{
public:
    CGuildServerProxy();                                        // 0x082a6a96 W
    CGuildServerProxy(std::string ip, int udpPort, int tcpPort);// 0x0846d818 T
    ~CGuildServerProxy();                                       // 0x082b31be W
    void Init(std::string ip, int udpPort, int tcpPort);        // 0x082a6b3a W

    bool Init();                        // 0x0846d8c2
    bool InitUdp();                     // 0x0846d912
    bool InitTcp();                     // 0x0846d96c
    void Disconnect();                  // 0x0846da84
    void SendCharLevelGrowType(unsigned int, unsigned int, short, short);  // 0x0846da9a
    void SendHeartBeat(int);            // 0x0846daf4
    void SendLogin(int, unsigned int, int, char*, unsigned char, char*);   // 0x0846db2a
    void SendLogout(unsigned int, int, char*, unsigned int, unsigned int,
                    unsigned int, unsigned char);               // 0x0846dbfe
    void SendPacket(char*, int);        // 0x0846dc7e
    void SendTcpPacket(char*, int);     // 0x0846dcce
    void SendPacketCallGuildMembers(unsigned int, unsigned int);// 0x0846dd12
    void SendPacketCallGuildAllMembers(unsigned int, unsigned int);// 0x0846dd4e
    void SendGuildLevelup(int, unsigned int, unsigned int, unsigned int);// 0x0846dd8a
    void SendCallGuildInfo(unsigned int, unsigned int, unsigned char,
                           unsigned int*);                      // 0x0846ddd2
    void SendPowerWarStart(unsigned int);  // 0x0846de80
    void SendPowerWarEnd(unsigned int);    // 0x0846dece
    void SendPowerWarProcessInfo(unsigned int);  // 0x0846df1c
    void SendGuildWarEnd(unsigned char);   // 0x0846df52
    void SendGuildWarStart(unsigned char); // 0x0846df90
    void SendPowerWarCfg(unsigned short, int, int, int, int);   // 0x0846dfce
    void SendBuyGuildSkill(unsigned int, unsigned int, int, int,
                           short, unsigned int);                // 0x0846e026
    void SendGuildMail(unsigned int, unsigned int, int, char*); // 0x0846e084
    void SendCallGuildInvite(unsigned int, unsigned int, char*);// 0x0846e0f2
    void SendReplyGuildInvite(unsigned int, unsigned char);     // 0x0846e14c
    void SendRequestGuildSecede(unsigned int, unsigned int, int, char*);// 0x0846e190
    void SendMessageToGuild(unsigned int, int, char*);          // 0x0846e1f4
    void SendMemoToGuild(unsigned int, int, int, char*);        // 0x0846e254
    void SendGuildMasterDelegate(unsigned int, unsigned int, int, char*);// 0x0846e2ac
    void SendGuildCreate(unsigned int, char*, char*);           // 0x0846e304
    void SendGuildNameChange(unsigned int, unsigned int, char*);// 0x0846e376
    void SendChangeCharName(unsigned int, unsigned int, unsigned int,
                            char*);                             // 0x0846e3c8
    void SendReqestPowerWarStartInfo();  // 0x0846e420
    void SendPowerWarPoint(class Packet_Guild_Change_Power_War_Point&);// 0x0846e460
    void SendJoinPower(unsigned int, unsigned int, unsigned char);// 0x0846e4b6
    void SendSecedePower(unsigned int, unsigned int);           // 0x0846e500
    int OnTcpRecv();                    // 0x0846e53c
    int OnTcpSend();                    // 0x0846e5cc
    bool OnTcpError();                  // 0x0846e66c
    bool Parsing(int);                  // 0x0846e6f6
    bool LoadGuildInfoProxyFromDB(ENUM_SERVER_GROUP);           // 0x0846e714
    bool ModifyGuildInfoProxy(unsigned int, STGuildProxy&);     // 0x0846e95e
    void ChangeGuildNameInProxy(unsigned int, char*);           // 0x0846ea3e
    void ChangeGuildAgitFlagInProxy(unsigned int, unsigned char);// 0x0846eb0e
    bool GetGuildInfoProxy(unsigned int, STGuildProxy&);        // 0x0846ebba
    bool InsertGuildInfoProxy(unsigned int, STGuildProxy&);     // 0x0846ec88
    void SendIncreaseGuildExp(unsigned int, unsigned int, unsigned int,
                              char);                            // 0x0846ece2
    void SendIncreaseDecreasePowerWarPoint(unsigned int, unsigned int,
                                           int);                // 0x0846ed36
    void SendCreateGuildAgit(unsigned int, unsigned int, unsigned int,
                             unsigned int);                     // 0x0846ed78
    void SendDeleteGuildAgit(unsigned int, unsigned int);       // 0x0846edc0
    void SendUpgradeGuildAgit(unsigned int, unsigned int, unsigned int,
                              unsigned int);                    // 0x0846edfc
    void SendGuildExpLimit();           // 0x0846ee44
    void SendApplyOriginalPowerSide(unsigned int, unsigned int,
                                    unsigned char);             // 0x0846eeda
    void OnBuyItem(CUser*, CItem const*, unsigned int, unsigned int);// 0x0846ef24
    void SendGuildBoardOpen(unsigned int, unsigned int, char);  // 0x0846efce
    void SendGuildBoardWrite(char*, unsigned int, unsigned int, int,
                             char, unsigned int);               // 0x0846f01a
    void SendGuildBoardDelete(unsigned int, unsigned int, unsigned int);// 0x0846f0b0
    void SendGameServerInfo(char*, unsigned short, unsigned char,
                            unsigned char);                     // 0x0846f0f2
    void SendRefreshGuildInfo(unsigned int, unsigned int);      // 0x0846f16c
    void SendAddGuildFund(unsigned int, unsigned int, int,
                          Packet_Add_Guild_Fund::eFundType);    // 0x0846f1a8

    CNetwork<100000, 100000>* GetTcpHandler()  // 0x0846fc8e W
    {
        return &m_tcp;
    }

private:
    std::string m_ip;                                  // +0x00
    int m_udpPort;                                     // +0x04
    std::map<unsigned int, STGuildProxy> m_guildInfoMap;// +0x08
    int m_tcpPort;                                     // +0x20
    CNetwork<100000, 100000> m_tcp;                    // +0x24
    CUdpHandler m_udp;                                 // +0x30e1c
};

// ---- 内联弱符号（ORIG 0x82a6a96 / 0x82b31be / 0x82a6b3a）----
inline CGuildServerProxy::CGuildServerProxy()
{
    m_udpPort = 0;
    m_tcpPort = 0;
}

// 0x0846d818 为强符号（T）：实现置于 CGuildServerProxy.cpp（本 TU）。

inline CGuildServerProxy::~CGuildServerProxy()
{
}

inline void CGuildServerProxy::Init(std::string ip, int udpPort, int tcpPort)
{
    m_ip = ip;
    m_udpPort = udpPort;
    m_tcpPort = tcpPort;
    Init();
}

#endif  // GAME_CGUILDSERVERPROXY_H_

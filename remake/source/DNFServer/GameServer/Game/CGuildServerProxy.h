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
// STGuildProxy（0x1a 字节：name[0x16] + pad16 + lev + powerSide + agitFlag）
// 两 ctor 为 ORIG 弱符号（0x82344de / 0x84ed19a，地址分散在非本 TU 段）⇒ 头内
// 定义、ODR 使用点发射 W（P233；判据㉓ 我方原为 T 强符号）。
// ============================================================================

#include <map>
#include <string.h>   // STGuildProxy inline ctor 的 memset/memcpy

class MySQL;
#include <list>
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
    // ORIG 两 ctor 均为 **W**（_ZN12STGuildProxyC1Ev/C2Ev @0x082344de(0x38)、
    // C1EPKchh/C2EPKchh @0x084ed19a(0x42)），地址分散在非本 TU 段（后者落在
    // InterDispatcher 段）⇒ 原码为头内定义、ODR 使用点发射 W（P233/判据㉓）。
    // 体与旧 .cpp 外联定义逐条相同；sizeof/偏移/mangled 不变。
    STGuildProxy()
    {
        m_level = 0;
        m_powerSide = 0;
        m_agitFlag = 0;
        ::memset(&m_guildName, 0, 0x17);   // ORIG：先算地址(加载 this)再装常量（序匹配）
    }
    STGuildProxy(const char* name, unsigned char lev,
                 unsigned char powerSide)
    {
        m_level = lev;
        m_powerSide = powerSide;
        ::memcpy(m_guildName, name, 0x16);
    }

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
// DeliveryMsg（ORIG W C2 0x086978c0 / setBaseData 0x086978ec，无虚函数；定义 CUserDepsB.cpp）。
class DeliveryMsg
{
public:
    DeliveryMsg();
    void setBaseData(int type, unsigned int accId, int characNo, int serverGroup);

    int m_type;           // +0x00（ctor 置 0；eventDeliveryNotify 首参取此字段）
    unsigned int m_accId; // +0x04
    int m_characNo;       // +0x08
    int m_serverGroup;    // +0x0c（ctor 置 -1）
};

// BuyItemDeliveryMsg（ORIG W C2 0x0869795a：基类构造后清零两扩展字段；
// 定义 CUserDepsB.cpp）。
class BuyItemDeliveryMsg : public DeliveryMsg
{
public:
    BuyItemDeliveryMsg();

    int m_deliveryType;          // +0x10
    const struct Inven_Item* m_item;  // +0x14
};

// stADDINFO（CEventBase +0x04，sizeof 4）：两个 short，ctor 0x0810af10 置 0xffff。
// 字段为 signed short（ORIG 2026-08-28 OrigEventLeaf8f 实证：AttendanceEvent/
// CFatigueAttendance StartEvent(Word_Param) 写入 movzwl→eax→edx→movw 中转形态，
// getRewordItem 读取 movzwl+cwtl 符号扩展；ushort 声明生成 edx 直存异形）。
struct stADDINFO
{
    stADDINFO();          // 0x0810af10（CEventBase TU 内联或独立 TU）
    short m_info0;        // +0x00
    short m_info1;        // +0x02
};

// stEVENTADDINFO（CEventManager::MakeNotiEventInfo 的 vector<stEVENTADDINFO> 元素）：
// ORIG 0x08115de2 读取 +0x00(movswl eventId)/+0x04(movzwl info0)/+0x06(movzwl info1)，
// 中间 +0x02 为 2 字节 gap；sizeof 0x08。CEventManager 此前用本地异名 EventAddInfo
// （连续 3 ushort，6 字节）——布局不符，此地为权威声明（§4.4 唯一声明点）。
struct stEVENTADDINFO
{
    stEVENTADDINFO() {}
    ~stEVENTADDINFO() {}          // ORIG D1/D2 0x08118b56 W 空体（非平凡，发射 D1/D2）
    short eventId;            // +0x00（mov (%eax);movswl %ax = 有符号短，符号扩展）
    unsigned short m_pad2;    // +0x02（ORIG 布局 2 字节 gap，未访问）
    unsigned short info0;     // +0x04（movzwl;movswl = 无符号短存储，读取处符号短）
    unsigned short info1;     // +0x06（movzwl;movswl = 无符号短存储，读取处符号短）
};

// CEventBase（ORIG vtable 0x08b4bae0）：0/1 = 虚析构，2/3 = StartEvent()/
// EndEvent() 纯虚，4 = StartEvent(Word_Param)（基类弱实现），5..13 =
// StartAction/AppendInfo/GetAddInfo/changeCharacName/dailyresetData/
// deliveryStart/deliveryEnd/delivery/IsEventing。ORIG SendPowerWarPoint 与
// CGuildWarEvent 均经 vptr+0x34（slot 13 = IsEventing(CUser*)）判活。
class CEventBase
{
public:
    CEventBase();                          // 0x0810ae2c
    virtual ~CEventBase();
    virtual void StartEvent() = 0;              // vtable+0x08（ORIG 0x08724fe0）
    virtual void EndEvent() = 0;                // vtable+0x0c（ORIG 0x08724fe0）
    virtual void StartEvent(Word_Param param);  // vtable+0x10（ORIG 0x0810af26）
    virtual void StartAction();                 // vtable+0x14（ORIG 0x080c84c4）
    virtual void AppendInfo(PacketGuard& packet);  // vtable+0x18（ORIG 0x080c84ca）
    virtual stADDINFO GetAddInfo();   // vtable+0x1c（ORIG 0x080c84d0）；返回 4 字节非 POD stADDINFO
    // （ORIG 0x080c84d0 sret 形态：写 this+0x04 至隐藏 out 指针，ret $4，
    //  2026-08-28 OrigEventLeaf8f 修正 int→stADDINFO，mangled 不变 Ev）
    virtual void changeCharacName(CUser* user, int a, char* name, int b);
                                                // vtable+0x20（ORIG 0x0810ae96）
    virtual void dailyresetData();              // vtable+0x24（ORIG 0x0810ae9c）
    virtual void deliveryStart(CDelivery* delivery);  // vtable+0x28（ORIG 0x0810aea2）
    virtual void deliveryEnd(CDelivery* delivery);    // vtable+0x2c（ORIG 0x0810aea8）
    virtual void delivery(DeliveryMsg& msg);    // vtable+0x30（ORIG 0x0810aeae）
    virtual bool IsEventing(CUser* user) const; // vtable+0x34（ORIG 0x080c84fa）
    void SetEventFlag(bool flag);               // 非虚 0x080c84e2

protected:
    // ORIG 布局：+0x00 vptr，+0x04 stADDINFO，+0x08 eventing flag 字节。
    stADDINFO m_addInfo;    // +0x04
    bool m_isEventing;      // +0x08
};

// CDelivery（ORIG sizeof 0x18）：int → CEventBase* 列表 的分发表。
class CDelivery
{
public:
    CDelivery();                                 // 0x08116386
    ~CDelivery();                                // 0x0811639a
    bool checkAddress(int type);                 // 0x081163dc
    void attach(int type, CEventBase* event);    // 0x081163fa
    void detach(int type, CEventBase* event);    // 0x081165fa
    void notify(int type, DeliveryMsg& msg);     // 0x08116682

private:
    std::map<int, std::list<CEventBase*> > m_eventMap;   // +0x00
};

class CEventManager
{
public:
    CEventManager();                       // 0x08114ce4 T（CEventManager.cpp）
    ~CEventManager();                      // 0x08114d4e T（CEventManager.cpp）
    CEventBase* GetRepeatEvent(int type);  // 0x08115998
    void changeCharacName(CUser* user, int key, char* name, int flag);
                                           // ORIG T _ZN13CEventManager16changeCharacNameEP5CUseriPci
                                           // @0x08116276（定义由后续批次补产）
    // ORIG T 0x08116212：存在重叠经验加成事件（set_clear_reward_exp_parameter 消费）。
    bool IsExistOverlappedExpBonusEvent();
    int AddEvent(int type, CEventBase* event);            // 0x08115982
    void dailyresetData();                                 // 0x0811630e
    bool LoadEventFromDB();                                // 0x081159b6
    bool TriggerEventEnd(int idx);                         // 0x08115d60（ORIG 调用点 test %al/je 直折；P9/P22）
    int InitEventManager();                                // 0x08114dcc（ORIG 尾 mov $0/%1,%ebx
                                                           // + mov %ebx,%eax 无归一化 ⇒ int）
    void MakeNotiEventInfo(PacketGuard& packet, CUser* user); // 0x08115de2
    bool TriggerEventStart(int idx, Word_Param param);     // 0x08115cc6（ORIG 调用点 test %al/je 直折；P9/P22）
    void BroadcastEventInfo();                             // 0x08116108
    void eventDeliveryNotify(int type, DeliveryMsg& msg);  // 0x08116874

    // ORIG 布局（CEventManager.md ctor 0x08114ce4 实证）：+0x00..+0x297 事件
    // 指针数组（0xa6 = 0x298/4 × CEventBase*，ctor memset(0x298)），+0x298
    // CDelivery*（heap 分配，ctor 内 new）。sizeof = 0x29c。
    CEventBase* m_events[0xa6];   // +0x00..+0x297
    CDelivery*  m_delivery;       // +0x298
};
static_assert(sizeof(CEventManager) == 0x29c,
              "CEventManager ABI drift");

// WongWork::CGMAccounts：§4.4 合并——本头原重复定义已删除（与
// Statics/GMAccounts.h:707 冲突，阻塞同时包含两头的 TU，如 CSecretShop.cpp）。
// 唯一声明点改为 Statics/GMAccounts.h（接口/字段/mangled 均不变：
// sizeof=8、m_gmList@+0x00）。
#include "../Statics/GMAccounts.h"

class Gm_List
{
public:
    Gm_List();                            // 0x08299912 T（独立 TU）
    ~Gm_List();                           // 0x8299948（编译器生成形态）
    // ORIG 0x0829996c 本体：三处失败路径 `mov $0x0,%ebx; jmp`、成功尾
    // `movzbl -0x11(%ebp),%ebx` + `mov %ebx,%eax; ret` ⇒ 返回 ok(bool)。
    // 返回类型不入 mangled（_ZN7Gm_List16LoadGmListFromDbEv 不变），ABI 零变。
    bool LoadGmListFromDb();              // 0x0829996c T
    void SendGMList_To_UpperServer(int param);  // 0x08299d54
private:
    MySQL* m_db;   // +0x00（ORIG 存 DBMgr::GetDBHandle(6,0) 句柄）
};


class CGuildServerProxy
{
public:
    CGuildServerProxy();                                        // 0x082a6a96 W
    CGuildServerProxy(std::string ip, int udpPort, int tcpPort);// 0x0846d818 T
    ~CGuildServerProxy();                                       // 0x082b31be W
    // ORIG 0x082a6b3a W：尾 `call Init(); leave; ret`（eax = 无参 Init 的 bool）
    // ⇒ 返回类型 bool（返回类型不入 mangled；createServerProxy 以该返回值作返回）。
    bool Init(std::string ip, int udpPort, int tcpPort);        // 0x082a6b3a W

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
    bool OnTcpRecv();                   // 0x0846e53c（SIGFIX: ORIG 串 "bool CGuildServerProxy::OnTcpRecv()"）
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

inline bool CGuildServerProxy::Init(std::string ip, int udpPort, int tcpPort)
{
    m_ip = ip;
    m_udpPort = udpPort;
    m_tcpPort = tcpPort;
    return Init();
}

#endif  // GAME_CGUILDSERVERPROXY_H_

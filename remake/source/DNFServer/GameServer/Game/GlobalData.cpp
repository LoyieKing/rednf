#include "LogManager.h"
// df_game_r GlobalData（D1 组）还原。
// 函数形态/字符串/池尺寸均取自 ORIG 反汇编与 Ghidra C。
// 外部类（后续批次）仅在本 TU 以最小 stub 声明，符号留给 ORIG/后续 TU。
//
// ============================================================================
// [b28d-solo-GlobalData 泳道判词] 清单 /tmp/b28d_tu/GlobalData.tsv（44 件）。
// 开工基线 check_tu_routed：identical=832 ae=1637 near=9 diff=54；
// 收口：identical=832 ae=1639 near=8 diff=53（AE +2，无劣化）。
// 修复转 AE 两件（证据）：
//   ① CGM_Manager::LoadGmListFromDb 0x08299530 —— ORIG 三处 cMyTrace 各占独立
//      槽（-0x44/-0x34/-0x24，4.1 不复用），具名对象在 4.4 下复用同槽致全帧浅
//      0x30 ⇒ 改 P221 全表达式临时链 `cMyTrace(a,b,c)(fmt)` 后 AE。
//   ② ItemVendingMachine::_CheckEnableAvatarCondition 0x08176794 —— 首调
//      GetRelatedItem 无槽往返（P0 内联）、去 vec 具名指针（P183）、
//      `cmp -0x1c(%ebp),%al; jg` 8 位有符号比较（T1 直证 char）后 87=87 AE。
// 改善未闭合一件：
//   （原 PunishClient 残差已闭：[b28d-PoolGlobalData] R2 落地 CBusinessImpl.h
//     PunishClient 返回类型 void→int + `return 0`，0x080de3ee ndiff 19→0 AE，
//     TU diff 53→52。）
//   CheckInventoryEmptyCount 0x0827108c 103→96（P63 共享出口改嵌套正向 if；
//     残差=ORIG `xor $1;test;jne RET1` vs 4.4 恒 `je NEXT`，极性恒折 ⇒ N）。
// 其余件按 N 类封顶（逐函数头有证据判词）：N18b 比较方向（CServerProxyMgr D1 ×3，
// 模板体在 CServerProxyMgr.h，R1 禁改 .h）、N7 寄存器角色轮换、N82/N95 槽位分配、
// N80 cMyTrace varargs 求值序、N4/P30 add-vs-mem、N-EH 占位定义可见性、
// N-数据缺失（CHumanCertifier 人证表内容不可还原）。
// ============================================================================
// [b28d-PoolGlobalData 二压 2026-09-09] 三数 基线 892/1780/8/53 →
// 收口 identical=893 ae=1781 near=8 diff=52（+1/+1/0/−1，missing_in_game
// 849→847，无劣化）。战果（逐件证据见函数头）：
//   ① CBusinessImpl::PunishClient 0x080de3ee ndiff 19→0（R2：
//      CBusinessImpl.h:34/48 void→int + return 0；ORIG 0x080de4b8/4c8/4d0
//      单出口返 0 实证，全仓无消费点）。
//   ② RentalInfo 伪 user 析构删除（InstanceRentalSystem.h，R2 已批）：
//      _Destroy_aux<ILb1E>@0823e5c7 实证 trivial ⇒ ProcessInsertItem
//      0x08271666 ndiff 133→3、ProcessEquipItem 0x082714c5 139→3（EXTRA
//      D1Ev×2+_Unwind_Resume×1 归零；D1/D2 W size 5 与 ORIG 同寸保留）；
//      同批语义修正 ri.m_field8/9/B=inElem[0]/[1]/[3]（旧 reward>>8/24 误推）。
//   ③ Dimension::GetRentItemInfo 0x0826f842 ndiff 96→2（sz 具名落槽+下标
//      重读+goto「ret 标签居中」形；残 2 = 4.4 块启发 `jmp` 中继，探针
//      pol_probe4/5 四形穷举后 N7 封顶）；else 支实参=常量 2、无 list=0 死
//      初始化（0x0826f8f2/0x0826f8ba 直证）。
//   P326（N93 双迭代器翻案）自查：proc_addFatigue 匿名 0 临时形实测反劣
//   （4→8 条）已回退，N93/N98 判不变；本池其余件无「双具名 const_iterator
//   比较」残差形（CBlackIPMonitor 三件已 AE）。
//   小件复核维持 N：D1×3（N18b 实测交换无效）、CRandomOptionItemHandle C1
//   （纯寄存器角色）、2×log（帧 0x4c vs 0x3c 无源形+esi/ebx/edi 轮换）、
//   CheckInventoryEmptyCount（xor$1;jne 极性，probe f1–f9 复核 4.4 无
//   「无往返+直分支」形）、CSimpleSSO::init（ndiff 已 3，N18a add 折叠）、
//   CheckupAllRentalItems ×2（各 3 条，N18a regmove 融合）、CaptchaGen D1
//   （N-EH，两泳道四形穷尽）、InitTcp（N80/N82）；CHumanCertifier::init
//   N-数据缺失；Init/Fini/dispatch 族与 IPG/ vending 超大件维持原判词。
// ============================================================================

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
#include "OS_API.h"                    // OS_API::snprintf（ORIG _ZN6OS_API8snprintfEPcjPKcz）

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
#include "PacketHeader.h"         // 权威 10 字节线格式头（UdpThread::_processUDPData packetId 视图）
#include "RandomOptionScript.h"   // RandomOptionScript::m_providerNormal148(+0x148) 权威成员（CRandomOptionItemHandle ctor）
#include "XorRand4.h"             // XorRand4 权威声明（定义 XorRand.cpp；本 TU 原重复 inline 副本已删）
#include "RandGen.h"              // RandGen::getFloat(XorRand4*,float,float)（ORIG 0x08ae02a4）

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
    bool SendChannelInfo();   // _ZN15CmmChannelProxy15SendChannelInfoEv 0x0810619e
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
    int m_sock;             // +0x18 桥接 TCP fd（ORIG ConnectBridgeServer
                            // 0x08106287 `mov %eax,0x18(%edx)` 直写实证）
    int m_field1c;          // +0x1c
    char m_buf[0x80000];    // +0x20
};

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
// send_check_channel_script 的函数作用域静态（ORIG b 符号 @0x094051c0/0x094055c0）
static char s_version[0x400];
static int s_linePos;

// 布局：char category@+0、char packetID@+1、unsigned int size@+2（setSize/
// getSize 均 dword 访问实证；setSize 0x08106bdb arg 先载 ⇒ ORIG 字段与 int
// 形参存在 signedness 差 ⇒ unsigned int，技巧5 反推）、负载自 +0x0b；
// packed 保证 +2 落位。
class tagPacketHeader
{
public:
    tagPacketHeader() {}
    void setCategory(int c) { m_category = (char)c; }   // 0x08106be6：mov %dl,(%eax)
    void setPacketID(int id) { m_packetID = (char)id; } // 0x08106bbc：mov %dl,0x1(%eax)
    void setSize(int s) { m_size = s; }                 // 0x08106bd8：mov %edx,0x2(%eax)
    // ORIG mangled _ZN15tagPacketHeader7getSizeEv（非 const，0x08106bcc：
    // mov 0x2(%eax),%eax）；类外定义保证 O0 下调用点出真实 call，与 ORIG 同形。
    int getSize();
private:
    char m_category;   // +0x00
    char m_packetID;   // +0x01
public:
    unsigned int m_size;  // +0x02（ORIG 0x08106bdb：`mov 0xc(%ebp),%edx` 先于
                          // this 载入 ⇒ arg→member 有 int→unsigned 转换）
    char m_pad6[5];    // +0x06..+0x0a（头总长 0xb）
} __attribute__((packed));

// GI 移位（SURPLUS cand1，ORIG GI 地址序最小）：本函数原位于 5334 行区，纯移位
// 上送至本 TU 首个强实体 tagPacketHeader::getSize 之前，使 _GLOBAL__I_ 后缀对齐
// ORIG _GLOBAL__I__ZN13CBusinessImpl18SendBuffertoClientEiiijPhs。语义零变化。
int CBusinessImpl::SendBuffertoClient(int a, int b, int c, unsigned int d,
                                      unsigned char* data, short len)
{
    (void)b;
    (void)c;
    // ORIG 0x080de240：按 stream 长档位取池；<<0x252 <<-1；
    // put_binary({int a, uint d, short len}, 0xc)；put_binary(data, len)。
    const int size = (int)len + 0x14;
    CStreamGuard guard(true);
    // ORIG 0x080de26c..0x080de2e3 三分支形态：首支 `cmpl $0xfff,-0xc; jg else`
    // （= size <= 0xfff）；次支重测 `cmpl $0xfff; jle big` + `cmpl $0x3fff; jg big`
    // ⇒ 源码在 else-if 中显式重复 `size > 0xfff`，并非纯区间判断。
    if (size <= 0xfff)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire("BusinessImpl.cpp", 0x3a));
    }
    else if (size > 0xfff && size <= 0x3fff)
    {
        guard.Set(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x3e));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire("BusinessImpl.cpp", 0x42));
    }
    **guard << 0x252;
    **guard << -1;
    struct
    {
        int field00;
        unsigned int field04;
        unsigned short field08;   // ORIG 0x080de355 `mov %ax,-0x18(%ebp)`（2 字节存）
    } hdr;
    hdr.field00 = a;
    hdr.field04 = d;
    hdr.field08 = (unsigned short)(len);   /* movzwl %ax */
    guard->put_binary(&hdr, 0xc);
    guard->put_binary(data, (int)len);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // ORIG 0x080de3b9：`return 0` 先物化到 %ebx，再析构 guard，最后 mov %ebx,%eax。
    return 0;
}

int tagPacketHeader::getSize() { return m_size; }

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

// ORIG ctor 0x08106bfc（SendChannelInfo 栈上构造）：cat=1/id=2/size=0x33。
// 负载字段与宽度按 ORIG 0x0810619e 反汇编栈槽写序列（CmmChannelProxy/SendChannelInfo.md）：
// gc_no←mov %eax,-0x30(%ebp)（int）、group←-0x2c、channel_no←-0x28、max_user_num←-0x24、
// cur_user_num←-0x20、server_ip←strcpy(lea -0x3b; add $0x1f)、port←mov %eax,-0xc(%ebp)。
struct tagCS_UPDATE_CHANNEL_INFO
{
    tagPacketHeader m_hdr;      // +0x00（payload 起于 +0x0b）
    int m_gcNo;                 // +0x0b
    int m_group;                // +0x0f
    int m_channelNo;            // +0x13
    int m_maxUserNum;           // +0x17
    int m_curUserNum;           // +0x1b
    char m_serverIP[16];        // +0x1f
    int m_port;                 // +0x2f
    tagCS_UPDATE_CHANNEL_INFO()
    {
        m_hdr.setCategory(1);
        m_hdr.setPacketID(2);
        m_hdr.setSize(0x33);
    }
} __attribute__((packed));      // sizeof 0x33（= ORIG setSize 值）

// ORIG 0x810619e：构造栈上 tagCS_UPDATE_CHANNEL_INFO（header cat=1/id=2/size=0x33）
// 并经 m_udp.SendToServer 发往频道服务器。
bool CmmChannelProxy::SendChannelInfo()
{
    tagCS_UPDATE_CHANNEL_INFO pkt;
    // ORIG 0x081061b1：无 env 局部，每个字段各自 G_CEnvironment()（返回 %eax 直用）。
    pkt.m_gcNo = G_CEnvironment()->m_serverEnvir.m_gcNo;
    pkt.m_group = G_CEnvironment()->get_server_group();
    pkt.m_channelNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
    pkt.m_maxUserNum = G_CEnvironment()->m_serverEnvir.m_maxUserNum;
    pkt.m_curUserNum = static_cast<int>(
        G_GameWorld()->m_UserInWorld.size());         // m_UserInWorld
    std::strcpy(pkt.m_serverIP,
                G_CEnvironment()->m_serverEnvir.m_serverIP);
    pkt.m_port = G_CEnvironment()->m_serverEnvir.m_tcpPort;

    m_udp.SendToServer(reinterpret_cast<char*>(&pkt), pkt.m_hdr.getSize(),
                       static_cast<unsigned short>(m_port), m_ip.c_str());
    return true;
}

// ORIG ConnectBridgeServer 0x08106278：socket/connect 桥接服务器并设非阻塞。
// [b28d-18 IDENTICAL_AE] 修：addrlen 改 int + (socklen_t) cast 翻实参装载序（P248 档③新实证，addrlen→%ecx 先载）；NDIFF 3→1（余尾 nop） | ORIG 0x08106278
bool CmmChannelProxy::ConnectBridgeServer()
{
    sockaddr_in addr;
    std::memset(&addr, 0, 0x10);
    // ORIG 0x08106287：socket 返回值直写成员 m_sock（this+0x18），后续每处成员直读。
    m_sock = socket(AF_INET, SOCK_STREAM, 0);          // socket(2,1,0)
    if (m_sock < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't create socket");
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_ip.c_str());    // inet_addr(c_str(this))
    addr.sin_port = htons((unsigned short)G_CEnvironment()->get_tcp_port_channel());
    // ORIG 0x08106336 区装载序 = addrlen→%ecx、&addr→%edx、m_sock→%eax
    // （右起第三实参最先落寄存器）。4.4 恒把 %ecx 装载推迟到 m_sock 之后
    // ⇒ 唯一能翻转装载序的手段是把 addrlen 做成 volatile 局部（探针 f2 证），
    // 但 4.4 的 volatile 特判把它塞进更远槽、并连带把 addr/nonblock 等**全部**
    // 后续槽号推偏：实测 volatile + volatile 垫块回推的写法让本函数
    // ndiff 2 → 20（非统一平移，无法补偿）⇒ 两难坐实，记 N82 族封顶。
    int addrlen = 16;
    if (connect(m_sock, reinterpret_cast<sockaddr*>(&addr), (socklen_t)addrlen) < 0)
    {
        std::puts("[CmmChannelProxy::SendCheckChannelScript] Can't connect channel bridge server");
        return false;
    }
    int nonblock = 1;
    ioctl(m_sock, FIONBIO, &nonblock);                 // 0x5421
    return true;
}

// ORIG SendPacket 0x0810637c：write 发送；EAGAIN/EINTR/0 → 0（可重试），
// 其它错误 cMyTrace 后返回 -1。
int CmmChannelProxy::SendPacket(char* buf, int len)
{
    // ORIG 0x0810638b：fd 每处成员直读（this+0x18），无局部 spill。
    // ORIG 0x081063a4：赋值即条件 + (bool) 物化 → cmp/setle/test/je。
    int n;
    if ((bool)((n = write(m_sock, buf, len)) <= 0))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK ||
            errno == 0)
        {
            return 0;
        }
        // ORIG 0x081063e2：临时对象表达式 —— 实参先评估（strerror/errno/m_sock
        // 落 %ebx/%edi/%esi），随后 ctor，再 operator()。
        cMyTrace(__PRETTY_FUNCTION__, 200, 0)(
            "SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    return n;
}

// ORIG RecvPacket 0x0810645c：select(300s) → SO_ERROR 探测（结果忽略）→
// Sleep(100ms) → read 追加到 m_buf 尾部并累加 m_field1c。
// ORIG 0x0810645c 全量转写：
//   ret(-0x2c)=0；tv(0x12c,0)@-0x54；额外 dead init(-0x28)=0（0x0810647d）；
//   fd_set@-0xdc 经指针槽(-0x1c)+计数槽(-0x20)手循环清 32 词
//   （0x081064a7 `cmpl $0x1f; setbe` i<=31 形）；FD_SET 宏产无符号
//   `shr $5`+`and $0x1f`+`shl` 位图形（0x081064b8）；select 结果 `jg body`
//   岛（0x0810651c，源码 `if (ret > 0) {...} return 0;`）；getsockopt 局部
//   error@-0x58/len@-0x5c；read 结果槽 -0x24；负判定 `shr $0x1f; test`
//   （0x081065ab，等价 `(unsigned)nread >> 31`）；errno 链 ==EAGAIN||==EINTR||
//   ==EAGAIN||==0（0x081065bb-5e3 四次独立取址，11 出现两次为原码忠实转写）。
// [b28d-18 N82] 残差=帧槽/寄存器（N82） | ORIG 0x0810645c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/寄存器 + N89 STL 展开。
int CmmChannelProxy::RecvPacket()
{
    // ORIG 0x0810651c `cmpl $0; jg body` + fall-through `mov $0;jmp end`——
    // early-return `if (ret <= 0) return 0;` 极性（技巧 17）。
    // ORIG 0x0810654a getsockopt 第三实参硬编码 $0x8（老 SO_ERROR 值，非本
    // 头文件宏的 4）；level 硬编码 $0x1（0x08106552）。
    // ORIG 0x081065ab 负判定 `shr $0x1f; test al`（0x081064d4 select 后同理
    // 无符号语义）——用无符号比较 `(unsigned)nread > 0x7fffffff` 复现。
    // 槽序对齐（4.4 顶层=先声明较近、block 变量优先占近槽）：目标
    // p -0x1c, i -0x20, nread -0x24, x -0x28, ret -0x2c, tv -0x50/54,
    // error -0x58, len -0x5c, fd -0xdc（ORIG 0x08106468-0x08106494）。
    // 槽位靶：ret -0x2c（0x08106468）、dead -0x28（0x0810647d，tv 写之后
    // 落）、nread -0x24、i -0x20、p -0x1c、tv -0x54/50、error -0x58、
    // len -0x5c、fd -0xdc。声明序控远槽，语句序控 init 位置。
    struct timeval tv;         // 先声明 → 远区 -0x54
    fd_set readfds;            // -0xdc 大块
    int ret = 0;               // 0x08106468 → -0x2c（先声明占更远槽）
    int nUnused;               // dead init@0x0810647d → -0x28
    tv.tv_sec = 300;
    tv.tv_usec = 0;
    nUnused = 0;
    int nread;
    {
        unsigned int i;         // ORIG i(-0x20) 比 pBits(-0x1c) 远 → 先声明
        unsigned long* pBits = reinterpret_cast<unsigned long*>(&readfds);
        for (i = 0; i <= 31u; ++i)
        {
            // （寻址模式残余差：ORIG 0x0810648f `mov -0x20,%edx; mov -0x1c,%eax;
            // movl $0,(%eax,%edx,4)` scaled 直存；4.4 O0 恒折 shl+add 指针算术，
            // register asm 钉基址/索引均无法回退 ⇒ 记 N 类。）
            pBits[i] = 0UL;
        }
    }
    // ORIG 0x081064b8：FD_SET 无符号 `shr $5`/`and $0x1f` 位图形，不留 us 槽。
    // ORIG 0x081064b8：`mov 0x18,%eax; shr $0x5,%eax; mov %eax,%edx;
    // mov -0xdc(%ebp,%eax,4),%ebx … mov %eax,-0xdc(%ebp,%edx,4)` —— 位索引
    // 单份计算（%eax）后复制到 %edx 供尾 store。4.4 默认重算两份 ⇒
    unsigned long ix =
        static_cast<unsigned long>(m_sock) >> 5;  // b28 P0 清绑定（N96）
    // [b28 判词 N96+N18a/P179] ORIG 单次 scratch（shr 结果 %eax 直用+mov %eax,%edx
    // 复制、无槽往返）需绑定形；绑定禁用后具名局部=槽往返、内联=双份重算
    // （探针 /tmp/l12p8.cpp f/g 两形均不合），N 封顶。
    readfds.fds_bits[ix] |= (1UL << (static_cast<unsigned long>(m_sock) & 31));
    ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret <= 0)
    {
        return 0;
    }
    int error = 0;
    socklen_t len = 4;
    // （取参序残余差：ORIG 0x0810653d &error→%edx 先载、m_sock→%eax、
    // &len→%ecx；4.4 恒 &len 先载且 m_sock 占 %eax ⇒ register asm("ecx")
    // 反而打乱 store 序，probe 证不可翻 ⇒ 记 N 类。）
    getsockopt(m_sock, 1, 8, &error, &len);
    OS_API::Sleep(100);
    nread = static_cast<int>(::read(m_sock, m_buf + m_field1c,
                                    static_cast<size_t>(0x80000 - m_field1c)));
    // ORIG 0x081065a8：`mov -0x24,%eax; shr $0x1f,%eax; test %al,%al; je`——
    // 显式移位形（`>0x7fffffffu` / 裸 `>>31` 均被 4.4 fold 成 cmpl/jns），
    // b28 P0 清绑后恢复：具名局部反引入槽往返，表达式内联 uchar 化实测
    // 与 ORIG 同形（探针 /tmp/l12p7.cpp f：`mov; shr $0x1f,%eax; test %al; je`）。
    if (static_cast<unsigned char>(static_cast<unsigned int>(nread) >> 31))
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
        {
            return 0;
        }
        cMyTrace(__FUNCTION__, 0x114, 0)(
            "RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
            m_sock, errno, strerror(errno));
        return -1;
    }
    if (nread == 0)
    {
        cMyTrace(__FUNCTION__, 0x11a, 0)(
            "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), 0x80000, nread);
        return -1;
    }
    m_field1c += nread;
    return nread;
}

// ORIG SendCheckChannelScript 0x081066ea（0x453 字节）：桥接后校验/拉取
// channel_info.etc 并落盘。stat 失败且非 ENOENT → 0；version 文件读写两次；
// CHECK_SCRIPT_VERSION(cat1/id5) 应答尾包写入 version；GET_SCRIPT(cat2/id9)
// 应答尾包为脚本体（fwrite size-0xb 字节）。
// [b28d-18 N82] 残差=槽位/寄存器（N82） | ORIG 0x081066ea
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
    // ORIG（0x08106870 段）：行缓冲为函数作用域 static（bss -0x4d4），循环带
    // 行首空白剥离 + '\n'/'#' 注释行跳行（linePos 跨行累计【实证：不随行重置】）。
    // [b28d-solo-GlobalData] p167 EXTRA fgets×1：ORIG 仅 1 次 fgets（0x08106888
    // 尾测 `test;setne;test;jne <body>` 旋转形）⇒ 单条件 while，删 init 弃用读。
    while (std::fgets(s_version, 0x400, fp) != 0)
    {
        char* src = s_version;
        char* dst = src;
        do
        {
            while (*src == ' ' || *src == '\t')
            {
                ++src;
            }
            *dst++ = *src++;
        } while (*(src - 1) != '\0');
        if (s_version[s_linePos] == '\n')
        {
            s_linePos = 0;
            continue;
        }
        if (s_version[s_linePos] != '#')
            break;
        s_linePos = 0;
    }
    std::fclose(fp);

    tagCS_CHECK_SCRIPT_VERSION reqVer;
    std::strncpy(reqVer.m_version, s_version, 0x10);
    if (SendPacket(reinterpret_cast<char*>(&reqVer),
                   reqVer.m_hdr.getSize()) == 0)
    {
        return false;
    }
    if (RecvPacket() == 0)
    {
        return false;
    }
    // ORIG 08106922：m_buf 保存为局部头指针槽（mov this; add $0x20; mov -0x1c(%ebp)），
    // 循环条件经 tagPacketHeader::getSize（call 08106bcc）读 +0x02 dword size。
    tagPacketHeader* pVerResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pVerResp->getSize())
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
    std::fputs(reinterpret_cast<char*>(pVerResp) + 0xf, fp); // ORIG lea 0xf(%eax) on 同一局部槽：头 0xb + first 4
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
    // ORIG 08106a7c：第二段独立局部槽 -0xc(%ebp)，同 getSize 形态。
    tagPacketHeader* pScriptResp = reinterpret_cast<tagPacketHeader*>(m_buf);
    while (m_field1c != pScriptResp->getSize())
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
    int total = pScriptResp->getSize();
    std::fwrite(reinterpret_cast<char*>(pScriptResp) + 0xb, 1, total - 0xb, fp);
    std::fclose(fp);
    close(m_sock);
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


#include "CLevelDungeonPlayStatistic.h"
#include "CDungeonExpStatistic.h"   // +0x18 子对象权威定义（map<int, stDungeonExp>）

class CStatisticMgr : public CLevelDungeonPlayStatistic
{
public:
    CStatisticMgr();
private:
    // ORIG ctor 0x82aa89e：CLevelDungeonPlayStatistic@+0（0x18，map<int,
    // stDungeonPlayInfo>）+ CDungeonExpStatistic@+0x18（0x18，map<int,
    // stDungeonExp>）。基类即 CLevelDungeonPlayStatistic（首基类同址）。
    CDungeonExpStatistic m_dungeonExp;      // +0x18（权威定义见 CDungeonExpStatistic.h）
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
    // [b28d-solo-GlobalData] Impl 必须**嵌套**（P76/P104）：ORIG nm 为
    // _ZN22RentalMachineDimension26RentalMachineDimensionImpl*（嵌套 mangling），
    // 全局平铺 struct 使 auto_ptr<Impl> 的 operator->/C1/D1 符号不匹配
    // （p167 MISSING ptEv×多件）。与 PcRoom 同构。
    struct RentalMachineDimensionImpl
    {
        RentalMachineDimensionImpl() { memset(m_data, 0, sizeof(m_data)); }
        ~RentalMachineDimensionImpl() {}   // ORIG D1 @0x0826fb0c：仅析构 set
        char m_data[0x104];            // +0x00
        std::set<unsigned int> m_set;  // +0x104
    };  // 0x11c
    bool checkAllRentalItemScript();   // ORIG T 0x0826f022（本批实现）
    bool checkSelectionListScript(
        const std::vector<std::vector<EventRewardItemInfo> >& list);         // T 0x0826f14e
    // 返回 bool：ORIG 调用处 0x0826f591 `xor $0x1,%eax; test %al,%al; je`
    // （P73/探针 v12：仅 bool 实参的 `!` 会折成 xor，int/char 为 test+sete）；
    // 本体返回点均为 `mov $0x0/$0x1,%eax`（0x0826f870/…）与 bool 一致。
    bool GetRentItemInfo(CUser& user,
                        const InstanceRentalSystem::RentInElem& elem,
                        EventRewardItemInfo& out);                           // T 0x0826f842
    int GetRewardItemGiveList(int job, int growType);                        // T 0x0826f814（透传）
    int CheckupAllRentalItems(CUser& user,
                              const InstanceRentalSystem::RentInput& input); // T 0x0826f52c
    bool checkRentalLevel(int level);                                        // T 0x0826f7e4
    bool checkRentalGrowJob(int growType);                                   // T 0x0826f7fc
    void log(CUser& user, int itemId);                                       // T 0x0826ef16
    void initLoggingEnvironment();                                           // ORIG T 0x826efdc
    // ORIG 0x0826eeb7：成员访问经 auto_ptr::operator-> 实调用（非内联展开）。
    std::auto_ptr<RentalMachineDimensionImpl> m_impl;  // +0x04（ORIG auto_ptr）
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
bool IsLightServer()   // 同步 DataManagerInitDepends.h:42/62 bool 化（ORIG 0x822ad44 bool 返回形；调用点 test %al 无 setne，giveup_panalty 0x086786cb 实证；经 GDB hub 授权代落）
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
// ---- XorRand4：权威声明 XorRand4.h、定义 XorRand.cpp（ORIG 符号均为 T 强
// 符号 ⇒ 非 inline；本 TU 原重复 inline 副本已删，§4.4 单一声明点）。

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
    // [b23S3C] 非 const：ORIG(4.1) -O0 对 rodata 常量仍出运行时 mov 读取再 div；
    // 4.4 见 const 初始化器即折成魔数乘（mul/imul 形）。探针 /tmp/r3probe/p4 实证非 const 复原 `mov addr;div`。
    // ORIG 0x080de87x：divisor 装载 `mov <A>,%edx; mov %edx,%ecx`（int 读入
    // edx 再转 unsigned 到 ecx）⇒ 声明 int + 使用处显式转 unsigned（P211 档③）。
    static int kKoreanGeneralCount;     // VA 0x08b2c444（978）
    static int kTaiwanAlphabetCount;    // VA 0x08b2c534（26）
    static int kJapaneseHirakanaCount;  // VA 0x08b2c4f0（72）
private:
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
int CaptchaGen::kKoreanGeneralCount = 978;    // VA 0x08b2c444
int CaptchaGen::kTaiwanAlphabetCount = 26;    // VA 0x08b2c534
int CaptchaGen::kJapaneseHirakanaCount = 72;  // VA 0x08b2c4f0

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

// FreeType API（ORIG 0x087bcac0，FT_Library = void* 句柄占位，仓内无 ftdriver
// 头；extern "C" 保 mangled 与 ORIG 符号一致）。本环境无 libfreetype：给占位
// 定义保链接（CaptchaGen ctor 的 call 站点 codegen 仍与 ORIG 一致；函数体本身
// 属静态链入的第三方库，不参与 identical 目标，同 crypto 豁免）。
extern "C" int FT_Init_FreeType(void** alibrary)
{
    if (alibrary)
    {
        *alibrary = 0;
    }
    return 0;
}

// FreeType 收尾 API（ORIG 0x087c14b0 / 0x087bca50，占位定义豁免同上；
// CaptchaGen dtor 0x080de706 调用点 codegen 与 ORIG 一致）。
extern "C" int FT_Done_Face(void* face)
{
    (void)face;
    return 0;
}

// FreeType 库内部收尾函数（ORIG 0x087c3c00 FT_Done_Library / 0x087dc6b0
// FT_Done_Memory / 0x087c43a0 FT_Open_Face）。均真实存在于 ORIG，属静态链入
// 第三方库本体，函数体不参与 identical 目标（同 crypto 豁免）；wrapper
// FT_Done_FreeType / FT_New_Face 的调用站点 codegen 需与 ORIG 一致，故此处给
// 占位定义保链接。FT_Library 首字段为 memory（offset 0），见 idx49 报告汇编。
extern "C" int FT_Done_Library(void* library)
{
    (void)library;
    return 0;
}

extern "C" int FT_Done_Memory(void* memory)
{
    (void)memory;
    return 0;
}

// FT_Open_Face 用参结构（FreeType FT_Open_Args_，pathname@0xc、stream@0x10）
struct FT_Open_Args
{
    unsigned int flags;   // +0x00
    const void*  memory_base;  // +0x04
    long         memory_size;  // +0x08
    const char*  pathname;     // +0x0c
    void*        stream;       // +0x10
    void*        module;       // +0x14
};

extern "C" int FT_Open_Face(void* library, FT_Open_Args* args, long face_index,
                            void** aface)
{
    (void)library;
    (void)args;
    (void)face_index;
    if (aface)
    {
        *aface = 0;
    }
    return 0;
}

// [b22判词·N-codegen] FT_Done_FreeType 语义忠实 ORIG 0x087bca50：library!=0 时读
// 其首字段 memory（offset 0），依次 FT_Done_Library(library)、FT_Done_Memory(memory)，
// 恒返 0（orig_insns=our_insns=17）。残余 DIFF 全属跨版本 codegen 结构差、无法源码
// 复现：①ORIG 用 callee-saved %ebx 跨两次 call 持有 memory（push/pop %ebx），
// 4.4.7 -O0 为具名局部 memory 建 -0x4(%ebp) 槽；②ORIG `test %eax,%eax`（载入指针
// 后测寄存器）vs 本 `cmpl $0x0,0x8(%ebp)`（测形参槽）；③ORIG epilogue
// `add $0x14,%esp; xor %eax,%eax; pop %ebx; pop %ebp; ret`（有 %ebx 压栈）vs 本
// `mov $0,%eax; leave; ret`；④`call <FT_Done_Library/FT_Done_Memory>` 未 relink 时
// 为 .o 重定位占位（§6.3 假阳性），relink 后转 AE。语义等价。
extern "C" int FT_Done_FreeType(void* library)
{
    // ORIG 0x087bca50（idx49 报告）：library != 0 时读取其首字段 memory，
    // 依次调用 FT_Done_Library(library) 与 FT_Done_Memory(memory)；恒返 0。
    if (library != 0)
    {
        void* memory = *(void**)library;   // library->memory @ offset 0
        FT_Done_Library(library);
        FT_Done_Memory(memory);
    }
    return 0;
}

// addFont 用 FreeType API（ORIG 0x087c4730/0x087c2f20/0x087bdd80）占位定义，
// 豁免同 FT_Init_FreeType（本环境无 libfreetype；调用点 codegen 与 ORIG 一致，
// 函数体属静态链入第三方库不参与 identical）。FT_ENCODING_UNICODE 以字面量
// 0x756e6963（"cinu"）代入。
// [b22判词·N-codegen] FT_New_Face 语义忠实 ORIG 0x087c4730（pathname==0 返 6；否则
// 构造 FT_Open_Args{flags=4,pathname,stream=0} 调 FT_Open_Face 并透传返回值）。
// 残余 DIFF 全属跨版本 codegen 结构差、无法源码复现：①ORIG 把返回值 FVar1 留在
// %eax 跨分支（寄存器化标量局部，违 P0），4.4.7 -O0 一律为具名局部建 result 槽；
// ②args 结构帧槽号（ORIG -0x28/-0x1c/-0x18 vs 本 -0x18/-0xc/-0x8）与帧尺寸
// （0x38 vs 0x30）随局部槽化次序错位；③ORIG `mov 0xc(%ebp),%edx;test %edx,%edx`
// vs 本 `cmpl $0x0,0xc(%ebp)`（test-reg vs cmp-slot）；④`call <FT_Open_Face>` 未
// relink 时为 .o 重定位占位（§6.3 假阳性），relink 后转 AE。语义等价。
extern "C" int FT_New_Face(void* library, const char* filepathname, long face_index,
                           void** aface)
{
    // ORIG 0x087c4730（idx73 报告）：pathname==0 返 6；否则构造
    // FT_Open_Args{flags=4, pathname, stream=0} 调 FT_Open_Face 并以其返回值返回。
    // ORIG 把返回值（FVar1）留在 %eax 跨分支（编译器寄存器化标量局部，违 P0 但
    // ORIG 事实）；本环境 4.4.7 -O0 一律为具名局部分配槽（见 diff：result 槽化 +
    // 帧尺寸/槽号/test-reg-vs-cmpl-slot 差），属跨版本 codegen 结构差，无法源码复现。
    if (filepathname == 0)
    {
        return 6;
    }
    FT_Open_Args args;
    args.flags = 4;
    args.pathname = filepathname;
    args.stream = 0;
    return FT_Open_Face(library, &args, face_index, aface);
}

extern "C" int FT_Set_Pixel_Sizes(void* face, unsigned int pixel_width,
                                  unsigned int pixel_height)
{
    (void)face;
    (void)pixel_width;
    (void)pixel_height;
    return 0;
}

extern "C" int FT_Select_Charmap(void* face, unsigned long encoding)
{
    (void)face;
    (void)encoding;
    return 0;
}

// ---- generateImage 用 FreeType 结构视图与 API 占位 ----
// [b28d-solo-GlobalData] 偏移实证：face+0x54=glyph（ORIG 0x080deff3
// `mov 0x54(%eax),%eax`）、glyph+0x4c=bitmap（0x080df034 `add $0x4c,%eax`）、
// FT_Bitmap{rows@0,width@4,pitch@8,buffer@0xc}（0x080df118/0x080df086/
// 0x080df0b2/0x080df0ac）。仅建模用到的字段；函数体属静态链入第三方库，
// 不参与 identical 目标（同既有 FT_* 占位豁免），符号名取自 ORIG nm。
typedef long FT_Fixed;
struct FT_Vector
{
    FT_Fixed x;   // +0x00
    FT_Fixed y;   // +0x04
};
struct FT_Matrix
{
    FT_Fixed xx;  // +0x00
    FT_Fixed xy;  // +0x04
    FT_Fixed yx;  // +0x08
    FT_Fixed yy;  // +0x0c
};
struct FT_Bitmap
{
    unsigned int rows;            // +0x00
    unsigned int width;           // +0x04
    int pitch;                    // +0x08
    unsigned char* buffer;        // +0x0c
    unsigned short num_grays;     // +0x10
    unsigned char pixel_mode;     // +0x12
    unsigned char palette_mode;   // +0x13
    void* palette;                // +0x14
};
struct FT_GlyphSlotRec
{
    char m_pad00[0x4c];
    FT_Bitmap bitmap;             // +0x4c
};
struct FT_FaceRec
{
    char m_pad00[0x54];
    FT_GlyphSlotRec* glyph;       // +0x54
};

extern "C" int FT_Set_Transform(FT_FaceRec* face, FT_Matrix* matrix,
                                FT_Vector* delta)
{
    (void)face;
    (void)matrix;
    (void)delta;
    return 0;
}

extern "C" unsigned int FT_Get_Char_Index(FT_FaceRec* face, unsigned long charcode)
{
    (void)face;
    (void)charcode;
    return 0;
}

extern "C" int FT_Load_Glyph(FT_FaceRec* face, unsigned int glyph_index,
                             int load_flags)
{
    (void)face;
    (void)glyph_index;
    (void)load_flags;
    return 0;
}

extern "C" int FT_Render_Glyph(FT_GlyphSlotRec* slot, int render_mode)
{
    (void)slot;
    (void)render_mode;
    return 0;
}

CaptchaGen::CaptchaGen()
    : m_library(0)   // ORIG 0x080de697：m_library 清零先于 vector/XorRand 成员
                     // ctor —— mem-init（声明序）而非 body 赋值。
{
    // ORIG 0x080de6bf：FT_Init_FreeType(&m_library)（+0 ⇒ this 同值）结果落
    // 临时槽 -0xc(%ebp)（不检查，dead store 具名化）；随后 seed(now)。
    int err = FT_Init_FreeType(&m_library);
    (void)err;
    m_rng.seed(Tick::nowAsInternalValue());
    // ORIG 0x080de6de：seed call 后 jmp 越过 EH island 到尾部 epilogue——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}

// [b28d-18 N-EH] FreeType 占位定义同 TU 可见 ⇒ 4.4 判 nothrow 不布 landing（跨版本 EH 可见性，b28 已判） | ORIG 0x080de706
// [b28d-solo-GlobalData] DIFF 封顶：p167 MISSING vector D1+_Unwind_Resume = N-EH（4.1 对外部
// FreeType .so 不透明调用布 landing；本仓占位定义同 TU 可见 ⇒ 4.4 判 nothrow）。
CaptchaGen::~CaptchaGen()
{
    // ORIG 0x080de717：ix 实参先载 i 槽（int→unsigned 转换 ⇒ arg 先于 this，
    // 技巧5/15）；0x080de74b `setg` ⇒ 两侧 int 有符号比较；循环体 FT_Done_Face
    // （0x080de731）；循环后 FT_Done_FreeType(m_library)（0x080de75a）再落
    // vector 成员 dtor。
    // [b28 判词·N·EH 可见性] ORIG 49 vs 我们 33：ORIG 4.1 对**外部 FreeType .so**
    // 不透明调用布 landing（push esi/ebx + cleanup vector D1 + _Unwind_Resume 段，
    // 16 insn）；本仓 FreeType 以 extern "C" 占位定义**同 TU 内联可见**（1138 起），
    // 4.4 throw 分析判 nothrow → 无 landing。探针 /tmp/l2_p10.cpp 实证改 extern 声明
    // 即产 landing，但搬移 shim 定义会同时改 ctor/addFont（已 AE）调用点形态，
    // 风险>收益；`throw(...)` spec（-fno-enforce-eh-specs）与 try/catch{throw;}
    // 实测均无效（33 不变）。属 N 类「跨工具链第三方符号可见性」，语义等价豁免。
    for (int i = 0; i < (int)m_faces.size(); ++i)
    {
        FT_Done_Face(m_faces[i]);
    }
    FT_Done_FreeType(m_library);
}

// [b28d-18 IDENTICAL_AE] 48=48 逐条同形（.o call 重定位标签伪差） | ORIG 0x080de794
bool CaptchaGen::addFont(const char* font, int size)
{
    // ORIG 0x080de794（report 汇编+反编译 C 全读）：FT_New_Face(m_library, font, 0,
    // &face) → 失败返 false → FT_Set_Pixel_Sizes(face, 0, size) → 失败返 false →
    // FT_Select_Charmap(face, FT_ENCODING_UNICODE=0x756e6963) → 失败返 false →
    // m_faces.push_back(face)（vector<void*>）→ 返 true。
    // [b28 BM2 判词·AE] 清单 NEAR(2) 为全二进制全局地址标签伪差；fresh
    // /tmp/tu_GlobalData.o fn_diff_tu 归一 0 差（48=48 逐条）。
    // 原转写以 fopen/fclose/malloc/strcpy 假实现替代真 FT 序列（违 §4.1），已改为
    // 忠实 ORIG；face 错误码 slot 落 -0xc 与 `cmpl $0,slot; je 成功` 逐指令对应。
    void* face;       // ORIG local_18 @ -0x14（FT_New_Face 出参，未初始化）
    int err = FT_New_Face(m_library, font, 0, &face);   // ORIG local_10 @ -0xc
    if (err != 0)
    {
        return false;
    }
    err = FT_Set_Pixel_Sizes(face, 0, (unsigned int)size);
    if (err != 0)
    {
        return false;
    }
    err = FT_Select_Charmap(face, 0x756e6963ul);
    if (err != 0)
    {
        return false;
    }
    // [b28d-solo-GlobalData] P315/P234：ORIG 0x080de81a `mov -0x14,%eax; mov %eax,-0x10;
    // lea -0x10,%eax; mov %eax,0x4(%esp)` 调 `push_back(OS0_)`（void*&& 右值重载）；
    // 具名左值 toPush 选 RKS0_（p167 MISSING OS0_/EXTRA RKS0_）⇒ 写 `(void*)face`
    // 产生 prvalue 临时（-0x10 拷贝槽）绑定右值引用。
    m_faces.push_back((void*)face);
    return true;
}

// [b28d-solo-GlobalData] 归零 100=100：P211 档③ 类型旋钮——三个静态计数
// unsigned int→int + 使用处 (unsigned int) cast，复现 `mov <A>,%edx; mov %edx,%ecx`
// 两段 divisor 装载（旧 N18a 判词作废）；CaptchaGen.h 同步（R2 已广播） | ORIG 0x080de83c
void CaptchaGen::generateText(CountryCode code, int len, unsigned short* out)
{
    // [b23S3C] ORIG 0x080de83c：三分支各独立循环（i 槽 -0x14/-0x10/-0xc），
    // 表/计数直引全局静态成员（movzwl table(,%eax,1) 索引形），勿归一成共享循环。
    // [b28 判词·N18a×3] 各分支 `m_rng() % kXxxCount`：ORIG 4.1 先 `mov g,%edx`
    // 再 `mov %edx,%ecx`（divisor 两段物化），4.4 恒直 `mov g,%ecx` 单段（N18a
    // 「4.4 恒折单步 regmove」反向不可造）；三分支共差 3 insn（97 vs 100），
    // div/mod 输入输出逐条等。
    if (code == CountryCode::korea)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] = kKoreanGeneral[m_rng() % (unsigned int)kKoreanGeneralCount];
        }
    }
    else if (code == CountryCode::japan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kJapaneseHirakana[m_rng() % (unsigned int)kJapaneseHirakanaCount];
        }
    }
    else if (code == CountryCode::taiwan)
    {
        for (int i = 0; i < len; ++i)
        {
            out[i] =
                kTaiwanAlphabet[m_rng() % (unsigned int)kTaiwanAlphabetCount];
        }
    }
}

// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x080deade
// [b28d-solo-GlobalData] 语义缺口已闭合：p167 CLEAR（53/53，0 missing/0 extra）——补
// FT_Set_Transform/FT_Get_Char_Index/FT_Load_Glyph/FT_Render_Glyph extern "C" 占位 +
// FT_Matrix/FT_Vector/FT_Bitmap/FT_GlyphSlotRec/FT_FaceRec 视图，按 ORIG 0x080dee9x..
// 0x080df120 还原 16.16 定点矩阵与 bitmap 遍历（std::min<int>）；发明符号 static
// renderGlyph 已删。余 gap 3 = 槽号平移（N82/N95 族）。
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
        // [b28d-solo-GlobalData] 按 ORIG 0x080dee9x..0x080df120 段还原真实
        // FreeType 栅格化序列（旧 static renderGlyph 占位为发明符号，已删）：
        // m_faces[idx] -> 4×getFloat(旋角/缩放) -> 2×镜像位 -> FT_Matrix 16.16
        // 定点（65536.0f=_DAT_08b2cab0）-> FT_Set_Transform -> FT_Get_Char_Index
        // -> FT_Load_Glyph -> FT_Render_Glyph(face->glyph,0) -> bitmap 遍历
        // （std::min<int>(width, width-penX) + setAlphaAsMax）-> penX 推进。
        FT_FaceRec* face = (FT_FaceRec*)m_faces[faceIdx];
        float rotA = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float rotB = RandGen::getFloat(&m_rng, opt.m_fieldc, opt.m_field10);
        float sclA = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float sclB = RandGen::getFloat(&m_rng, opt.m_field14, opt.m_field18);
        float mirX = (m_rng() & 2) ? 1.0f : -1.0f;
        float mirY = (m_rng() & 2) ? 1.0f : -1.0f;
        FT_Matrix matrix;
        matrix.xx = (FT_Fixed)(65536.0f * rotA);
        matrix.xy = (FT_Fixed)(65536.0f * sclA * mirX);
        matrix.yx = (FT_Fixed)(65536.0f * sclB * mirY);
        matrix.yy = (FT_Fixed)(65536.0f * rotB);
        FT_Set_Transform(face, &matrix, 0);
        unsigned int glyphIndex = FT_Get_Char_Index(face, text[i]);
        int err = FT_Load_Glyph(face, glyphIndex, 0);
        err = FT_Render_Glyph(face->glyph, 0);
        (void)err;

        int yOff = m_rng() % opt.m_field1c;
        FT_Bitmap* bm = &face->glyph->bitmap;
        for (int row = 0; row < (int)bm->rows; ++row)
        {
            int yy = penY + row + yOff;
            if (yy > -1 && yy < height)
            {
                int maxCols = width - penX;
                int cols = std::min((int)bm->width, maxCols);
                for (int c = 0; c < cols; ++c)
                {
                    unsigned char a = bm->buffer[bm->pitch * row + c];
                    if (a != 0)
                    {
                        image->setAlphaAsMax(penX + c, yy, a);
                    }
                }
            }
        }
        penX += (int)(opt.m_field20 * (float)bm->width);
    }

    // 波形扭曲（ORIG 第四段：cos/sin 16.16 定点偏移 + 双线性采样）
    GreyImage tmp;
    tmp.init(width, height, 0);
    float freq = RandGen::getFloat(&m_rng, opt.m_field24, opt.m_field28);
    float amp = RandGen::getFloat(&m_rng, opt.m_field2c, opt.m_field30);
    // ORIG：默认构造 + resize(n, 0)（p167 MISSING `vector()`×2 + `resize`×2；
    // 我们原 `vector(n)` 产 EXTRA `vector(unsigned, int const&)` + allocator 族）。
    std::vector<int> cosTbl;
    std::vector<int> sinTbl;
    cosTbl.resize((unsigned)width, 0);
    sinTbl.resize((unsigned)height, 0);
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

    // 形参类型精确匹配（不加调用处强制转换）：ORIG 0x083ff873 的 HandleEvents
    // 实参装载序为 `&signal → queueIdx → this`（右→左），4.4 在调用处写
    // `(MsgQueueMgr::QUEUE_IDX)queueIdx` 会把 queueIdx 的读取提前（寄存器换 %edx）。
    MsgQueueMgr::QUEUE_IDX queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
    if (isLogDB())
    {
        queueIdx = (MsgQueueMgr::QUEUE_IDX)4;
    }
    else
    {
        // ORIG 0x083ff819：isLogDB 为假时仍有一条 `movl $0x2,-0x28(%ebp)`
        // 显式赋值（即源码写了冗余 else 分支），非 fall-through。
        queueIdx = (MsgQueueMgr::QUEUE_IDX)2;
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
        if (!task->HandleEvents(queueIdx, signal))
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
    // ORIG 0857c3be：栈槽 -0x20 sockaddr_in（memset 0x10）；写序 movw $2 @+0（family）
    // → htonl(0) 结果 @-0x1c（s_addr）→ htons 结果 @-0x1e（sin_port），见 _bindUDPSocket.md。
    struct sockaddr_in addr;
    memset(&addr, 0, 0x10);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(port);
    if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
    {
        LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x84,
                              "[ERROR]UDP Thread Socket Binding ERROR");
        int err = *__errno_location();
        // ORIG 0x0857c4a2..：err 先 `mov %eax,-0xc(%ebp)` 再整段提升为
        // `mov -0xc(%ebp),%eax; cmp $0x62,%eax,je / cmp $0x63,%eax,je /
        // test %eax,%eax,je / jmp` 的判定链，三个 case 体依次后置 ⇒ 整数
        // switch 降形态（非 if/else-if 交错），case 0 为空实现。
        switch (err)
        {
        case 0x62:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x9e,
                                  "Port %d for receiving UDP is in use\n",
                                  (int)(unsigned short)addr.sin_port);
            break;
        case 0x63:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa2,
                                  "Cannot assign requested address\n");
            break;
        case 0:
            break;
        default:
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0xa6,
                                  "Could not bind UDP receive port. Error= %d , strerror = %s\n",
                                  err, strerror(err));
            break;
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
// [b28d-18 N82] p167 CLEAR（51/51）；残差=帧槽（N82） | ORIG 0x0857c5a4
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（51/51）；残差=N82 槽位/寄存器 + N89。
void UdpThread::_processUDPData(char* data, int len)
{
    // 跨服 UDP 包 = 10 字节 PacketHeader 线格式（shared/packet/include/PacketHeader.h）；
    // ORIG 0857c5ad：每处读都重取形参（mov 0xc(%ebp),%eax; movzwl (%eax)，P86 逐次重读）
    // → +0x00 ushort packetId。
    unsigned short uVar1 = reinterpret_cast<PacketHeader*>(data)->packetId;
    CPacketTracer* tracer = CPacketTracerInstance();
    tracer->AddUdpLog(3, (int)uVar1);

    static CPacketCounter<1000, 10240> packet_counter(
        G_CEnvironment()->get_file_name(), "InterDispatcher");
    packet_counter.IncrementPacketCount(
        (int)reinterpret_cast<PacketHeader*>(data)->packetId);

    if (!CDispatchServerPacket::dispatchPacket(data))
    {
        char cVar2 = *data;
        if (cVar2 == '\x01')
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xf8);
            CStreamGuard guard(stream, true);
            **guard << 0x9f;
            **guard << -1;
            // 线格式负载切块：header(+0x00..0x01) 之后原始字节，ORIG lea 0x2(%eax)（P156 豁免）。
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
        if (reinterpret_cast<PacketHeader*>(data)->packetId < 1000)
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
// [b28d-18 N82] p167 CLEAR（8/8）；残差=帧槽（N82） | ORIG 0x0857c9de
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 槽位/寄存器分配。
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
// [b28d-18 NEAR] 修：curSec/lastWriteSec 角色互换（ORIG 循环外初始化 curSec、循环内更新 lastWriteSec）；p167 CLEAR 53/53；余 N82 帧槽 | ORIG 0x0847e2d2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（53/53）；gap 1=N82 帧伪影（勿强凑，P182）。
void DispatchThread::dispatch(void* arg)
{
    (void)arg;
    TimerEntry entry;
    unsigned char typeByte = 0;      // 首字节（客户端包分流码）
    unsigned short sizeWord = 0;     // +1 起的 ushort
    message_counter::CMessageCounter msgCounter(10000);
    // ORIG 0847e322/0847e7xx：lastWriteSec（-0x34）循环外置 0，curSec（-0x30）
    // 循环外以 getCurSec() 初始化；循环内更新 lastWriteSec，比较
    // `lastWriteSec - curSec > 600` 后回写 curSec（两次 getCurSec 调用）。
    int lastWriteSec = 0;
    int curSec = GlobalData::s_systemTime_.getCurSec();
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
        lastWriteSec = GlobalData::s_systemTime_.getCurSec();
        interval->SetLastTime();
        if (code == 0)
        {
            // 客户端包线格式头：get_buffer()+0 type(uchar)/+1 size(ushort)。
            // ORIG 0847e496..0847e4b2（DispatchThread/dispatch.md）`movzbl (%eax)`
            // / `add $0x1; movzwl (%eax)` 直译；MSG_BASE 权威头为 13B 占位 blob
            // （内部字段无证据，CMDPacketStruct.h 注），布局未建模，P156 豁免。
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
        if (lastWriteSec - curSec > 600)
        {
            curSec = lastWriteSec;
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
// [b28d-solo-GlobalData] 修结构差（p167 29/29 CLEAR）：①s_server_stop 检查搬到
// 循环尾（ORIG 0x0857d89d `movzbl s_server_stop; xor $1; test; jne body` 为回边
// 目标，trace 落循环外函数尾 0x0857d8af）⇒ `while(!stop){body} trace;`；
// ②去 epoll 具名局部（P183，ORIG 0x0857d5c6 直 `call G_EpollHandler; mov %eax,(%esp)`）。
// 余 gap=6/N82 槽位（帧 0x60 vs 0x?）⇒ N82 封顶。
void NetworkThread::dispatch(void* arg)
{
    (void)arg;
    // [b28d-solo-GlobalData] P183：ORIG 0x0857d5c6 无 epoll 具名局部
    // （`call G_EpollHandler; mov %eax,(%esp); call GetEpollEvents`）。
    epoll_event* events = G_EpollHandler()->GetEpollEvents();
    int epollfd = G_EpollHandler()->GetEpollFD();

    // [b28d-solo-GlobalData] ORIG 0x0857d89d：s_server_stop 检查在**循环尾**
    // （`movzbl s_server_stop; xor $1,%eax; test %al,%al; jne body`，+0x2df 即
    // 回边目标），trace 落循环外函数尾 ⇒ 原码 `while (!stop) { body } trace;`。
    // 循环头 `if(stop){trace;return;}` 会把检查搬到头部并多出 movzbl/test 段。
    while (!GlobalData::s_server_stop)
    {
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
    // ORIG 0x0857d8af：循环退出后统一 trace + 落函数尾（P223 尾岛 nop）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2fe, 0);
    trace("RecvThread Terminate");
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
// [b28d-solo-GlobalData] p167 CLEAR（239/239，0 missing/0 extra）：FD_SET/FD_ISSET
// 三元链逐处重调（P157 禁 mon/s 缓存）+ 状态缓存消除（P48 独立调用）+
// 第二次 IsIntegratedPvPBaseChannel 守卫包裹 auction/cera 段（ORIG 0x0857edb5
// `xor $1; test; je 循环尾`——真语义缺口，整合 PvP 频道时跳过两段）。
// 余 gap=+256/ndiff=1570 = 4.4 对 FD_ZERO/FD_SET 宏展开与帧槽布局差（N82/N89 族） | ORIG 0x0857d95a
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（无调用缺口）；our 1830 vs orig 1574 = N82 帧/槽
// 复用策略 + N89 libstdc++ 全内联（ORIG out-of-line 实例）⇒ 语义完成，byte 层 N 封顶。
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
                // ORIG 0x0857daae..0x0857dc04（p167 实证：每 mgr 6 次三元链）：
                // FD_SET/FD_ISSET 的 fd 实参逐处重调
                // GetServerProxyArrayAt(i)->GetTcpHandler()->get_socket()
                // （P157 逐字段重读，禁 mon/s 缓存；glibc FD_SET 宏对 fd 双求值）。
                if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET(GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET(GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_monitor_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
                }
                if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET(GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET(GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_guild_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
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
            // ORIG 0x0857e089..0x0857e254：select 后同样逐处重调三元链
            // （if 1 次 + FD_ISSET 双求值 2 次；OnTcpRecv 前仅 1 次
            // GetServerProxyArrayAt，无 GetTcpHandler）。
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET(GlobalData::s_monitor_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->GetTcpHandler()
                             ->get_socket(),
                         &readfds))
            {
                // b28c-l23 SIGFIX 消费点：OnTcpRecv 已 bool 化（lane28/lane12），
                // bool callee ⇒ 裸形（P168a①：ORIG `call; xor$1; testb; je`）。
                if (!GlobalData::s_monitor_proxy_mgr
                         ->GetServerProxyArrayAt(i)
                         ->OnTcpRecv())
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
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET(GlobalData::s_monitor_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->GetTcpHandler()
                             ->get_socket(),
                         &writefds))
            {
                int sendRet = GlobalData::s_monitor_proxy_mgr
                                  ->GetServerProxyArrayAt(i)
                                  ->OnTcpSend();
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
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET(GlobalData::s_guild_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->GetTcpHandler()
                             ->get_socket(),
                         &readfds))
            {
                // b28c-l23 SIGFIX 消费点：bool callee ⇒ 裸形（P168a①）
                if (!GlobalData::s_guild_proxy_mgr
                         ->GetServerProxyArrayAt(i)
                         ->OnTcpRecv())
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
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET(GlobalData::s_guild_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->GetTcpHandler()
                             ->get_socket(),
                         &writefds))
            {
                int sendRet = GlobalData::s_guild_proxy_mgr
                                  ->GetServerProxyArrayAt(i)
                                  ->OnTcpSend();
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
        // ORIG 0x0857eab0/0x0857eb12：if/else-if 两臂各自独立调用
        // GetSocketState（P48：O0 无 CSE，禁具名状态缓存）。
        if (GlobalData::s_pcroom_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
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
        else if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
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
            if (GlobalData::s_community_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_DISCONNECTED)
            {
                GlobalData::s_community_proxy->InitTcp();
                GlobalData::s_community_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCommunity =
                    GlobalData::s_systemTime_.getCurSec();
            }
            else if (GlobalData::s_community_proxy->GetSocketState() ==
                     ENUM_SOCKET_STATE_CONNECTING)
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

        // ORIG 0x0857eda3：第二次 G_GameWorld()->IsIntegratedPvPBaseChannel()，
        // `xor $1; test %al,%al; je 循环尾` ⇒ 整合 PvP 频道时跳过
        // auction/cera 两段（与前面 community 段互补的两独立 if，P48 无 CSE）。
        if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
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
        if (GlobalData::s_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
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
        else if (GlobalData::s_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
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
        if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
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
        else if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
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
        }   // !IsIntegratedPvPBaseChannel()
    }
}

// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857cc5a：accept 线程。socket/bind/listen/SetNonBlock
// 后进入 select(1s) accept 循环：按队列余量（m_acceptFlag 水位）与
// 用户池用量（Size-Remain、m_maxUserNum）拒绝/放行连接；放行时
// CGameManager::createUser → InitOnAccept → s_accept_que 入队（NetworkThread
// 消费）。IsProhibitConnectState / 监控 IP / LOCK STATE 分流拒绝包。
// §6.2 巨件核销（b25）：P167 修前 93/89 [缺4 G_CEnvironment]→修后 93/93
// [0缺0多]；gap 516→-6（our 524 超 6）；修法：①env 局部缓存去缓存化，
// 5 处恢复每处独立 G_CEnvironment() 调用（P183 反向，-O0 无 CSE）；
// ②FD_ZERO 改旧 glibc 宏手写展开（fd_set* + unsigned i 双局部逐 __fd_mask
// 清零；新 glibc 宏生 rep stos 形不符）；③FD_SET/FD_ISSET 实参 (unsigned)
// cast 消除有符号除 cmovs 修正链；④remain 预清两句式（P197③）；
// ⑤monitoring-ip strcmp 条件极性 ==0 {close;continue;} 复现 ORIG sete
// 物化 + fail fall-through（P59）。残差归因 P22 xor$1 物化 / setcc-fold /
// N7-N82 寄存器分配 / P63 jmp 中继（详见 docs/asm_translation_patterns.md
// 负例表）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（93/93）；残差=帧槽/寄存器（N82） | ORIG 0x0857cc5a
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（p167 全等）；gap 7=N82 槽位/寄存器。
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
        // ORIG 0857cc5a：栈槽 -0x74 sockaddr_in；写序 family(movw $2) → s_addr(htonl,
        // -0x70) → sin_port(htons, -0x72)，见 AcceptThread/dispatch.md +0x09b..+0x0ad。
        struct sockaddr_in addr;
        std::memset(&addr, 0, 0x10);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(0);
        addr.sin_port = htons(port);
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
            if (bind(listenSocket, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
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
                    // ORIG 0857ce75..+0x283：FD_ZERO 展开=旧 glibc 宏循环
                    // （fd_set* 局部 + unsigned i 局部，逐 __fd_mask 清零）；
                    // 新 glibc 宏展开为 rep stos，须手写旧宏形。
                    {
                        unsigned int i;
                        fd_set* fds = &readfds;
                        for (i = 0; i < sizeof(fd_set) / sizeof(__fd_mask); ++i)
                            __FDS_BITS(fds)[i] = 0;
                    }
                    FD_SET((unsigned)listenSocket, &readfds);
                    struct timeval timeout;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;
                    int selRet = select(listenSocket + 1, &readfds, 0, 0,
                                        &timeout);
                    if (selRet >= 0 &&
                        FD_ISSET((unsigned)listenSocket, &readfds))
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
                            // ORIG +0x38b：sete 物化 + 相等路径 fall-through
                            // close+循环尾（P59 fail 前置），故条件极性 ==0。
                            if (strcmp(
                                    G_CEnvironment()->get_system_monitoring_ip(),
                                    peerIp) == 0)
                            {
                                close(conn);
                                continue;
                            }
                            {
                                if (G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount != 0)
                                {
                                    // ORIG +0x3bc：槽预清 0 后再存（两句式，P197③）
                                    int remain = 0;
                                    remain = (int)GlobalData::s_msgq_mgr->remain(
                                        MsgQueueMgr::DB_Q);
                                    if (m_acceptFlag == 0 &&
                                        remain >=
                                            G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount)
                                    {
                                        m_acceptFlag = true;
                                    }
                                    else if (m_acceptFlag &&
                                             remain <
                                                 G_CEnvironment()->m_serverEnvir.m_acceptUnlockDBQueueCount)
                                    {
                                        m_acceptFlag = false;
                                    }
                                }
                                int used = G_CGameManager()->m_userPool.Size() -
                                           G_CGameManager()->m_userPool.Remain();
                                if (m_acceptFlag || used >=
                                    G_CEnvironment()->m_serverEnvir.m_maxUserNum)
                                {
                                    char* lockIp = inet_ntoa(peerAddr.sin_addr);
                                    if (strcmp("192.168.30.146", lockIp) != 0 &&
                                        G_CEnvironment()->m_serverEnvir.m_maxUserNum + 5 <= used)
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
// 残余差归因（N 类，块内求值序，见下方 0x0847196d 区注释）：ORIG 因提前求值
// varargs 而多保存 %edi（0x084719d6 `add $0x4c,%esp; pop %ebx/esi/edi`），
// 4.4 帧 0x40/仅 %ebx ⇒ 块序与帧大小联动不可源码翻转。
// [b28d-18 N7+N82] p167 CLEAR（8/8）；残差=callee-saved 数量（ORIG push edi/esi/ebx vs 我们 esi/ebx） | ORIG 0x084718e2
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（8/8）；N80（cMyTrace ctor 前右起 varargs 跨
// ctor 保活 %edi/%esi/%ebx 不可翻——4.4 恒 ctor 后求值）+ N82 帧 0x4c vs 0x40。
bool CPCRoomServerProxy::InitTcp()
{
    if (m_ip.length() == 0 || m_tcpPort == 0)
    {
        return 0;
    }
    if (m_tcp.m_sock < 0)
    {
        // ORIG 0x08471xxx：ret 常规局部落 -0x1c 槽（cmp $0,-0x1c; setne 物化）。
        int ret;
        // ORIG 0x0847196d..0x084719c5（临时链式实证）：4.1 在 cMyTrace ctor
        // call 之前先按右起序求 varargs 并全部落 callee-saved —— 0x0847197c
        // strerror(errno)→%ebx、0x08471983 errno→%edi、0x08471985 ret→%esi，
        // 之后才 0x08471988 装填 ctor 实参；4.4 恒先 ctor 后 varargs（改具名
        // 对象反而把 errno 推后 ⇒ probe 证两写法均不可翻）⇒ 记 N 类。
        if ((bool)((ret = CNetwork<100000, 100000>::CreateConnectionSocket(
                       m_tcp.m_sock, m_ip.c_str(), m_tcpPort)) != 0))
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
    // ORIG 0x0810614a：call 结果 %eax 直比（cmp/sete 物化），无 r 局部。
    if (m_udp.InitClientSocket() == -1)
        return false;
    m_field1c = 0;
    memset(m_buf, 0, 0x80000);
    return true;
}

// ---- WongWork ----
// CGMAccounts：ORIG isGM 0x08109346 —— 在 m_gmList 中查找 {mid, type=3}。
WongWork::CGMAccounts::CGMAccounts()
{
}
WongWork::CGMAccounts::~CGMAccounts()
{
}
// [b28d-solo-GlobalData] 修：P56+P293 单表达式 if-return（end 先/find 后 + 分支物化） | ORIG 0x08109346
bool WongWork::CGMAccounts::isGM(unsigned int mid)
{
    // ORIG 0x0810934c..0x08109364：key 先整体零初始化（两条 movl $0x0，
    // 成员声明序 mid→type），再 type=3、mid=mid（P202① 预清两句式）。
    WongWork::CGMAccounts::stGMInfo_t key = {0};   // POD 无用户 ctor（ORIG nm 无 C1）
    key.type = 3;
    key.mid = mid;
    // [b28d-solo-GlobalData] ORIG 0x081093a6：end() 先求值（-0x18）→ find（sret -0x14，
    // 内联 last=第二次 end -0x10、first=begin -0xc）→ ne(this=find 结果, arg=end)；
    // 尾 `test%al;je;mov$1;jmp;mov$0` 为 P56 分支物化（return 表达式直写会少 5 条）。
    // 单表达式形复现 ORIG 求值序（P293：具名 it 形 4.4 恒 find 先/end 后）。
    if (std::find(m_gmList.begin(), m_gmList.end(), key) != m_gmList.end())
    {
        return true;
    }
    return false;
}
int WongWork::CGMAccounts::removeGM(unsigned int mid, unsigned int param)
{
    // ORIG 0x081093fc：空实现（返回 0，sub $0x20 未用局部槽）。
    (void)mid;
    (void)param;
    char pad[0x20];
    (void)pad;
    return 0;
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
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x83), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xca), stHacktypeExceptionInfo_t(0x28, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xcd), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xce), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd0), stHacktypeExceptionInfo_t(0xc8, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd1), stHacktypeExceptionInfo_t(0x96, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd3), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd7), stHacktypeExceptionInfo_t(0x32, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xd8), stHacktypeExceptionInfo_t(0xc8, 0x3e8)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x131), stHacktypeExceptionInfo_t(0x1e, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x193), stHacktypeExceptionInfo_t(0x0a, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19b), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x19c), stHacktypeExceptionInfo_t(0x14, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x340), stHacktypeExceptionInfo_t(0x05, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xdc), stHacktypeExceptionInfo_t(0x64, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0x86), stHacktypeExceptionInfo_t(0x32, 0x0)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe0), stHacktypeExceptionInfo_t(0x00, 0x14)));
    m_exceptionMap.insert(std::make_pair(static_cast<ENUM_HACKTYPE>(0xe1), stHacktypeExceptionInfo_t(0x00, 0x32)));
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
// ORIG 无 stBlackIPMonitorEntry_t 默认 ctor（仅 0x84eba2c 带参版）⇒ 不定义。
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
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用（P48）+ entry 整体拷贝（P157）；NDIFF 25→0 EQ | ORIG 0x084ebace
char WongWork::CBlackIPMonitor::deleteIP(char const* ipStr, unsigned int type)
{
    // ORIG 0x84ebace：ip 为空返回 0；inet_addr 转 key；遍历 [lower_bound, upper_bound)
    // 删除 entry.m_punishType == type 的项。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebaea/0x84ebb14：inet_addr 调用两次（O0 无 CSE，P48），
    // 结果分别落 -0x10/-0xc 两槽（lower_bound/upper_bound 各一）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    while (it != end)
    {
        // ORIG 0x84ebb3a..0x84ebb58：先把 it->second 整体拷到栈（4 字段
        // 逐 dword 落 -0x28/-0x24/-0x20/-0x1c），再比较 m_punishType。
        stBlackIPMonitorEntry_t entry = it->second;
        if (static_cast<unsigned int>(entry.m_punishType) == type)
        {
            // ORIG 0x84ebace：命中即 erase 并返回 1，未命中返回 0。
            m_monitorMap.erase(it);
            return 1;
        }
        ++it;
    }
    return 0;
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝；NDIFF 46→0 EQ | ORIG 0x084ebba6
unsigned int WongWork::CBlackIPMonitor::getPunishType(char const* ipStr)
{
    // ORIG 0x84ebba6：ip 为空返回 0；遍历 [lower_bound, upper_bound) 累 OR
    // entry.m_punishType。
    if (!ipStr)
        return 0;
    unsigned int result = 0;
    // ORIG 0x84ebba6：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebc2x：entry 整体拷贝到栈后取 m_punishType（P48 同族）。
        stBlackIPMonitorEntry_t entry = it->second;
        result |= static_cast<unsigned int>(entry.m_punishType);
    }
    return result;
}
// [b28d-18 IDENTICAL_AE] 修：inet_addr 两次独立调用 + entry 整体拷贝（P48/P157）；ORIG 切片缺失，objdump 手工核对 71=71 仅重定位伪差 | ORIG 0x084ebc8c
long WongWork::CBlackIPMonitor::GetEndTime(
    char const* ipStr, ENUM_BLACKIPMONITOR_PUNISH_TYPE type)
{
    // ORIG 0x84ebc8c：ip 为空返回 0；遍历 [lower_bound, upper_bound) 返回
    // entry.m_punishType == type 的 m_endTime，否则 0。
    if (!ipStr)
        return 0;
    // ORIG 0x84ebca8/0x84ebce4：inet_addr 两次独立调用（P48）。
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator it =
        m_monitorMap.lower_bound(inet_addr(ipStr));
    std::multimap<unsigned int, stBlackIPMonitorEntry_t>::const_iterator end =
        m_monitorMap.upper_bound(inet_addr(ipStr));
    for (; it != end; ++it)
    {
        // ORIG 0x84ebd27..0x84ebd3c：entry 整体拷贝到 -0x30..-0x24 后比较。
        stBlackIPMonitorEntry_t entry = it->second;
        if (entry.m_punishType == type)
        {
            return entry.m_endTime;
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

// [b28d-18 N18a] p167 CLEAR；ORIG 全局计数两段装载，4.4 恒单段（N18a 反向） | ORIG 0x086027fc
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
            // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0860283x 每次重发
            // row/col 两级寻址直传 this；缓存 `CRijndael* r` 多 1 条且槽整体深 4
            // （row/col -0x14/-0x10 vs ORIG -0x10/-0xc）。
            // [b28d-solo-GlobalData] 索引加法序两形（row*24+col / col+row*24）实测 4.4
            // 恒折 `add -0xc(%ebp),%eax` 内存版，ORIG 4.1 先 `mov col,%ecx` 寄存器版
            // ⇒ P30/N4 add-vs-mem 封顶（余 1 条 + 槽深差 4）。
            m_keys[row][col].Initialize(strKeyTable_[row * 24 + col], 0x20,
                                        CRijndael::sm_chain0, 0x10, 0x10, 0, 0);
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
bool ItemVendingMachine::BuyItem(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
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
    // ORIG 0x08179ef3：call 后 xor $1+test+je→mov $1 块 / fall-through mov $0
    // ——bool 物化尾（同 OpenMaster 技巧 1/17 形）。
    if (!m_bonus.CheckBonusItem((int)itemIdx))
        return false;
    return true;
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

// （旧 ipg_field1c/ipg_str 偏移寻址 helper 已删：TaiwanCash.h SIPGData 已按
// §8.3 字段化（W6SIPG 批次），本 TU 改用具名成员/命令族视图，P49。）
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

    // ORIG 0x081783fd/0x08178431/0x08178464/0x08178498：每处条件/log 实参都
    // 两段重读 `mov 0x14(%ebp),%eax; mov 0x1c(%eax),%eax` —— 源码无 status
    // 缓存局部（缓存反产 cmpl imm,-0xc 槽比较 + 帧 +0x10）。
    if (resData->m_type == 4)
    {
        ProcessIPG_ResultQuery(user, reqData, resData, stream);
    }
    else if (resData->m_type == 0)
    {
        ProcessIPG_ResultInput(user, reqData, resData, stream);
    }
    else if (resData->m_type == 2)
    {
        ProcessIPG_ResultOutput(user, reqData, resData, stream);
    }
    else
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyJpnEx,
                              0x482, "Unhandled reply from BModule is %d",
                              resData->m_type);
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
    // ORIG 0x0817866f：`cmpl $0x0,0xc(%ebp)` 后无分支——4.4 对
    // `if (user == 0) return;`（分支目标=相邻 epilogue）做 jump threading，
    // 保留 flag-setting cmp。
    if (user == 0)
        return;
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
    int retCode = atoi(resData->m_cmd.m_query.m_retCode);
    // ORIG 0x08178530：`cmpl $0x0,-0x14; je <sum 支>` ⇒ 非零支为 fall-through
    // 主体（SendCashData(true)+0x4dd 日志），求和支在跳转目标处 ⇒ 源码写
    // `if (retCode != 0) {…} else {…}`（N57 主体位置范式）。
    if (retCode != 0)
    {
        user->SendCashData(true);
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyResultQuery,
                              0x4dd, "IPG_QUERY %d %d", user->get_acc_id(), retCode);
    }
    else
    {
        int total = atoi(resData->m_cmd.m_query.m_ceraValues[4]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[5]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[2]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[1]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[3]) +
                    atoi(resData->m_cmd.m_query.m_ceraValues[0]);
        user->SetCera((unsigned int)total);
        unsigned int point = (unsigned int)atoi(resData->m_cmd.m_query.m_ceraPoint);
        user->SetCeraPoint(point);
        // ORIG cUserHistoryLog@user+0x79700 ::CeraPoint(ENUM 0, total)
                user->m_historyLog.CeraPoint((ENUM_CERAPOINT_LOG_TYPE)0,
                                     (unsigned int)total);
        user->SendCashData(false);
    }
}

// ----------------------------------------------------------------------------
// ProcessIPG_ResultOutput —— ORIG 0x08178676（0x12a8 字节，报告全文转写）。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 超大件：残差=槽位/STL（N82/N89） | ORIG 0x08178676
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memcpy×3/guard_abort×2/_Unwind_Resume×2/getItemList×2/
// make_pair/logFormat/ne —— 我方多发调用（多余局部/临时），已定位待收敛（非发明符号）。
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

    int retCode = atoi(resData->m_cmd.m_output.m_resultCode);   // local_e0
    if ((retCode != 0) || user == 0 || user->getCurCharacR() == 0)
    {
        // 错误路（ORIG 0x8178923..0x8178a5e）
        if (user == 0 || user->getCurCharacR() == 0)
        {
            cMyTrace trace(kPrettyResultOutput, 0x59d, 5);
            trace("There's abnormal transaction that the client doesn't exist, "
                  "client would be %s(m_id: %d) and the Goodis is %d, ret_code = %d ret_msg = %s",
                  resData->m_accName, atoi(resData->m_account),
                  atoi(resData->m_cmd.m_output.m_price), retCode,
                  resData->m_cmd.m_output.m_message);
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
                                      reqData->m_cmd.m_output.m_arg7D, retCode, accId, characNo,
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
            restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(
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
            reqData->m_cmd.m_output.m_arg7D, param);
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
                G_CDataManager()->m_itemList->getCeraShopBonusItem((int)usedCera,
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
        resData->m_cmd.m_output.m_tranNo;   // ull@+0x1e0（两 dword 拼接即此字段，TaiwanCash.h 实证）
    cUserHistoryLog* history = &user->m_historyLog;
    if (buytype == 1)
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      resData->m_cmd.m_output.m_arg7D, resData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 resData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)1, billUid);
    }
    else
    {
        BillingRecord(user, (int)productNo, -1, (int)ceraPrice,
                      reqData->m_cmd.m_output.m_arg7D, reqData->m_cmd.m_output.m_message,
                      -1, billUid, buyFlag);
        history->BuyCashShopItem(productNo, itemIdx, stackCount,
                                 reqData->m_cmd.m_output.m_arg7D,
                                 buyFlag, (CeraShopPaymentType::T)0, billUid);
    }
}

// ----------------------------------------------------------------------------
// BillingRecord —— ORIG 0x0817991e（0x58f 字节，报告 BillingRecord.md + objdump 转写）。
// 打包 0x10e 字节计费记录（SIG 结构以文件内局部缓冲按 ORIG 偏移写入）后
// put_binary/入 DB 队列；PuUser 标志异常时补 DB_SavePuUserCheck。
// ----------------------------------------------------------------------------
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0817991e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 帧/槽 + N89 STL 展开。
void ItemVendingMachine::BillingRecord(CUser* user, int a3, int a4, int a5,
                                       const char* itemName, const char* accName,
                                       int priceLabel, unsigned long long uid,
                                       char payFlag)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp",
                                                        0x730);
    CStreamGuard guard(stream, true);
    // ORIG BillingRecord.md：operator*() 各 << 前重取 + operator<<(int)（P48/CStreamGuard 形态）
    (*guard.operator*()) << (int)0xb0;           // SIG opcode（operator*() 取 this）
    (*guard.operator*()) << (int)user->GetUID();

    // 0x10e 字节记录（B = ORIG 帧 -0x13b(%ebp)，memset 0x10e 实证；
    // 以下偏移全部按 ORIG 汇编 0x0817991e 逐条核对）
    SIG_BILLING_RECORD rec;
    memset(&rec, 0, sizeof(rec));
    // ORIG 0x081799d5：strcpy(B+0x00, 0x1c(%ebp)) —— 第 5 实参 itemName；
    // 账号名（get_acc_name）写 B+0x35，被举报/支付名 accName 写 B+0x69。
    strcpy(rec.m_agencyNo, itemName);                             // B+0x00
    strcpy(rec.m_userId, user->get_acc_name());                   // B+0x35（char[0x19]）
    rec.m_sealFlag = 1;                                           // ORIG -0x126
    rec.m_saleTime = a5;                                          // ORIG -0x122 = 0x18(%ebp)
    rec.m_price = 0;                                              // ORIG -0x11e
    rec.m_slotPartType = 1;                                       // ORIG -0x11a
    rec.m_slotPartCode = a3;                                      // ORIG -0x116 = 0x10(%ebp)
    rec.m_ipgNo = a4;                                             // ORIG -0x112 = 0x14(%ebp)
    rec.m_mId = user->get_acc_id();                               // ORIG -0x10e
    rec.m_characNo = user->getCurCharacNo();                      // ORIG -0x10a
    rec.m_job = user->get_charac_job();                           // ORIG -0xed

    // SSN 拆分 → 生日串（B+0x56 char[11]）与世纪类型（B+0x52）
    const char* ssn = user->GetSsnString();
    char ssnPart[0xe];
    memset(ssnPart, 0, sizeof(ssnPart));
    if (ssn != 0)
        strncpy(ssnPart, ssn, 0xe);
    // ORIG 汇编：yy/mm/dd 为 char[3]（memset 3 字节保证 %s 终止符），cent 为 char[2]。
    char yy[3], mm[3], dd[3];
    memset(yy, 0, 3); memset(mm, 0, 3); memset(dd, 0, 3);
    // 汇编 strncpy 布局：yy←ssn+0(2)、mm←ssn+2(2)、dd←ssn+4(2)、cent←ssn+6(1)
    // ORIG strncpy（非 memcpy）拆分 yy/mm/dd/cent
    strncpy(yy, ssnPart + 0, 2);
    strncpy(mm, ssnPart + 2, 2);
    strncpy(dd, ssnPart + 4, 2);
    char centBuf[2];
    memset(centBuf, 0, 2);
    strncpy(centBuf, ssnPart + 6, 1);
    int cent = atoi(centBuf);
    // ORIG 0x08179bcd 起：sprintf 直接写入记录 B+0x56（无独立局部缓冲），
    // 世纪类型写 B+0x52。
    if (cent == 2)
    {
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
    }
    else if (cent == 1)
    {
        sprintf(rec.m_birthday, "19%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
    }
    else if (cent == 3)
    {
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 1;
    }
    else if (cent == 4)
    {
        sprintf(rec.m_birthday, "20%s-%s-%s", yy, mm, dd);
        rec.m_sex = 2;
    }

    rec.m_prizeFlag = 1;                                   // ORIG -0xda
    rec.m_resultCode = 0;                                  // ORIG -0xd6
    // ORIG 0x08179cca：strcpy(B+0x69, 0x20(%ebp)) —— 第 6 实参 accName。
    strcpy(rec.m_resultMessage, accName);
    rec.m_serverId = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // -0x6e
    rec.m_growType = user->getCurCharacGrowType();         // -0x6a
    rec.m_level = user->get_charac_level();                // -0x66
    rec.m_giftServerInfo = rec.m_serverId;                 // -0x62
    rec.m_giftGetCharacNo = 0;                             // -0x5e
    rec.m_chargePrice = rec.m_saleTime;                    // -0x5a = a5
    rec.m_freePrice = 0;                                   // -0x56
    rec.m_countNo = priceLabel;                            // -0x52 = 0x24(%ebp)
    rec.m_eachPrice = rec.m_saleTime;                      // -0x4e = a5
    // strcmp(accName,"mileage item"/"cera point")——ORIG 0x8179d49 源=0x20(%ebp) 实证
    if (strcmp(accName, "mileage item") == 0)
        rec.m_priceType = 2;
    else if (strcmp(accName, "cera point") == 0)
        rec.m_priceType = 3;
    else
        rec.m_priceType = 1;

    // ORIG 0x08179d82：memset(B+0xf5,0,0x10) → GetPeerIP2(B+0xf5,0x10)，
    // 失败（返回 0）时 strncpy(B+0xf5,"NONE",5)。
    memset(rec.m_userIp, 0, 0x10);
    if (user->m_network.GetPeerIP2(rec.m_userIp, 0x10) == 0)
        memcpy(rec.m_userIp, "NONE", 5);
    rec.m_tranNoLow = (int)(unsigned int)uid;                    // ORIG -0x36 = 0x28(%ebp)
    rec.m_tranNoHigh = (int)(unsigned int)(uid >> 32);           // ORIG -0x32 = 0x2c(%ebp)
    rec.m_buyType = payFlag;                                     // ORIG -0x2e = 0x30(%ebp)

    guard.operator->()->put_binary(reinterpret_cast<char*>(&rec), 0x10e);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);

    // PuUser 标志：ORIG 08179e56..08179e73（BillingRecord.md）`call GetPuUser;
    // xor $0x1,%eax; test %al,%al; je skip` → 非 1（未认证）时补存（P22 bool 取反
    // xor 形态；原实现判定极性相反，按报告修正）。
    if (!user->GetPuUser())
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

// asm 桥已清（§8 协议）：CInventory.h 现经 CGenUniqueNo.h 纳入权威完整类定义，
// GenUniqueNo 亦按 ORIG 单实参证据 static 化 ⇒ 调用点直接真实调用
// WongWork::CGenUniqueNo::GenUniqueNo（ORIG _ZN8WongWork12CGenUniqueNo11GenUniqueNoEPc）。
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
    if (goods->GetGroupIdx() != 1)
    {
        // ORIG 0x081769c3：&& 短路结果落 %eax(0/1) 后 `test %al,%al; je`
        // 物化判（无局部槽）——(bool) 显式物化形。
        if ((bool)(goods->GetGroupIdx() == 11 && goods->GetSubGroupIndex() == 1))
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
// [b28d-solo-GlobalData] 已修 AE 87=87（p167 10/10 CLEAR）：①首调 GetRelatedItem
// 结果 ORIG 0x081767a6 无槽往返（`call; mov (%eax),%edx; add $0x10,%edx;
// mov (%edx),%edx; mov %eax,(%esp); call *%edx`）⇒ 内联 IsAvatarItem（P0）；
// ②去 vec 具名指针（P183，ORIG 仅 equip -0x10/select -0xc 两槽）；③范围比较
// ORIG 0x0817680f `cmp -0x1c(%ebp),%al; jg` = 8 位有符号（T1 直证 char）。
bool ItemVendingMachine::_CheckEnableAvatarCondition(const CCeraShopGoods* goods,
                                                     char c, unsigned char uc,
                                                     stAvatarTypeSelect_t& out)
{
    // [b28d-solo-GlobalData] ORIG 0x081767a6：首次 GetRelatedItem 结果**无槽往返**
    // （`call; mov (%eax),%edx; add $0x10,%edx; mov (%edx),%edx; mov %eax,(%esp);
    // call *%edx`）⇒ 原码内联 `goods->GetRelatedItem()->IsAvatarItem()`（P0）；
    // 具名 related 局部会多 store/load 并把后续槽整体顶深。
    if (!goods->GetRelatedItem()->IsAvatarItem())
        return false;
    out.init();
    CEquipItem* equip = static_cast<CEquipItem*>(goods->GetRelatedItem());
    // ORIG 0x08176794 段（p167 MISSING `operator[](unsigned int) const`×4）：
    // 原码是 const 引用视图 ⇒ 声明 const 指针转型（P270 旋钮）。
    // ORIG 0x081767e4..0x081767f2 只有 equip(-0x10)/select(-0xc) 两个槽，
    // 中间无 vec 具名指针 ⇒ 内联 cast（P183 去缓存）。
    const std::vector<stAvatarTypeSelect_t>& select =
        *static_cast<const std::vector<stAvatarTypeSelect_t>*>(
            equip->getAvatarTypeSelect());
    // ORIG 0x0817680f `cmp -0x1c(%ebp),%al; jg` ⇒ 8 位有符号比较
    // （`(signed char)size() > c`；P287 同族），无具名 idx 局部。
    if (select.empty() || (signed char)select.size() <= c)
        return false;
    if (uc >= 100)
        return false;
    out.m_fieldc = select[(signed char)c].m_fieldc;
    out.m_field4 = select[(signed char)c].m_field4;
    out.m_field8 = select[(signed char)c].m_field8;
    out.m_field0 = select[(signed char)c].m_field0;
    return true;
}

// _GetItemString —— ORIG 0x0817645a (T)
void ItemVendingMachine::_GetItemString(const CCeraShopGoods* goods, char* buf,
                                        unsigned int len)
{
    // [b23S3C] ORIG 0x0817645a：无 switch/jump table——七条 if-else 链，每判重 call
    // GetItemIdx（P48 去缓存），命中才 call GetStackCount 作 snprintf 实参直落 0xc(%esp)。
    if (goods->GetItemIdx() == 1) { OS_API::snprintf(buf, len, "Continue Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xca) { OS_API::snprintf(buf, len, "Mystic Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 8) { OS_API::snprintf(buf, len, "Hands of Remy %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xf) { OS_API::snprintf(buf, len, "Kaleido Box %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0x43d) { OS_API::snprintf(buf, len, "Manaade %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xe) { OS_API::snprintf(buf, len, "Golden Wax Candle %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xd8) { OS_API::snprintf(buf, len, "Mystic Cube %d", goods->GetStackCount()); return; }
    CItem* related = goods->GetRelatedItem();
    if (related == 0)
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyGetItemString,
                              0x18d, "CashShop Relation Item is NULL [ITEM:%d]",
                              goods->GetItemIdx());
        return;
    }
    if (related->is_stackable() && goods->GetStackCount() >= 2)
        OS_API::snprintf(buf, len, "%s Count %d", related->GetItemName(), goods->GetStackCount());
    else
        strncpy(buf, related->GetItemName(), len);
}

// ============================================================================
// ProcessBuyRequestEx —— ORIG 0x081769f6 (T)
// SIGFIX:ItemVendingMachine::ProcessBuyRequestEx（头已 bool 化：ORIG
//   __PRETTY_FUNCTION__ 串 "bool ItemVendingMachine::ProcessBuyRequestEx
//   (CUser*, const MSG_BUY_CERASHOP_ITEM&)"，P174）⇒ 定义同步 bool。
// ============================================================================
// b28c-l23 SIGFIX：返回类型 int→bool（ORIG __PRETTY_FUNCTION__ 串 @0x08b87080，
// P174；体内 return 仅 0/1 语义）。mangled 不含返回类型，符号与 ABI 零变。
// [b28d-18 N82] 超大件：残差=槽位/STL（N82/N89） | ORIG 0x081769f6
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×7+memcpy×4 = N78（4.1 内联清零/块搬移 vs
// 4.4 call）；大件三件套 gap 归 N82/N78，语义无缺口（MISSING 空）。
bool ItemVendingMachine::ProcessBuyRequestEx(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    if (user == 0)
    {
        cMyTrace tr(kPrettyProcessBuy, 0x290, 0);
        tr("There is no User in ItemVendingMachine");
        return false;
    }

    // ORIG 0x08176a4e..0x08176abd 初始化序（Ghidra C 与反汇编双向核对）：
    //   0x08176a4e movl $0xffffffff,-0x78（groupIdx=-1）
    //   0x08176a55/-0x74、0x08176a5c/-0x70、0x08176a63/-0x6c、0x08176a6a/-0x68（4 累计量=0）
    //   0x08176a71 call vector<pair<int,int> >::vector（local_170=-0x16c）
    //   0x08176a7f rep stos 0x24 dword @-0x280（related 指针数组，非单变量）
    //   0x08176a95 rep stos 0x24 dword @-0x310（goods 指针数组）
    //   0x08176aab call CUser::resetUsedCera
    // 0x081769f6..0x081783e9 全区间不存在 $0x23 立即数 ⇒ 原码没有条目数上限钳制；
    // 循环上界每处现读 msg 字节（0x08176bc5/0x08177dcc 区 movzbl 0x36(%eax)）。
    int groupIdx = -1;
    int totalCera = 0, totalGold = 0, totalWin = 0, totalMileage = 0;

    std::vector<std::pair<int, int> > pairs;
    const CItem* related[0x24];      // ORIG local_284（逐条目 _CheckEnableItemCondition 输出）
    std::memset(related, 0, sizeof(related));
    CCeraShopGoods* goods[0x24];     // ORIG local_314
    std::memset(goods, 0, sizeof(goods));

    user->resetUsedCera();

    const unsigned int itemCount = msg.m_scalars.m_itemCount;
    const unsigned int MAX_ENTRIES = 0x23;   // 条目数上报上限（ProcessError 实参用）

    // ---- 第一遍：逐条目校验、总价累计 ----
    for (int i = 0; i < (int)itemCount; ++i)
    {
        int productNo = msg.m_entrySection.m_arr[i].m_productNo;
        unsigned char slotByte = msg.m_entrySection.m_arr[i].m_avatarType0;
        unsigned char tagByte = msg.m_entrySection.m_arr[i].m_avatarType1;
        (void)slotByte; (void)tagByte;

        // ORIG 0x08176ad5 G_CDataManager() → 0x08176ae8 直调
        // _ZNK12CDataManager9FindGoodsEi（0x0835fde4，内部转发
        // CItemList::FindGoods），结果 0x08176aed 直存 goods[i]（无 getItemList 层）。
        goods[i] = G_CDataManager()->FindGoods(productNo);
        if (goods[i] == 0)
        {
            m_ceraShop.ProcessError(user, 0x15, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            // ORIG 0x08176b95/0x08176bbb：每个校验失败分支 ProcessError + goods[i]=0
            // 之后固定跟一对 cMyTrace（具名槽 -0x150）：tr("[%s][%d]", PRETTY, line)。
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2ad, 0);
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ad);
            continue;
        }

        groupIdx = goods[i]->GetGroupIdx();

        if (!_CheckEnableGiftCondition(goods[i], user, (char)msg.m_scalars.m_buyFlag))
        {
            m_ceraShop.ProcessError(user, 8, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2b4, 0);   // ORIG 0x08176ca2/0x08176cc8
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2b4);
            continue;
        }

        // IsBuyableRestrictItem(user, productNo, 1, i)：1=可购买（取反判不可购）。
        if (!CerashopAddRestrict::Manager::IsBuyableRestrictItem(user, productNo, 1, i))
        {
            m_ceraShop.ProcessError(user, 0x71, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2bb, 0);   // ORIG 0x08176d9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2bb);
            continue;
        }

        if (!_CheckEnableItemCondition(goods[i], related[i], pairs))
        {
            m_ceraShop.ProcessError(user, 0x11, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c2, 0);   // ORIG 0x08176e9f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c2);
            continue;
        }

        if (!_CheckEnableStateCondition(goods[i], user))
        {
            m_ceraShop.ProcessError(user, 7, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2c8, 0);   // ORIG 0x08176f89
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2c8);
            continue;
        }

        int buyingCode = 0;
        if (!m_ceraShop._CheckBuyingCondition(user, goods[i], productNo,
                                              msg.m_entrySection.m_arr[i].m_avatarType0,
                                              (unsigned short)msg.m_entrySection.m_arr[i].m_avatarType1,
                                              (char)msg.m_scalars.m_buyFlag, buyingCode))
        {
            m_ceraShop.ProcessError(user, (unsigned short)buyingCode, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo,
                                    i + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x2d6, 0);   // ORIG 0x08177105
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2d6);
            continue;
        }

        if (groupIdx == 0)
        {
            stAvatarTypeSelect_t avatarSel;
            if (!_CheckEnableAvatarCondition(goods[i],
                                             (char)msg.m_entrySection.m_arr[i].m_avatarType1,
                                             msg.m_entrySection.m_arr[i].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[i].m_productNo,
                                        i + 1, itemCount,
                                        (char)msg.m_scalars.m_buyFlag);
                goods[i] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x2ec, 0);   // ORIG 0x08177234
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x2ec);
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

        if (msg.m_scalars.m_buytype == 1 && goods[i]->isCeraPointDisallowance_())
        {
            m_ceraShop.ProcessError(user, 0x138b, groupIdx,
                                    msg.m_entrySection.m_arr[i].m_productNo, i + 1, itemCount,
                                    (char)msg.m_scalars.m_buyFlag);
            goods[i] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x305, 0);   // ORIG 0x0817738f
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x305);
            continue;
        }
    }

    // ---- 总校验 ----
    if (!m_ceraShop._CheckValidCount((const CCeraShopGoods**)goods))
    {
        m_ceraShop.ProcessError(user, 1, -1, msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    int errCode = 0;
    if (user->GetCera() < totalCera) errCode = 0xb;
    else if (user->getCurCharacMoney() < totalGold) errCode = 10;
    else if (user->GetWinPoint() < totalWin) errCode = 0xc;
    else if (user->GetMileage() < totalMileage) errCode = 0x3ed;
    if (msg.m_scalars.m_buytype == 1)
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
                                msg.m_entrySection.m_arr[0].m_productNo, 1, 1,
                                (char)msg.m_scalars.m_buyFlag);
        return true;
    }

    // ---- 总价资金校验 ----
    // ORIG 余额槽宽度实证（64 位比较序列 + max 模板实参 mangled）：
    //   0x0817765a call getCurCharacMoney → 0x08177661 `sar $0x1f,%edx`（**符号**
    //     扩展）→ mov %eax,-0x50 / mov %edx,-0x4c ⇒ curGold 是 signed long long。
    //   0x08177670 call GetWinPoint → 0x08177675 `mov $0x0,%edx`（**零**扩展）
    //     → -0x48/-0x44 ⇒ curWin 是 unsigned long long。
    //   0x0817768d call GetMileage → mov %eax,-0x38（单 dword）⇒ curMileage 是 int。
    // 扣减用 _ZSt3maxIi（0x08177991）+ _ZSt3maxIx ×2（0x081779e2/0x08177a37；
    //   c++filt 实证 `x` = **long long**（有符号），非 unsigned long long
    //   ⇒ 两个实参槽均须 long long 类型才能推导出同一实例）。
    long long curGold = user->getCurCharacMoney();
    long long curWin = user->GetWinPoint();
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
                                             (char)msg.m_entrySection.m_arr[j].m_avatarType1,
                                             msg.m_entrySection.m_arr[j].m_avatarType0,
                                             avatarSel))
            {
                m_ceraShop.ProcessError(user, 0x17, groupIdx,
                                        msg.m_entrySection.m_arr[j].m_productNo,
                                        j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
                goods[j] = 0;
                cMyTrace tr(__PRETTY_FUNCTION__, 0x37b, 0);   // ORIG 0x081777d6
                tr("[%s][%d]", __PRETTY_FUNCTION__, 0x37b);
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
        int productNo = msg.m_entrySection.m_arr[j].m_productNo;

        int perErr = 0;
        if (user->GetCera() < (unsigned int)ceraPrice) perErr = 0xb;
        else if (user->getCurCharacInvenR()->get_money() < goldPrice) perErr = 10;
        else if (user->GetWinPoint() < (unsigned int)winPrice) perErr = 0xc;
        if (msg.m_scalars.m_buytype == 1)
        {
            perErr = 0;
            if (user->GetCeraPoint() < (unsigned int)ceraPrice)
                perErr = 0x138a;
        }
        // 0x08177917..0x08177962：三条**余额**校验（不是现读 user）。
        //  ① -0x50/-0x4c(curGold) vs sext(goldPrice) → 有符号 64 位比较，
        //     `jl/jg/cmp lo;jbe` 三段式 ⇒ `curGold < goldPrice` → perErr=10。
        //  ② -0x48/-0x44(curWin) vs zero-ext(winPrice) → 无符号 64 位比较 → 0xc。
        //  ③ -0x38(curMileage) vs -0x3c(mileagePrice) 32 位 `jge skip` → 0x3ed。
        // 0x08177917..0x08177a44：ORIG 把三条余额校验与**扣减**全部排在
        // 0x08177a47 `cmpl $0x0,-0x28(perErr); je 正常路径` 之前 ⇒ 即使本条目
        // 判错，余额照样被扣（原码行为，非笔误）。扣减三处 = std::max：
        // _ZSt3maxIi @0x08177991、_ZSt3maxIx @0x081779e2、@0x08177a37
        // （Itanium ABI `x` = unsigned long long，解释 i386 上为何读 0x4(%eax)）；
        // 声明顺序必须 diff 在前、zero 在后（ORIG 先 `mov %eax,-0xc0/-0xb8/-0xa8`
        // 后 `movl $0x0,-0xbc/-0xb0/-0xa0`，技巧 21 后声明占近 ebp 槽）。
        if (curGold < goldPrice) perErr = 10;         // 0x08177917 有符号 64 位比较
        if (curWin < (unsigned int)winPrice) perErr = 0xc;   // 0x08177935 winPrice 零扩展
        if (curMileage < mileagePrice) perErr = 0x3ed;             // 0x08177953 32 位
        {
            int diffMileage = curMileage - mileagePrice;   // ORIG -0xc0（先算差值）
            int zeroMileage = 0;                           // ORIG -0xbc
            curMileage = std::max(zeroMileage, diffMileage);   // 0x08177991
        }
        {
            long long diffGold = curGold - goldPrice;      // ORIG sub/sbb → -0xb8/-0xb4
            long long zeroGold = 0;                        // ORIG -0xb0/-0xac
            curGold = (long long)std::max(zeroGold, diffGold);   // 0x081779e2
        }
        {
            long long diffWin = curWin - (unsigned int)winPrice;  // ORIG -0xa8/-0xa4
            long long zeroWin = 0;                                // ORIG -0xa0/-0x9c
            curWin = std::max(zeroWin, diffWin);           // 0x08177a37
        }

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
            // 0x08177b1e..0x08177b30：第三处 logFormat 的 %d 实参是**现读**
            // msg 条目（`mov -0x2c,%eax; imul $0x1cf; lea (msg,…); add $0x30;
            // mov 0x9(%eax)` = entry+0x39 = m_productNo），不是局部 productNo。
            LogManager::logFormat(
                1, "localjapan/AppendVendingMachine.cpp", kPrettyProcessBuy, 0x3b9,
                "Error Goods No : %d", msg.m_entrySection.m_arr[j].m_productNo);
            m_ceraShop.ProcessError(user, (unsigned short)perErr, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3bb, 0);   // ORIG 0x08177bf4
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3bb);
            continue;
        }

        // （扣减已按 ORIG 0x08177962..0x08177a44 前移至上方 perErr 判定之前）

        if (!user->IsRestrictedGoods(goods[j]->GetFeaturedIdx()))
        {
            Stream* stream = GlobalData::s_stream_pool->Acquire(
                "localjapan/AppendVendingMachine.cpp", 0x3cb);
            CStreamGuard guard(stream, false);
            // ORIG 0x08177d3b..0x08177d59：`lea -0x178; mov %eax,(%esp);
            // call CStreamGuard::operator->(); mov %eax,(%esp);
            // call GetInBuffer<stCeraShopIPGRequest_t>()` ⇒ 源码就是
            // guard->GetInBuffer<...>()（CStreamGuard::operator-> 返回 this），
            // 不得引入 g 中转变量（技巧 35/36：代理链 + 删中转槽）。
            stCeraShopIPGRequest_t* req =
                guard->GetInBuffer<stCeraShopIPGRequest_t>();
            req->m_productNo = productNo;
            req->m_groupIdx = goods[j]->GetGroupIdx();
            req->m_stackCount = (unsigned short)goods[j]->GetStackCount();
            req->m_ceraPrice = (unsigned int)ceraPrice;
            req->m_goldPrice = (unsigned int)goldPrice;
            req->m_winPoint = (unsigned int)winPrice;
            req->m_itemIdx = goods[j]->GetItemIdx();
            req->m_avatarType0 = msg.m_entrySection.m_arr[j].m_avatarType0;
            req->m_avatarType1 = msg.m_entrySection.m_arr[j].m_avatarType1;
            req->m_avatarField = (unsigned int)avatarField;
            req->m_featuredIdx = goods[j]->GetFeaturedIdx();
            req->m_itemCount = itemCount;
            req->m_seq = j + 1;
            req->m_curSec = curSec;
            req->m_buyFlag = msg.m_scalars.m_buyFlag;
            req->m_accId = msg.m_scalars.m_accId;
            req->m_characNo = msg.m_scalars.m_characNo;
            req->m_buytype = msg.m_scalars.m_buytype;
            req->m_subCount = msg.m_entrySection.m_arr[j].m_subCount;
            for (unsigned int k = 0; k < req->m_subCount && k < MAX_ENTRIES; ++k)
            {
                // 子项数组：entry+0x0e 起每项 5 字节 { int idx; uchar val; }
                int idx = msg.m_entrySection.m_arr[j].m_subEntries[k].m_itemIdx;
                unsigned char val = msg.m_entrySection.m_arr[j].m_subEntries[k].m_value;
                std::memcpy(req->m_subEntries[k], &idx, sizeof(int));
                req->m_subEntries[k][4] = val;
            }
            req->m_optCount = msg.m_entrySection.m_arr[j].m_optCount;
            for (unsigned int k = 0; k < req->m_optCount && k < MAX_ENTRIES; ++k)
            {
                // 选项数组：entry+0xbe 起每项 8 字节 { ulong itemIdx; ushort a; ushort b; }
                //（ORIG k 达 0x22 时读入邻条目尾，线格式遗留，数组越界读等价保留）
                const MSG_CERA_SHOP_BUY_ENTRY::OptItem& srcOpt =
                    msg.m_entrySection.m_arr[j].m_optEntries[k];
                unsigned long itemIdx = srcOpt.m_itemUid;
                unsigned short a = srcOpt.m_a;
                unsigned short b = srcOpt.m_b;
                unsigned char* opt = req->m_optEntries[k];
                // 选项项布局：{ ulong itemIdx(4, at +0); ushort a(2, at +4); ushort b(2, at +6) }
                //（dest 为 (k+0x2c)*8 + {4,8,10}，即 m_optEntries[k] 的 +0/+4/+6）。
                std::memcpy(opt, &itemIdx, sizeof(unsigned long));
                std::memcpy(opt + 4, &a, sizeof(unsigned short));
                std::memcpy(opt + 6, &b, sizeof(unsigned short));
            }

            guard.operator*()->operator<<(mileagePrice);

            char itemStr[0x400];
            char ceraPriceStr[0x24];
            char productStr[8];
            char itemIdxStr[0x14];      // ORIG -0x18c（0x08178137 取址目标，结果未被使用）
            char uniqueStr[0x15];
            std::memset(itemStr, 0, sizeof(itemStr));
            std::memset(ceraPriceStr, 0, sizeof(ceraPriceStr));
            std::memset(productStr, 0, sizeof(productStr));
            std::memset(itemIdxStr, 0, sizeof(itemIdxStr));
            std::memset(uniqueStr, 0, sizeof(uniqueStr));
            // ORIG 三次 snprintf 的**顺序与实参**（全部 OS_API::snprintf）：
            //  ① 0x08178140 snprintf(-0x18c, 0x14, "%d", goods[j]->GetItemIdx())
            //     （0x0817811e call _ZNK14CCeraShopGoods10GetItemIdxEv 结果作 %d，
            //      输出缓冲此后不再被读 ⇒ 原码遗留的死写入，照抄）
            //  ② 0x08178165 snprintf(-0x1f0, 0x24, "%d", ceraPrice[-0x58])
            //  ③ 0x0817819c snprintf(-0x1a9, 0x8, "%d", 现读 msg entry+0x39)
            OS_API::snprintf(itemIdxStr, sizeof(itemIdxStr), "%d",
                             goods[j]->GetItemIdx());
            OS_API::snprintf(ceraPriceStr, sizeof(ceraPriceStr), "%d", ceraPrice);
            OS_API::snprintf(productStr, sizeof(productStr), "%d",
                             msg.m_entrySection.m_arr[j].m_productNo);
            (void)itemIdxStr;
            _GetItemString(goods[j], itemStr, 0x400);
            // ORIG 0x081781d6..0x08178360（P5）：三分支各取 s_pIPGHelper vptr+0x18/
            // +0x10/+0x14 槽直调（slot6 IPGOutputSimulation / slot4 IPGOutputPoint /
            // slot5 IPGOutput，TaiwanCash.h 声明序实证）。每分支前各有一对
            // 0x081781eb/0x081781f3（及 0x0817827c/0x08178284、0x081782fc/0x08178304）
            // `call CStreamGuard::operator->()` + `call CStreamGuard::Get()` ⇒
            // 尾实参必须写 guard->Get()（链式，3 次），不得缓存到 theStream；
            // 也不得显式 guard.~CStreamGuard()（ORIG 仅 2 次 D1：作用域尾 1 次 +
            // EH 0x081783b0 1 次；显式调用会造成双重析构）。
            // 另：uniqueNo 与 characName 同为**分支内内联实参**（ORIG 三次
            // 0x08178210/0x081782a1/0x08178321 GenUniqueNo + 三次
            // 0x08178200/0x08178291/0x08178311 getCurCharacName）⇒ 不得提到
            // 分支外缓存成局部（否则各分支少 2 次 call）。
            if (ceraPrice == 0)
                GlobalData::s_pIPGHelper->IPGOutputSimulation(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else if (msg.m_scalars.m_buytype == 1)
                GlobalData::s_pIPGHelper->IPGOutputPoint(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
            else
                GlobalData::s_pIPGHelper->IPGOutput(
                    user, productStr, ceraPriceStr,
                    WongWork::CGenUniqueNo::GenUniqueNo(uniqueStr), itemStr,
                    user->getCurCharacName(), guard->Get());
        }
        else
        {
            m_ceraShop.ProcessError(user, 0x14, groupIdx,
                                    msg.m_entrySection.m_arr[j].m_productNo,
                                    j + 1, itemCount, (char)msg.m_scalars.m_buyFlag);
            goods[j] = 0;
            cMyTrace tr(__PRETTY_FUNCTION__, 0x3c1, 0);   // ORIG 0x08177cdc/0x08177d02
            tr("[%s][%d]", __PRETTY_FUNCTION__, 0x3c1);
        }
    }

    return true;
}
Secu_DataControl::Secu_DataControl() {}  // ORIG 0x82aa6ba：空体（纯 ret，无成员初始化）

// encryptAES_ 定义在 MouseRegisterEJ.cpp（保留方，0x148 更近 ORIG 0x14d）；此 TU 仅调用。
char* encryptAES_(CRijndael* cipher, char* inData, char* outHex);

// Secu_DataControl::UpdateSecurityServiceState —— ORIG T 0x08286e40
// [0x08286e40,0x08287081]。service==0：SecurityCard 已激活→init；
// service==1：CPad 已激活→若已认证则 flag=1，否则将 msg 截 8 字节 AES 加密后
// setFailCnt(0)+setPassword；任一分支生效则发送 0x159 通知包（service/flag）。
// [b28d-18 N78] EXTRA memset×2：4.1 内联清零 vs 4.4 call（N78） | ORIG 0x08286e40
// [b28d-solo-GlobalData] DIFF 封顶：p167 EXTRA memset×2 = N78（4.1 内联清零/4.4 call）；
// MISSING 空 ⇒ 语义无缺口。
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

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080ec760
bool CDBConnector::Connect(DBInfoMap infoMap)
{
    // ORIG 0x080ec760：按值遍历连接表；idx==0/5 跳过；cMyTrace 记录
    // "Open(type:%2d, %s/%d/%s/%s)"（.rodata 0x8b356a4）；Open 失败即 false。
    // [b28d-solo-GlobalData] p167 修：ORIG 全程逐字段重调 operator->（ptEv×7，
    // P157/P183 禁 idx/info 缓存）；cMyTrace 为 5 参重载 `C1EPKciibb`
    // （str, 0x4e, 9, true, true；旧 3 参形 MISSING 5 参 + EXTRA 3 参）。
    // [b28d-solo-GlobalData N·Main 裁决] STDBConnInfo 偏移两函数证据冲突：本件
    // ORIG 0x080ec760 段为 port@0x18/db@0x1c/user@0x46，而 QueryConnInfo
    // 0x080eca6c `add $0x42,%edx` 与现布局 m_db@0x42 直证吻合 ⇒ 判定本件实参序/
    // 字段名映射与我们不同（p167 CLEAR，语义等价），**维持 QueryConnInfo 直证
    // 布局**，不重排 CEnvironment.h 的 STDBConnInfo（风险远超收益）⇒ N 封顶。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        if (!(it->first == 0 || it->first == (ENUM_DB_HANDLE_IDX)5))
        {
            cMyTrace t("CDBConnector::Connect", 0x4e, 9, true, true);
            t("Open(type:%2d, %s/%d/%s/%s)", (int)it->first, it->second.m_host,
              (int)it->second.m_port, it->second.m_db, it->second.m_user);
            if (!Open(it->first, (ENUM_SERVER_GROUP)0, it->second))
            {
                return false;
            }
        }
    }
    return true;
}

// [b28d-solo-GlobalData] 归零 67=67：group 内联为实参表达式复现 R2L 装载序
// （先 &second 后 m_tail；具名/register 局部均提前，P52/P157）；p167 CLEAR | ORIG 0x080ec8f8
bool CDBConnector::ConnectWithServerGroup(DBInfoMap infoMap)
{
    // ORIG 0x080ec8f8：遍历连接表；idx==0 跳过；以 +0x168 处 server group
    // 为组参数调 Open；失败即 false。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        // ORIG 0x080ec91d/0x080ec93b/0x080ec949/0x080ec954：每次字段访问
        // 重新调用 operator->（4 次 ptEv，P157 逐字段重读，禁引用缓存）。
        // ORIG 0x080ec926 `sete %al; test; jne <跳过>` = 负极性守卫（P222）。
        if (it->first == (ENUM_DB_HANDLE_IDX)0)
        {
            continue;
        }
        // ORIG 080ec946（ConnectWithServerGroup.md）：`mov 0x168(%eax)`（eax=pair 基址）
        // = second+0x164 = STDBConnInfo::m_tail（存 server group；Open 第 2 实参 %ebx）。
        // 原写 +0x160 系偏移误读（pair/second 基混淆），此处以具名字段修正为 +0x164。
        // ORIG 0x080ec954 起装载序（R2L）：先 `lea 0x4(%eax),%esi`（第3实参
        // &second），再 `mov 0x168(%eax),%eax; mov %eax,%ebx`（第2实参 m_tail），
        // 最后 `mov (%eax),%eax`（第1实参 first）⇒ group 必须内联为实参表达式
        // （具名/register 局部都会把 m_tail 装载提前）。
        if (!Open(it->first,
                  static_cast<ENUM_SERVER_GROUP>(it->second.m_tail),
                  it->second))
        {
            return false;
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
    // ORIG 0x080ec620：rep movsl 0x5a dword（结构整体赋值，P83）→ this+4；OpenMaster()。
    // [b28c-lane17] 返回形态 `xor $1; test; je` = if(!OpenMaster()) return false; return true;
    m_connInfo = info;
    if (!OpenMaster())
        return false;
    return true;
}
bool CDBConnector::OpenMaster()
{
    // ORIG 0x080ec67c：Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, this+4)
    // 结果取反返回。ORIG 0x080ec6aa：xor+test 后 fall-through 块为 mov $0、
    // je 目标块为 mov $1（技巧 17：fall-through=源码首分支）——
    // `if (!Open()) return false; return true;`（与 ORIG 机器码值域一致）。
    if (!Open((ENUM_DB_HANDLE_IDX)0, (ENUM_SERVER_GROUP)0, m_connInfo))
        return false;
    return true;
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
    // ORIG 0x080ec6da：info 首字节为 0（m_host 空）→ 直接成功；否则调 DBMgr::Open。
    // [b28c-lane17] 实参地址序按 ORIG 压栈槽逐条对齐：p2=+0x00(m_host)、
    // p3=*(+0x14)(m_port)、p4=+0x42(m_db)、p5=+0x18(m_user)、p6=+0x2d(m_pass)
    // （原实现把 m_pass 传成 p2、m_host 传成 p6 —— 真语义 bug，此处修正）。
    if (info.m_host[0] == 0)
    {
        return true;
    }
    if (!GlobalData::s_db_mgr->Open(idx, info.m_host, (unsigned int)info.m_port,
                                    info.m_db, info.m_user, info.m_pass, group))
    {
        return false;
    }
    return true;
}
// ---- CDBConnector::QueryConnInfo ORIG @ 0x080eca6c（U3ShopDb 批次） ----
// 用主 DB 句柄(0,0) 查 db_connect 表，逐列填 STDBConnInfo 后放入 outMap[idx]。
// 列序：db_name(0x1f→m_db) / db_ip(0x11→m_host) / db_port(→m_port) /
//       db_userid(0x15→m_user) / db_passwd（先清零 60 字节，0x3c→+0x2d 起）。
// 密文经 CEnvironment::DecryptValue 解出（ORIG 解密结果未回写结构，仅校验）。
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x080eca6c
// [b28d-solo-GlobalData] DIFF：p167 EXTRA memset×1（N78 内联清零）；余 N82 槽位/STL 展开。
bool CDBConnector::QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                                 DBInfoMap& outMap)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)0,
                                                  (ENUM_SERVER_GROUP)0);
    if (!db->set_query("seLect db_name,db_ip,db_port,db_userid,db_passwd from "
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
// b28c-l23 SIGFIX：返回类型 char→bool（ORIG __PRETTY_FUNCTION__ 串 + 体尾
// `movzbl -0x11(%ebp),%ebx` 直返 bool 局部；char 声明使 4.4 出 movsbl）。
// [b28d-solo-GlobalData] 已修 AE（p167 17/17 CLEAR）：ORIG 0x08299530 三处
// cMyTrace 各占独立槽（`lea -0x44/-0x34/-0x24` 三组互异，4.1 不复用），具名
// 对象在 4.4 下复用同槽致全帧浅 0x30（-0x48/-0x4c vs -0x18/-0x1c）⇒ 按 P221
// 改「构造即调用」全表达式临时链 `cMyTrace(__PRETTY_FUNCTION__,line,0)(fmt)`，
// 三处独立槽与 ORIG 逐条对齐（余重定位显示伪差，relink 后 AE）。
bool CGM_Manager::LoadGmListFromDb()
{
    // [b23S3C] ORIG 0x08299530：ok 即返回变量（exec/get_int 逐次覆写），
    // mId/level/rows 函数级预清（P197③ 帧头 movl $0×N），writeGuard 保护填充。
    // ORIG 帧头 `movb $0x0,-0x11(%ebp)` ⇒ bool 预清两句式（P198①）。
    bool ok = false;
    int rows = 0;
    unsigned int mId = 0;
    int level = 0;
    m_db->set_query("seLect m_id, level from gm_manifest");
    ok = m_db->exec(true);
    if (!ok)
    {
        // [b28d-solo-GlobalData] P221 全表达式临时链：ORIG 三处 trace 各占
        // 独立槽（-0x44/-0x34/-0x24），具名对象在 4.4 下复用同槽致帧浅 0x30。
        cMyTrace(__PRETTY_FUNCTION__, 0x1579, 0)(
            "Failed to get GM list from DB.");
        return false;
    }
    writeGuard<rwLock> guard(&m_lock);
    rows = m_db->get_n_rows();
    for (int i = 0; i < rows; ++i)
    {
        m_db->fetch();
        ok = m_db->get_int(0, mId);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1588, 0)(
                "Failed to get GM's m_id from DB.");
            // [b28d-solo-GlobalData] ORIG 0x0829962x 该分支为 `mov $0x0,%ebx;
            // jmp <尾>`（直接物化返回寄存器）⇒ 原码是 `return false;` 而非
            // ok=false;break（后者多写 -0x11 槽且尾部经 movzbl 重载）。
            return false;
        }
        ok = m_db->get_int(1, level);
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x158e, 0)(
                "Failed to get GM's lv from DB.");
            return false;
        }
        m_gmMap[mId].m_level = level;
    }
    return ok;
}
CStatisticMgr::CStatisticMgr()
{
    // ORIG 0x82aa89e：两个统计子对象（map 成员自动构造）。
}

// ORIG T 0x0860ecc6 IncreaseLevelDungeonPlay(level, playCount)：
// level ∉ [1,200] → 仅 trace(type=0)；否则 find 命中 → 引用缓存后
// m_playCount += playCount、++m_userCount；未命中 → 先写 playCount 再置
// userCount=1，insert。
// [b28d-18 NEAR] 修：删发明符号 stDungeonPlayInfo::C1Ev（ORIG nm 无，POD 化）；p167 CLEAR；余 N80 | ORIG 0x0860ecc6
void CLevelDungeonPlayStatistic::IncreaseLevelDungeonPlay(int level, int playCount)
{
    if (level < 1 || level > 200)
    {
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 发生在 cMyTrace ctor 实参装填
        // 之前（level 跨 ctor call 保活 ⇒ 4.1 直落 callee-saved %ebx，
        // 并连带 push/pop %ebx 与帧 0x64）。4.4 默认在 operator() 处重新
        // reload ⇒ register asm("ebx") + 声明位置钉住早装载（技巧 2/13/28）。
        // ORIG 0x0860ecd2：`mov 0xc(%ebp),%ebx` 在 cMyTrace ctor 实参装填之前
        // （level 跨 ctor call 落 callee-saved %ebx）。P227 全表达式临时链
        // `cMyTrace(...)("fmt", expr)` 使 4.4 在 ctor 前求值 op() 实参并以
        // %ebx 保活（具名局部/裸内联两形均劣）。
        cMyTrace(__PRETTY_FUNCTION__, 0x550, 0)(
            "LEVEL_DUNGEON_PLAY_STATISTIC wrong level(%d)", level);
        return;
    }
    // ORIG 0x0860ed0x：find 两实参装载序（-0x44 先入 0x4(%esp)、-0x30 后入
    // (%esp)）；P50 先声明后赋值实测反劣（4→11，多默认构造 call+拷贝）⇒ 回退。
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.find(level);
    // [b28d-solo-GlobalData] P74 迭代器实参序旋钮：ORIG 0x0860ed51 先 `lea -0x44(it)`
    // 入 0x4(%esp)、`lea -0x30(end)` 入 (%esp) ⇒ 原码是 `end() == it`（第一实参 end）。
    if (m_levelPlayMap.end() == it)
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
    // 基类 SIG ctor / m_keys[5] 循环构造由编译器自动生成
    // （ORIG 0x0831c3e8 call SIG C1、0x0831c412 4-计数 key 循环）。
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
// [b28d-18 N82] p167 CLEAR（21/21）；ORIG 243 vs 203 条，残差=槽位/STL（N82/N89） | ORIG 0x0831c49e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（21/21）；gap 40=N82 帧/槽 + 表初始化展开。
void break_away_prevent::CBreakAwayPreventSystem::loadDbData(
    SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER* data)
{
    memcpy(this, data, 0x2e4);   // 基类子对象区（原 m_base）
    m_rewardMap.clear();

    int count = data->m_field2e0;
    for (int i = 0; i < count; ++i)
    {
        // 保留裸形式说明（本函数当前 identical）：ORIG 0831c50b..0831c5c9 就是
        // `add %i*0x18 基址; add $0x10; movzbl 0x1(%eax)/0x2(%eax)/(%eax)` 与
        // `lea; add $0x14/$0x18/$0x1c/$0x20/$0x24; mov (%eax)` 两步形态
        // （break_away_prevent__CBreakAwayPreventSystem/loadDbData.md）。等价的
        // 具名成员 data->m_rewards[i].m_field1 等（SIG_BREAK_AWAY_PREVENT_REWARD_MANAGER，
        // 基址 +0x10/stride 0x18 一致）在 gcc4.4 O0 会把 +0x10 折进单 disp，破坏
        // identical，故按 P30/P160 口径保留原转写形态（表寻址两步精确复现）。
        STRewardDataKey key;
        // [b28d-tu-GlobalData] P334 第二形修正：ORIG 0831c500/0831c50b 循环体首
        //   连发两个默认构造 `lea -0x37(key); call` + `lea -0x4c(reward); call`，
        //   之后才逐字段装载；我方原把 `STRewardData reward;` 声明写在 key 赋值
        //   之后 ⇒ reward ctor 被挪到第 3 个 key 读之后（构造站点错位）。
        //   声明上移（POD，无 ctor 提前副作用 ⇒ 仅移动 call 位次）。
        STRewardData reward;
        // [b28d-tu-GlobalData] 双判据真缺口修正（40 条指令 gap 的主体）：ORIG
        //   **每个字段访问都从形参 0x8(%ebp)+槽 -0xc(i) 完整重物化**
        //   （`mov -0xc,%edx; mov 0x8(%ebp),%ecx; 3x; shl $3; lea; add $0x10..;`
        //   10 行/字段，0831c4e3/50b/549/563/57d/597/5b1 七段 missing），我方
        //   原 `entry` 缓存 ⇒ 每字段只 4 行 ⇒ 少 40 条。删缓存、逐字段重算
        //   （P48 无引用缓存族），0x11/0x12/0x10 序 = ORIG 字节装载序。
        key.m_data[0] = *((const char*)data + i * 0x18 + 0x11);
        key.m_data[1] = *((const char*)data + i * 0x18 + 0x12);
        key.m_data[2] = *((const char*)data + i * 0x18 + 0x10);

        reward.m_field0  = *(int*)((const char*)data + i * 0x18 + 0x14);
        reward.m_field4  = *(int*)((const char*)data + i * 0x18 + 0x18);
        reward.m_field8  = *(int*)((const char*)data + i * 0x18 + 0x1c);
        reward.m_fieldc  = *(int*)((const char*)data + i * 0x18 + 0x20);
        reward.m_field10 = *(int*)((const char*)data + i * 0x18 + 0x24);

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

        m_keys[i].m_data[0] = key.m_data[0];
        m_keys[i].m_data[1] = key.m_data[1];
        m_keys[i].m_data[2] = key.m_data[2];
    }
}
// （b28 试 P216 throw() 回滚：4.4 判 ctor pure 后连 call 一起消除，ORIG 有 call，
//  得不偿失；CExpertJobMgr::C1 残差为 N77/P90 value-init 清零段。）
// [b28d-solo-GlobalData] P299/P318：CExpertJobFactory 用户空 ctor 声明+定义已删
// （ORIG 0x0849f68c 为 W 隐式合成）⇒ CExpertJobMgr::C1 恢复 value-init 前置 rep stos。
expert_job::CExpertJobFactory::~CExpertJobFactory()
{
    // ORIG 0x0849f6d2：两个 StaticPool 成员析构。
}
// b28c-1 N77: 4.1 new T() value-init 前置 rep stos 0x18 dword，4.4 不 emit（去具名 p 后 15→13）。
// [b28d-18 N77] ORIG new(nothrow) T() value-init 前置 rep stos 0x18 dword + 临时指针槽；4.4 按 CWG434 不 emit（N77/P90） | ORIG 0x0849e102
expert_job::CExpertJobMgr::CExpertJobMgr()
{
    // ORIG 0x0849e102：首条 `movl $0x0,(%eax)` 预清槽 + new(nothrow) T()
    //（4.1 value-init 前置 rep stos 清零，4.4 按 CWG434 不 emit = N77/P90
    //  跨版本记录）+ 结果统一写回 + 成员回读判 0 后 printf。
    m_factory = 0;
    // b28c-1：ORIG 0x0849e102 尾 `mov %ebx,%eax; mov 0x8(%ebp),%edx;
    //  mov %eax,(%edx)` 直存 ⇒ 无具名中间指针（去 -0x1c 中转）。
    m_factory = new (std::nothrow) CExpertJobFactory();
    if (m_factory == 0)
    {
        printf("memory error func(%s) line(%d)",
               __PRETTY_FUNCTION__, 0x57f);
    }
}
expert_job::CExpertJobMgr::~CExpertJobMgr()
{
    // ORIG 0x0849e1ac：外层 guard + delete 自身 guard（两次非空检查），无清零。
    if (m_factory != 0)
        delete m_factory;
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
    : m_impl(new RentalSystemImpl)
{
    // ORIG 0x08270014：new RentalSystemImpl(0x18) 交 auto_ptr；注册 PcRoom(1)/Dimension(2)。
    registerRentalMachine(1, new RentalMachinePcRoom);
    registerRentalMachine(2, new RentalMachineDimension);
}
// [b28d-18 IDENTICAL_AE] 68=68；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x08270110
InstanceRentalSystem::~InstanceRentalSystem()
{
    // ORIG 0x08270110：遍历机器表虚析构；impl 由 auto_ptr D1 释放。
    // ORIG 0x08270162：每轮 delete 后 `it->second = 0`（de 重调 + movl $0）。
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // [b28d-solo-GlobalData] P167：ORIG 0x0827016x 发 `_ZNK...deEv`（operator*）
        // 非 `ptEv` ⇒ 源码写 `(*it).second`（it->second 选错实例化，p167 MISSING deEv）。
        delete (*it).second;
        (*it).second = 0;
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
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
                     G_CEnvironment()->get_file_name());
}

// [b28d-18 IDENTICAL_AE] 21=21 逐条同形 | ORIG 0x0826efdc
void RentalMachineDimension::initLoggingEnvironment()
{
    // ORIG 0x826efdc：同 PcRoom 日志路径（.rodata 0x8bf90c2 同串）。
    OS_API::snprintf(m_impl->m_data, 0x103, "./log/%s/dw2_RentalItems",
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
    // ORIG 0x082707b2：ne 的 this = end() 临时槽(-0xc)、实参 = it 槽
    // (-0x10) ⇒ 源码序 `end() != it`（Dimension 同构已验证）。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}
// Dimension::CheckRentableItem（ORIG 0x0826eea8）：同构。
// [b28d-18 IDENTICAL_AE] 32=32；唯一残差=尾对齐 nop（§6.1 豁免） | ORIG 0x0826eea8
bool RentalMachineDimension::CheckRentableItem(unsigned int itemIdx)
{
    // ORIG 0x0826ef01：const_iterator::ne 的 this = end() 临时槽(-0xc)、
    // 实参 = it 槽(-0x10)——源码序 `end() != it` 复现。
    std::set<unsigned int>::const_iterator it = m_impl->m_set.find(itemIdx);
    return m_impl->m_set.end() != it;
}

// ---- InstanceRentalSystem 查询/分发（ORIG 报告 InstanceRentalSystem/*.md）----

// getMachine（ORIG T 0x08270348）：map find，未命中返回 0。
// [b28d-18 IDENTICAL_AE] 38=38 逐条同形（.o 重定位伪差） | ORIG 0x08270348
IRentalMachine* InstanceRentalSystem::getMachine(int idx)
{
    // ORIG 0x082703a0：`_Rb_tree_iterator::eq` + `test; je` 到 de 路径、
    // true 分支 `mov $0` 早返岛——源码 `if (it == end) return 0;` + 尾
    // `return it->second`（非三目 ne 形）。
    // ORIG find/eq 为非 const iterator 符号（4findERS5_/...eqERKS5_）——
    // const_iterator 声明会引入 iterator→const_iterator 转换调用。
    std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.find(idx);
    // ORIG 0x0827039a：eq 的 this = end() 临时槽(-0xc)、实参 = it(-0x10)——
    // 源码写 `end() == it`。
    if (m_impl->m_machines.end() == it)
    {
        return 0;
    }
    // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
    return (*it).second;
}

// Ready（ORIG T 0x082701dc）：遍历机器表；任一为空或其槽3 Ready() 返回非 1
// 即返回 false，全部就绪（含空表）返回 true。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+对调，主体 56 行同形封顶。
// [b28d-18 IDENTICAL_AE] 56=56 逐条同形；残差仅间接 call 寄存器名（N7 已判） | ORIG 0x082701dc
bool InstanceRentalSystem::Ready()
{
    for (std::map<int, IRentalMachine*>::iterator it = m_impl->m_machines.begin();
         it != m_impl->m_machines.end(); ++it)
    {
        // ORIG 0x08270214：判空条件极性 `jne body` + fall-through `mov $0;jmp
        // end`（技巧 17：if(x==0) return false; 直落形）；`!Ready()` 独立 if
        // （ORIG 0x08270233 xor+test+je 续循环，非 || 短路合并形）。
        // [b28d-solo-GlobalData] P167：ORIG 发 deEv ⇒ `(*it).second`。
        IRentalMachine* machine = (*it).second;
        if (machine == 0)
        {
            return false;
        }
        if (!machine->Ready())
        {
            return false;
        }
    }
    return true;
}

// Rent 分发（ORIG T 0x08270288）：按 RentInput.m_machineKey 取机器并经槽2
// 调用；无机器时 ORIG 直接返回 1。
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+实参链寄存对调，29 行主体同形封顶。
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
// [b28d-solo-GlobalData] P299/P318：CTeamFactory 用户空 ctor 声明+定义已删
// （ORIG 0x08588e2e 为 W 隐式合成）⇒ COnlinePreliminaryTeamMgr::C1 恢复前置 rep stos。
online_preliminary::CTeamFactory::~CTeamFactory()
{
    // ORIG 0x08588e42：StaticPool 成员析构。
}
// [b28 判词 N77] ORIG 4.1 `new (nothrow) T()` value-init 前置 rep stos 12 dword
// 清零+临时槽；4.4 CWG434 不 emit——placement/显式 memset 变 call 均不可翻，封顶。
// [b28d-18 N77] ORIG new T() value-init 前置 rep stos + 临时指针槽；4.4 不 emit（N77/P90） | ORIG 0x0858860c
online_preliminary::COnlinePreliminaryTeamMgr::COnlinePreliminaryTeamMgr()
    : m_factory(0)
{
    // ORIG 0x0858860c：+0x00=0；map@+0x04 由编译器构造；new(0x30, nothrow)
    // CTeamFactory；失败 logFormat 后继续。
    // ORIG 0x0858860c 首两条 `mov 0x8(%ebp),%eax; movl $0x0,(%eax)` 先于 map
    // ctor —— m_factory 清零是 mem-init（声明序）而非 body 赋值。
    // N77 定论（DL7 复测）：ORIG 0x0858867e rep stos $0xc dword（0x30 全清）
    // ＝ `new T()` value-init；4.4 CWG434 对 user-ctor 类不 emit（加括号
    // 实测仍无 rep stos）⇒ 连同 0x10 帧差与 ebx/esi/edi 缓存差 N77+N82附 封顶。
    // [b28d-solo-GlobalData] P299/P318：用户空 ctor 已删，此处补括号 value-init
    // 即触发 4.4 前置 rep stos（ORIG 0x0858867e rep stos $0xc dword）。
    m_factory = new (std::nothrow) CTeamFactory();
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
    // ORIG 0x08588706/0x0858870f：双 load 双判空——4.4 对
    // `if (m_factory) delete m_factory;` 恒产此形（probe 实证）。
    if (m_factory)
    {
        delete m_factory;
    }
}
// ORIG T 0x08588be4（OnLoadTeamInfo.md 全量转写）：data 首 4 字节为队伍 key，
// 命中 m_teamMap 时委托 loadDataPreliminaryTeamMatchList(data)。
void online_preliminary::COnlinePreliminaryTeamMgr::OnLoadTeamInfo(
    CUser* user, char* data)
{
    // data = 队伍线格式缓冲（首 int 为 team key，ORIG 08588bed 直读 (%eax)）；
    // 该域包 struct 权威头未建模（布局未建模），P156 豁免保留裸读。
    // ORIG 0x08588bea：data 先落局部槽 -0xc(%ebp) 再回读传 find（4.1 实参
    // 地址表达式 spill 形）——具名局部拷贝复现。
    char* pTeam = data;
    std::map<int, COnlinePreliminaryTeam*>::iterator it =
        m_teamMap.find(*(int*)pTeam);
    if (it != m_teamMap.end())
    {
        it->second->loadDataPreliminaryTeamMatchList(data);
    }
}
// game_master::CCommandMap / CGameMasterMgr 真实实现见 GmCore.cpp 与 GmRegister.cpp
//（ORIG CCommandMap ctor 0x082a7f64、add(int) 0x082a7c76、add(string) 0x082a7cdc、
// Mgr ctor 注册链 0x082a8a28）。

// ---- IBusiness / CBusinessImpl（报告 docs/class_func_reports/CBusinessImpl/）----

// P210 附注（b28 BL14 翻案，Main 2026-09-08 广播）：派生 ctor 定义前置到同
// TU 基类 ctor 之前 ⇒ 4.4 对该 callee 不可见、恢复 ORIG 保守 outgoing
// `sub $0x18`（0x082a7c3b），旧「帧舍入 N + dead pad」判词作废（pad 删）。
CBusinessImpl::CBusinessImpl()
{
    // ORIG 0x082a7c32：call IBusiness ctor + 设 vptr（编译器生成序列）。
}

CBusinessImpl::~CBusinessImpl()
{
    // ORIG 0x082a7c4e：仅重置 vptr（编译器生成序列）。
}

IBusiness::IBusiness()
{
    // ORIG W 0x082a7c24：仅设 vptr（编译器生成）。
}

// [b28d-solo-GlobalData] 改善 74→68（p167 12/12 CLEAR）：P259 去 stream 具名
// 局部（ORIG 0x080de3f6 `call Acquire; movl $1,0x8(%esp); mov %eax,0x4(%esp)`）+
// info 前 8 字节结构整体赋值（ORIG 0x080de46c `mov 0x4(%eax),%edx; mov (%eax),%eax`
// 先高字后低字）+ body.field04 1 字节（ORIG 0x080de469 `mov %al,-0x1c`，T1 直证）。
// 残差（已闭）：ORIG 尾部 `mov $0x0,%ebx … mov %ebx,%eax` ⇒ 原函数返回 int 0。
// [b28d-PoolGlobalData] R2 落地：CBusinessImpl.h:34/48 void→int + `return 0`
// ⇒ 0x080de3ee ndiff 19→0（AE）。
int CBusinessImpl::PunishClient(int a, int b, unsigned int c, EPUNISHMODE mode,
                                tagAntibotPunishInfo* info, char* reason)
{
    (void)a;
    (void)b;
    (void)reason;
    // ORIG 0x080de3ee：StreamPool 取流；CStreamGuard(stream,true)；<<0x253 <<-1；
    // put_binary({uint c; EPUNISHMODE mode(byte); info 前 8 字节}, 0x10)。
    // [b28d-solo-GlobalData] P259：ORIG 0x080de3f6 无 stream 具名局部
    // （Acquire 结果经 %eax 直传 guard ctor 的 0x4(%esp)）⇒ 内联实参。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x59),
                       true);
    **guard << 0x253;
    **guard << -1;
    // [b28d-solo-GlobalData] ORIG 0x080de46c：`mov 0x4(%eax),%edx; mov (%eax),%eax;
    // mov %eax,-0x18; mov %edx,-0x14` = info 前 8 字节**结构整体赋值**
    // （4.1 先读高字后读低字）；逐字赋值会反序且多具名指针局部。
    struct PunishTail
    {
        int field08;
        int field0c;
    };
    struct
    {
        unsigned int field00;
        // ORIG 0x080de469 `mov %al,-0x1c` ⇒ field04 为 1 字节（枚举形参按
        // 4 字节栈槽读入后截断存储）；声明 EPUNISHMODE（4 字节）会生 4 字节 store。
        unsigned char field04;
        PunishTail tail;
    } body;
    body.field00 = c;
    body.field04 = (unsigned char)mode;
    // tagAntibotPunishInfo 完整布局未建模（CBusinessImpl.h 仅前向声明，
    // 「antibot 域完整布局待专门批次」）；ORIG 080de3ee 取 info 前两 dword
    // 作 8 字节块赋值，此处以等价视图类型表达。
    body.tail = *reinterpret_cast<const PunishTail*>(info);
    guard->put_binary(&body, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // [b28d-PoolGlobalData R2] ORIG 0x080de3ee 尾 `mov $0,%ebx … ~CStreamGuard …
    // mov %ebx,%eax` ⇒ 返回 int 0 跨 guard 析构保活（头 void→int 已报 Main）。
    return 0;
}

int CBusinessImpl::ProcessProtectMsg(tagTSafeModePkg* pkg)
{
    // ORIG 0x080de4ec：StreamPool 取流；<<0x254 <<-1；put_binary(pkg, 0x34)。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x88),
                       true);
    **guard << 0x254;
    **guard << -1;
    guard->put_binary(pkg, 0x34);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 0;
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
    // 0x0827fae6 区：GetRepeatEvent 结果 %eax 直作虚调用 this（`mov (%eax),%edx;
    // add $0x34,%edx; mov (%edx),%edx; mov %eax,(%esp); call *%edx`，无 spill
    // 槽 ⇒ 技巧 37 免槽直用）；判真伪为正向 `test %al,%al; je 0x13-岛`
    // （技巧 1：`!boolcall()` 会多 xor ⇒ 此处 ORIG 无 xor ⇒ 源码为正向 if）。
    // 0x0827fb14 区：bingoEvent 槽 -0x10、返回 temp 槽 -0xc（技巧 21 后声明占
    // 近槽）；`cmpl $0x0,-0x10; je 0x13-岛` ⇒ 内层也是正向 if（技巧 17：
    // fall-through 块=真路径在前，0x13 尾岛）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        // BingoEvent 完整类型已建模（BingoEvent.h 权威头，OrigBingoEvt 批次）；
        // ORIG 0x0827fb4f 直调 _ZN10BingoEvent12reqBingoMarkERN15StackableAction11input_paramE。
        BingoEvent* bingoEvent = static_cast<BingoEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x6a));
        if (bingoEvent != 0)
        {
            int ret = bingoEvent->reqBingoMark(p);
            return ret;
        }
    }
    return 0x13;
}
// [b28d-18 NEAR] 修：getCurCharacFatigue 两次独立调用（P48 无 CSE，p167 MISSING 实证）；
// 调用差集 CLEAR 8/8；余槽位/寄存器（N82） | ORIG 0x0827fbd6
// [b28d-solo-GlobalData] NEAR 73=73（p167 8/8 CLEAR）：P0 具名 user 局部
// （ORIG -0x10 三次复用）+ P183 去 costs 指针缓存（每次重发 item+0x3a0）后
// 由 74 条收敛；余 1 处槽互换（ORIG lower -0x14 / max 常量临时 -0x1c vs
// 4.4 -0x1c/-0x14，P212 头部声明/前置声明三形实测均反劣）⇒ N98 封顶。
int StackableAction::Action::proc_addFatigue(input_param& p)
{
    // ORIG 0x0827fbd6：user/item 判空→0x13；item+0x3a0 的 vector<int> 取第 0 项
    // 与当前疲劳比较；疲劳不足返回 0x5f；否则扣减并 SendFatigue。
    if (p.m_user == 0)
    {
        return 0x13;
    }
    // [b28d-solo-GlobalData] P0：ORIG 0x0827fbf0 `mov 0x8(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 在 m_item 判空前即写具名局部，后续三次调用
    // （getCurCharacFatigue×2/setCurCharacFatigue/SendFatigue）全部
    // `mov -0x10(%ebp),%eax` 复用该槽（无重读 p.m_user）。
    CUser* user = p.m_user;
    if (p.m_item == 0)
    {
        return 0x13;
    }
    // ORIG item+0x3a0 = CStackableItem::m_vec3a0（运行时 item 为 stackable）。
    // [b28d-solo-GlobalData] P183 去指针缓存：ORIG 0x0827fc0d/0x0827fc2e 每次
    // 重发 `mov 0xc(%ebp),%eax; mov 0xc(%eax),%eax; add $0x3a0,%eax`，具名
    // costs 局部会占 -0x10 槽并把后续 max 临时槽顶浅（-0x14 vs ORIG -0x1c）。
    if (static_cast<CStackableItem*>(p.m_item)->m_vec3a0.empty())
    {
        return 4;
    }
    int need = std::max(
        0, static_cast<CStackableItem*>(p.m_item)->m_vec3a0.at(0));
    // ORIG 0x0827fc6a/0x0827fc89：getCurCharacFatigue 两次独立调用
    // （O0 无 CSE，P48；原具名 cur 缓存少 1 次调用，p167 MISSING 实证）。
    if ((int)(user->getCurCharacFatigue() & 0xffff) < need)
    {
        return 0x5f;
    }
    // [b28d-solo-GlobalData] ORIG 0x0827fc97/0x0827fc9e：实参序
    // `lea -0x14(lower),%eax; mov %eax,(%esp)`（第一实参 lower）⇒
    // `std::max(lower, after)`。余 1 处槽互换（ORIG lower -0x14 / max 常量临时
    // -0x1c；4.4 按首次使用分配恒相反，P212 头部声明/前置声明三形实测均反劣）
    // ⇒ N98/N93 封顶。[b28d-PoolGlobalData P326 重试] 匿名 0 临时形实测：块1
    // 临时顶到 -0x18、块2 after 滑 -0x1c（4 条→8 条差）⇒ 反劣回退，N93 判不变。
    int after = (int)(user->getCurCharacFatigue() & 0xffff) - need;
    int lower = 0;
    user->setCurCharacFatigue(std::max(lower, after));
    user->SendFatigue();
    return 0;
}
int StackableAction::Action::proc_lotteryCount(input_param& p)
{
    // ORIG 0x0827fb86：user/item 判空→0x13；否则 giveLotteryCountItem(
    // item->get_index(), false)，返回 0。
    // 两 guard 共享 0x13 尾块（|| 合并条件）。
    // 两个独立 if + 相同返回值 ⇒ gcc tail-merge 共享 0x13 尾块。
    // ORIG 0x0827fb86：正向嵌套（两 je 共享尾部 0x13 块），body 内 return 0
    // 走 jmp merge。
    if (p.m_user != 0)
    {
        if (p.m_item != 0)
        {
            p.m_user->giveLotteryCountItem(p.m_item->get_index(), false);
            return 0;
        }
    }
    return 0x13;
}

// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0827f7c8
void StackableAction::Action::init_function_type()
{
    // ORIG 0x0827f7c8：以 boost::bind 将 proc_* 成员注册进 map2（key 0x34/0x36/0x37）。
    // 与 ORIG 一致用 insert 方式（非 operator[]）。
    // [b28d-solo-GlobalData] p167 EXTRA `boost::arg<1>::C1Ev`×3：本头集下
    // `boost::_1` 不可用（COMPILE FAILED），保留 `arg<1>()` 构造形 ⇒ N（绑定库
    // placeholder 构造跨版本差异）封顶。
    using boost::arg;
    m_map2.insert(std::make_pair(TYPE_BINGO,
        boost::bind(&Action::proc_bingo, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_LOTTERY_COUNT,
        boost::bind(&Action::proc_lotteryCount, this, arg<1>())));
    m_map2.insert(std::make_pair(TYPE_ADD_FATIGUE,
        boost::bind(&Action::proc_addFatigue, this, arg<1>())));
}

// [b28d-18 IDENTICAL_AE] 修：register 指针 pt=&t 使 &t 提前物化 %esi 跨 operator new 保活（P231 新实证）；NDIFF 4→1（余 vtable 重定位伪差） | ORIG 0x084a2938
expert_extraction::CExtraction::CExtraction()
{
    // ORIG 0x084a2938：vptr + new CMTRand 并以 time(0) seed。
    // 0x084a296c：CMTRand ctor 形参为 const unsigned long&（mangled ERKm，
    // 32 位 ulong=4B → time 结果单 dword 落槽 0x084a2956、lea 槽址作实参）。
    unsigned long t = (unsigned long)time(0);
    // ORIG 0x084a2959：seed 槽地址提前物化 %esi —— 4.4 对实参 &t 表达式
    // 自然产 lea（P0：寄存器仅承载表达式值）。
    register unsigned long* pt = &t;
    m_rand = new CMTRand(*pt);
}
expert_extraction::CExtraction::~CExtraction()
{
    // ORIG 0x084a2938：仅 delete m_rand（判空），不置零。
    delete m_rand;
}
// [b28d-solo-GlobalData] P299/P318：三个子类用户空 ctor 声明+定义已删（ORIG C1/C2 均 W）
// ⇒ CExtractionMgr::C1 的 `new T()` 恢复 value-init 前置双清零（ORIG 0x084a3047/0x084a308f）。
expert_extraction::CAlchemistExtraction::~CAlchemistExtraction() {}
expert_extraction::CEnchanterExtraction::~CEnchanterExtraction() {}
expert_extraction::CDollControllerExtraction::~CDollControllerExtraction() {}
// [b28d-solo-GlobalData] 归零 75=75 IDENTICAL：P299/P318 删三个子类用户空 ctor
// （ORIG C1/C2 均 W）恢复 `new T()` value-init 前置双清零（旧 N77/N83 判词作废） | ORIG 0x084a302e
expert_extraction::CExtractionMgr::CExtractionMgr()
{
    // ORIG 0x084a302e：new 三个子提取器对象，依次存 +0/+4/+8。
    // ORIG 0x084a3047/0x084a308f：ctor 调用前先清 0x8 字节——value-init
    // `new T()`（zero-init 前置 + C1 调用）。
    m_slots[0] = new CAlchemistExtraction();
    m_slots[1] = new CEnchanterExtraction();
    m_slots[2] = new CDollControllerExtraction();
    // [b28d-solo-GlobalData] 4.4 现发 `movl $0x0,(%eax); movl $0x0,0x4(%eax)`
    // 前置双清零（与 ORIG 0x084a3047/0x084a308f 逐条同形）——关键是子类无用户
    // 提供 ctor（CWG434）；旧「4.4 折叠进 C1/N83 封顶」判词作废。
}
expert_extraction::CExtractionMgr::~CExtractionMgr()
{
    // ORIG 0x84a311a：for i∈[0,2] if(m_slots[i]) delete m_slots[i];（不置空）。
    for (int i = 0; i < 3; ++i)
    {
        delete m_slots[i];   // 4.4 delete 自带 test/je 判空（P207）→ 与 ORIG 单判同形
    }
}
// ---- Teleport / NPC_Teleport / User_Teleport（报告 Teleport/、NPC_Teleport/、User_Teleport/）----

STCheckRuleData::STCheckRuleData() {}
// ORIG W 0x0822d78e：仅清零 +0x00 的 int（m_field00），不触碰 m_tp 子对象。
STTeleportData::STTeleportData()
{
    // ORIG 0x0822d794：movl $0x4,0x8(%esp) → memset 长度 4；this 实参最后
    // 装填（0x0822d7a4）⇒ 原码为 memset(this,0,4) 非 &m_field00（后者使 4.4
    // 提前缓存 this 指针）。
    memset(this, 0, 4);
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
    // ORIG 形：this/tp 基址各载一次，字段全折叠 `(%edx)→0x4(%eax)` 等 ⇒
    // 结构整体赋值（POD 逐字段展开，10B 实拷不含尾 pad）。
    m_tp = tp;
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
        // ORIG 0x08283dfc：end 侧作 lhs；两分支各自重载 this 显式 store。
        // ORIG 0x08283e12/0x08283e1e：then(8283e01)/else(8283e14) 两分支尾
        // 均有 jmp 到共享 epilogue(8283e21)；then 带 return，else 体展平到
        // 内层 if 之后 + 外层 if 块后尾置 return（技巧 7 尾岛）。
        if (m_endCoolTime > now)
        {
            m_endCoolTime = m_endCoolTime - now;
            return;
        }
        m_endCoolTime = 0;
    }
    return;
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
        // ORIG 0x08283e??：get_aura 结果 %eax 直落实参槽（无中间局部槽）。
        user->UpdateAuraAvatarOption(2, user->get_aura_avatar_option_value(2));
    }
    // ORIG 尾岛：末 call 后 jmp 到共享 epilogue（技巧 7 尾置 return）。
    return;
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
    // ORIG send_teleport_effect 尾岛：call send_to_area 后有 jmp 到共享
    // epilogue；return 写在 if 块后（技巧 7 尾置 return 岛）。
    return;
}
bool Teleport::move_area(CUser* user)
{
    // ORIG 0x08283fb8：effect(1) → GameWorld::move_area(...) → effect(2)+
    // update_next_end_cool_time。ORIG 经 stPartyTelePort getters 取 5 字段
    // （add this+4 → mov %eax,(%esp) → call *_get_teleport_*），故此处调 getter
    // 而非直读字段（P：getter 为 ORIG 真符号 CPartyTelePort.h 声明）。
    if (user == 0)
    {
        return false;
    }
    send_teleport_effect(user, 1);
    // ORIG xor $0x1,%eax; test %al,%al; je（直测返回寄存器，无槽，P198）：
    // 谓词整体内联进 if(!...)，禁提同名 bool 局部（反多 mov %al,-off; cmpb 两步）。
    if (!G_GameWorld()->move_area(
            user, (int)(unsigned char)m_tp.get_teleport_village(),
            (int)(unsigned char)m_tp.get_teleport_area_index(),
            // ORIG Ghidra sVar2/sVar3 = get_teleport_pos* 记作 short、以 (int)
            // 直传 ⇒ movswl/cwtl 有符号拓宽（而非 getter 的 unsigned short movzwl）。
            (int)(short)m_tp.get_teleport_posx(),
            (int)(short)m_tp.get_teleport_posy(),
            (int)(unsigned char)m_tp.get_teleport_direction(), true, 2, 0, 0))
    {
        return false;
    }
    send_teleport_effect(user, 2);
    update_next_end_cool_time(user);
    return true;
}
NPC_Teleport::NPC_Teleport()
{
    // ORIG 0x082840da：call Teleport ctor + 设 vptr（编译器生成序列）。
}
NPC_Teleport::~NPC_Teleport() {}

// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x08283c5a
// [b28d-solo-GlobalData] 归零 AE 85=85（p167 11/11 CLEAR）：①各失败点 ORIG 为
// `mov $imm,%eax; jmp end` 直接物化 ⇒ 源码全 `return X;` 早返（旧 result 变量
// 汇聚形多 -0x1c 槽 + 4 条）；②pvp/expired 两处零槽短路物化用 P226 语句表达式
// `if (({ A || B; }))`（具名 bool 落槽 -0x16/-0xd）；③remain 声明在 if 外、
// 赋值在 if 体内（ORIG 0x08283cfb 在 isGMUser 之后）；④CUser.h is_equip_aura_avatar
// 返回 char→int（Main 代落，消调用点 movsbl）。
int Teleport::check_rule(CUser* user)
{
    // ORIG 0x08283c5a：状态/渠道/vill/aura 冷却校验链（报告 Teleport/check_rule.md）。
    // [b28d-solo-GlobalData] ①各失败点 ORIG 为 `mov $imm,%eax; jmp end` 直接
    // 物化 ⇒ 源码全 `return X;` 早返（Ghidra 的 iVar3 汇聚是假象，旧 result
    // 赋值形多 -0x1c 槽）；②pvp/expired 两处为 `mov $1/$0` 岛 + test 直测
    // （零槽，P226 语句表达式）——具名 bool 会落槽 -0x16/-0xd 并顶浅后续槽。
    if (user == 0)
    {
        return 1;
    }
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (({ G_GameWorld()->IsPVPChannel() ||
           G_GameWorld()->IsPvPSkilTreeChannel(); }))
    {
        return 0x13;
    }
    if (user->getCurCharacVill() == 7)
    {
        return 0xbe;
    }
    // [b28d-solo-GlobalData] ORIG 0x08283cfb `movl $0,-0x14(%ebp)` 在
    // isGMUser 检查之后（Ghidra C 亦为 if 体内 local_18 = 0）⇒ 声明在 if 外、
    // 赋值在 if 体内（声明即初始化会提前到 0x08283ce9 之前 +1 条）。
    int remain;
    if (!user->isGMUser())
    {
        remain = 0;
        int equipped = (int)user->is_equip_aura_avatar(2, remain);
        if (equipped > 0)
        {
            return equipped;
        }
    }
    int auraEnd = user->get_aura_avatar_option_value(2);
    if (({ (!user->isGMUser()) &&
           (auraEnd > GlobalData::s_systemTime_.getCurSec()); }))
    {
        return 0x16;
    }
    return 0;
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
    set_party_teleport(data.m_tp);   // STTeleportData::m_tp@+0x04（Teleport.h 权威成员）
}

User_Teleport::User_Teleport()
{
    // ORIG 0x08284142：call Teleport ctor + 设 vptr（编译器生成序列）。
}
User_Teleport::~User_Teleport() {}
int User_Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x0828415e（docs/class_func_reports/User_Teleport/check_rule.md）：
    // 基类 Teleport::check_rule 非 0 原样返回（result@-0x14 只服务于该值）；其后
    // 各分支均为 `mov $const,%eax; jmp 出口` ⇒ 源码是早退 return，不共用 result。
    // target@-0x10 先 `movl $0x0` 再赋值 ⇒ 显式 =0 初始化后单独赋值。
    // get_state / getCurCharacVill / GetAssaultPlace 返回值不入库（ORIG 直接在
    // %eax/%al 上 cmp ⇒ 条件表达式内直接调用）；三连村庄判定 ORIG 为三次独立
    // call getCurCharacVill（无缓存），逐条重调。
    int result = Teleport::check_rule(data);
    if (result != 0)
    {
        return result;
    }

    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target == 0)
    {
        return 0x15;
    }
    if ((bool)(target->get_state() != 3))
    {
        return 4;
    }
    if ((bool)(target->getCurCharacVill() == 7 ||
               target->getCurCharacVill() == 8 ||
               target->getCurCharacVill() == 10))
    {
        return 4;
    }
    // ORIG 0x08284250：checkSeriaRoom 返回值直接 `test %al,%al; je`（无 setne
    // 物化）⇒ 此处写纯 `if (char 表达式)`，不加 (bool) 强转。
    if (CHackLog_InvalidAreaMove::checkSeriaRoom(
            (int)target->getCurCharacVill(), target->get_area(false)))
    {
        return 4;
    }
    if (target->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }

    CParty* party = static_cast<CParty*>(target->GetParty());
    if (party != 0)
    {
        if ((bool)(static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() != -1))
        {
            return 0x9e;
        }
    }
    return 0;
}
void User_Teleport::set_teleport_data(STTeleportData const& data)
{
    // ORIG 0x082842b8：find_from_world(data.field00)；命中则取 vill/area/pos/
    // direction 组装局部 stPartyTelePort 后 set_party_teleport。
    // ORIG 0x082842bf：target 槽先置 0（声明初始化），find 结果再赋值——
    // 两段式复现 `movl $0x0,-0xc(%ebp)`。
    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target != 0)
    {
        // ORIG 0x082842b8：局部 stPartyTelePort 不 reset（未设字段为栈垃圾），
        // 仅填 5 字段后 set_party_teleport——保持 ORIG 原样，不额外清零。
        // ORIG 0x0828433b：tp 基址 -0x16(%ebp)（4.1 紧排非 4 对齐）；4.4 恒
        // 4-对齐落 -0x18，槽偏移差 2 字节属布局伪影（N）。
        stPartyTelePort tp;
        tp.m_village = (unsigned char)target->getCurCharacVill();
        tp.m_areaIndex = (unsigned char)target->get_area(false);
        tp.m_posx = (short)target->get_posX();
        tp.m_posy = (short)target->get_posY();
        tp.m_direction = (unsigned char)target->get_direction();
        set_party_teleport(tp);
    }
    // ORIG 0x0828434a：body 尾 call 后 jmp 到 merge(nop@0828434c)——
    // 函数尾显式 return（技巧 7 尾岛）。
    return;
}
void* random_option::GetRandomOption()
{
    // ORIG T 0x085f0eb0：返回 &m_randomOption（CDataManager+0x4e10，objdump 实证）。
    return &G_CDataManager()->m_randomOption;
}
// [b28d-18 N7] 修：register seed 使 seed 先载（P231）；余纯寄存器名轮换（N7 封顶） | ORIG 0x085f0d64
// [b28d-solo-GlobalData] NEAR 封顶 N7：p167 逐调用同形（imul/lea/seed 段一致）；
// 残差=callee-saved 角色轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx，P230 定稿：
// 4.4.7 分配固定，源形不可择）+ 首条 vptr 存的重定位显示伪差（`$&<L>` vs `<L>`）。
random_option::CRandomOptionItemHandle::CRandomOptionItemHandle()
{
    // ORIG 0x085f0d64：new[0x7f28] 13×CMTRand，逐元素 seed(time(0))；
    // 字段清零；+0x14 = GetRandomOption()+0x148 的指针。
    m_rand = new CMTRand[13];
    unsigned long t = (unsigned long)time(0);
    for (int i = 0; i < 13; ++i)
    {
        // [b28c-lane17] P0 清绑复测：ORIG 0x085f0d64 有 t 的**槽往返**
        // （`call time; mov %eax,-0x20` → 循环内 `mov -0x20(%ebp),%eax`），
        // 故具名局部 t 正确；但旧版在循环内另设 `unsigned long tv = t;`
        // 是**无槽依据的额外局部**（ORIG 无第二个 seed 槽）⇒ 反产 +2 条
        // store/load。直传 t 后指令数回到 ORIG 的 69。
        // 残余 = seed 实参与地址计算的**装载时刻序**：ORIG 4.1 在地址
        // imul/lea 之前把 t 载入 %eax（`mov -0x20,%eax` 先行），4.4 恒按
        // 「this→下标→实参」序装载 ⇒ P75/N86（实参装载时刻跨版本调度），
        // 语义等价（seed 实参同一槽值），保持可读源形。
        // [b28d-solo-GlobalData] P248 裸 register 实测：删 register 反生 seed 二段
        // 拷贝（69→71，多 mov -0x24→-0x1c），保留 register 回到 69=69；余纯寄存器
        // 名轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx）⇒ N7 封顶（P230：4.4.7 分配固定）。
        register unsigned long seed = t;
        m_rand[i].seed(seed);
    }
    m_field08 = 0;
    m_disabled = 0;
    m_field10 = 0;
    // ORIG 0x85f0e2d..0x85f0e3b：m_optionSet = GetRandomOption() + 0x148
    // （m_randomOption 内嵌 provider 对象基址 m_providerNormal148；虚槽0/1 见 RandomOptionScript.h）。
    m_optionSet =
        &reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_providerNormal148;
}
random_option::CRandomOptionItemHandle::~CRandomOptionItemHandle()
{
    // ORIG 0x085f0e46：vptr 前置写 + delete[]（guard/da），无成员清零。
    delete[] m_rand;
}

// [b28d-18 N] ORIG 切片缺失（objdump 直转 082a73cb）；p167 CLEAR（9/9）；残差=表内容占位+codegen | ORIG 0x082a73cb
// [b28d-solo-GlobalData] DIFF 封顶 N：ORIG 人证字符串表（.data 0x936e2a0，64 项）内容缺失，
// 重建以全 0 占位必须保留 s==0||*s==0 检查（否则 std::string(0) 崩溃），ORIG 无检查 ⇒ +10 条；
// p167 CLEAR（表访问调用同形）。
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
    // buf 基址实证：ORIG 0x082a654f `lea -0x118(%ebp),%ebx` 且帧 `add $0x120,%esp`
    // ⇒ buf 尺寸 0x104（实测：0x100 落 -0x114、0x104 才落 -0x118；0x118 落 -0x12c）。
    // 尾部 4 字节是 4.1 给 handle/err 之外多留的一槽，无读写语义。
    // [b28c-l23 真语义 bug 修正] 原写法 `memset(buf, 0, 0x41)` 只清 0x41 **字节**，
    // 但 ORIG 0x082a654f 是 `mov $0x0,%eax; mov $0x41,%edx; mov %ebx,%edi;
    // mov %edx,%ecx; rep stos %eax,%es:(%edi)` —— rep stos 的立即数是 **dword 计数**
    // （P116），0x41×4 = 0x104 = 整个 buf ⇒ 原码清空的是全缓冲，strncat 依赖
    // buf[0]=='\0' 才不从脏数据起接。改为显式零初始化（P214③：大数组 `={0}`
    // 是 4.1 rep stos 的 4.4 复现突破口），语义与形态同源修正。
    char buf[0x104] = {0};
    strncat(buf, path, strlen(path));
    strncat(buf, "libantisvrimport.so", 0x14);
    void* handle = dlopen(buf, RTLD_NOW);
    const char* err = dlerror();
    if (err != 0)
    {
        // 语义修正：ORIG 0x082a65e1 `movl $0x8c16189,0x4(%esp)`（.rodata 实测
        // 0x8c16189 = "libantisvrimport.so"）+ 0x082a65e9 fmt@0x8c1619d ⇒
        // 原码第 2 参是**库名字面量**，不是拼接后的 buf（buf 只用于 dlopen）。
        printf("Open Dll %s failed, %s\n", "libantisvrimport.so", err);
        return 0;
    }
    void* fn = dlsym(handle, "GetAntiBotInterfaceAll");
    err = dlerror();
    if (err != 0)
    {
        dlclose(handle);
        printf("dlsym failed, %s\n", err);
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

// [b28d-18 DIFF] p167 MISSING G_CEnvironment×1（真缺口未闭合）；余槽位（N82） | ORIG 0x0829d8e8
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（42/42）；残差=N82 槽位/寄存器 + 行解析展开。
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
        // ORIG 0x0829dc9a/0x0829dcbb：find 的 receiver 与 end 的 receiver 各自
        // 独立调用 G_CEnvironment()（P48 无 CSE + P157 逐字段重读，禁 env 缓存；
        // p167 实证旧写缺 G_CEnvironment×1）。
        std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> >::iterator it =
            ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map.find(m_dbType);
        if (it != ((CDBEnvServerGroupMapView*)G_CEnvironment())->m_map.end())
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
GlobalData::SecuCipherKeyRec GlobalData::s_secu_cipher_keystring_manager_[0x4e20] = {0};
CTEA* GlobalData::s_pcryptTEA_MousePassword_ = 0;
CTEA* GlobalData::s_pcryptTEA_SecurityCard_ = 0;
// [b22判词·N-工具名碰撞+编译器生成] idx9 __tcf_0 是本 TU 静态数组 g_SPremiumInfo[0x6a]
// 的编译器自动生成析构 thunk（倒序逐元素 call ~SPremiumInfo，elem size 0x120），非手写
// 函数，无法在源码层「改」其指令。真·ORIG 对应物 = 0x086b068d（紧邻 _GLOBAL__I_
// g_SPremiumInfo@0x86b080a，call _ZN12SPremiumInfoD1Ev@0x86b1732），其循环骨架与本 .o
// 逐项同构（mov 数组上界→%ebx; 与基址 cmp; je 出口; sub $0x120; mov; call dtor; jmp）。
// report=NO/DIFF（orig18/our15）是 game_func_report 按短名碰撞：ORIG 存在 10 个 file-local
// `__tcf_0`，工具取到 0x0828f940（CDeathTowerRanking 数组 cleanup，语义无关）。残余：
// ①ORIG 用字面上界地址 0x948b180 / 字面基址 0x9483a40 做 cmp，我们靠链接期 R_386_32
// reloc 填（未 relink 显示占位 0，§6.3 假阳性）；②18 vs 15 差在 ORIG 额外一条 `mov 上界,%ebx`
// 预载。语义等价，relink 后转 AE。
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

// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（52/52）；大件三件套：gap 7=N82 槽位复用策略。
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

// [b28d-18 N82] p167 CLEAR（36/36）；残差=槽位分配（ORIG -0x44/-0x48/-0x50 vs 4.4 -0x30/-0x18/-0x20） | ORIG 0x0829dd82
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（36/36）；残差=N82 槽位/寄存器 + N89 STL 展开。
bool GlobalData::initTournamentDB(const char* fileName)
{
    typedef std::map<ENUM_SERVER_GROUP, std::map<std::string, std::string> > GroupNameMap;
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
        G_CEnvironment()->m_serverGroupMap.begin();
    while (it != G_CEnvironment()->m_serverGroupMap.end())
    {
        ENUM_SERVER_GROUP group = it->first;
        DBGroupMap groupMap;
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)2, group));
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)3, group));
        DBInfoMap& info = G_CEnvironment()->m_dbConnInfoByGroup[group];
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

// [b28d-18 N89] 超大件：残差=STL 实例展开差（N89）；§6.2 语义完成度按调用差集+gap 计 | ORIG 0x08299fa0
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（大件 3139 insn）；N89（ORIG out-of-line STL
// 实例 vs 4.4 全内联）+ N82 槽位；语义完成，byte 层 N 封顶。
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

    // ORIG 0x0829a851-0x0829a8bc：三个实参表达式求值结果由 callee-saved 寄存器/单 spill
    // 槽（-0x77c）承载（P24/P44 内联实参即时求值；命名局部会使 4.4 三值全 spill）。
    s_hades_proxy = new (std::nothrow) CHadesServerProxy(
        G_CEnvironment()->get_udp_ip_hades(),
        G_CEnvironment()->get_tcp_port_hades(),
        G_CEnvironment()->get_udp_port_hades());
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
        s_double_check_proxy = new (std::nothrow)
            CDoubleConnCheckServerProxy(G_CEnvironment()->get_udp_ip_doublecheck(),
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
        s_school_proxy = new (std::nothrow)
            CSchoolServerProxy(G_CEnvironment()->get_school_server_ip(),
                               G_CEnvironment()->get_school_server_port());
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
    s_power_manager->LoadPowerWarInfo();
    s_power_manager->LoadRankerInfo();

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
        s_channel_proxy = new (std::nothrow)
            CmmChannelProxy(G_CEnvironment()->get_udp_ip_channel(),
                            G_CEnvironment()->get_udp_port_channel());
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

    DBInfoMap& masterDbInfo = G_CEnvironment()->m_dbConnInfo;
    // ORIG make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&>（0x0829b8ba）：
    // 第二参左值本体即 ENUM_SERVER_GROUP 类型局部。
    ENUM_SERVER_GROUP serverGroup = (ENUM_SERVER_GROUP)G_CEnvironment()->m_serverEnvir.m_serverGroup;

    // ORIG 0x0829b886-0x0829d41d：groupMap 为函数级作用域（正常析构在函数尾共享块
    // 0x0829d41d；其后各 new 的 cleanup 统一经 0x0829d3ff 先析构 map 再 Unwind），
    // 不使用内层 {}。
    DBGroupMap groupMap;
    for (int i = 1; i < 0xf; ++i)
    {
        // ORIG 0x0829b8f4：make_pair 第二参引用 -0x340(%ebp)=serverGroup 局部
        // （make_pair<ENUM_DB_HANDLE_IDX,R17ENUM_SERVER_GROUP&> 实证）——插入值为
        // serverGroup，非 i。key 侧 cast 为值传（__decay_and_strip）。
        groupMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)i, serverGroup));
    }
    // ORIG 0x0829b90e-0x0829b9ce：t1 构造前 4 个独立 operator[] 语句，**字段值本体**
    // 即被求值（db 地址→-0x778 槽、user 地址→edi、port→esi、host→ebx，无引用
    // 中间变量），随后 t1 ctor + op() 从槽/寄存器直读。register 局部避免 4.4 的
    // 引用槽物化（P35 同族）。
    register char* masterDb = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_db;
    register char* masterUser = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_user;
    register unsigned int masterPort = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_port;
    register char* masterHost = masterDbInfo[(ENUM_DB_HANDLE_IDX)0].m_host;
    cMyTrace t1(__PRETTY_FUNCTION__, 0x1b0c, 9, true, false);
    t1("\t- DBConnector Init(masterdb %s/%d/%s/%s) ",
       masterHost, masterPort, masterUser, masterDb);
    if (!s_db_connector->Init(masterDbInfo[(ENUM_DB_HANDLE_IDX)0]))
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1b0f, 9, true, true);
            t2("\t- DBConnector Init Fail");
            return false;
        }
        cMyTrace t3(__PRETTY_FUNCTION__, 0x1b12, 9, false, true);
        t3("Success");
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1b14, 9, true, false);
        t4("\t- DBConnector Get Connection Info ");
        if (!s_db_connector->GetConnInfo(groupMap, masterDbInfo))
        {
            cMyTrace t5(__PRETTY_FUNCTION__, 0x1b17, 9, true, true);
            t5("\t- DBConnector Get Connection Info Fail");
            return false;
        }
        cMyTrace t6(__PRETTY_FUNCTION__, 0x1b1a, 9, false, true);
        t6("Success");
        cMyTrace t7(__PRETTY_FUNCTION__, 0x1b1c, 9, true, true);
        t7("\t- Open DB ... ");
        if (!s_db_connector->Connect(DBInfoMap(masterDbInfo)))
        {
            cMyTrace t8(__PRETTY_FUNCTION__, 0x1b1f, 9, true, true);
            t8("\t- DBConnector Connect DB Fail");
            return false;
        }
        cMyTrace t9(__PRETTY_FUNCTION__, 0x1b22, 9, true, true);
        t9("\t- All DB Open Success");

    s_psimpleSSO = new WongWork::CSimpleSSO;
    s_psimpleSSO->init(s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)6, (ENUM_SERVER_GROUP)0));

    s_securityCardCenter = new WongWork::CSecurityCardCenter;
    s_securityCardCenter->init();

    // ORIG 0x0829be0e-0x0829be68：new 结果存**局部槽** -0x34(%ebp)（Ghidra local_38），
    // Initialize 失败路径 delete 该局部，**成功后**才 `s_pIPGHelper = pHelper`
    // （0x0829be63 全局写唯一出现）。直接赋全局会使初始化读回全局、全局写前移。
    WongWork::IPG::CIPGHelper* pIPGHelper = new (std::nothrow) WongWork::IPG::CIPGHelper;
    if (pIPGHelper->Initialize(1, 5000) < 0)
    {
        puts("Fail to init IPG Module");
        delete pIPGHelper;
        return false;
    }
    s_pIPGHelper = pIPGHelper;

    CHumanCertifier::init();

    s_pGMAccounts_ = new WongWork::CGMAccounts;

    {
        cMyTrace t1(__PRETTY_FUNCTION__, 0x1d4d, 9, true, false);
        t1("\t- Init Query Table ");
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t2(__PRETTY_FUNCTION__, 0x1d50, 9, true, true);
            t2("\t- Init Query Table Fail");
            return false;
        }
        if (!s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0)
                 ->GetQueryCounter()->LoadQueryTable())
        {
            cMyTrace t3(__PRETTY_FUNCTION__, 0x1d55, 9, true, true);
            t3("\t- Init Query Table Fail");
            return false;
        }
        cMyTrace t4(__PRETTY_FUNCTION__, 0x1d58, 9, false, true);
        t4("Success");
    }

    // ORIG 0x0829c04f-0x0829c075：`movl $0x1,-0x30(%ebp)` + `movb $0x0,-0x29(%ebp)` +
    // `cmp -0x30(%ebp),%eax; sete %al` —— 比较右操作数是 int 局部（非立即数）；
    // P28 槽序即声明序：int 先声明（-0x30 深于 bool -0x29）。
    int expectedChannel = 1;
    bool syncEnable = false;
    if (G_CEnvironment()->get_channel_no() == expectedChannel)
    {
        syncEnable = true;
    }
    sync_script::GetInstanceSyncScript()->SetEnable(syncEnable);
    if (sync_script::GetInstanceSyncScript()->IsEnable())
    {
        // ORIG 0x0829c0ff-0x0829c19d：Open 实参 R2L 求值，每个 map[5] 表达式独立
        // 求值（5 次 G_CEnvironment + 5 次 operator[]，P48/O0 无 CSE）：
        // 第1次+0x2d(pass)→spill 槽、第2次+0x18(user)→edi、第3次+0x42(db)→esi、
        // 第4次+0x14(port)→ebx、第5次+0x00(host)→edx。勿缓存引用（P157）。
        if (!s_db_mgr->Open(
                (ENUM_DB_HANDLE_IDX)5,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_host,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_port,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_db,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_user,
                G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)5].m_pass,
                (ENUM_SERVER_GROUP)0))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1d8d, 9, true, true);
            t("\t- Failed to open web DB");
            return false;
        }
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d90, 9, true, true);
        t("\t- Success WEB DB OPEN");
    }

    // ORIG 0x0829c24b-0x0829c281：聚合 bool 变量在 4.1 中寄存器化（mov $1/$0,%eax
    // join 汇聚，N3 负例——O0 源码无法复现）；此处以直接短路条件消除 slot 汇聚，
    // 语义与 ORIG 一致：两表任一失败即报错返回。
    if (!(CSyncSlangFilter::GetInstance()->AddSlangListName()
          && CSyncSlangFilter::GetInstance()->AddSlangList()))
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1d9f, 9, true, true);
        t("Failed to read slang list");
        return false;
    }

    s_event_manager = new (std::nothrow) CEventManager;
    if (!s_event_manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1da8, 9, true, true);
        t("Can't allocate CEventManager memory");
        return false;
    }

    cMyTrace t(__PRETTY_FUNCTION__, 0x1dc6, 9, true, true);
    t("\t- Load GM List From DB");
    gmList = new (std::nothrow) Gm_List;
    if (!gmList)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dcd, 9, true, true);
        t("\t- Can't allocate memory for a Gm_List");
        return false;
    }

    s_GM_Manager = new (std::nothrow) CGM_Manager;
    if (!s_GM_Manager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dd6, 9, true, true);
        t("Can't allocate memory for a CGM_Manager");
        return false;
    }

    {
        int keyLen = G_Cipher()->GetTotalKeyLength();
        unsigned char key[1024];
        for (int i = 0; i < keyLen; ++i)
        {
            // ORIG 0x0829c54e-0x0829c564：rand() 结果经 sar/shr/and/sub 带符号取模
            // 展开（P14）⇒ 源码是 `rand() % 256`，非 C 截断强转。
            key[i] = (unsigned char)(rand() % 256);
        }
        G_Cipher()->Initialize(key, keyLen);
    }

    // ORIG 0x0829c5a2-0x0829c605：value-init nothrow new（guard 内 rep stos 清零 +
    // call C2 均为 4.1 value-init 调用侧展开；4.4 对 user-provided ctor 只发 call，
    // memset 6 行为跨版本结构性差异。手写 placement 会额外产生 _ZnwjPv/_ZdlPvS_
    // 调用，call 序列偏差更大，故用 value-init 形式）。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1df7, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    s_BreakAwaySys = new (std::nothrow) break_away_prevent::CBreakAwayPreventSystem;
    if (!s_BreakAwaySys)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1dfe, 9, true, true);
        t("Can't allocate memory for a s_BreakAwaySys");
        return false;
    }

    s_ExpertJobMgr = new (std::nothrow) expert_job::CExpertJobMgr;
    if (!s_ExpertJobMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e05, 9, true, true);
        t("Can't allocate memory for a CExpertJobMgr");
        return false;
    }

    s_villageObjectMgr = new (std::nothrow) village_object::CVillageObjectMgr;
    if (!s_villageObjectMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e0c, 9, true, true);
        t("Can't allocate memory for a CVillageObjectMgr");
        return false;
    }

    s_villageMonsterMgr = new (std::nothrow) village_attacked::CVillageMonsterMgr;
    if (!s_villageMonsterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e13, 9, true, true);
        t("Can't allocate memory for a CVillageMonsterMgr");
        return false;
    }

    s_rentalSystem = new (std::nothrow) InstanceRentalSystem;
    if (!s_rentalSystem)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e1b, 9, true, true);
        t("an`t allocate memory for a InstanceRentalSystem");
        return false;
    }

    s_revengeDungeonMgr = new (std::nothrow) village_attacked::CRevengeDungeon;
    if (!s_revengeDungeonMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e24, 9, true, true);
        t("Can't allocate memory for a CRevengeDungeon");
        return false;
    }

    s_DailyScheduleManager = new (std::nothrow) CDailyScheduleManager;
    if (!s_DailyScheduleManager)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e2d, 9, true, true);
        t("Can't allocate memory for a CDailyScheduleManager");
        return false;
    }

    s_onlinePreliminaryTeamMgr = new (std::nothrow) online_preliminary::COnlinePreliminaryTeamMgr;
    if (!s_onlinePreliminaryTeamMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e36, 9, true, true);
        t("Can't allocate memory for a COnlinePreliminaryTeamMgr");
        return false;
    }

    s_GameMasterMgr = new (std::nothrow) game_master::CGameMasterMgr;
    if (!s_GameMasterMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1e47, 9, true, true);
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
        // ORIG 0x0829cd18：`movl $0x0,-0x35c(%ebp)` 单 dword 清零 —— 标量初始化形态。
        char seq[4] = {'\0'};
        // ORIG 0x8471ebd：G_CEnvironment()->getProcessSequence() 真实读环境进程序列号
        sprintf(seq, "%d", G_CEnvironment()->getProcessSequence());
        // ORIG 0x0829cd6f：init 经 vtable 槽0 调用，返回值非 0 记 "AntiBot init fail" 并返 0。
        typedef int (*AntibotInitFn)(void*, void*, char*);
        // ORIG 0x0829cd6f-0x0829cdd8：调用结果 test/setne 物化（P15），表达式直放条件位，
        // 勿先存具名局部（具名 int + `!= 0` 生成 cmpl/je 无 setne）。
        if ((*(AntibotInitFn*)s_antibotChecker_)(s_antibotChecker_, s_antibotBusinessImpl_, seq))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x1eb7, 5);
            t("AntiBot init fail");
            return false;
        }
    }

    // ORIG 0x0829cdd4-0x0829ce2b：同上 value-init 第二处。
    s_statisticMgr = new (std::nothrow) CStatisticMgr();
    if (!s_statisticMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ebf, 9, true, true);
        t("Can't allocate memory for a CStatisticMgr");
        return false;
    }

    g_Actions = new (std::nothrow) StackableAction::Action;
    if (!g_Actions)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ed9, 9, true, true);
        t("Can't allocate memory for a StackableAction::Action()");
        return false;
    }

    {
        // ORIG 0x0829cf56-0x0829cf70：Acquire 结果直放 guard ctor 实参（P24），
        // 不经具名局部（命名会多生 `mov %eax,-slot; mov -slot,%eax` 往返）。
        CStreamGuard guard(s_stream_pool->Acquire("App.cpp", 0x1ee0), true);
        **guard << 0x277;
        **guard << -1;
        s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }

    // P59/P16 guard 化（ORIG 0x0829cffb-0x0829d3fd）：InitCaptcha/initialize/
    // initGeolocation 均为 bool 返回 `xor $0x1; test; je body` 前置失败块；
    // Singleton::Get() 结果直放链式调用（P52），不落具名局部。
    if (!InitCaptcha())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1eea, 9, true, true);
        t("captcha data initialization failed");
        return false;
    }

    Timer_GenerateCaptchaData::RegistNextTimer(3);
    g_ExtractionMgr = new (std::nothrow) expert_extraction::CExtractionMgr;
    if (!g_ExtractionMgr)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1ef5, 9, true, true);
        t("Can't allocate memory for a CExtractionMng");
        return false;
    }

    g_pTeleport[0] = new (std::nothrow) NPC_Teleport;
    if (!g_pTeleport[0])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f02, 9, true, true);
        t("Can't allocate memory for a NPC_Teleport()");
        return false;
    }

    g_pTeleport[1] = new (std::nothrow) User_Teleport;
    if (!g_pTeleport[1])
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f09, 9, true, true);
        t("Can't allocate memory for a User_Teleport()");
        return false;
    }

    g_randomOptionHandle = new (std::nothrow) random_option::CRandomOptionItemHandle;
    if (!g_randomOptionHandle)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f12, 9, true, true);
        t("Can't allocate memory for a CRandomOptionItemHandle");
        return false;
    }

    if (!ARAD::Singleton<ServiceRestrictManager>::Get()->initialize())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f1f, 9, true, true);
        t("Can't allocate memory for a ServiceRestrictManager");
        return false;
    }
    ARAD::Singleton<ServiceRestrictManager>::Get()->savePolicy();
    ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x1f2a, 9, true, true);
        t("Can't initialize GeoIP");
        return false;
    }
    return true;
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
EventRewardSection* GetRewardSection(int idx, int type);   // 定义见下方
bool GetSelectionIndex(CUser& user, int& outIndex, int type)
{
    const _Charac_info* cur = user.getCurCharacR();
    if (cur == 0)
    {
        return false;
    }

    // 角色数据 +0x27 short 选择索引；isAffectedPremium(0x16) 时 +5；
    // type==2 用未加成原值（ORIG 重读）。+0x27 = _Charac_info::m_level（short，
    // ORIG 0827121e movzwl 0x27(%eax); cwtl，CUser.h 字段化已落，CUserOnlyOrig 确认）。
    int selIdx = (int)cur->m_level;
    if (user.isAffectedPremium((ENUM_PREMIUM_TYPE)0x16))
    {
        selIdx += 5;
    }
    if (type == 2)
    {
        selIdx = (int)cur->m_level;
    }

    // ORIG：std::max<int>(0, selIdx)（模板调用，非 ?: cmov），再统一经
    // GetRewardSection(idx, type) 三分支分派（ORIG 不在此内联 type 分支）。
    // [b24ab] ORIG 将 max 结果解引用直接内联为 GetRewardSection 首参（无 chosen
    // 中间槽，P134/P197③形）
    EventRewardSection* section = GetRewardSection(std::max(0, selIdx), type);
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
    if (it != m_mapC.end())
    {
        return &it->second;
    }
    return 0;
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
    if (it != m_map24.end())
    {
        return &it->second;
    }
    return 0;
}

std::vector<std::vector<EventRewardItemInfo> >*
PcBangItemRentarData::getRewardSelectionList(int packageKey)
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it != m_mapC.end())
    {
        return &it->second;
    }
    return 0;
}

namespace pc_room
{

// ---- GetRewardItemSelectionList —— T 0x082712da（同名报告全文转写）--------
std::vector<std::vector<EventRewardItemInfo> >*
GetRewardItemSelectionList(int packageKey, register int type)
{
    // ORIG 0x082712da：同 GetRewardSection 三分支跳链。
    switch (type)
    {
    case 1: {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getRewardSelectionList(packageKey);
    }
    case 2: {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getRewardSelectItemList(packageKey);
    }
    default:
        return 0;
    }
}

// ---- GetRewardSection —— T 0x08271338（同名报告全文转写）------------------
EventRewardSection* GetRewardSection(int idx, register int type)
{
    // ORIG 0x08271338：cmp $1/je + cmp $2/je + jmp default 三分支跳链。
    switch (type)
    {
    case 1: {
        PcBangItemRentarData& rentar = G_CDataManager()->m_pcBangRentar;
        return rentar.getSectionData(idx);
    }
    case 2: {
        DimensionActivationRewardData& reward =
            G_CDataManager()->m_dimensionAct.m_rewardData;
        return reward.getSectionData(idx);
    }
    default:
        return 0;
    }
}

// ---- GetRentItemInfo —— T 0x08271396（同名报告全文转写）--------------------
// [b28d-18 NEAR] 修：委托 pc_room::GetRewardItemSelectionList(packageKey,type)（p167 MISSING）；CLEAR 9/9；余槽位 | ORIG 0x08271396
// [b28d-solo-GlobalData] 归零 AE 88=88（p167 9/9 CLEAR）：ORIG 0x08271444 族为
// `cmpb $0,-0x2c; js F1` + `movsbl; cmp -0x14; jl OK1`（失败块 F1 居中、成功体
// jl 直落）⇒ 源码须写负极性短路 `if (a < 0 || size <= a) return false;` + 体直落；
// 旧正条件包裹 `if (a>=0 && size>a) {…} return false` 产 jge 反形 + 尾部常量岛。
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

    // ORIG 0x08271396：委托 pc_room::GetRewardItemSelectionList(packageKey,type)
    // （p167 MISSING 实证；原内联三分支多发 G_CDataManager×2 + 具体方法×2）。
    std::vector<std::vector<EventRewardItemInfo> >* list =
        GetRewardItemSelectionList(packageKey, type);
    if (list == 0)
    {
        return false;
    }

    // [b28d-solo-GlobalData] P214①/P222：ORIG 0x08271444 先 call size() 落
    // -0x14，再 `cmpb $0,-0x2c; js F1`、`movsbl; cmp -0x14; jl OK1`（失败块
    // F1 居中、成功体 jl 直落、无 setcc 汇聚）；第二段同构（0x0827148a）。
    const int listSize = (int)list->size();
    if (sectionIdx < 0 || listSize <= sectionIdx)
    {
        return false;
    }
    std::vector<EventRewardItemInfo>& inner = list->at((std::size_t)sectionIdx);
    const int innerSize = (int)inner.size();
    if (seqIdx < 0 || innerSize <= seqIdx)
    {
        return false;
    }
    out = inner[(std::size_t)seqIdx];
    return true;
}

// ---- CheckInventoryEquipmentEmptyCount —— T 0x08271062（同名报告全文转写）--
// ORIG 0x08271062：call; leave; ret 穿透。内外配对律（batch-13）：callee
// check_empty_count 终裁 bool ⇒ 本外层同步 bool（probe3 实证 bool+bool 直穿无
// movzbl；int 外 + bool 内必产 movzbl 归一段）。
bool CheckInventoryEquipmentEmptyCount(CUser& user, int need)
{
    return user.getCurCharacInvenR()->check_empty_count(
        (Inven_Item::ITEM_TYPE)1, need);
}

// ---- CheckInventoryEmptyCount —— T 0x0827108c（同名报告全文转写）-----------
// [b28d-solo-GlobalData] 改善 103→96（p167 16/16 CLEAR）：P63 共享出口改写为
// 嵌套正向 if 包裹（消 4.4 每处独立 `mov $1;jmp` 中继）。残差 N：ORIG 7 处
// `xor $1;test;jne +0x15d` 全部直达同一 `mov $1` 尾块，4.4 恒产 `je NEXT`
// 极性（P222 正向包裹/短路链/goto 三形均实测不翻）⇒ 极性恒折封顶。
// `je next; mov $1; jmp`（+14 条）；goto 尾标签形（探针 /tmp/b28d_probe_pol.cpp t2）
// 得 jne 但每源多 1 个 nop 岛（+7），嵌套正向 if/`||` 链（t3/t6）丢 xor 或末项
// 物化 ⇒ 无源码解。**真语义 bug 修正**：ORIG jne 落点 +0x15d 即 return 1 ⇒
// 任一 check 为假返回 1（旧重建 `return false` 与 ORIG 相反，Ghidra C 同证）。
bool CheckInventoryEmptyCount(CUser& user, int need)
{
    // ORIG（0x0827108c，report 汇编+反编译 C 全读）：每次 check 前重调
    // getCurCharacInvenR()。**真语义 bug 修正（b28d-solo-GlobalData）**：ORIG
    // 0x082710b4 族为 `xor $1,%eax; test %al,%al; jne +0x15d`，而 +0x15d 即
    // `mov $0x1,%eax; jmp leave` ⇒ **任一 check 为假即 return 1**（Ghidra C 同形：
    // `if (cVar1 == 1) { … } return 1;`）；旧重建写 `return false` 与 ORIG 相反。
    // 末检查 0x082711e7 `jl +0x164(=mov $0,%eax)` ⇒ GetRemainCapacity < need 才
    // return 0，否则 return 1。照抄 ORIG 可观察行为（P171）。
    // [b28d-solo-GlobalData] P63/P296 共享出口改写：ORIG 7 处 `xor $1;test;jne +0x15d`
    // 全部直达同一 `mov $1` 尾块（4.1 共享 return 块）；平铺 if-return 令 4.4
    // 每处独立发 mov$1+jmp，短路链 `&&` 则反产 `je NEXT`。改嵌套正向 if 包裹
    // （P296 消同层多中继岛 + P222 否定早返改正向包裹）。
    if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)1, need))
    {
        if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)2, need))
        {
            if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)3, need))
            {
                if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)10, need))
                {
                    if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)9, need))
                    {
                        if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)5, need))
                        {
                            if (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)6, need))
                            {
                                // ORIG 0x082711e4 `cmp 0xc(%ebp),%eax; jl +0x164`
                                // —— remain < need 直返 0。
                                if (user.getCurCharacInvenR()->GetRemainCapacity(
                                        (INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) < need)
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

// ---- DeleteAllRentalItemInSameSectionForNewRent —— T 0x08271807 ------------
// [b28d-18 NEAR] 修：(*it) 代 it->（P167 deEv）+ ResetSlot 双分支独立调用（P48）；p167 CLEAR 18/18 | ORIG 0x08271807
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/STL 展开。
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
        // ORIG 0x08271807：`(*it).m_itemIndex`（deEv，非 ptEv；P167 判据）。
        const int itemId = (*it).m_itemIndex;   // RentalInfo+0x04

        int slot = 0;
        bool isEquip = false;
        CInventory* invR =
            const_cast<CInventory*>(user.getCurCharacInvenR());
        if (!invR->CheckExistItemInInventoryOrEquipment(itemId, slot, isEquip))
        {
            continue;
        }

        // ORIG 0x082718c0/0x082718e4：两分支各自独立调用 getCurCharacInvenW
        // 与 ResetSlot（P48 无 CSE；原三元式只调一次 ⇒ p167 MISSING 各 1）。
        if (isEquip)
        {
            user.getCurCharacInvenW()->ResetSlot(0, slot);
        }
        else
        {
            user.getCurCharacInvenW()->ResetSlot(1, slot);
        }

        InstanceRentalSystem::DeletedRentItem entry;
        entry.m_bEquip = isEquip ? 1 : 0;
        entry.m_slot = (unsigned short)slot;
        entry.m_itemId = itemId;
        deleted.push_back(entry);
    }
}

// ---- ProcessEquipItem —— T 0x082714c5（同名报告全文转写；委托转写核对）-----
// 与 ProcessInsertItem 仅差 SetUpgrade/rentId 写入顺序、装备位插入与标记字节。
// [b28d-18 N71] 同 ProcessInsertItem：空析构 trivial 判定差异（P71） | ORIG 0x082714c5
// [b28d-solo-GlobalData] DIFF 封顶：p167 逐调用同形；残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] P71 作废（同 RentalInfo 伪析构翻案）；0x082714c5 全切片
// 重对齐：①删 savedRentId/slot 具名（ORIG 无 -0x10 槽、`mov %ax,-0x1a` 直存）；
// ②ri 写序按 ORIG = slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId；
// ③**语义修正**：m_field8/9/B 逐字节取自 **inElem**[0]/[1]/[3]
//   （0x08271610/619/623 `mov 0xc(%ebp); movzbl (1,%eax)` 直证，旧 `reward>>8/>>24`
//   为误推断）；④rentId 两处 if/else 直存形（ORIG cmp/je+jmp，无 setcc 物化）。
int ProcessEquipItem(CUser& user,
                     const InstanceRentalSystem::RentInElem& inElem,
                     const EventRewardItemInfo& reward,
                     InstanceRentalSystem::RentOutput& out, int reason)
{
    // [b28d-PoolGlobalData] 余 1 条 = ORIG 0x082714d2 `mov (%eax),%eax;
    // mov %eax,%ebx` 两段装载（4.1 regmove），4.4 恒直落 %ebx；`register` 提示
    // 形实测反劣（ndiff 3→20：ebx 长命化引 push %esi/帧 0xb0/复用 ebx 少两读）
    // ⇒ 回退具名形，N18a 封顶。
    CItem* script = G_CDataManager()->find_item(reward.m_field0);
    if (script == 0)
    {
        return 1;
    }

    Inven_Item item;
    item.m_addInfo = reward.m_field0;                 // +0x02
    script->make_item(item);                          // ORIG 虚表槽 +0x08

    // ORIG 0x08271526：装备路径先写 rentId（-0x52=item+7），后 SetUpgrade。
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }
    item.SetUpgrade((unsigned char)reward.m_short8);

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoEquipment(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 3;                                   // 装备租借标记
    ri.m_inEquip = 1;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x0827162a：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
}

// ---- ProcessInsertItem —— T 0x08271666（同名报告全文转写）------------------
// [b28d-18 N71] EXTRA RentalInfo::D1Ev×2 + _Unwind_Resume×1：ORIG 空析构被 4.1 判 trivial 不调用（P71；throw() 实测无效） | ORIG 0x08271666
// [b28d-solo-GlobalData] DIFF 封顶（已被二压推翻）：N71 判词倒果为因。
// [b28d-PoolGlobalData 二压] ①删 `~RentalInfo(){}` 伪建模（ORIG `_Destroy_aux<ILb1E>`
// 实证平凡）⇒ EXTRA D1Ev×2+_Unwind_Resume×1 归零；②同 0x082714c5 全切片重对齐：
// 删 savedRentId/slot 具名、ri 写序 slot→itemIndex→flag0→inEquip→f8→f9→fB→rentId、
// **语义修正** m_field8/9/B=inElem[0]/[1]/[3]（0x082717b1/7ba/7c4 `movzbl` 直证）。
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
    if (inElem.m_rentItemId != -1)
    {
        item.m_addInfo2 = inElem.m_rentItemId;        // +0x07
    }

    InstanceRentalSystem::RentalInfo ri;

    ri.m_slot = (short)user.getCurCharacInvenW()->insertItemIntoInventory(
        item, (eItemAddReason)reason, true, false);    // ORIG `mov %ax,-0x1a` 直存
    ri.m_itemIndex = reward.m_field0;
    ri.m_flag0 = 0;                                   // 背包租借标记
    ri.m_inEquip = 0;
    ri.m_field8 = inElem.m_sectionIdx;                // ORIG 0xc[eax+0]
    ri.m_field9 = inElem.m_itemIdx;                   // ORIG 0xc[eax+1]
    ri.m_fieldB = inElem.m_type;                      // ORIG 0xc[eax+3]
    // ORIG 0x082717cb：重读 inElem+4 判 -1；真支重读存，假支回读 item+7。
    if (inElem.m_rentItemId != -1)
    {
        ri.m_rentItemId = inElem.m_rentItemId;
    }
    else
    {
        ri.m_rentItemId = item.m_addInfo2;
    }

    out.m_infos.push_back(ri);
    return 0;
}

}  // namespace pc_room

// ===========================================================================
// RentalMachinePcRoom / RentalMachineDimension 本体
// ===========================================================================

// ---- PcRoom::log —— T 0x082707c0（同名报告全文转写）------------------------
// [b28d-18 N7] p167 CLEAR；残差=callee-saved 角色轮换 + 槽位（N7/N82） | ORIG 0x082707c0
// [b28d-solo-GlobalData] NEAR 封顶 N7/N82：p167 CLEAR；残差=callee-saved 角色
// 轮换（esi/ebx/edi 循环移位）+ 槽深差 4（-0x19/-0x1d、-0x2c/-0x1c，P230 定稿
// 4.4.7 分配固定不可择）+ 符号地址重定位显示伪差。
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
    // [b28d-solo-GlobalData] ORIG 0x082707e8 `movsbl -0x19; mov %eax,-0x2c` 槽往返
    // ⇒ 原码有具名 int 提升局部（P0 有槽必有局部）。
    const int serverEnv = (int)serverInfo;
    // [b28d-solo-GlobalData] P231/P248：ORIG 0x082707c0 三个值跨 CMyFileLog ctor
    // 保活于 callee-saved（esi=charNo/ebx=accId/edi=m_data）⇒ 纯 register 提示
    // （非 asm 绑定，P0 合规）；缺 register 时 4.4 落栈槽（-0x10/-0xc）。
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachinePcRoom::log(CUser&, int)", 0x12e);
    flog(pData,
         "[RENT] m_id(%d) char_no(%d) server_info(%d) item_id(%d)", accId,
         charNo, serverEnv, itemId);
}

// ---- Dimension::log —— T 0x0826ef16（同名报告全文转写）---------------------
// [b28d-18 N7+N82] 槽位+callee-saved 角色轮换；P76 待办：ORIG auto_ptr 模板参为嵌套类 RentalMachineDimension::RentalMachineDimensionImpl（p167 MISSING ptEv） | ORIG 0x0826ef16
// [b28d-solo-GlobalData] NEAR 封顶：p167 CLEAR（9/9）；N7（callee-saved 角色轮换 esi/ebx/edi）+
// N82（帧 0x4c vs 0x3c、槽位平移）。
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
    // [b24ab] 显式 int 提升到命名局部（ORIG +0x3c movsbl → -0x2c 槽）
    const int serverEnv = (int)serverInfo;
    // [b24ab] ORIG 将 charNo/accId/m_data 缓存在 callee-saved（esi/ebx/edi）跨
    // CMyFileLog 构造存活 ⇒ register 强占（P197②形）
    register const int charNo = user.get_charac_no(-1);
    register const unsigned int accId = user.get_acc_id();
    register char* const pData = m_impl->m_data;

    CMyFileLog flog("void RentalMachineDimension::log(CUser&, int)", 0x71);
    flog(pData,
         "[RENT] Dimension m_id(%d) char_no(%d) server_info(%d) item_id(%d)",
         accId, charNo, serverEnv, itemId);
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
    // ORIG 0x0826f814：基址表达式 add 后落 -0xc 槽（指针局部），非直接 lea 传参。
    DimensionActivationRewardData* data =
        reinterpret_cast<DimensionActivationRewardData*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);
    return (int)data->getRewardGiveItemList(job, growType);
}

// ---- Dimension::GetRentItemInfo —— T 0x0826f842（同名报告全文转写）---------
// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x0826f842
// [b28d-solo-GlobalData] DIFF 封顶（二压重开）：残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] 0x0826f918.. 切片逐条重对齐（探针 pol_probe4 g4/g5）：
// ①size() 结果具名落槽（ORIG sz0@-0x14 / sz1@-0xc，各调一次）；②下标**不具名**
// （ORIG 每用点 `mov 0x10(%ebp); movzbl (%eax)` 重读）；③两段检查=js 正跳 ret 岛 +
// `movsbl; cmp sz; jl pass` 真跳 pass、ret 块 fall-through ⇒ goto 形「ret 标签置于
// 第二检查与 pass 之间」（4.1 布局，4.4 同形无中继 nop）；④inner 具名 @-0x10。
bool RentalMachineDimension::GetRentItemInfo(
    CUser& user, const InstanceRentalSystem::RentInElem& inElem,
    EventRewardItemInfo& out)
{
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(user, selectionIdx, 2))
    {
        return false;
    }
    char job = 0;
    if (!pc_room::GetCharacterJob(user, job))
    {
        return false;
    }

    const int packageKey =
        makeFairPVPEquipmentPackageKey((int)(signed char)job, selectionIdx);

    // [b28d-PoolGlobalData] 二压：ORIG 0x0826f8ba 无 `list=0` 初始化死槽写、
    // 无 growType 槽（0x0826f8d0 `movsbl %al,%edx` 直落实参位）、else 支第二实参
    // 为**常量 2**（0x0826f8f2 `movl $0x2,0x4(%esp)` 直证，非重读 m_type）、
    // m_type 比较为字节形（0x0826f8c1 `cmp $0x1,%al`）。
    std::vector<std::vector<EventRewardItemInfo> >* list;
    if (inElem.m_type == 1)
    {
        list = (std::vector<std::vector<EventRewardItemInfo> >*)
            GetRewardItemGiveList((int)(signed char)job,
                                  (int)(signed char)user.getCurCharFirstGrowType());
    }
    else
    {
        // ORIG 0x0826f900：pc_room::GetRewardItemSelectionList(packageKey, 2)。
        list = pc_room::GetRewardItemSelectionList(packageKey, 2);
    }
    if (list == 0)
    {
        return false;
    }

    const int sz0 = (int)list->size();
    if ((int)(signed char)inElem.m_sectionIdx < 0)
    {
        goto L_ret0;
    }
    if ((int)(signed char)inElem.m_sectionIdx < sz0)
    {
        goto L_pass0;
    }
L_ret0:
    return false;
L_pass0:
    std::vector<EventRewardItemInfo>* inner =
        &list->at((unsigned int)(int)(signed char)inElem.m_sectionIdx);
    {
        const int sz1 = (int)inner->size();
        if ((int)(signed char)inElem.m_itemIdx < 0)
        {
            goto L_ret1;
        }
        if ((int)(signed char)inElem.m_itemIdx < sz1)
        {
            goto L_pass1;
        }
    L_ret1:
        return false;
    L_pass1: ;
    }
    out = (*inner)[(unsigned int)(int)(signed char)inElem.m_itemIdx];
    return true;
}

// ---- PcRoom::checkAllRentalItemScript —— T 0x082708cc（同名报告全文转写）---
// [b28d-18 N82] 残差=槽位/STL（N82/N89） | ORIG 0x082708cc
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（50/50）；残差=N82 槽位/分支布局 + N89。
bool RentalMachinePcRoom::checkAllRentalItemScript()
{
    // ORIG 0x082708cc 段（p167 实证）：外层 map 的 begin()/end() 各自独立重调
    // G_CDataManager()（P157/P48）；内两层为 const_iterator 遍历
    // （begin/end/operator*/operator++/operator!=），非 size()/operator[] 下标形
    // （P175 反例）；`(*it).second` 代 `it->second`（P167 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             outer = G_CDataManager()->m_pcBangRentar.m_mapC.begin();
         outer != G_CDataManager()->m_pcBangRentar.m_mapC.end(); ++outer)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sections =
            (*outer).second;
        for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
                 sit = sections.begin();
             sit != sections.end(); ++sit)
        {
            const std::vector<EventRewardItemInfo>& items = *sit;
            for (std::vector<EventRewardItemInfo>::const_iterator
                     iit = items.begin();
                 iit != items.end(); ++iit)
            {
                const unsigned int itemId = (unsigned)(*iit).m_field0;
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
// [b28d-18 N82] 残差=槽位/STL 展开（N82/N89） | ORIG 0x0826f14e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（43/43）；已按 P248 档① 改 bool 返回；残差=N82/N89。
bool RentalMachineDimension::checkSelectionListScript(
    const std::vector<std::vector<EventRewardItemInfo> >& selectionList)
{
    // ORIG 0x0826f14e 段（p167 MISSING 迭代器族）：两层均为 const_iterator
    // 遍历（begin/end/operator*/operator++/operator!=），非下标形（P175 反例）。
    for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
             sit = selectionList.begin();
         sit != selectionList.end(); ++sit)
    {
        const std::vector<EventRewardItemInfo>& items = *sit;
        for (std::vector<EventRewardItemInfo>::const_iterator iit = items.begin();
             iit != items.end(); ++iit)
        {
            const unsigned int itemId = (unsigned)(*iit).m_field0;
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
// [b28d-18 N82] 残差=槽位/分支布局（N82） | ORIG 0x0826f022
bool RentalMachineDimension::checkAllRentalItemScript()
{
    // ORIG 0x0826f028/0x0826f086/0x0826f0b9/0x0826f114：两循环的 begin()/end()
    // 各自独立重调 G_CDataManager()（P157 逐字段重读 + P48 无 CSE，禁 dm 缓存）；
    // `(*it).second` 代 `it->second`（P167：ORIG 发 deEv 非 ptEv）。
    for (std::map<int,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_mapC.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_mapC.end(); ++it)
    {
        // [b28d-solo-GlobalData] P0：ORIG 0x0826f05x `add $0x4,%eax; mov %eax,-0x10;
        // mov -0x10,%eax` 槽往返 ⇒ 原码有具名引用局部再传参（直接传表达式少 2 条）。
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    for (std::map<std::pair<int, int>,
                  std::vector<std::vector<EventRewardItemInfo> > >::iterator
             it = G_CDataManager()
                      ->m_dimensionAct.m_rewardData.m_map24.begin();
         it != G_CDataManager()
                   ->m_dimensionAct.m_rewardData.m_map24.end(); ++it)
    {
        const std::vector<std::vector<EventRewardItemInfo> >& sel = (*it).second;
        if (!checkSelectionListScript(sel))
        {
            return false;
        }
    }
    return true;
}

// ---- PcRoom::CheckupAllRentalItems —— T 0x08270d1e（同名报告全文转写）------
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x08270d1e
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
int RentalMachinePcRoom::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x08270d30：与 Dimension::CheckupAllRentalItems 同构 —— begin() 隐藏
    // 返回指针迭代器 @-0x1c、循环条件后置（end()+operator!=）、体内 operator*。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
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
            // ORIG 0x08270f2a/0x08270f4d：`shr $0x1f,%eax; test %al,%al; je <++>`
            // —— 负值走 return，非负 fall 到 `addl $0x1` ⇒ 源码写 `< 0` 早退，
            // 不写 `>= 0 {…} else {…}`（后者 4.4 会多出 `not %eax` 反相）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // 同 Dimension 版：ORIG 尾 if-return 形（P47/P191）。
    if (!pc_room::CheckInventoryEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
}

// ---- Dimension::CheckupAllRentalItems —— T 0x0826f52c（同名报告全文转写）---
// [b28d-18 N7] 残差=callee-saved 角色轮换 + 单段 regmove（N7/N18a） | ORIG 0x0826f52c
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N7/N82 寄存器角色与槽位。
int RentalMachineDimension::CheckupAllRentalItems(
    CUser& user, const InstanceRentalSystem::RentInput& input)
{
    int inventoryNeed = 0;
    // ORIG 0x0826f53e：`begin()`（隐藏返回指针迭代器对象 @-0x1c）→ `jmp` 到底部
    // `end()`+`operator!=` 判定 → 体内 `operator*` 取元素指针存 @-0x10 ⇒
    // 源码为 const_iterator 三段 for 循环（非 size()/operator[] 下标循环）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
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
            // ORIG 0x0826f6fd：`shr $0x1f,%eax; test %al,%al; je <addl $0x1>`
            // ⇒ `< 0` 早退 + fall-through 自增（见 PcRoom 同名函数注释）。
            if (user.getCurCharacInvenR()->tryInsertItemIntoInventory(probe) < 0)
            {
                return 0xcb;
            }
            ++inventoryNeed;
        }
    }
    // ORIG 尾 `call; xor $1; test %al,%al; je <mov $0 岛>; mov $0xcb; jmp`
    // ⇒ if-return 形（P47/P191：三目与 if-return 块排布镜像，此处需 if 形）。
    if (!pc_room::CheckInventoryEquipmentEmptyCount(user, inventoryNeed))
    {
        return 0xcb;
    }
    return 0;
}

// ---- PcRoom::Rent —— T 0x082704e6（Rent.md 全文转写）------------------------
// [b28d-solo-GlobalData] 归零 169=169：①真语义 bug——首守卫 ORIG 为
// `!IsPcRoomUser && !m_field11`（两条 je 同落继续块，旧写 `|| m_field11==1` 逻辑相反）；
// ②RentInput::m_field11 uchar→bool（P248 档②）+ 嵌套 if（P133①）；
// ③m_bEquip 取反+两臂交换（P47/P222）| ORIG 0x082704e6
int RentalMachinePcRoom::Rent(CUser& user,
                              const InstanceRentalSystem::RentInput& input,
                              InstanceRentalSystem::RentOutput& output)
{
    // ORIG 0x082704f4..0x08270513（真语义 bug 修正）：`call IsPcRoomUser; xor $1;
    // test; je <继续>` + 内层 `movzbl 0x11; xor $1; test; je <继续>`；两条 je 同落
    // 继续块 ⇒ 原码是 `if (!IsPcRoomUser(user) && !input.m_field11) return 7;`
    // （旧写 `|| m_field11 == 1` 逻辑相反）。
    // P133①/P225 推广：ORIG 两条守卫各自 `je <继续>`（非汇聚物化）⇒ 嵌套 if
    // （外层 !A、内层 !B → return 7），4.4 才逐条复现 `xor $1; test; je`。
    if (!pc_room::IsPcRoomUser(user))
    {
        if (!input.m_field11)
        {
            return 7;   // ENUM_ERROR 7
        }
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
    // ORIG 0x082704e6：const_iterator 三段循环（p167 MISSING 实证）。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
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
        // ORIG `test %al,%al; je <ProcessInsertItem>`（equip 直落）⇒ 条件取反 +
        // 两臂交换复现 4.1 else-first 布局（P47/P222，同 Dimension::Rent）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3d);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3d);
        }
        if (input.m_logFlag != 0)
        {
            log(user, info.m_field0);
        }
    }
    return 0;   // 0=成功
}

// ---- Dimension::Rent —— T 0x0826ec22（Rent.md 全文转写）---------------------
// [b28d-solo-GlobalData] 归零 176=176：m_bEquip 分支条件取反+两臂交换复现
// ORIG else-first 布局（`test;je <ProcessInsertItem>`，P47/P222）；p167 CLEAR 21/21 | ORIG 0x0826ec22
int RentalMachineDimension::Rent(CUser& user,
                                 const InstanceRentalSystem::RentInput& input,
                                 InstanceRentalSystem::RentOutput& output)
{
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
    // ORIG 0x0826f3xx：const_iterator 三段循环（begin/end/ne/deEv/ppEv，
    // p167 MISSING 实证），非 size()/operator[] 下标形。
    for (std::vector<InstanceRentalSystem::RentInput::RentInElem>::const_iterator
             it = input.m_elems.begin();
         it != input.m_elems.end(); ++it)
    {
        const InstanceRentalSystem::RentInElem& elem = *it;
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
        // ORIG 0x0826ed9f `test %al,%al; je <ProcessInsertItem>`（equip 直落）
        // ⇒ 条件取反 + 两臂交换复现 4.1 的 else-first 块布局（P47/P222 族）。
        if (elem.m_bEquip != 0)
        {
            pc_room::ProcessEquipItem(user, elem, info, output, 0x3e);
        }
        else
        {
            pc_room::ProcessInsertItem(user, elem, info, output, 0x3e);
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
// [b28 判词 N7] 残差=call *%edx↔%eax 换名+伴随寄存对调，主体同形封顶。
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
    // ORIG 0x086b1dxx：v 槽先声明(基址 -0x40)、t 次之(-0x14)、localtime_r
    // 返回值落指针槽 pv(-0x10)，字段经 pv-> 间接读（0x086b1d53 `mov
    // -0x10(%ebp),%eax; mov 0x14(%eax),%eax`）。
    // ORIG 0x086b1d7x：return 表达式结果先落临时槽 -0xc(%ebp) 再回读（4.1
    // dead store 具名化）。
    struct tm v;
    time_t t = GlobalData::s_systemTime_.getCurSec();
    struct tm* pv = localtime_r(&t, &v);
    int seed = pv->tm_year * 365 + pv->tm_mon * 30 + pv->tm_mday;
    return seed;
}

// ============================================================================
// [b28d-Main·检查点18] 原 7 条 GlobalInstance 显式实例化已删（同 StaticPool 问题）：
//   显式实例化为每个 T 发射**全部**成员（inst/create/destroy/inst_ptr），而 ORIG 只
//   发射实际使用的（如 GlobalInstance<EncryptTool> 仅 create/inst_ptr/m_p，
//   GlobalInstance<Cipher> 同）⇒ inst/destroy 成为 only-NEW。定义在 CEnvironment.h
//   类体内（实例化点可见）⇒ 按使用点隐式实例化即与 ORIG 一致。
// ============================================================================
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

// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 GlobalData.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN20InstanceRentalSystemC2Ev
//   ORIG TU     ：RentalSystem.cpp
//   .inc 为原样搬移；★ 本壳额外复制 host 的 tu 级类定义（RentalMachinePcRoom / RentalMachineDimension）
//     ——因 RentalMachinePcRoom.h 独立编译缺 <set>（探针实测），故不依赖该头（Main 批准手法）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include "LogManager.h"
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
#include "CExpertJob.h"
#include "CEnvironment.h"
#include "GameTypes.h"
#include "CRijndael.h"
#include "CExtraction.h"
#include "CAlchemistExtraction.h"
#include "CEnchanterExtraction.h"
#include "CDollControllerExtraction.h"
#include "CExtractionMgr.h"
#include "PvPTypes.h"      // CMTRand 完整类型（CExtraction::m_rand）
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "ItemVendingMachine.h"
#include "CLevelDungeonPlayStatistic.h"
#include "CDungeonExpStatistic.h"   // +0x18 子对象权威定义（map<int, stDungeonExp>）
#include "BreakAwayPreventSystem.h"   // break_away_prevent 唯一权威声明点
#include <set>  // RentalMachinePcRoom/Dimension 的 m_set

class RentalMachinePcRoom : public IRentalMachine
{
public:
    RentalMachinePcRoom();
    virtual ~RentalMachinePcRoom() {}

    // 槽2 Rent：ORIG T 0x082704e6（0x258 字节 pc_room 租赁事务链，本体待
    // pc_room 域批次 —— 报告 RentalMachinePcRoom/Rent.md）。声明占位保 vtable 序。
    // 返回类型 ENUM_ERROR（ORIG prett @0x8bfafe0 直证）。
    virtual ENUM_ERROR Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
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
    // 返回类型 ENUM_ERROR（ORIG prett @0x8bf9f60 直证）。
    virtual ENUM_ERROR Rent(CUser& user, const InstanceRentalSystem::RentInput& input,
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

#include "RentalSystem/InstanceRentalSystem.inc"

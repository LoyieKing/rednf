// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 GlobalData.cpp 拆出。
//   ORIG TU     ：CaptchaGen.cpp
//   ORIG 期望键：_GLOBAL__I__ZN10CaptchaGenC2Ev
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

extern "C" int FT_Init_FreeType(void** alibrary);   // 定义在 host（占位保链接）

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

#include "CaptchaGen/CaptchaGen.inc"

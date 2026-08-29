#ifndef GAME_DB_INTERDEPS_H_
#define GAME_DB_INTERDEPS_H_

// ============================================================================
// InterDispatcher 域 DB/公会辅助声明（InterWave1 批）。
// 类本体（ctor/dispatch 等）在 W8LT_DBTask.cpp DBDispatcher 注册表；
// 此处仅声明 ORIG 真实静态 makeRequest 符号（定义由 Main 统一分派属主批次，
// TU 持 U 引用合规，见 game_verify_guide.md §8.1）。
// ============================================================================

struct ST_Guild_Mem_Info;   // 权威定义 shared/packet/include/Packet_Monitor_Call_Guild_Members_ToChannel.h
class PacketGuard;

// ORIG _ZN20DB_SaveCleanpadPoint11makeRequestEji T @0x08432fe2（静态成员）
class DB_SaveCleanpadPoint
{
public:
    static void makeRequest(unsigned int point, int accId);
};

// ORIG _ZN23DB_ArrangeUserLoginInfo11makeRequestEjbj T @0x0842ad30（静态成员）
class DB_ArrangeUserLoginInfo
{
public:
    static void makeRequest(unsigned int accId, bool flag, unsigned int uid);
};

// ORIG _ZN33DB_SecurityCardUpdateValidityTime11makeRequestEjj T @0x0842bce8（静态成员）
class DB_SecurityCardUpdateValidityTime
{
public:
    static void makeRequest(unsigned int accId, unsigned int flag);
};

// ORIG _ZN30DB_GoblinPadUpdateValidityTime11makeRequestEjj T @0x0842f2ae（静态成员）
class DB_GoblinPadUpdateValidityTime
{
public:
    static void makeRequest(unsigned int accId, unsigned int flag);
};
// ORIG _ZN23DB_UpdateCharacPlayTime11makeRequestEil T @0x0843f7ee（静态成员；
// DisPatcherCore.cpp Exit::dispatch_sig 调用，DisPatcherWave1 增）
class DB_UpdateCharacPlayTime
{
public:
    static void makeRequest(int characNo, long playTick);
};

// ORIG _ZN30DB_SecuServiceUpdateValidityTime11makeRequestEjjtPc T @0x0843a34e
// （静态成员；DisPatcherCore.cpp Exit::dispatch_sig 调用，DisPatcherWave1 增）
class DB_SecuServiceUpdateValidityTime
{
public:
    static void makeRequest(unsigned int accId, unsigned int type, unsigned short flag, char* uid);
};


// ORIG _ZN20DB_SaveUserHandicap11makeRequestEjiiiii T @0x0842801a（静态成员；
// 定义已在 W8LT_DBTask.cpp:1299，Inter_SaveClientHackLog 调用）
class DB_SaveUserHandicap
{
public:
    static void makeRequest(unsigned int accId, int a, int b, int rate, int e, int f);
};

// ORIG _ZN26DB_DeathTowerLoadTopRanker11makeRequestEb T @0x0842a910（静态成员）
class DB_DeathTowerLoadTopRanker
{
public:
    static void makeRequest(bool reload);
};

// ORIG _ZN13DB_HadesPunish11makeRequestEjjjjjPc T @0x08445dd4（静态成员；
// 定义属主未落地，Inter_HadesAction 调用）
class DB_HadesPunish
{
public:
    static void makeRequest(unsigned int a, unsigned int b, unsigned int c,
                            unsigned int d, unsigned int e, char* msg);
};


// ORIG _ZN17DB_BlackIPMonitor11makeRequestEv T @0x0837501a（Inter_BlackIPMonitorReq）
class DB_BlackIPMonitor
{
public:
    static void makeRequest();
};

// ORIG _ZN24DB_BlackIPMonitorPartLoad11makeRequestEv T @0x08433?? （Inter_BlackIPMonitorPartLoadReq）
class DB_BlackIPMonitorPartLoad
{
public:
    static void makeRequest();
};

// ORIG _ZN24DB_Find_Factory_Hub_User11makeRequestEjcPt T（Inter_FindFactoryHub）
class DB_Find_Factory_Hub_User
{
public:
    static void makeRequest(unsigned int uid, char* name, unsigned short flag);
};

#endif  // GAME_DB_INTERDEPS_H_

// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CInGameAdvertisementManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CInGameAdvertisementManager.cpp —— LinkF1 批次（2026-08-28），OrigAdv2 修正。
// InitADExposeInfo / InsertADExposeInfo 按 docs/class_func_reports/
// CInGameAdvertisementManager/{InitADExposeInfo,InsertADExposeInfo}.md 忠实转写；
// CAdvertisementExposeInfo 生命周期符号按 ORIG nm -S：
//   operator new(unsigned)            T 0x080fc998（块 0x850）
//   operator delete(void*)            T 0x080fc9ce
//   operator delete(void*, unsigned)  T 0x080fc9ac
//   SetAdvertisementExposeInfo(stInGameADExposeCondition&) W 0x080fdd0a
//       （memcpy(this,&cond,0x84c)，见 global-ctors 键区 +0x36 内联体）
//   getExposeCondition()              W 0x080fdd2c（返回 this）
// ORIG 经 MemPool<CAdvertisementExposeInfo> 静态池分配：pool 对象
// _ZN24CAdvertisementExposeInfo28advertisementExposeInfoPool_E B 0x093fea90
//（0x14 字节，ctor count 0x32 实证见 __static_initialization_and_destruction_0
//  80fdc63）、headOfFreeList_ V 0x093feaec；alloc W 0x080fe038 / free W 0x080fe188/
// 0x080fe1ce 为显式实例化弱符号。operator new/delete 三函数直调 alloc/free
//（ORIG 080fc998/080fc9ac/080fc9ce 反编译实证）。
#include "CInGameAdvertisementManager.h"
#include "../Guild/MemPool.h"      // MemPool<CAdvertisementExposeInfo>（显式实例化在 Guild/MemPool.cpp）
#include "PvPTypes.h"     // CMTRand（ctor/dtor/RandNum 用）
#include "GlobalData.h"      // s_db_mgr / DBMgr
#include "DBMgr.h"           // GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE)
#include "MySQL.h"           // MySQL set_query/exec/fetch/get_*
#include "GameTypes.h"       // cMyTrace
#include "CUser.h"          // CUser getters（checkLevel/checkCondition/CheckIngame…）
#include "CStreamGuard.h"   // CStreamGuard/Stream（ReloadADExposeInfo）
#include "MsgQueueMgr.h"    // MsgQueueMgr::DISPATCH_Q/put
#include "SigTypes_D.h"     // SIG_RELOAD_IN_GAME_ADVERTISEMENT
#include "DNFFunctionLib.h" // DNFFLib::ExplodeString（checkLevel）
#include <cstring>
#include <cstdlib>          // atoi（checkLevel）
// CDataManager.h 引入 <iostream>（冻结头，勿改）：ORIG 该 TU 无 __ioinit 静态
// 初始化（static-init 独立 _GLOBAL__I_ 实证），此处仅需 G_CDataManager() 声明，
// 前向替代 include（ORIG 同样只调用符号不 include）。
class CDataManager;
CDataManager* G_CDataManager();
// ---------------- CAdvertisementExposeInfo ----------------

#include "CInGameAdvertisementManager/advertisementExposeInfoPool_.inc"


// ORIG T 0x080fc9ce：直调 MemPool::free(void*)（反编译实证）。
void CAdvertisementExposeInfo::operator delete(void* p)
{
    advertisementExposeInfoPool_.free(p);
}

// ORIG T 0x080fc9ac：直调 MemPool::free(void*, unsigned)（反编译实证）。
void CAdvertisementExposeInfo::operator delete(void* p, unsigned int size)
{
    advertisementExposeInfoPool_.free(p, size);
}

#include "CInGameAdvertisementManager/SetAdvertisementExposeInfo.inc"


#include "CInGameAdvertisementManager/getExposeCondition.inc"


#include "CInGameAdvertisementManager/CInGameAdvertisementManager.inc"


#include "CInGameAdvertisementManager/dtor_CInGameAdvertisementManager.inc"


#include "CInGameAdvertisementManager/InitADExposeInfo.inc"


#include "CInGameAdvertisementManager/InsertADExposeInfo.inc"


#include "CInGameAdvertisementManager/queryADExposeInfo.inc"


#include "CInGameAdvertisementManager/LoadADExposeInfoAtApp.inc"

// ORIG T _Z23InGameADManagerInstancev @0x080fc9e9：函数局部 static 单例
//（guard 0x093feac8 / instance 0x093fead0；__cxa_atexit 析构由 static 语义给出）。
CInGameAdvertisementManager* InGameADManagerInstance()
{
    static CInGameAdvertisementManager instance;
    return &instance;
}

#include "CInGameAdvertisementManager/checkLevel.inc"


#include "CInGameAdvertisementManager/checkCondition.inc"


#include "CInGameAdvertisementManager/CheckIngameAdvertisementCondition.inc"


#include "CInGameAdvertisementManager/RandNum.inc"


#include "CInGameAdvertisementManager/ReloadADExposeInfo.inc"


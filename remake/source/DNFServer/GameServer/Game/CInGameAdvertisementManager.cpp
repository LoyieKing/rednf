// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CInGameAdvertisementManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG 静态池实例：MemPool<CAdvertisementExposeInfo>（ctor count 0x32 =
// 50 块，实证 __static_initialization_and_destruction_0 @0x080fdc63 push $0x32）。
#include "CInGameAdvertisementManager/advertisementExposeInfoPool_.inc"
}

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

// ORIG W 0x080fdd0a：memcpy(this, &cond, 0x84c)。
#include "CInGameAdvertisementManager/SetAdvertisementExposeInfo.inc"
}

// ORIG W 0x080fdd2c：返回 this（条件缓冲即对象首 0x84c）。
#include "CInGameAdvertisementManager/getExposeCondition.inc"
}

// ORIG W _ZN27CInGameAdvertisementManagerC1Ev @0x080fdd6a：multimap 默认构造
// → time(0) → new CMTRand(0x9c8, seed) 存 +0x18。
#include "CInGameAdvertisementManager/CInGameAdvertisementManager.inc"
}

// ORIG W _ZN27CInGameAdvertisementManagerD1Ev @0x080fca7c：delete m_rand(+0x18)
// → multimap 析构（ORIG 无 m_rand 置零）。
#include "CInGameAdvertisementManager/dtor_CInGameAdvertisementManager.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager16InitADExposeInfoEv @0x080fd122
// （报告 InitADExposeInfo.md）：遍历 multimap delete 各 info → clear(this)。
// ORIG 循环体将 it->second 存入命名局部(-0xc)再 delete（P28 槽位序）。
#include "CInGameAdvertisementManager/InitADExposeInfo.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager18InsertADExposeInfoER25stInGameADExposeCondition
// @0x080fd8e4（报告 InsertADExposeInfo.md）：new(0x850) → SetAdvertisementExposeInfo
// → multimap<short,CAdvertisementExposeInfo*>::insert(make_pair(cond+0x204, info))。
// 注：CAdvertisementExposeInfo 无用户 ctor（ORIG nm 无 C1/C2），new 不生成 ctor call。
#include "CInGameAdvertisementManager/InsertADExposeInfo.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager17queryADExposeInfoEP25stInGameADExposeCondition
// @0x080fd19e（报告 queryADExposeInfo.md 全转写）：DB_HANDLE_9 取连接，
// "seLect * from in_game_ad where visible = 1"；行 → 0x84c 条件记录逐列填充
//（失败即返 0）；循环后写哨兵（cond[i]+0x210 = -1）并双 trace，返 1。
// ORIG 形态：char 暂存槽 b(-0x15) 先于 db(-0x14) 声明；cMyTrace 临时链式
//（P37，__PRETTY_FUNCTION__）；无 rec 命名局部（每列 cond+i 内联重算）。
#include "CInGameAdvertisementManager/queryADExposeInfo.inc"
}

// ORIG 形态：if(q()){…;return true;} return false;（test %al; je 直测，P59）；
// 循环 i<=199（P2 setle）；无 tailPad（buf 恰 200*0x84c = 0x67b60）。
#include "CInGameAdvertisementManager/LoadADExposeInfoAtApp.inc"
}
// ORIG T _Z23InGameADManagerInstancev @0x080fc9e9：函数局部 static 单例
//（guard 0x093feac8 / instance 0x093fead0；__cxa_atexit 析构由 static 语义给出）。
CInGameAdvertisementManager* InGameADManagerInstance()
{
    static CInGameAdvertisementManager instance;
    return &instance;
}

// ORIG T _ZN27CInGameAdvertisementManager10checkLevelEP5CUserPc @0x080fca9e
//（报告 checkLevel.md）：data 逗号分隔等级表 → ExplodeString → atoi 逐项；
// 命中 -1（表尾）或 user 当前等级即返 1。
#include "CInGameAdvertisementManager/checkLevel.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager14checkConditionEP5CUserP24CAdvertisementExposeInfo
// @0x080fcd2e（报告 checkCondition.md）：年龄表(G_CDataManager+0x4a5c)/
// 性别/等级/商城币区间/职业/账号取模逐项过滤，写 info 暴露标记(+0x549)。
#include "CInGameAdvertisementManager/checkCondition.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi
// @0x080fcfa8（报告 CheckIngameAdvertisementCondition.md）：multimap 按 short
// 键 [lower_bound, upper_bound) 遍历 → checkCondition 命中者 memcpy 0x63c 入
// vector → RandNum 随机取一条 rep movsl 0x63c 拷出。
// b28d-solo-CInGameAdvertisementManager：AE 闭合（无源码改动）。复验
// /tmp/b28d_solo_fcmp.py（AE 归一口径，新鲜 .o）verdict=AE 114/114；
// game_func_report 报 DIFF 全系 .o 未解析 call/jmp 重定位显示差（P264），非正文差。
#include "CInGameAdvertisementManager/CheckIngameAdvertisementCondition.inc"
}

// ORIG W _ZN27CInGameAdvertisementManager7RandNumEi @0x080fdd34：
// m_rand->randInt() 无符号 % range（xor edx / div 实证）。
#include "CInGameAdvertisementManager/RandNum.inc"
}

// ORIG T _ZN27CInGameAdvertisementManager18ReloadADExposeInfoEv @0x080fd95e
//（报告 ReloadADExposeInfo.md）：stream 0x2f9 + (-1) → GetInBuffer 缓冲 →
// queryADExposeInfo 成功则 put(DISPATCH_Q) 广播重载。
#include "CInGameAdvertisementManager/ReloadADExposeInfo.inc"
}

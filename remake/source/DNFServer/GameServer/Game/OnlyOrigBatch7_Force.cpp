#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlyOrigBatch7_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#if remake_verify
#include <cstdatomic>      // memory_order / __atomic_flag_base（先于 atomic_0.h）
#include <bits/atomic_0.h> // std::__atomic0::atomic_flag（ORIG 用互斥版）
#else
#include <atomic>          // 现代路线：std::atomic_flag / std::memory_order
#endif
#include <pthread.h>       // pthread_mutex_t / pthread_mutex_lock（ORIG 形互斥）
#include <bits/functexcept.h> // std::__throw_system_error（ORIG 0x086d96b0）

#include "CDataManager.h"    // _str_cost/_str_option_value_ratio/pieceQuestRewardStatus
                             // /rewardKey/rewardSubKey/seriaRoomDecoRewardInfo/
                             // stTempRewardInfo/stQuestShoptInitCost
#include "CEquipItem.h"      // WpBonusPointData
#include "CTitleBook.h"      // stAdjust
#include "CStackableItem.h"  // stLimitCubeResult/stStackableBoosterElement_t
#include "RefPvpGrade.h"     // stPvPGradeExp_t
#include "STEquipmentScript.h" // stSetItemBackImageInfo
#include "CItemAmplifier.h"  // upgrade_info_t
#include "CGuildServerProxy.h"  // stEVENTADDINFO（§4.4 权威声明）
#include "SigTypes_C.h"      // stMonsterInfo_t
#include "CompiledDNFLex.h"  // CompiledDNFLex::stream_t
#include "Quest.h"           // stQuestTargetCharacter_t
#include "CDeathTowerRanking.h" // stBestRecord_t
#include "CVillageObjectMgr.h"  // village_object::Zone
#include "CVillageMonsterMgr.h" // village_attacked::Zone
#include "NpcScript.h"       // stFavorableRelationShip_t::stPeriodRewardInfo
#include "WriteLogSwitch.h"  // WriteLogSwitch

// ============================================================================
// 缺失类型定义（ORIG 全局/共享头缺失，按 nm+objdump 恢复布局）
// ============================================================================

// stEVENTADDINFO：权威声明在 CGuildServerProxy.h（ORIG D1/D2 0x08118b56 W 空体 +
// 字段布局 +0x00/+0x04/+0x06，§4.4 唯一声明点）。此处不再本地定义。

// tagPacketHeader / tagCS_UPDATE_CHANNEL_INFO（ORIG ctor 0x08106bfc W）：
// header cat=1/id=2/size=0x33（setCategory/setPacketID/setSize 逐指令转写）。
struct tagPacketHeader
{
    char classification;   // +0x00（setCategory 写 dl）
    char msg_no;           // +0x01（setPacketID）
    unsigned int sLength;  // +0x02（setSize 写 4 字节）
    unsigned int check_sum; // +0x06
    char ack;              // +0x0a

    tagPacketHeader() {}   // ORIG C1 0x08106bf6 空体
    void setCategory(int n) { classification = (char)n; }
    void setPacketID(int n) { msg_no = (char)n; }
    void setSize(int n) { sLength = (unsigned int)n; }
    int getSize() { return (int)sLength; }   // ORIG W 0x08106bcc
} __attribute__((packed));

struct tagCS_UPDATE_CHANNEL_INFO : public tagPacketHeader
{
    tagCS_UPDATE_CHANNEL_INFO();
    int gc_no;
    int group;
    int channel_no;
    int max_user_num;
    int cur_user_num;
    char server_ip[16];
    int port;
};

#include "OnlyOrigBatch7_Force/tagCS_UPDATE_CHANNEL_INFO.inc"


// ============================================================================
// std::__atomic0::atomic_flag（ORIG T 0x086d92d0/0x086d9330）：
// gcc4.4.7 的 atomic_0.h 仅声明成员，ORIG 为 libstdc++ 头内联互斥实现。
// 按 ORIG 逐指令形恢复：TU-local get_atomic_mutex()（ORIG 0x086d9230，
// _ZN12_GLOBAL__N_116get_atomic_mutexEv）+ gthr 活跃检查（ORIG 取
// &pthread_cancel 弱符号判非零：0x086d92e9/0x086d9346 `mov $&.plt,%eax;
// test %eax,%eax; je`）+ pthread_mutex_lock/unlock。4.4 头集在 -nostdinc 下
// 选 gthr-single（__gthread_active_p 恒 0 桩），故显式取弱符号地址复现 posix
// 形；裸 register（P231，非 asm 绑定，P0 合规）复现 ORIG 免槽 callee-saved。
// ============================================================================
#if remake_verify
extern "C" int pthread_cancel(pthread_t) __attribute__((weak));

namespace
{
// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N 封顶）ORIG 0x086d9230
// _ZN12_GLOBAL__N_116get_atomic_mutexEv（41 条）：ORIG 为函数局部静态互斥
// （__cxa_guard_acquire/release + 24B 临时清零拷贝到静态 0x948b2f0），即
// 4.1 的非平凡初始化发射；4.4 对 PTHREAD_MUTEX_INITIALIZER（全零常量）走
// 静态初始化（.bss 直返、无 guard）⇒ guard/构造发射差异不可复现（N 封顶）。
// 语义等价：同一 TU 单例互斥，初值同为全零。
pthread_mutex_t& get_atomic_mutex()
{
    // [P6S6 2026-09-11 试形] 父泳道点名（p167fix MISSING __cxa_guard_acquire/
    //   release/__stack_chk_fail）：ORIG 侧为 4.1 的**动态**局部静态初始化
    //   （24B 临时清零 + 拷贝 + guard）。试值初始化形是否触发 4.4 的 guard。
    // [Q7S2 2026-09-11 符号名轴] ORIG 该函数局部 static 的 mangled 名 = `atomic_mutex`：
    //   nm 直证 `_ZZN12_GLOBAL__N_116get_atomic_mutexEvE12atomic_mutex` @0x948b2f0(.bss)
    //   + guard `_ZGVZN12_GLOBAL__N_116get_atomic_mutexEvE12atomic_mutex` @0x948b2e8；
    //   原 `m` 为我方发明名（ORIG 无 `…E1m`）。纯改名，零语义变化。
    static pthread_mutex_t atomic_mutex = pthread_mutex_t();
    return atomic_mutex;
}
}  // namespace

namespace std
{
namespace __atomic0
{

#include "OnlyOrigBatch7_Force/test_and_set.inc"


#include "OnlyOrigBatch7_Force/clear.inc"


}  // namespace __atomic0
}  // namespace std
#endif  /* remake_verify */

// ============================================================================
// force 发射（P147）
// ============================================================================
namespace
{

#include "OnlyOrigBatch7_Force/forceCtorEmissions.inc"


}  // namespace

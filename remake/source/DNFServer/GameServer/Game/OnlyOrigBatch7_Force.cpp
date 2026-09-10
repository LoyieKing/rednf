// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OnlyOrigBatch7_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OnlyOrigBatch7_Force.cpp —— OnlyOrigBatch7 批次头内联值类 W 符号发射 +
// 缺失类型定义。
//
// P147：头内联值类的 C1/C2/D1/D2 均为 W；重建若从未实例化则不发射。
// 本 TU 以 force 函数显式构造/析构一遍，O0 下为每个 ODR 使用点发射同名 W。
// 全部按 ORIG nm -S + objdump 逐函数核对（OnlyOrigBatch7 批次）。
// ============================================================================

#include <cstdatomic>      // memory_order / __atomic_flag_base（先于 atomic_0.h）
#include <bits/atomic_0.h> // std::__atomic0::atomic_flag（ORIG 用互斥版）
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

// stEVENTADDINFO（ORIG D1/D2 0x08118b56 W 空体；trivial POD，无成员析构）。
struct stEVENTADDINFO
{
    ~stEVENTADDINFO() {}
};

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

// ORIG 0x08106bfc（W 同址 C1/C2）：tagPacketHeader 构造 + 三 setter。
#include "OnlyOrigBatch7_Force/tagCS_UPDATE_CHANNEL_INFO.inc"
}

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
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
    return m;
}
}  // namespace

namespace std
{
namespace __atomic0
{

// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N 封顶）ORIG 0x086d92d0
// orig/ours=32/34。已按 ORIG 形恢复：TU-local get_atomic_mutex() + gthr posix
// 活跃检查（&pthread_cancel 弱符号，ORIG 0x086d92e9 同形）+ pthread_mutex_lock/
// unlock + true/false 双路写 _M_i（ORIG 只在 true 臂调 unlock，0x086d9307）。
// 残余为跨版本 codegen，无源码解：①prologue/epilogue 4.1 `sub $0x28;
// mov %ebx,-0xc(%ebp)` vs 4.4 `push %edi/%esi/%ebx; sub $0x2c`（N82 族）；
// ②4.4 对 `int __e` 恒占栈槽（ORIG 留 %eax 直判 `test %eax,%eax; jne`）；
// ③throw 块布局（ORIG jne 跳函数尾 vs 4.4 je 跳过块）。语义逐条等价
// （锁保护 + 返回旧值；libpthread 存在时恒加锁，与 ORIG 的 gthr 判定一致）。
// [b29-R28 复核] p167 **CALLSITE CLEAR**（ORIG 4 calls = 我方 4 calls）⇒ 无缺口；
// 差额 2 条全为跨版本 codegen（4.1 尾调用/prologue 保存形）⇒ N 封顶。
#include "OnlyOrigBatch7_Force/test_and_set.inc"
}

// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N 封顶）ORIG 0x086d9330
// orig/ours=30/29。同 test_and_set：get_atomic_mutex + 活跃检查 + 互斥 +
// true/false 双路清零（ORIG 0x086d935b/0x086d936f 两处 movb $0）。残余同上
// （prologue/epilogue、`int __e` 槽、4.1 unlock 尾调用 jmp vs 4.4 call+ret）。
// 语义逐条等价。
// [b29-R28 复核] p167：EXTRA `pthread_mutex_unlock`×1（ORIG 以 `jmp
// pthread_mutex_unlock@plt` **尾调用**收尾，4.4 因 push 保存寄存器只能 call+ret，
// ORIG 0x086d936a vs 我方尾部 `add $0x20; pop...; ret`）⇒ 跨版本 sibcall 差异，
// 非语义缺口 ⇒ N 封顶。
#include "OnlyOrigBatch7_Force/clear.inc"
}

}  // namespace __atomic0
}  // namespace std

// ============================================================================
// force 发射（P147）
// ============================================================================
namespace
{

#include "OnlyOrigBatch7_Force/forceCtorEmissions.inc"
}

}  // namespace

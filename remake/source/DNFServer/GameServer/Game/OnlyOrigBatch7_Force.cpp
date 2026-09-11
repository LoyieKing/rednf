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
{
    // 基类 tagPacketHeader 自动默认构造（ORIG 0x08106bfc 仅一次构造调用；
    // 显式 tagPacketHeader::tagPacketHeader(); 会让 g++ 多构造一个临时对象）
    setCategory(1);
    setPacketID(2);
    setSize(0x33);
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

// b28d-solo-OnlyOrigBatch7_Force ORIG 0x086d92d0。**最终读数：our 34→33（gap −1）**。
// 已按 ORIG 形恢复：TU-local get_atomic_mutex() + gthr posix
// 活跃检查（&pthread_cancel 弱符号，ORIG 0x086d92e9 同形）+ pthread_mutex_lock/
// unlock + true/false 双路写 _M_i（ORIG 只在 true 臂调 unlock，0x086d9307）。
// 已消 1 条：`int __e` 条件声明 → `register int __e` 提到 if 之前（见内注 P0）。
// 残余 1 条 = `mov %eax,%edi`（4.4 把 register __e 落到 callee-saved %edi，%ebx/%esi
// 已被 self/&mutex 占用；ORIG 4.1 让 __e 留 %eax 直接 `test %eax,%eax; jne`）——
// register 变量是 -O0 下唯一免栈槽手段（非 register ⇒ 恒落槽 +2 条，实测 34），
// 其寄存器选择不可由源码控 ⇒ N 封顶。另 prologue `sub $0x28; mov %ebx,-0xc(%ebp)`
// vs 4.4 `push %edi/%esi/%ebx; sub $0x1c` 为 N82 族（指令数相同）。语义逐条等价
// （锁保护 + 返回旧值；libpthread 存在时恒加锁，与 ORIG 的 gthr 判定一致）。
// [b29-R28 复核] p167 **CALLSITE CLEAR**（ORIG 4 calls = 我方 4 calls）⇒ 无缺口；
// [b29-S3 复核] 残余 1 条全为跨版本 codegen（register 变量寄存器类）⇒ N 封顶。
#include "OnlyOrigBatch7_Force/test_and_set.inc"
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    register bool __ret;
    if ((void*)&pthread_cancel != 0)
    {
        // [P0 消槽] ORIG 0x086d92fa 为 `test %eax,%eax; jne 0x086d9323`
        // （__e 留 %eax 无栈槽，throw 臂直接 `mov %eax,(%esp)`）；4.4 对条件
        // 内声明的 __e 恒落槽（-0x1c 三指令：mov/cmpl/mov）⇒ our 34 vs orig 32。
        // register 让 4.4 也走寄存器（语义不变：仅存储类说明符）；C++ 的
        // condition 声明不允许存储类说明符，故声明提到 if 之前。
        register int __e = pthread_mutex_lock(&__mutex);
        if (__e)
            __throw_system_error(__e);
        __ret = self->_M_i;
        self->_M_i = true;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        __ret = self->_M_i;
        self->_M_i = true;
    }
    return __ret;
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
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    if ((void*)&pthread_cancel != 0)
    {
        if (int __e = pthread_mutex_lock(&__mutex))
            __throw_system_error(__e);
        self->_M_i = false;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        self->_M_i = false;
    }
}

}  // namespace __atomic0
}  // namespace std

// ============================================================================
// force 发射（P147）
// ============================================================================
namespace
{

#include "OnlyOrigBatch7_Force/forceCtorEmissions.inc"
{
    // ---- 头内联值类 ctor（W）----
    WpBonusPointData wp;              (void)wp;
    _str_cost sc(0, 0);               (void)sc;
    _str_option_value_ratio ovr(0.0f, 0.0f, 0); (void)ovr;
    pieceQuestRewardStatus pqr;       (void)pqr;
    rewardSubKey rsk;                 (void)rsk;
    rewardKey rk;                     (void)rk;
    seriaRoomDecoRewardInfo srd;      (void)srd;
    stTempRewardInfo str;             (void)str;
    stAdjust sa;                      (void)sa;
    stLimitCubeResult slc;            (void)slc;
    stStackableBoosterElement_t sbe;  (void)sbe;
    stPvPGradeExp_t spe(0, 0);        (void)spe;
    stSetItemBackImageInfo ssb;       (void)ssb;
    upgrade_info_t ui;                (void)ui;
    stMonsterInfo_t smi;              (void)smi;
    stQuestShoptInitCost sqic;        (void)sqic;
    WriteLogSwitch wls;               (void)wls;
    stNPCRelationship_t snr;          (void)snr;
    tagCS_UPDATE_CHANNEL_INFO tcu;    (void)tcu;
    volatile int tsz = tcu.getSize(); (void)tsz;   // force _ZN15tagPacketHeader7getSizeEv
    stQuestTargetCharacter_t sqtc((ENUM_CHARACTERJOB)0, 0, 0); (void)sqtc;

    // ---- 析构（D1/D2）：new/delete 触发完整对象析构 ----
// b28d-solo-OnlyOrigBatch7_Force verdict=DIFF（N89 封顶）ORIG 0x08ad1320（W，
// 0x8d）orig/ours=50/27。ORIG 是 4.1 头内联的 std::string 析构：成员 +4 与
// +0 各自 `_M_p-0xc` sentinel 比较 + `lock xadd` 引用计数递减 + gthr 活跃检查
// + 引用计数归零时 call _ZNSs4_Rep10_M_destroyERKSaIcE。4.4 头集的
// basic_string::~basic_string 为 out-of-line（call _ZNSsD1Ev ×2），且对第二个
// 成员析构布 EH landing pad（_Unwind_Resume）。libstdc++ 头实现差异不可改
// （N89；`~stream_t() throw()` 探针实测不能消 landing pad）⇒ 语义等价封顶
// （两 string 成员按声明逆序析构，与 ORIG 相同）。
// [b29-R28 复核] p167：MISSING `_ZNSs4_Rep10_M_destroyERKSaIcE`×2（4.1 头内联
// `_M_dispose` 后只剩 out-of-line `_M_destroy`）vs EXTRA `_ZNSsD1Ev`×3 +
// `_Unwind_Resume`×1（4.4 头走 out-of-line `~basic_string` 且布 landing）
// = **libstdc++ 头实现代际差异，头文件不可改** ⇒ N89 封顶。
    // [b29-SymForceBatch] ORIG 仅发 stream_t D1/D2（W 0x08ad1320），无 C1/C2
    // （默认 ctor 从未 ODR-use）⇒ new/delete 改显式析构调用：只触发 dtor
    // 发射，不再发明 ctor 符号。force 函数不被调用，无运行时行为。
    CompiledDNFLex::stream_t* pst = 0;
    pst->~stream_t();
    stQuestTargetCharacter_t* psqtc =
        new stQuestTargetCharacter_t((ENUM_CHARACTERJOB)1, 0, 0);
    delete psqtc;

    // stPeriodRewardInfo（C1 定义 FavorableRelationDeps.cpp / D1 同 TU）
    stFavorableRelationShip_t::stPeriodRewardInfo spr;
    (void)spr;

    // stEVENTADDINFO D1/D2（ORIG W 0x08118b56 空体；new/delete 触发发射）
    stEVENTADDINFO* psea = new stEVENTADDINFO();
    delete psea;

    // ---- stBestRecord_t operator==/operator>（非 const 成员）----
    WongWork::CDeathTowerRanking::stBestRecord_t ba;
    WongWork::CDeathTowerRanking::stBestRecord_t bb;
    bool gt = ba > bb;
    bool eq = ba == bb;
    (void)gt; (void)eq;

    // ---- Zone::operator<（const 成员）----
    village_object::Zone za((unsigned char)0, (unsigned char)0);
    village_object::Zone zb((unsigned char)1, (unsigned char)1);
    bool zlt = za < zb;
    (void)zlt;
    village_attacked::Zone va((unsigned char)0, (unsigned char)0);
    village_attacked::Zone vb((unsigned char)1, (unsigned char)1);
    bool vlt = va < vb;
    (void)vlt;

    // ---- std::__atomic0::atomic_flag 成员（volatile）----
    std::__atomic0::atomic_flag af;
    bool tas = af.test_and_set(std::memory_order_seq_cst);
    af.clear(std::memory_order_seq_cst);
    (void)tas;
}

}  // namespace

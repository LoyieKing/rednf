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
tagCS_UPDATE_CHANNEL_INFO::tagCS_UPDATE_CHANNEL_INFO()
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
bool atomic_flag::test_and_set(memory_order) volatile
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    register bool __ret;
    if ((void*)&pthread_cancel != 0)
    {
        if (int __e = pthread_mutex_lock(&__mutex))
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
void atomic_flag::clear(memory_order) volatile
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

void forceCtorEmissions()
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
    CompiledDNFLex::stream_t* pst = new CompiledDNFLex::stream_t();
    delete pst;
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

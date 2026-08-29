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

// stNPCRelationship_t（ORIG C1 0x0848fa50 W）：4×ushort + int 清零，sizeof 0xc。
struct stNPCRelationship_t
{
    stNPCRelationship_t()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
        m_d = 0;
        m_e = 0;
    }
    unsigned short m_a;   // +0x00
    unsigned short m_b;   // +0x02
    unsigned short m_c;   // +0x04
    unsigned short m_d;   // +0x06
    int m_e;              // +0x08
};

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
};

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
// gcc4.4.7 的 atomic_0.h 仅声明成员，ORIG（旧工具链）内联互斥实现。
// 按 ORIG 语义补定义（pthread 互斥保护；ORIG 含 gthread 活跃检查，等价省略）。
// ============================================================================
namespace std
{
namespace __atomic0
{

static pthread_mutex_t g_atomic0FlagMutex = PTHREAD_MUTEX_INITIALIZER;

bool atomic_flag::test_and_set(memory_order) volatile
{
    pthread_mutex_lock(&g_atomic0FlagMutex);
    const bool r = _M_i;
    _M_i = true;
    pthread_mutex_unlock(&g_atomic0FlagMutex);
    return r;
}

void atomic_flag::clear(memory_order) volatile
{
    pthread_mutex_lock(&g_atomic0FlagMutex);
    _M_i = false;
    pthread_mutex_unlock(&g_atomic0FlagMutex);
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

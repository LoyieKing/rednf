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

// ORIG 静态池实例：MemPool<CAdvertisementExposeInfo>（ctor count 0x32 =
// 50 块，实证 __static_initialization_and_destruction_0 @0x080fdc63 push $0x32）。
#include "CInGameAdvertisementManager/advertisementExposeInfoPool_.inc"

// ORIG T 0x080fc998：直调 MemPool::alloc（反编译实证）。
void* CAdvertisementExposeInfo::operator new(unsigned int size)
{
    (void)size;
    return advertisementExposeInfoPool_.alloc();
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
    stInGameADExposeCondition& cond)
{
    std::memcpy(&m_condition, &cond, 0x84c);
}

// ORIG W 0x080fdd2c：返回 this（条件缓冲即对象首 0x84c）。
#include "CInGameAdvertisementManager/getExposeCondition.inc"
{
    return &m_condition;
}

// ORIG W _ZN27CInGameAdvertisementManagerC1Ev @0x080fdd6a：multimap 默认构造
// → time(0) → new CMTRand(0x9c8, seed) 存 +0x18。
#include "CInGameAdvertisementManager/CInGameAdvertisementManager.inc"
{
    time_t now = time(0);
    // ORIG 0x080fdd8a：time 结果槽 -0x1c 单槽直绑（lea -0x1c 早于 operator new，
    // %esi 跨调用缓存 &now）。const unsigned long& 形参 → time_t 槽同址绑定。
    // 4.4 对 &now 实参表达式自然产 lea -0x1c 并物化（P0：寄存器仅承载表达式值）。
    // b28d-solo-CInGameAdvertisementManager 判词（N 封顶，ORIG 0x080fdd8a）：
    // ORIG 4.1 在 `call _Znwj` **之前**发单条 `lea -0x1c(%ebp),%esi`（&now 直接
    // 提前入 callee-saved，无中间槽）；4.4.7 恒把 lea 放到 operator new 之后且用
    // %edx。本轮独立复测 9 形（/tmp/b28d_solo_probe{,2,3}.cpp：现形 /
    // reinterpret_cast 引用 cast / C cast / 裸语句表达式 ({&now;}) / volatile now /
    // 同型 unsigned long 直传 / 声明后赋值 / 具名指针局部 / 具名引用局部）：前 7 形
    // lea 恒在 new 之后（%edx）；仅具名中间局部形（指针/引用/unsigned long 副本）
    // 能让 lea 提前，但额外物化 1~2 个槽 + 2 条 store/load（ORIG 无该槽），形态更劣。
    // ⇒ 4.1「无副作用地址计算提前调度 + callee-saved 直绑」不可复现（P75 跨版本
    // 实参装载调度 + N7 寄存器分配族）。残差 3 行（lea 位置 + %esi/%edx）。
    // [L6S6b 2026-09-11 复核] 46/46 gap0 ndiff4：本轮重读 ORIG 0x080fdd8d/0x080fdd9c
    //   确认唯一差异就是 `lea -0x1c,%esi`（new 前）vs 我方 `lea -0x1c,%edx`（new 后）
    //   —— 上轮 9 形探针已穷举（含裸语句表达式），无一能让 4.4 把 lea 提前且不加槽，
    //   故维持 N 封顶（P75 实参装载调度 + N7），不再派单。
    m_rand = new CMTRand(*reinterpret_cast<unsigned long*>(&now));
}

// ORIG W _ZN27CInGameAdvertisementManagerD1Ev @0x080fca7c：delete m_rand(+0x18)
// → multimap 析构（ORIG 无 m_rand 置零）。
#include "CInGameAdvertisementManager/dtor_CInGameAdvertisementManager.inc"
{
    delete m_rand;
}

// ORIG T _ZN27CInGameAdvertisementManager16InitADExposeInfoEv @0x080fd122
// （报告 InitADExposeInfo.md）：遍历 multimap delete 各 info → clear(this)。
// ORIG 循环体将 it->second 存入命名局部(-0xc)再 delete（P28 槽位序）。
#include "CInGameAdvertisementManager/InitADExposeInfo.inc"
{
    for (std::multimap<short, CAdvertisementExposeInfo*>::iterator it =
             m_exposeInfo.begin();
         it != m_exposeInfo.end(); ++it)
    {
        CAdvertisementExposeInfo* info = it->second;
        delete info;
    }
    m_exposeInfo.clear();
}

// ORIG T _ZN27CInGameAdvertisementManager18InsertADExposeInfoER25stInGameADExposeCondition
// @0x080fd8e4（报告 InsertADExposeInfo.md）：new(0x850) → SetAdvertisementExposeInfo
// → multimap<short,CAdvertisementExposeInfo*>::insert(make_pair(cond+0x204, info))。
// 注：CAdvertisementExposeInfo 无用户 ctor（ORIG nm 无 C1/C2），new 不生成 ctor call。
#include "CInGameAdvertisementManager/InsertADExposeInfo.inc"
{
    // ORIG new 无括号：trivial 类不做 value-init（无 rep stos 清零、无 ctor call）
    CAdvertisementExposeInfo* info = new CAdvertisementExposeInfo;
    info->SetAdvertisementExposeInfo(cond);
    m_exposeInfo.insert(std::make_pair(cond.m_key, info));
}

// ORIG T _ZN27CInGameAdvertisementManager17queryADExposeInfoEP25stInGameADExposeCondition
// @0x080fd19e（报告 queryADExposeInfo.md 全转写）：DB_HANDLE_9 取连接，
// "seLect * from in_game_ad where visible = 1"；行 → 0x84c 条件记录逐列填充
//（失败即返 0）；循环后写哨兵（cond[i]+0x210 = -1）并双 trace，返 1。
// ORIG 形态：char 暂存槽 b(-0x15) 先于 db(-0x14) 声明；cMyTrace 临时链式
//（P37，__PRETTY_FUNCTION__）；无 rec 命名局部（每列 cond+i 内联重算）。
#include "CInGameAdvertisementManager/queryADExposeInfo.inc"
{
    bool b;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("seLect * from in_game_ad where visible = 1");
    b = db->exec(true);
    if (!b)
    {
        return false;
    }
    int nRows = db->get_n_rows();
    int i = 0;
    while (i < nRows)
    {
        b = db->fetch();
        if (!b)
        {
            break;
        }
        if (199 < i)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xeb, 5)(
                "[IN_GAME_ADVERTISEMENT_ERROR] count(%d)", nRows);
            break;
        }
        b = db->get_uint(0, reinterpret_cast<unsigned int&>(cond[i].m_sentinel));
        if (!b) return false;
        b = db->get_str(1, cond[i].m_title, 0x32);
        if (!b) return false;
        b = db->get_byte(2, cond[i].m_pad0[0]);
        if (!b) return false;
        b = db->get_byte(3, cond[i].m_pad0[1]);
        if (!b) return false;
        b = db->get_byte(4, cond[i].m_pad0[2]);
        if (!b) return false;
        b = db->get_str(5, cond[i].m_pad0 + 3, 0x200);
        if (!b) return false;
        b = db->get_str(6, cond[i].m_field246, 0xff);
        if (!b) return false;
        b = db->get_uint(7, cond[i].m_field444);
        if (!b) return false;
        b = db->get_str(8, cond[i].m_field345, 0xff);
        if (!b) return false;
        b = db->get_str(9, cond[i].m_field54a, 0xff);
        if (!b) return false;
        b = db->get_str(10, cond[i].m_field649, 0xff);
        if (!b) return false;
        b = db->get_uint(0xb, cond[i].m_field748);
        if (!b) return false;
        b = db->get_str(0xc, cond[i].m_field74c, 0xff);
        if (!b) return false;
        b = db->get_uint(0xd, cond[i].m_minCera);
        if (!b) return false;
        b = db->get_uint(0xe, cond[i].m_maxCera);
        if (!b) return false;
        b = db->get_byte(0xf, cond[i].m_field547);
        if (!b) return false;
        b = db->get_ubyte(0x11,
            reinterpret_cast<unsigned char&>(cond[i].m_pad0[0x203]));
        if (!b) return false;
        b = db->get_short(0x12, cond[i].m_key);
        if (!b) return false;
        b = db->get_str(0x14, cond[i].m_field448, 0xff);
        if (!b) return false;
        b = db->get_byte(0x15, cond[i].m_field548);
        if (!b) return false;
        ++i;
    }
    cMyTrace(__PRETTY_FUNCTION__, 300, 9, true, true)(
        "[!] Load In Game Advertisement Expose Info From DB(%d) Complete", nRows);
    cond[i].m_sentinel = -1;
    return true;
}

// ORIG 形态：if(q()){…;return true;} return false;（test %al; je 直测，P59）；
// 循环 i<=199（P2 setle）；无 tailPad（buf 恰 200*0x84c = 0x67b60）。
#include "CInGameAdvertisementManager/LoadADExposeInfoAtApp.inc"
{
    stInGameADExposeCondition buf[200];
    std::memset(buf, 0xff, 0x67b60);
    if (queryADExposeInfo(buf))
    {
        for (int i = 0; i <= 199; ++i)
        {
            if (buf[i].m_sentinel == -1)
            {
                break;
            }
            InsertADExposeInfo(buf[i]);
        }
        return true;
    }
    return false;
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
{
    if (user == 0)
    {
        return false;
    }
    char* list[0x79];
    int count = DNFFLib::ExplodeString(data, ",", list, 0x78);
    for (int i = 0; i < count; ++i)
    {
        int lv = atoi(list[i]);
        if (lv == -1)
        {
            return true;
        }
        if (user->get_charac_level() == lv)
        {
            return true;
        }
    }
    return false;
}

// ORIG T _ZN27CInGameAdvertisementManager14checkConditionEP5CUserP24CAdvertisementExposeInfo
// @0x080fcd2e（报告 checkCondition.md）：年龄表(G_CDataManager+0x4a5c)/
// 性别/等级/商城币区间/职业/账号取模逐项过滤，写 info 暴露标记(+0x549)。
#include "CInGameAdvertisementManager/checkCondition.inc"
    CAdvertisementExposeInfo* info)
{
    if (info == 0)
    {
        return false;
    }
    // b28d-36：ORIG 0x080fcd2c 段为 `mov -0xc(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
    // （基址入 edx、索引留 eax 的 scaled 形）。4.4 对**指针**下标恒折
    // `add base,%eax; movzbl (%eax)`（探针 /tmp/d36_probe_idx.cpp f1..f8 八形全败）；
    // 改用**数组引用**（探针 f9/f10）即逐条复现。
    char (&ageTbl)[1] =
        *reinterpret_cast<char(*)[1]>(&G_CDataManager()->m_inGameAd);  // +0x4a5c
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[1]) != 0xffu)
    {
        if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[1]) == 0)
        {
            if (ageTbl[user->GetAge()] == 1)
            {
                return false;
            }
        }
        else
        {
            if (ageTbl[user->GetAge()] == 0)
            {
                return false;
            }
        }
    }
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[0]) != 0xffu)
    {
        if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[0]) == 1)
        {
            if (static_cast<int>(user->getSex()) == 0)
            {
                return false;
            }
        }
        else
        {
            if (static_cast<int>(user->getSex()) == 1)
            {
                return false;
            }
        }
    }
    if (checkLevel(user, info->getExposeCondition()->m_pad0 + 3) != 1)
    {
        return false;
    }
    // 商城币区间：ORIG 嵌套三目汇聚（ja/jb + mov $1/jmp/mov $0 + test，无 bool 局部）
    if (info->getExposeCondition()->m_minCera >
            static_cast<unsigned int>(user->GetCera())
        ? true
        : (static_cast<unsigned int>(user->GetCera()) <
               info->getExposeCondition()->m_maxCera
           ? false
           : true))
    {
        return false;
    }
    if (static_cast<unsigned char>(info->getExposeCondition()->m_pad0[2]) != 0xffu)
    {
        if (user->get_charac_job() !=
            static_cast<signed char>(info->getExposeCondition()->m_pad0[2]))
        {
            return false;
        }
    }
    if (static_cast<unsigned char>(
            info->getExposeCondition()->m_pad0[0x203]) != 0)
    {
        if (user->get_acc_id() % static_cast<unsigned int>(
                static_cast<unsigned char>(
                    info->getExposeCondition()->m_pad0[0x203])) == 0)
        {
            info->getExposeCondition()->m_exposeFlag = 0;
        }
        else
        {
            info->getExposeCondition()->m_exposeFlag = 1;
        }
    }
    else
    {
        info->getExposeCondition()->m_exposeFlag = 1;
    }
    return true;
}

// ORIG T _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi
// @0x080fcfa8（报告 CheckIngameAdvertisementCondition.md）：multimap 按 short
// 键 [lower_bound, upper_bound) 遍历 → checkCondition 命中者 memcpy 0x63c 入
// vector → RandNum 随机取一条 rep movsl 0x63c 拷出。
// b28d-solo-CInGameAdvertisementManager：AE 闭合（无源码改动）。复验
// /tmp/b28d_solo_fcmp.py（AE 归一口径，新鲜 .o）verdict=AE 114/114；
// game_func_report 报 DIFF 全系 .o 未解析 call/jmp 重定位显示差（P264），非正文差。
#include "CInGameAdvertisementManager/CheckIngameAdvertisementCondition.inc"
    CUser* user, stNotifyIngameADInfo& info, int flag)
{
    stNotifyIngameADInfo buf;
    // ORIG：vec 先构造、it 后初始化（调用顺序）；upper_bound 在循环条件处
    std::vector<stNotifyIngameADInfo> vec;
    std::multimap<short, CAdvertisementExposeInfo*>::iterator it =
        m_exposeInfo.lower_bound(static_cast<short>(flag));
    while (it != m_exposeInfo.upper_bound(static_cast<short>(flag)))
    {
        CAdvertisementExposeInfo* ad = it->second;
        if (checkCondition(user, ad))
        {
            std::memcpy(&buf,
                &ad->getExposeCondition()->m_sentinel,   // +0x210（连续块起点）
                0x63c);
            vec.push_back(buf);
        }
        ++it;
    }
    int n = static_cast<int>(vec.size());
    if (n == 0)
    {
        return false;
    }
    // ORIG 尾部 rep movsl 0x18f×4：operator[] 内联后对齐已知，聚合赋值内联；
    // at() 边界检查路径会展开成通用逐段拷贝。
    info = vec[static_cast<unsigned int>(RandNum(n))];
    return true;
}

// ORIG W _ZN27CInGameAdvertisementManager7RandNumEi @0x080fdd34：
// m_rand->randInt() 无符号 % range（xor edx / div 实证）。
#include "CInGameAdvertisementManager/RandNum.inc"
{
    return static_cast<unsigned int>(m_rand->randInt()) %
           static_cast<unsigned int>(range);
}

// ORIG T _ZN27CInGameAdvertisementManager18ReloadADExposeInfoEv @0x080fd95e
//（报告 ReloadADExposeInfo.md）：stream 0x2f9 + (-1) → GetInBuffer 缓冲 →
// queryADExposeInfo 成功则 put(DISPATCH_Q) 广播重载。
#include "CInGameAdvertisementManager/ReloadADExposeInfo.inc"
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "InGameAdvertisementManager.cpp", 0x150), true);
    guard.operator*()->operator<<(0x2f9);
    guard.operator*()->operator<<(-1);
    SIG_RELOAD_IN_GAME_ADVERTISEMENT* sig =
        guard->GetInBuffer<SIG_RELOAD_IN_GAME_ADVERTISEMENT>();
    if (queryADExposeInfo(
            reinterpret_cast<stInGameADExposeCondition*>(sig)))
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return true;
    }
    return false;
}

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
MemPool<CAdvertisementExposeInfo> CAdvertisementExposeInfo::advertisementExposeInfoPool_(0x32);

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
void CAdvertisementExposeInfo::SetAdvertisementExposeInfo(
    stInGameADExposeCondition& cond)
{
    std::memcpy(&m_condition, &cond, 0x84c);
}

// ORIG W 0x080fdd2c：返回 this（条件缓冲即对象首 0x84c）。
stInGameADExposeCondition* CAdvertisementExposeInfo::getExposeCondition()
{
    return &m_condition;
}

// ORIG W _ZN27CInGameAdvertisementManagerC1Ev @0x080fdd6a：multimap 默认构造
// → time(0) → new CMTRand(0x9c8, seed) 存 +0x18。
CInGameAdvertisementManager::CInGameAdvertisementManager()
{
    time_t now = time(0);
    // ORIG lea &now 早于 operator new：time_t → const unsigned long& 临时绑定形。
    m_rand = new CMTRand(now);
}

// ORIG W _ZN27CInGameAdvertisementManagerD1Ev @0x080fca7c：delete m_rand(+0x18)
// → multimap 析构（ORIG 无 m_rand 置零）。
CInGameAdvertisementManager::~CInGameAdvertisementManager()
{
    delete m_rand;
}

// ORIG T _ZN27CInGameAdvertisementManager16InitADExposeInfoEv @0x080fd122
// （报告 InitADExposeInfo.md）：遍历 multimap delete 各 info → clear(this)。
// ORIG 循环体将 it->second 存入命名局部(-0xc)再 delete（P28 槽位序）。
void CInGameAdvertisementManager::InitADExposeInfo()
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
void CInGameAdvertisementManager::InsertADExposeInfo(stInGameADExposeCondition& cond)
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
bool CInGameAdvertisementManager::queryADExposeInfo(stInGameADExposeCondition* cond)
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
bool CInGameAdvertisementManager::LoadADExposeInfoAtApp()
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
bool CInGameAdvertisementManager::checkLevel(CUser* user, char* data)
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
bool CInGameAdvertisementManager::checkCondition(CUser* user,
    CAdvertisementExposeInfo* info)
{
    if (info == 0)
    {
        return false;
    }
    char* ageTbl = reinterpret_cast<char*>(&G_CDataManager()->m_inGameAd);  // +0x4a5c
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
bool CInGameAdvertisementManager::CheckIngameAdvertisementCondition(
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
unsigned int CInGameAdvertisementManager::RandNum(int range)
{
    return static_cast<unsigned int>(m_rand->randInt()) %
           static_cast<unsigned int>(range);
}

// ORIG T _ZN27CInGameAdvertisementManager18ReloadADExposeInfoEv @0x080fd95e
//（报告 ReloadADExposeInfo.md）：stream 0x2f9 + (-1) → GetInBuffer 缓冲 →
// queryADExposeInfo 成功则 put(DISPATCH_Q) 广播重载。
bool CInGameAdvertisementManager::ReloadADExposeInfo()
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

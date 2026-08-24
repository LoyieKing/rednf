// ============================================================================
// df_game_r 还原 —— RestrictCategory + ServiceRestrictManager
// 权威实现（原 GlobalData.cpp 本地类迁移合并；报告 docs/class_func_reports/
// ServiceRestrictManager/）。
//   initialize 0x816e2b2 / savePolicy 0x816e8ce / loadPolicy 0x816e7d6 /
//   isRestricted(CUser*,Enum,int) 0x816e6b8 / isRestricted(Enum,int) 0x816e75e /
//   ctor 0x816e272。
// 策略类 InterfaceServicePolicy 的 vtable 槽 +0x14 为 isRestricted 虚入口
// （ORIG 虚调用 `call *(vtable+0x14)`，单 int 参数）；槽 +0x8/+0xc/+0x10 为
// getCode/save 等（推断：具体签名未还原，按 GlobalData 迁移版本建模）。
// ============================================================================

#include "ServiceRestrictManager.h"
#include "MsgQueueMgr.h"

#include "CEnvironment.h"
#include "CStreamGuard.h"
#include "CUser.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "TaiwanInternal.h"

#include <new>

// ---- Taiwan::GarenaAuthData（ORIG _ZN6Taiwan14GarenaAuthData13getMobileAuthEv
// 0x8173022：返回 this+0x2c 字节 == 1）。CUser::getGarenaAuthData 返回该缓冲。----
namespace Taiwan
{
struct GarenaAuthData
{
    char m_pad[0x2c];
    unsigned char m_mobileAuth;   // +0x2c

    bool getMobileAuth() const { return m_mobileAuth == 1; }  // ORIG 0x8173022
};
}  // namespace Taiwan

class CStreamGuard;

namespace
{

// ---- InterfaceServicePolicy（ORIG 抽象接口；vtable+0x14 = isRestricted 入口）----
struct NullPolicy;
struct AuthMobileRestrictPolicy;
struct CreateCharacterRestrictPolicy;
struct SecurityRestrictPolicy;

}  // namespace

class InterfaceServicePolicy
{
public:
    virtual ~InterfaceServicePolicy() {}                       // vtable +0x00/+0x04
    virtual int getCode() const = 0;                           // +0x08
    virtual void save(Taiwan::SigLoadRestrictPolicy*) {}       // +0x0c【推断】
protected:
    virtual void unknown_10() {}                               // +0x10【推断】占位槽
public:
    virtual bool isRestricted(int /*v*/) { return false; }     // +0x14（ORIG 虚调用槽）
};

namespace
{

struct NullPolicy : InterfaceServicePolicy
{
    virtual int getCode() const { return 0; }
};
struct AuthMobileRestrictPolicy : InterfaceServicePolicy
{
    virtual int getCode() const { return 1; }
};
struct CreateCharacterRestrictPolicy : InterfaceServicePolicy
{
    virtual int getCode() const { return 2; }
};
struct SecurityRestrictPolicy : InterfaceServicePolicy
{
    virtual int getCode() const { return 3; }
};

}  // namespace

// ---- ctor / dtor（ORIG 0x816e272：map 构造 + 两标志字节清零）----
ServiceRestrictManager::ServiceRestrictManager() : m_policyMap(), m_flag18(0), m_flag19(0)
{
}

ServiceRestrictManager::~ServiceRestrictManager()
{
    for (std::map<int, InterfaceServicePolicy*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        delete it->second;
        it->second = 0;
    }
}

// ORIG 0x0816e2b2：channel_no==1 → 置 +0x19=1；创建 4 个策略对象注册进 map。
bool ServiceRestrictManager::initialize()
{
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        m_flag19 = 1;
    }
    m_policyMap[0] = new (std::nothrow) NullPolicy;
    m_policyMap[1] = new (std::nothrow) AuthMobileRestrictPolicy;
    m_policyMap[2] = new (std::nothrow) CreateCharacterRestrictPolicy;
    m_policyMap[3] = new (std::nothrow) SecurityRestrictPolicy;
    if (m_policyMap[0] == 0 || m_policyMap[1] == 0 ||
        m_policyMap[2] == 0 || m_policyMap[3] == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              "bool ServiceRestrictManager::initialize()", 0x169,
                              "[Taiwan, Restrict] Fail create NullPolicy.");
        return false;
    }
    return true;
}

// ORIG 0x0816e8ce：+0x19==1 时遍历 map，每条策略经 SigSaveRestrictPolicy 流序列化
// 后入 DB 队列。策略 save() 为虚调用（策略数据未还原，推断）。
void ServiceRestrictManager::savePolicy()
{
    if (m_flag19 != 1)
    {
        return;
    }
    for (std::map<int, InterfaceServicePolicy*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1de), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace t("void ServiceRestrictManager::savePolicy()", 0x1e3, 5);
            t("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
            return;
        }
        it->second->save(sig);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0816e7d6：SigLoadRestrictPolicy 请求（internal_stream(1,-1)）入 DB 队列。
void ServiceRestrictManager::loadPolicy()
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/ServiceRestrictManager.cpp", 0x1c5), true);
    Taiwan::internal_stream(guard, TaiwanInternalPack::T(1), 0xffffffff);
    Taiwan::SigLoadRestrictPolicy* sig =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (sig == 0)
    {
        LogManager::logFormat(1, "localtaiwan/System/ServiceRestrictManager.cpp",
                              "void ServiceRestrictManager::loadPolicy()", 0x1cc,
                              "[Taiwan, Restrict] SigLoadRestrictPolicy is null.");
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}

// ORIG 0x0816e6b8：user 空→false；已开 Garena 手机认证→false；map 无该类别→true；
// 否则调策略虚函数（vtable+0x14）。
bool ServiceRestrictManager::isRestricted(CUser* user,
                                          RestrictCategory::Enum category, int v)
{
    if (user == 0)
        return false;
    Taiwan::GarenaAuthData* auth =
        reinterpret_cast<Taiwan::GarenaAuthData*>(user->getGarenaAuthData());
    if (auth->getMobileAuth())
        return false;
    std::map<int, InterfaceServicePolicy*>::iterator it = m_policyMap.find(category);
    if (it == m_policyMap.end())
        return true;
    return it->second->isRestricted(v);
}

// ORIG 0x0816e75e（WarField 用）：map 无该类别→true；否则策略虚调用。
bool ServiceRestrictManager::isRestricted(RestrictCategory::Enum category, int v)
{
    std::map<int, InterfaceServicePolicy*>::iterator it = m_policyMap.find(category);
    if (it == m_policyMap.end())
        return true;
    return it->second->isRestricted(v);
}

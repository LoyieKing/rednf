// SecurityRestrictPolicy —— Taiwan 服务限制策略（策略代码 3）
// ORIG 符号与布局证据：docs/class_func_reports/SecurityRestrictPolicy/（9 报告已读）。
//   ctor T 0x0816da50 / D1 T 0x0816dae0 / D0 T 0x0816db10 /
//   beginPolicy 0x0816db2e / endPolicy 0x0816db5a / isApply 0x0816db66 /
//   isApply(int) 0x0816db72 / printRestrict 0x0816dbe8 / getRestrictValue 0x0816dd22
// 布局（ORIG ctor 反汇编实证）：+0x4 bool m_apply（ctor 清 0）、
// +0x8 int m_code（基类 ctor 参数 3，getCode 读）、+0xc ServicePolicy m_svc
// （= vector<short>，ctor push 两条 1）。
//
// InterfaceServicePolicy（抽象接口，vtable @0x8b82f68，8 槽）：
//   0=D1 1=D0 2=beginPolicy 3=endPolicy 4=isApply() 5=isApply(int)
//   6=printRestrict 7=getRestrictValue（2-7 为 __cxa_pure_virtual 占位纯虚）。
//   getCode 为【非虚】成员（W 0x0816ee12，读 +0x8；无 vtable 槽）。
//   ctor W 0x0816ed98(RestrictCategory::Enum)：存 code + ServicePolicy 子对象构造；
//   +0x4 不在基类 ctor 初始化（ORIG 同，派生策略 ctor 各自清 0）。
//   dtor W 0x0816edc0/D0 W 0x0816edf4（虚）。
#ifndef DNF_GAME_SECURITYRESTRICTPOLICY_H_
#define DNF_GAME_SECURITYRESTRICTPOLICY_H_

#include <string>

#include "ServicePolicy.h"
#include "ServiceRestrictManager.h"   // RestrictCategory::Enum

namespace Taiwan { class SigLoadRestrictPolicy; }

class InterfaceServicePolicy
{
public:
    InterfaceServicePolicy(RestrictCategory::Enum code) : m_code(code), m_svc() {}
    virtual ~InterfaceServicePolicy() {}
    // ORIG W 0x0816ee12 _ZN22InterfaceServicePolicy7getCodeEv（非虚、非 const）。
    int getCode() { return m_code; }

    virtual bool beginPolicy(std::string const&) = 0;                 // vtable +0x08
    virtual void endPolicy() = 0;                                     // +0x0c
    virtual bool isApply() = 0;                                       // +0x10
    virtual bool isApply(int) = 0;                                    // +0x14
    virtual void printRestrict() = 0;                                 // +0x18
    virtual void getRestrictValue(Taiwan::SigLoadRestrictPolicy&) = 0;  // +0x1c

protected:
    bool m_apply;                  // +0x04（派生策略 ctor 清 0；基类 ctor 不初始化）
    // +0x08（ctor 参数）。类型与形参 RestrictCategory::Enum 同型：
    // ORIG C1 W 0x0816ee? store 形 `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx;
    // mov %edx,0x8(%eax)`（LHS-first）= 4.4 同型 store 形（探针 p211.cpp B5）；
    // int 字段←enum 形参异型反产 RHS-first（B1/B2 实测）。
    RestrictCategory::Enum m_code;
    ServicePolicy m_svc;           // +0x0c
};

class SecurityRestrictPolicy : public InterfaceServicePolicy
{
public:
    SecurityRestrictPolicy();   // ORIG T @0x0816da50
    ~SecurityRestrictPolicy();  // ORIG T @0x0816dae0 / D0 @0x0816db10

    virtual bool beginPolicy(std::string const&);         // @0x0816db2e
    virtual void endPolicy();                             // @0x0816db5a
    virtual bool isApply();                               // @0x0816db66
    virtual bool isApply(int);                            // @0x0816db72
    virtual void printRestrict();                         // @0x0816dbe8
    virtual void getRestrictValue(Taiwan::SigLoadRestrictPolicy&);  // @0x0816dd22
};

#endif  // DNF_GAME_SECURITYRESTRICTPOLICY_H_

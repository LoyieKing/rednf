// RestrictPolicy —— ServiceRestrictManager 三个具体策略类（独占 ServiceRestrictManager TU）
// ORIG 全局具名类（nm -S df_game_r 实证；策略类报告缺失 → objdump 转写，见
// docs/class_func_reports/ServiceRestrictManager/README.md 无对应条目）：
//   NullPolicy                ctor 0x0816d5aa / D1/D2 0x0816d608 / D0 0x0816d638 /
//                             beginPolicy 0x0816d656 / endPolicy 0x0816d660 /
//                             isApply 0x0816d666 / isApply(int) 0x0816d670 /
//                             printRestrict 0x0816d67a / getRestrictValue 0x0816d6c8
//   AuthMobileRestrictPolicy  ctor 0x0816d6ce / D1/D2 0x0816d75e / D0 0x0816d78e /
//                             beginPolicy 0x0816d7ac / endPolicy 0x0816d7d8 /
//                             isApply 0x0816d7e4 / isApply(int) 0x0816d7f0 /
//                             printRestrict 0x0816d866 / getRestrictValue 0x0816d9a0
//   CreateCharacterRestrictPolicy
//                             ctor 0x0816ddd2 / D1/D2 0x0816de64 / D0 0x0816de94 /
//                             beginPolicy 0x0816deb2 / endPolicy 0x0816dede /
//                             isApply 0x0816deea / isApply(int) 0x0816def6 /
//                             printRestrict 0x0816df6c / getRestrictValue 0x0816e0a6
// vtable（8 槽，含 offset/typeinfo 后 6 虚函数）与基类 InterfaceServicePolicy 一致
// （见 SecurityRestrictPolicy.h）；各派生 ctor 经基类 ctor 存 code、清 m_apply、
// ServicePolicy::Init。
#ifndef DNF_GAME_RESTRICTPOLICY_H_
#define DNF_GAME_RESTRICTPOLICY_H_

#include <string>

#include "SecurityRestrictPolicy.h"   // InterfaceServicePolicy / ServicePolicy

namespace Taiwan { class SigLoadRestrictPolicy; }

class NullPolicy : public InterfaceServicePolicy
{
public:
    NullPolicy();
    ~NullPolicy();
    virtual bool beginPolicy(std::string const&);
    virtual void endPolicy();
    virtual bool isApply();
    virtual bool isApply(int);
    virtual void printRestrict();
    virtual void getRestrictValue(Taiwan::SigLoadRestrictPolicy&);
};

class AuthMobileRestrictPolicy : public InterfaceServicePolicy
{
public:
    AuthMobileRestrictPolicy();
    ~AuthMobileRestrictPolicy();
    virtual bool beginPolicy(std::string const&);
    virtual void endPolicy();
    virtual bool isApply();
    virtual bool isApply(int);
    virtual void printRestrict();
    virtual void getRestrictValue(Taiwan::SigLoadRestrictPolicy&);
};

class CreateCharacterRestrictPolicy : public InterfaceServicePolicy
{
public:
    CreateCharacterRestrictPolicy();
    ~CreateCharacterRestrictPolicy();
    virtual bool beginPolicy(std::string const&);
    virtual void endPolicy();
    virtual bool isApply();
    virtual bool isApply(int);
    virtual void printRestrict();
    virtual void getRestrictValue(Taiwan::SigLoadRestrictPolicy&);
};

#endif  // DNF_GAME_RESTRICTPOLICY_H_

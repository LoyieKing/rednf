// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// TaiwanInternalPack::T 完整枚举定义见 MoreTypes.h:113。
// getCode() const 返回类型 demangled 缺失，按 Arad_IInterDispatcher::get_code 同型写 T【推断】。
#ifndef DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_
#define DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_

#include "MoreTypes.h"

// vtable 槽序 = [~D1 @+0x08][~D0 @+0x0c][纯虚 dispatch_sig @+0x10]（ORIG
// 基类 vtable 槽2=__cxa_pure_virtual；具体子类如 InterCheckStayTimeEvent
// vptr+8=dispatch_sig，见 ORIG _ZTV 实证）。
class CUser;

class TaiwanInterfaceInterDispatcher
{
public:
    TaiwanInterfaceInterDispatcher(TaiwanInternalPack::T) throw();   // W 0x08161e32（ORIG new 链无 EH → nothrow）
    virtual ~TaiwanInterfaceInterDispatcher();                       // W D1 0x08161e4a / D0 0x08161e70
    virtual int dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/) { return 0; }
    TaiwanInternalPack::T getCode() const;                    // W 0x08161e8e（mov 0x4(%eax) 直返，无规范化）

private:
    TaiwanInternalPack::T m_code;   // +0x04（ORIG ctor 08161e32 存 0xc(%ebp) → 0x4(%eax)；getCode 读 0x4）
};

#endif  // DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_

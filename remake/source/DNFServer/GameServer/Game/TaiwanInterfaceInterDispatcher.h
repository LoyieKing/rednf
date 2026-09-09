// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// TaiwanInternalPack::T 完整枚举定义见 MoreTypes.h:113。
// getCode() const 返回类型 demangled 缺失，按 Arad_IInterDispatcher::get_code 同型写 T【推断】。
#ifndef DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_
#define DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_

#include "MoreTypes.h"

// vtable 槽序 = [~D1 @+0x08][~D0 @+0x0c][纯虚 dispatch_sig @+0x10]（ORIG 基类
// vtable 0x08b7ad00 尺寸 0x14，槽2=__cxa_pure_virtual 0x8724fe0；具体子类如
// InterCheckStayTimeEvent vptr+8=dispatch_sig，见 ORIG _ZTV 实证）。ORIG 无
// _ZN30TaiwanInterfaceInterDispatcher12dispatch_sigEP5CUserPci ⇒ dispatch_sig
// 必为纯虚，基类不得提供定义（否则发射该 vague-linkage 符号，违反 ORIG 符号账）。
class CUser;

class TaiwanInterfaceInterDispatcher
{
public:
    TaiwanInterfaceInterDispatcher(TaiwanInternalPack::T) throw();   // W 0x08161e32（ORIG new 链无 EH → nothrow）
    virtual ~TaiwanInterfaceInterDispatcher();                       // W D1 0x08161e4a / D0 0x08161e70
    virtual int dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/) = 0;
    TaiwanInternalPack::T getCode() const;                    // W 0x08161e8e（mov 0x4(%eax) 直返，无规范化）

private:
    TaiwanInternalPack::T m_code;   // +0x04（ORIG ctor 08161e32 存 0xc(%ebp) → 0x4(%eax)；getCode 读 0x4）
};

#endif  // DNF_GAME_TAIWANINTERFACEINTERDISPATCHER_H_

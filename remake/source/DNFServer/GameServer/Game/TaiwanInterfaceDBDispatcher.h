// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only（MoreTypes.h 提供 TaiwanInternalPack::T）
#ifndef DNF_GAME_TAIWANINTERFACEDBDISPATCHER_H_
#define DNF_GAME_TAIWANINTERFACEDBDISPATCHER_H_

#include "MoreTypes.h"

// ORIG 符号：
//   W 0x0815df66 _ZN26TaiwanInterfaceDBDispatcherC1E18TaiwanInternalPackT
//   W 0x0815dfa4 dtor（D0 区；0x0815df7e D1/D2 同址别名）
//   W 0x0815dfc2 _ZNK26TaiwanInterfaceDBDispatcher7getCodeEv
// getCode 返回类型 demangled 缺失，按 TaiwanInternalPack::T 值回传写 T【推断】。
// vtable 槽序 = [~D1 @+0x08][~D0 @+0x0c][纯虚 dispatch @+0x10]（ORIG 基类
// vtable 0x8b79470 槽2=__cxa_pure_virtual 0x8724fe0；具体子类如
// LoadServiceRestrictPolicy vptr+8=dispatch @0x815a8f2）。
class Stream;

class TaiwanInterfaceDBDispatcher
{
public:
    TaiwanInterfaceDBDispatcher(TaiwanInternalPack::T) throw();  // ORIG W 0x0815df66（ORIG new 链无 EH → nothrow）
    virtual ~TaiwanInterfaceDBDispatcher();                      // ORIG W 0x0815dfa4
    virtual bool dispatch(int /*code*/, int /*uid*/, Stream* /*stream*/) { return false; }
    TaiwanInternalPack::T getCode() const;               // ORIG W 0x0815dfc2（返回 T【推断】）

private:
    TaiwanInternalPack::T m_code;   // +0x04（ORIG ctor 0815df66 存 0xc(%ebp) → 0x4(%eax)；getCode 读 0x4）
};

#endif  // DNF_GAME_TAIWANINTERFACEDBDISPATCHER_H_

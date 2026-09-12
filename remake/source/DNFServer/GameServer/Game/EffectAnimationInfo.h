#ifndef GAME_EFFECTANIMATIONINFO_H_
#define GAME_EFFECTANIMATIONINFO_H_

#include <string>

// HdrCreate2 批：ORIG 导出弱符号（头内联 W），C1==C2、D1==D2 同址。
// 布局：+0x00 int / +0x04 int / +0x08 std::string（gcc4.4 COW 单指针，sizeof=4，总大小 0xc）。
// C1 先隐式构造 string 再调 clear()；D1 仅析构 string（无虚函数，无 vptr）。

class EffectAnimationInfo
{
public:
    // ORIG 0x89c2d3a _ZN19EffectAnimationInfoC1Ev (size 0x48)
    EffectAnimationInfo() { clear(); }
    // ORIG 0x89c4fea _ZN19EffectAnimationInfoD1Ev (size 0x16)
    ~EffectAnimationInfo() {}
    // ORIG 0x89c2d10 _ZN19EffectAnimationInfo5clearEv (size 0x29)
    void clear() { m_a = 0; m_b = 0; m_str.clear(); }

    // [R7a05 2026-09-12 ④] 三成员为 public：ORIG 自由函数 readCondtion 在
    //   case 0x10fb 直接写对象自身 —— 0x089a02fc `lea -0x748(%ebp),%eax;
    //   add $0x8,%eax; ...call ScanStr(std::string*)`（= &eai.m_str，基址 -0x748）、
    //   0x089a0319 `mov %eax,-0x748(%ebp)`（= eai.m_a）、0x089a032b
    //   `mov %eax,-0x744(%ebp)`（= eai.m_b）⇒ 原码头成员可见（无 setter/无友元）。
    //   sizeof=0xc / 偏移 / mangled / 符号集全不变。
    int m_a;            // +0x00
    int m_b;            // +0x04
    std::string m_str;  // +0x08
};

#endif  // GAME_EFFECTANIMATIONINFO_H_

// CRabbitBox ORIG 符号实例化 TU（头内 inline 定义，本 TU 强制发出弱符号）。
#include "AradRabbitBox.h"

namespace
{
void forceEmit()
{
    CRabbitBox a;
    CRabbitBox b(a);              // C1 拷贝构造 0x0852488c
    CRabbitBox c;
    c = b;                        // operator= 0x08197ec2
    volatile int v = b.getItemIndex() + b.getItemCount();  // 0x08197e9e / 0x08197eaa
    b.setItemIndex(1);            // 0x089c3338
    b.SetItemCount(2);            // 0x089c3346
    b.SetDropRate(3);             // 0x089c3354
    delete new CRabbitBox();      // D0 0x08197e80（内部 call D1 0x08197e5a）
    (void)a; (void)c; (void)v;
}
}

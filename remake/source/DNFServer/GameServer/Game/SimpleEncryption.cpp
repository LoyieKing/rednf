// SimpleEncryption 仅ORIG 11 符号真实定义（9 函数 + 无额外辅助）。
// 证据：docs/class_func_reports/SimpleEncryption/*.md（9 份报告全文已读）。
// 关键点：
//  - instance() 为函数局部 static 单例（P20），局部名 simpleEncryption 对应
//    _ZZN16SimpleEncryption8instanceEvE16simpleEncryption（16=名字长度实测）。
//  - 单参 decryptUINT 在 ORIG 中调用的是双参 encryptUINT（0x0808cfd8 call 0x0808cf24），
//    非双参 decryptUINT——照抄 ORIG 行为。
//  - C1 用函数体逐句赋值（P29：初始化列表会缓存 this 到 ecx，ORIG 三次重读 0x8(%ebp)）。
//  - 双参 encrypt/decryptUINT：ORIG 局部先 movl $0 清零再赋值（0x0808cf2a/0x0808cf6e）；
//    u 以 int 声明、使用处 (unsigned int)u ^ shift，复现 ORIG xor 左先物化形态。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16SimpleEncryptionC2Ev`（ORIG 0x808d043）；ctor 已是首强符号。
#include <iostream>

#include "SimpleEncryption.h"

SimpleEncryption::SimpleEncryption()
{
    m_shift = 0;
    m_key = 0;
    initialize();
}

SimpleEncryption::~SimpleEncryption()
{
    uninitialize();
}

SimpleEncryption* SimpleEncryption::instance()
{
    static SimpleEncryption simpleEncryption;
    return &simpleEncryption;
}

void SimpleEncryption::initialize()
{
    m_shift = 3;
    m_key = 0xf089980f;
}

void SimpleEncryption::uninitialize()
{
}

void SimpleEncryption::encryptUINT(unsigned int shift, unsigned int& out)
{
    unsigned int t = 0;   // -0x8 槽
    int u = 0;            // -0x4 槽
    t = m_shift;
    u = (int)m_key;
    out = shift << t;
    out = (unsigned int)u ^ shift;
}

unsigned int SimpleEncryption::encryptUINT(unsigned int val)
{
    unsigned int out = 0;
    encryptUINT(val, out);
    return out;
}

void SimpleEncryption::decryptUINT(unsigned int shift, unsigned int& out)
{
    unsigned int t = 0;
    int u = 0;
    t = m_shift;
    u = (int)m_key;
    out = shift >> t;
    out = (unsigned int)u ^ shift;
}

unsigned int SimpleEncryption::decryptUINT(unsigned int val)
{
    unsigned int out = 0;
    encryptUINT(val, out);   // ORIG 0x0808cfd8 call 双参 encryptUINT（0x0808cf24），照抄
    return out;
}

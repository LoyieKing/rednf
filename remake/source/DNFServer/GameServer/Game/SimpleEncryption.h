// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + SimpleEncryption 报告
// 报告：docs/class_func_reports/SimpleEncryption/*.md
// 布局证据（initialize @0x0808cf06）：+0x00 = 3（int，shift 槽），+0x04 = 0xf089980f（unsigned，key 槽）。
// encryptUINT/decryptUINT(unsigned int, unsigned int&) 非静态（arg0=this）；单参重载为成员 __thiscall。
// sizeof 未证实（静态实例 b 0x093f5890），不加断言。
#ifndef DNF_GAME_SIMPLEENCRYPTION_H_
#define DNF_GAME_SIMPLEENCRYPTION_H_

class SimpleEncryption
{
public:
    SimpleEncryption();      // T 0x0808ce68
    ~SimpleEncryption();     // T 0x0808ce8e

    static SimpleEncryption* instance();   // T 0x0808cea2【推断返回 SimpleEncryption*（guard 单例）】

    void initialize();     // T 0x0808cf06（m_shift=3, m_key=0xf089980f）
    void uninitialize();   // T 0x0808cf1e（空体）

    void encryptUINT(unsigned int shift, unsigned int& out);   // T 0x0808cf24
    unsigned int encryptUINT(unsigned int val);                // T 0x0808cfac（报告 Ghidra C：uint 返回）
    void decryptUINT(unsigned int shift, unsigned int& out);   // T 0x0808cf68
    unsigned int decryptUINT(unsigned int val);                // T 0x0808cfd8（调 encryptUINT 单参型，对称）

    int m_shift;          // +0x00【推断】initialize 写 3
    unsigned int m_key;   // +0x04【推断】initialize 写 0xf089980f
};

#endif  // DNF_GAME_SIMPLEENCRYPTION_H_

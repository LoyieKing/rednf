// ---- IMethod 抽象基类（G7 加密模式基类，算法正确即可）----
IMethod::IMethod()
    // [R20scanNEW §26.3 · 2026-09-13] ORIG _ZN7IMethodC1Ev 0x80b4294 逐条实证：
    //   vptr + `movb $0x0,0x4` + `movl $0x0` 到 +0x8/+0xc/+0x10/+0x14（全 0 版本）。
    //   消费点 IMethod::Xor 0x80b4316 `mov 0x8(%eax),%eax; cmp -0x4(%ebp),%eax; setg`
    //   ⇒ m_blockSize@+0x8 作循环上界；原写 8 使未设置时 Xor 改写 8 字节（ORIG 为 0 次）。
    //   派生类（CRijndael::C1 m_blockSize=0x10 / CTEA 类 =8）各自设置，与本默认无关。
    : m_bInit(false), m_blockSize(0), m_keyLength(0), m_mode(0), m_padding(0)
{
}
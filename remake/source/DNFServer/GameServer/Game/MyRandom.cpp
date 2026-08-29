#include "MyRandom.h"

// ORIG 0x8aafe18 _ZN8MyRandomC1Ei (0xd)：
//   mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop; ret
// 仅保存种子到 m_seed。
MyRandom::MyRandom(int seed)
{
    m_seed = seed;
}

// ORIG 0x8aafe26 _ZN8MyRandomclEi (0x14)：
//   mov (%eax),%eax; mov %eax,%edx; sar $0x1f,%edx; idivl 0xc(%ebp); mov %edx,%eax
// 有符号除法取余：返回 m_seed % n，不更新 m_seed。
int MyRandom::operator()(int n)
{
    return m_seed % n;
}

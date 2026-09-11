// ORIG 0x8aafe18 _ZN8MyRandomC1Ei (0xd)：
//   mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop; ret
// 仅保存种子到 m_seed。
MyRandom::MyRandom(int seed)
{
    m_seed = seed;
}

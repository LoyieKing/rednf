void CUser::SetAge(unsigned int age)
{
    // ORIG 0x0823xxxx: mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x711e0(%eax)
    // ORIG SetAge：`mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x711e0(%eax)` this 先。
    // 直接赋值（uint=uint 同型）由 Main 落 CUser.h m_age→unsigned int 后复现（勿加 cast）
    m_age = age;

random_option::CRandomOptionItemHandle::~CRandomOptionItemHandle()
{
    // ORIG 0x085f0e46：vptr 前置写 + delete[]（guard/da），无成员清零。
    delete[] m_rand;

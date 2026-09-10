CRandomOptionItemHandleTest::CRandomOptionItemHandleTest()
{
    // ORIG 0x0822cb70：一次 this 读共享两 store（8 字节整块）
    *(long long*)this = 0;

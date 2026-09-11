// ORIG C1 0x0822cb70：mov this 一次缓存后连发两 store（P29 初始化列表形态）。
CRandomOptionItemHandleTest::CRandomOptionItemHandleTest()
{
    // ORIG 0x0822cb70：一次 this 读共享两 store（8 字节整块）
    *(long long*)this = 0;
}
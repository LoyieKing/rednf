// ORIG D1 0x8ac1dce W：clearScript() 后 string +0x0/+0x4 与 map +0x8 由编译器按成员序析构
// （P12 真实析构，非空桩——成员析构仍生成）。
DeleteInvalidItemScript::~DeleteInvalidItemScript()
{
    // ORIG D1 0x8ac1dce：先调 clearScript()（+0x0e call）再析构成员
    clearScript();
}
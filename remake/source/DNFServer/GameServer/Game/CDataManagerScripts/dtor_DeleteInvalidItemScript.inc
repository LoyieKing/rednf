DeleteInvalidItemScript::~DeleteInvalidItemScript()
{
    // ORIG D1 0x8ac1dce：先调 clearScript()（+0x0e call）再析构成员
    clearScript();

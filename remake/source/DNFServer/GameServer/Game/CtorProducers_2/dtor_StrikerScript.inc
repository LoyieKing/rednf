StrikerScript::~StrikerScript()
{
    // ORIG 0x08a9da28：destory() 后逆序析构容器成员。
    destory();

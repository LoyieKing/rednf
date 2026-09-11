// ORIG T 0x081a2dea：先写 vptr（基类 Arad_ScriptLoader vtable），再逆序析构
// +0x1c map<string,int> 与 +0x04 map<long long,Data>（无虚析构，按值析构）。
Arad_GiftItem_Set::~Arad_GiftItem_Set()
{
}
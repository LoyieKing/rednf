// ORIG T 0x08156b38（dtor）：ORIG 汇编开头即调 initStatistics（Guard 清双 map），
// 然后成员自动逆序析构。
CAdvanceAltarLog::~CAdvanceAltarLog()
{
    initStatistics();
}
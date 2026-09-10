CNPCScriptList::~CNPCScriptList()
{
    // ORIG D1 0x83762a8：_destroy() + stNPCCommonData_t D1 + hash_map D1
    _destroy();

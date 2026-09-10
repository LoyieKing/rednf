int Inter_LoadCleanPadPoint::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    int* point = reinterpret_cast<int*>(data);       // 整包首槽指针视图（非成员访问）
    WongWork::CMCAPManager::setPointPerHackType(point);
    return 0;

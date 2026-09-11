// ORIG T 0x080f8be0：trace 后把 0xf3c 个 int 拷贝到 pointPerHackType_。
void CMCAPManager::setPointPerHackType(int* data)
{
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x1d2, 0);
    trace("Load cleanpad point per hack type");
    for (int i = 0; i <= 0xf3b; ++i)
        pointPerHackType_[i] = data[i];
}
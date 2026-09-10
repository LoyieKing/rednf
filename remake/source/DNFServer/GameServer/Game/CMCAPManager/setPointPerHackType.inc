void CMCAPManager::setPointPerHackType(int* data)
{
    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x1d2, 0);
    trace("Load cleanpad point per hack type");
    for (int i = 0; i <= 0xf3b; ++i)
        pointPerHackType_[i] = data[i];

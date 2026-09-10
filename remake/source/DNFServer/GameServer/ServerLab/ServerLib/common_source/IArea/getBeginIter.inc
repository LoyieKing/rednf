MAP_OBJECTS_ITER IArea::getBeginIter()
{
    // ORIG：先写成员 mMapIter 再返回（-O0 经 -0xc 临时槽拷贝，
    // 与 begin() 返回槽直传的形态不同）。
    mMapIter = mMapObj.begin();
    return mMapIter;

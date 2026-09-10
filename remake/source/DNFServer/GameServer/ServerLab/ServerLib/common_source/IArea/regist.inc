bool IArea::regist(unsigned int id, ISession* obj)
{
    std::pair<MAP_OBJECTS_ITER, bool> ret;
    ret = mMapObj.insert(std::make_pair(id, obj));
    if (ret.second != false)
    {
        return true;
    }
    G_TraceLog()->sysLog(7, "IArea Insert Fail-%d, %x\n", id, this);
    return false;

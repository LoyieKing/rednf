bool IArea::isIterEnd(MAP_OBJECTS_ITER iter)
{
    // ORIG：test+je RET0(al==0 → false); mov $1; jmp EPI; RET0: mov $0。
    // 注意 je 在 al==0 时跳，故语义为 equal→true（与直接 return iter==end 相同）。
    if (iter == mMapObj.end())
    {
        return true;
    }
    return false;

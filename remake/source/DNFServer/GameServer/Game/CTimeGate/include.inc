// ORIG W 0x82845a0：vtable slot0 的 include 覆写（IObject 纯虚）；函数体
// return 0（恒不碰撞）。ORIG 无 CTimeGate::reset 符号。
bool CTimeGate::include(const village_object::IObject&) const
{
    return false;
}
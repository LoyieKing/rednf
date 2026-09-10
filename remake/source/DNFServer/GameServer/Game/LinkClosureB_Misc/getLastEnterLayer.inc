TOD_Layer TOD_UserState::getLastEnterLayer() const
{
    return *(const TOD_Layer*)((const char*)this + 0x14);

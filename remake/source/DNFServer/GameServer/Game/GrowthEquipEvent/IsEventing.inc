bool GrowthEquipEvent::IsEventing(CUser* user) const
{
    // ORIG W 0x080f7820：尾调 CEventBase::IsEventing（编译器弱拷贝）。
    return CEventBase::IsEventing(user);

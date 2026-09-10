_ActionPoint::_ActionPoint()
{
    memset(this, 0, sizeof(*this));   // ORIG 常量 0xb（pack(1) 后 sizeof 即 0xb）

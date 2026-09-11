// ORIG 0x08134650 W：memset(this, 0, 0x24)。
_SummonObject::_SummonObject()
{
    std::memset(this, 0, sizeof(*this));
}
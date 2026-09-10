RestrictGeolocation::~RestrictGeolocation()
{
    // ORIG 0x081709fe：closeGeolocation() + 集合析构（成员自动）。
    closeGeolocation();

float RandGen::getFloat01(XorRand4* rng)
{
    return (float)((unsigned long long)(rng->getUInt32() % 65535u) * (1.0f / 65536.0f));

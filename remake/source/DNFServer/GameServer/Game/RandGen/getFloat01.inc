float RandGen::getFloat01()
{
    return (float)((unsigned long long)(XorRand4::getStaticUInt32() % 65535u) * (1.0f / 65536.0f));
}
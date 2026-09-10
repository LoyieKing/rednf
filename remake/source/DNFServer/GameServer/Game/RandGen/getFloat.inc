float RandGen::getFloat(XorRand4* rng, float min, float max)
{
    float span = max - min;
    return getFloat01(rng) * span + min;

float RandGen::getFloat(float min, float max)
{
    float span = max - min;
    return getFloat01() * span + min;

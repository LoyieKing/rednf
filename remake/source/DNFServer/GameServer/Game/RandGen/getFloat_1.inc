// ORIG _ZN7RandGen8getFloatEff T 0x08ae0288（0x1c）：span=max-min 落 -0xc 槽、
// call getFloat01() 后 fmuls/fadds 逐条对位（P204① 补定义；头声明已备）。
float RandGen::getFloat(float min, float max)
{
    float span = max - min;
    return getFloat01() * span + min;
}
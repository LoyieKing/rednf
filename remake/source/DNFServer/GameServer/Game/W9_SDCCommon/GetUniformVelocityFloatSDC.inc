float GetUniformVelocityFloatSDC(float v0, float v1, int cur, int max)
{
    if (cur >= max)
        return v1;
    if (cur <= 0)
        return v0;
    // ORIG 0x088924c1 x87 序：fildl cur 先于 flds v1 —— 乘法交换序复刻
    // （IEEE 乘法 bit-exact 同值）。
    return v0 + (float)cur * (v1 - v0) / (float)max;

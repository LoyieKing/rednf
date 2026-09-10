int GetDistance(pvp_assault::Point2d a, pvp_assault::Point2d b)
{
    // ORIG 0x82e59f1 尾调 Point2d::operator-（= |dx|+|dy| 曼哈顿距离）。
    // 语义等价（ORIG 传参形态差异属编译器形式；operator- 实现一致）。
    return a - b;   // ORIG 尾调 Point2d::operator-（曼哈顿距离 10 insns，我们内联 29）

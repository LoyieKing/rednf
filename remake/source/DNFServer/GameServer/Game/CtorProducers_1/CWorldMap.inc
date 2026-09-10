CWorldMap::CWorldMap()
{
    // [b28c-35] ORIG 0x0837f6ea 只发 4 个 vector 成员的隐式 C1（+0x10/+0x1c/
    // +0x28/+0x34），**不写** +0x00/+0x04/+0x08/+0x0c 标量；旧初始化列表多 8 条
    // store（63 vs 55）。标量由 set_world_map 赋值，C1 保持空体即逐条对齐。

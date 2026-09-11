// ORIG 0x081b29ec：m_ratio = param.m_duration；trace 行号 29（带参）。
// 实参 param.m_duration 在 ORIG 中于 trace ctor 前物化到 ebx（P37 链式）。
// ORIG C1 0x081b291c：基类构造 + vptr + m_ratio(+0xc) 置 10。
// ORIG D1 0x081b2942：用户空析构（编译器生成 vptr+基类析构 + delete 分支）。
CMemberExpRewardEvent::~CMemberExpRewardEvent()
{
}
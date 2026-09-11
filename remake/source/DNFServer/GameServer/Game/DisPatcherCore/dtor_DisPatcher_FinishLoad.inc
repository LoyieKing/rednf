// ---------------------------------------------------------------------------
// ORIG W 0x08265540 D1 / 0x08265570 D0 _ZN21DisPatcher_FinishLoadD{1,0}Ev
// —— 无独立报告；objdump：写最终虚表指针后调模板基类实例 D1（0x08266d9e），
// 删除版追加 operator delete。空体由编译器按同形态生成，与同类
// DisPatcher_Fish/DisPatcher_UseSkill 写法一致。
// ---------------------------------------------------------------------------
DisPatcher_FinishLoad::~DisPatcher_FinishLoad() {}

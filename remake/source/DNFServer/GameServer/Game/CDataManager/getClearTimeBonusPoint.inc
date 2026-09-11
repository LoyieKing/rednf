// ORIG T 0x08363ac8 _ZN12CDataManager22getClearTimeBonusPointEii：
//   avgTime/clearTime 任一为 0 → 0；diff = avgTime - clearTime；
//   遍历 m_dungeonRank.m_vec18（vector<stBonusPointValues>）：
//     若 diff < 当前 lo：列表首项 → 0，否则返回上一项 value；
//     否则记录 (lo, value) 继续；循环结束返回最后一个 value。
// [b28d-solo-CDataManager AE] 修复：cur/prev 声明序（cur 先，ORIG 0x08363af0
// 先构造 -0x20）+ diff 赋值后置到两次 ctor 之后 + end 缓存到 e（P13，ORIG
// 0x08363b30 循环外一次 end）复现 ORIG 槽位与循环形。
int CDataManager::getClearTimeBonusPoint(int avgTime, int clearTime)

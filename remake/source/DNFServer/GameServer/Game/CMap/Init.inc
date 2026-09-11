// 先声明全部显式特化，避免定义顺序触发隐式实例化在前（GCC 4.4）。
template <> void cElection<int, 8, 8>::cVoter::Init();

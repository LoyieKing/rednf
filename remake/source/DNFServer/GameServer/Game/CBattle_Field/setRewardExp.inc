// b28 P211 翻案（DIFF 6→IDENTICAL）：字段 int→unsigned int 与 exp 形参同型化
// 后 store 展开翻为 LHS-first（this 于 idx 之后即载，`lea 0x4(%edx),%ecx` 并入
// scaled 寻址），与 ORIG 0x085bf144 逐条全等；旧「add vs lea」判词系字段异型
// 所致的装载方向差，非 ISel 伪差。
void CBattle_Field::CBloodClearRewardData::setRewardExp(int idx, unsigned int exp)
{
    m_rewardExp[idx] = exp;
}
// ORIG W 0x085e692a：sret 返回 std::pair<int,int>（8 字节）。ORIG 为逐字段内联拷贝
// （`mov 0x1c(%eax),%edx; mov 0x18(%eax),%eax; mov %eax,(%ecx); mov %edx,0x4(%ecx);
// mov %ecx,%eax; ret $0x4`）——pair<int,int> 拷贝构造 trivial ⇒ 无 call，与 C1
// 0x085e304b 同族。成员已按 ORIG 建模为 std::pair<int,int> m_rankRange（b28d-70）。
// [b28d-70] 判词：IDENTICAL 11=11。
MissionRankRange CMission::GetRankRange() const
{
    return m_rankRange;
}
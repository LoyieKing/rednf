// ORIG W 0x0822eef6（无报告，objdump）：
//   mov 0xc(%ebp),%eax ; lea -0x65(%eax),%edx ; add $0xf44,%edx
//   mov 0x8(%eax,%edx,4),%eax
// 即 B 表累计计数（与 CHackAnalyzer.cpp hackCntB 寻址一致；
// resetServerHackAccumulatedCnt 0x086950b6 清同一槽位）。按 ORIG 运算次序
// 书写以对齐指令选择。
// [b28d-solo-UserCoreDepsB2] IDENTICAL（9=9）：裸偏移 `*(const int*)((char*)this+8+…)`
// 改具名成员数组 `m_serverHackCntB[(int)type-0x65]`（§4.3 具名化），与同族
// setServerHackAccumulatedCnt（ORIG 0x084eb57c，IDENTICAL）同写法，逐条复现 ORIG
// `lea -0x65(%eax),%edx; add $0xf44,%edx; mov 0x8(%eax,%edx,4),%eax`。
int WongWork::CHackAnalyzer::getServerHackAccumulatedCnt(
    WongWork::ENUM_HACKTYPE type) const
{
    return m_serverHackCntB[(int)type - 0x65];
}
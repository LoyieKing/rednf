// [G7-sub 2026-09-10 复核·写域边界] 函数体（ORIG 0x085b75d4..0x085b7c9a，478 条）在
// CParty.cpp:2013 起，本 .inc 无源码行 ⇒ 本轮无改动。缺口/帧差请由 CParty.cpp 属主处理。
// 已知：P167 CLEAR（仅 PacketGuard/RewardData C1-C2·D1-D2 别名）；bf 视图宏重物化已前轮落地。
void CParty::beginBloodClearProcess(bool flag)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b75d4
// §P331 第二形（实测 709→666）：ORIG 每次 bf 成员访问都 `mov 0x8(%ebp),%eax;
// add $0xb24,%eax` 重物化（add$0xb24 8 处，本方原 1 —— 引用起基址吸收了 7 处）。
// 改宏每次直取：bf 全程只读该视图、this 不变 ⇒ 寻址同物，无可观察行为变化。

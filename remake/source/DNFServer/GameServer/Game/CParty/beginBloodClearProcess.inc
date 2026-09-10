void CParty::beginBloodClearProcess(bool flag)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x85b75d4
// §P331 第二形（实测 709→666）：ORIG 每次 bf 成员访问都 `mov 0x8(%ebp),%eax;
// add $0xb24,%eax` 重物化（add$0xb24 8 处，本方原 1 —— 引用起基址吸收了 7 处）。
// 改宏每次直取：bf 全程只读该视图、this 不变 ⇒ 寻址同物，无可观察行为变化。

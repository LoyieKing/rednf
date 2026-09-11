// [b28 P210] 调用者定义移到 callee 之前：4.4 对「TU 内已见定义的 callee」按其实
// 帧精确继承 outgoing（塌缩 sub $0x4）；ORIG 0x0808ec67 帧 0x18 为 callee 未见时
// 保守形。前置定义后恢复保守大帧。
// setCodePageByLocal — ORIG @ 0x808ec64：setCodePage(950 /*0x3b6*/)。
void setCodePageByLocal()
{
    setCodePage(950);
}
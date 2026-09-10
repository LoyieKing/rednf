DisPatcher_ReportClient4Hack::~DisPatcher_ReportClient4Hack() {}
// ORIG 0x082041a0 T _ZN28DisPatcher_ReportClient4Hack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReportClient4Hack/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=1 或 getCurCharacR()==0 → return 0。state==6 → 仅 analyzeHackReport。
// state==5 组队校验（GetParty==0 → LineFunc 0x9bb7）；state==7 PVP 校验
// （GetPvp==0 → LineFunc 0x9bc1）；state==8 战场校验（GetWarRoom==0 → LineFunc 0x9bc5）；
// 校验通过同 6 → analyzeHackReport；其余 state → beginCollect/analyze/report/reset 全链。

DisPatcher_DiePvP::~DisPatcher_DiePvP() {}
// ORIG 0x081ece64 T _ZN17DisPatcher_DiePvP12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧 0x6c vs 0x60 + 槽平移（ushort 数组 -0x36 vs -0x26），调用点全等  (ORIG 0x081ece64 insn 146->147)
// 报告：docs/class_func_reports/DisPatcher_DiePvP/dispatch_sig.md（汇编+反编译 C 全读）
// [Lane5] P227 已落（gap 146→146=0）：getter 内联进 CSwitchLog 构造即调用单表达式，
// trace ctor 提到 getter 之前的问题消除。残余=整帧槽深 +4（-0x36 vs -0x3a 等）+
// 帧 0x6c vs 0x5c（N82/N115 族），ndiff ~60。

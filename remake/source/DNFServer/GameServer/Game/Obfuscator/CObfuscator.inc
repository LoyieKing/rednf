// ===========================================================================
// CObfuscator —— 位重排混淆（7bit 有效位流水）
// ===========================================================================
CObfuscator::CObfuscator()                                   // 0x08585888
{
    // [REAL 修复·TU 拆分] ORIG 0x08585888 形：base CBitManager ctor → 局部临时
    // CBitManager ctor(0x085858a1)/dtor(0x085858ac) + EH landing pad
    // (+0x2b..+0x41：temp 半构造时析构基类子对象 + _Unwind_Resume)。
    // 本件原判 N-EH（14 vs 29），根因经查不是版本不可复现，而是**TU 归属错**：
    // c6root g++ 4.4.7 见到同 TU 内 `CBitManager::~CBitManager(){}` 空体即推导
    // nothrow，从而对该 temp 不发 cleanup（4.1 无此推导）。ORIG 侧 CBitManager
    // 全族连续落在 0x0831c09c..0x0831c3af、其静态初始化组为
    // _GLOBAL__I__ZN11CBitManagerC2Ev，而 CObfuscator 在 0x085858xx、组为
    // _GLOBAL__I__ZN11CObfuscatorC2Ev ⇒ 原属不同编译单元。
    // 修复（Main 批准）：CBitManager 的 ctor/dtor/SetAll/ReadData/GetBitFromLSB/
    // ShiftRight/ShiftLeft/GetBit 定义整体迁出至 Game/CBitManager.cpp（已登记
    // source/cmake/game/CMakeLists.txt），本 TU 仅经 WindowsPackFiles.h 取声明。
    // 实测本函数 orig_insns=29 / our_insns=29，landing pad 与 -0x18(%ebp) 槽、
    // push %esi/%ebx + sub $0x20 帧全部逐条复现（P167 调用点 5/5，仅 ORIG 侧
    // C1/D1 与我们 C2/D2 的构造别名地址同源）；连带的 CObfuscator::~CObfuscator
    // 亦由 NEAR 退出。残余仅 ret 后对齐 nop（§6.1 豁免）与 .o 未 relink 的
    // call 占位（§6.3 假阳性）。
    CBitManager temp;
}
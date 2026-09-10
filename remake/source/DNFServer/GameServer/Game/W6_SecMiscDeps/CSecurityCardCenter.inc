WongWork::CSecurityCardCenter::CSecurityCardCenter()
{
    // [b28d-solo-W6_SecMiscDeps 判词 · IDENTICAL_AE] ORIG 0x082a6654 W：构造
    // CRijndael@0x0（0x47c 子对象，成员而非基类——见 GlobalData.h 判词）与
    // list<uchar>@0x47c；无派生类 vptr 写。改成员子对象后逐条全等（原旧判词
    // 「4.4 多发 vptr 写、N18a 封顶」已作废：真因是建模成派生类导致发射
    // _ZTV/_ZTI + 派生 vptr 写）。时间计数不在此写入（init() 置 1）。

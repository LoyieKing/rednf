// [b28d-solo-GlobalData] 归零 75=75 IDENTICAL：P299/P318 删三个子类用户空 ctor
// （ORIG C1/C2 均 W）恢复 `new T()` value-init 前置双清零（旧 N77/N83 判词作废） | ORIG 0x084a302e
expert_extraction::CExtractionMgr::CExtractionMgr()
{
    // ORIG 0x084a302e：new 三个子提取器对象，依次存 +0/+4/+8。
    // ORIG 0x084a3047/0x084a308f：ctor 调用前先清 0x8 字节——value-init
    // `new T()`（zero-init 前置 + C1 调用）。
    m_slots[0] = new CAlchemistExtraction();
    m_slots[1] = new CEnchanterExtraction();
    m_slots[2] = new CDollControllerExtraction();
    // [b28d-solo-GlobalData] 4.4 现发 `movl $0x0,(%eax); movl $0x0,0x4(%eax)`
    // 前置双清零（与 ORIG 0x084a3047/0x084a308f 逐条同形）——关键是子类无用户
    // 提供 ctor（CWG434）；旧「4.4 折叠进 C1/N83 封顶」判词作废。
}
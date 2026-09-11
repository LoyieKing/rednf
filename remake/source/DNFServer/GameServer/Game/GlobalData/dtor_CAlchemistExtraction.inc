// [b28d-solo-GlobalData] P299/P318：三个子类用户空 ctor 声明+定义已删（ORIG C1/C2 均 W）
// ⇒ CExtractionMgr::C1 的 `new T()` 恢复 value-init 前置双清零（ORIG 0x084a3047/0x084a308f）。
expert_extraction::CAlchemistExtraction::~CAlchemistExtraction() {}

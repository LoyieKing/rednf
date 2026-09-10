StrikerSkill::StrikerSkill() : m_name() {}

// ============================================================================
// WongWork::CWorldDrop（无独立函数报告两处【标记】，objdump 转写；
// 双参 loadScript 为报告全文转写）
// ============================================================================

// ORIG W 0x0853a826：call CItemGeneratorC1Ei(count) → movl $0x8c9a718,(this)
// → 循环 WorldDropInfoC1Ev ×0xc8 @this+0x18 → 同 ×0xc8 @this+0xfcc。
// 成员数组的默认元素构造与 ORIG 循环等价。

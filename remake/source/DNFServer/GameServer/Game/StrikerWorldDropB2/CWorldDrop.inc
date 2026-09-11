// ORIG W 0x0853a826：call CItemGeneratorC1Ei(count) → movl $0x8c9a718,(this)
// → 循环 WorldDropInfoC1Ev ×0xc8 @this+0x18 → 同 ×0xc8 @this+0xfcc。
// 成员数组的默认元素构造与 ORIG 循环等价。
WongWork::CWorldDrop::CWorldDrop(int count) : CItemGenerator(count) {}

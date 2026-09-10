// [Q8 2026-09-11 旧N判词重扫] NEAR 307=307/ndiff=1，唯一残差 = 4.4 地址分解的
// 常量归属（codegen 轴，源码不可控）：本循环 `m_dw110[i]=0` 处 ORIG 0x83752xx
// `add $0x44,%edx; movl $0x0,(%eax,%edx,4)`（0x44=(0x110>>2)），我方
// `add $0x40,%edx; movl $0x0,0x10(%eax,%edx,4)`（同一地址 (i+0x44)*4）——
// 4.4 以 m_f100(0x100) 为基址、常量 0x10 落 disp，ORIG 全折进索引寄存器。
// 试过：语句序/视图写法均不翻（本 TU 其余 4 件为模板实例代际差 + static_init）。
void STSkillScript::Clear()

void CCharacList::destroy()
{
    // ORIG 0x083494da：循环内**两个**函数级槽 —— i(-0x10) 与元素地址(-0xc)：
    // `imul $0x7dc; add this; mov %eax,-0xc; mov -0xc,%eax; mov (%esp); call`。
    // 4.4 默认把地址直接留在 %eax（copy-prop）⇒ 少 2 条，需具名中间指针
    //   才能保住那对 `mov %eax,-slot; mov -slot,%eax`（技巧 36 的反向用法）。
    for (int i = 0; i <= 0xa; ++i)
    {
        CCharacter* p = &m_characs[i];
        p->destroy();   // ORIG 0x083494da：_ZN10CCharacter7destroyEv 真实调用（CCharacter.h 声明）
    }

void GreyImage::reset()
{
    // [b28c-35] ORIG 0x080df582 形：`je <nop 岛>; body; jmp <leave>; nop; leave; ret`
    // （4.1 空 else 占位岛，P223）。裸早返 `if (m_image == 0) return;` 复现该岛；
    // 原 `if (m_image) { … }` 形 4.4 jump-thread 掉 jmp+nop（19 vs 21 insn）。
    if (m_image == 0)
        return;
    m_width = 0;
    m_height = 0;
    delete m_image;   // ORIG reset 0x80df5ae 用标量 _ZdlPv（init 虽 _Znaj）
    m_image = 0;

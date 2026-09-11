// ============================================================================
// ORIG T 0x0867ceae：全部角色 m_needCheckOverEquipItem（+0xe3b）置 1。
// ============================================================================
void CUser::SetNeedCheckOverEquipItem()
{
    // ORIG 0x08??：operator[] 结果存 -0xc 槽（i=-0x10 先声明远槽，技巧 21）：
    // `mov %eax,-0xc; cmpl $0,-0xc; je NEXT; mov -0xc,%eax; movb $1,0xe3b(%eax)`
    // ——单次 call + 具名引用中转（双 call 形反产 test;setne 物化+二次 sret）。
    for (unsigned int i = 0; i < m_characList.size(); ++i)
    {
        _Charac_info& r = m_characList[i];
        // P223（b28 BL9 实证形）：`if(c==e) goto L; body; L:;` 精确复现
        // ORIG `je 岛; body; jmp; nop` 空 then 占位岛。
        if (&r == 0)
            goto NEXT;
        r.m_needCheckOverEquipItem = 1;
    NEXT:;
    }
}
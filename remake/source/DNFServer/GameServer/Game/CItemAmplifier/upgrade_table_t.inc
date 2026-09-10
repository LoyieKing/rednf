upgrade_table_t::upgrade_table_t()
    : m_machineFrames(0xa)   // ORIG C1 0x089000ca：_Hashtable 桶数 0xa
{
    // [b28d-solo-CItemAmplifier] 7 个循环各有**独立计数器**：ORIG 槽 -0x24..-0xc
    // 七个 int（Ghidra C 的 local_28..local_10），且 P212 全头部声明序=先声明深。
    // 旧版共用一个 i 使帧 0x30（ORIG 0x40）且计数器槽错位。
    int i1, i2, i3, i4, i5, i6, i7;
    // 初始化循环一律用 **dword 下标** 形（UPG_TBL_F32/I32 + `i + K`），复现 ORIG
    // 0x089001xx..0x089003a0 的 `mov %edx,%eax? ; add $K,%edx ;
    // movl $imm,disp(%eax,%edx,4)` 两步寻址（P30/N81：字节偏移表达式会被 4.4
    // 折成 lea/shl/add 链，实测本函数因此多 41 insn）。K 值 = (绝对偏移-0xc)/4。
    for (i1 = 0; i1 <= 3; ++i1)           // +0x0c..：1.0f  ⇒ k = i + 0x3
        UPG_TBL_F32(this, i1) = 1.0f;
    for (i2 = 0; i2 <= 0x15; ++i2)        // +0x1c..：1.0f  ⇒ k = i + 0x7
        UPG_TBL_F32(this, i2 + 0x4) = 1.0f;
    for (i3 = 0; i3 <= 5; ++i3)
    {
        // [b28d-solo-CItemAmplifier] 语义修复：ORIG 0x089001bb `lea 0x1c(%edx),%ecx
        // + mov %edx,0x4(%eax,%ecx,4)` = this+0x74+4i；旧 k=i+0x1b 落 +0x78+4i，
        // 漏写 +0x74 且误写 +0x8c（与第二句重叠）。
        UPG_TBL_F32(this, i3 + 0x1a) = 1.0f;   // +0x74..
        UPG_TBL_F32(this, i3 + 0x20) = 1.0f;   // +0x8c..
    }
    for (i4 = 0; i4 <= 0x63; ++i4)        // +0xa4..：整数 1
        UPG_TBL_I32(this, i4 + 0x26) = 1;
    UPG_TBL_I32(this, 0x8a) = 0;              // +0x234 公告阈值
    // [b28d-solo-CItemAmplifier] 语义修复：ORIG 0x0890022d `lea 0x9c(%edx),%ecx
    // + mov %edx,0x4(%eax,%ecx,4)` = this+0x274+4i；旧 k=i+0x9b 落 +0x278+4i。
    for (i5 = 0; i5 <= 0x63; ++i5)        // +0x274..：1.0f
        UPG_TBL_F32(this, i5 + 0x9a) = 1.0f;
    for (i6 = 0; i6 <= 5; ++i6)
    {
        UPG_TBL_I32(this, i6 + 0xfe) = -1;    // +0x404..
        UPG_TBL_I32(this, i6 + 0x104) = -1;   // +0x41c..
    }
    UPG_TBL_I32(this, 0x8b) = -1;             // +0x238 附加分解物 item
    UPG_TBL_I32(this, 0x8c) = 0;              // +0x23c 等级差阈值
    for (i7 = 0; i7 <= 5; ++i7)
    {
        // [b28d-solo-CItemAmplifier2] P238：ORIG 0x089002ba/0x089002bf 对 +0x240
        // 权重区发 `mov $0x0,%edx; mov %edx,(%eax,%ecx,4)`（edx 中转），而紧随的
        // +0x25c 基准等级区是直写 `movl $0x0,0xc(%eax,%edx,4)`。两区字段真型不同
        // （float 权重 / int 基准），故权重区走 float 视图 + 0.0f。
        UPG_TBL_F32(this, i7 + 0x8d) = 0.0f;   // +0x240.. 权重区（float）
        UPG_TBL_I32(this, i7 + 0x94) = 0;      // +0x25c.. 基准等级区（int）
    }
    UPG_TBL_I32(this, 0x93) = 0;              // +0x258 强化等级下限
    m_floatMap.clear();
    // [b28d-solo-CItemAmplifier] ORIG 0x0890030d `lea 0x4c4(%eax),%eax; call
    // _Hashtable::clear` 实证：构造后**照抄**清空 m_machineFrames（旧版漏此调用，
    // gap=5 主因）。
    m_machineFrames.clear();
    // 逐条直写：ORIG 0x089003a0 域是 4 条 `movl $0,0x4b4+4i(%eax)`，走 memset()
    // 会多出 ORIG 不存在的 memset 符号（P183 反向：库调用回退为物化）。
    m_zero4b4[0] = 0;
    m_zero4b4[1] = 0;
    m_zero4b4[2] = 0;
    m_zero4b4[3] = 0;

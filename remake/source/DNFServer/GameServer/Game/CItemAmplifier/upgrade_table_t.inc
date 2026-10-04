// ORIG 0x089000ca：按偏移初始化概率/等级混合表。
// [b28d-solo-CItemAmplifier] verdict=DIFF（200/199，归一 diff 47）：语义已逐条
//   对齐（含修 2 处真 bug：+0x74/+0x274 两处 1.0f 写入的 4 字节偏移 + 补
//   m_machineFrames.clear()）。残差全为 P30/N4 族「索引寄存器 + add-vs-lea」：
//   ORIG `mov idx,%edx; mov this,%eax; add $K,%edx` vs 4.4 `mov idx,%eax;
//   lea K(%eax),%ecx; mov this,%eax`（7 个初始化循环 × 2-3 条；穷举 lea/add/
//   常量拆分均不齐）⇒ 结构性封顶，无源码旋钮。
// [b28d-solo-CItemAmplifier2] P238 修复 i7 权重区：+0x240 是 float 字段
//   （m_additionalWeight），ORIG 0x089002ba/0x089002bf 发 `mov $0x0,%edx;
//   mov %edx,(%eax,%ecx,4)` 中转形；旧版两句都走 I32 视图 ⇒ 第一句被 4.4 折成
//   `movl $0x0,0xc(%eax,%edx,4)` 直写（少 1 条）。改 F32 视图 + 0.0f 后
//   gap 1→0（orig/our 208/208），TU 四数 92/193/2/2 零变差；p167 调用点
//   14=14 CLEAR（MISSING/EXTRA 仅 C1↔C2、D1↔D2 同址别名伪差，P93）。
// [P8S1 2026-09-11 本波复核] 维持封顶：N4/P30 7 组初始化循环 add-vs-lea(ORIG 0x089000ca 段)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
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
    m_bodyView.m_named.m_noticeUpgradeLevel = 0;
    // [b28d-solo-CItemAmplifier] 语义修复：ORIG 0x0890022d `lea 0x9c(%edx),%ecx
    // + mov %edx,0x4(%eax,%ecx,4)` = this+0x274+4i；旧 k=i+0x9b 落 +0x278+4i。
    for (i5 = 0; i5 <= 0x63; ++i5)        // +0x274..：1.0f
        UPG_TBL_F32(this, i5 + 0x9a) = 1.0f;
    for (i6 = 0; i6 <= 5; ++i6)
    {
        UPG_TBL_I32(this, i6 + 0xfe) = -1;    // +0x404..
        UPG_TBL_I32(this, i6 + 0x104) = -1;   // +0x41c..
    }
    m_bodyView.m_named.m_additionalItemId = -1;
    m_bodyView.m_named.m_additionalGradeGap = 0;
    for (i7 = 0; i7 <= 5; ++i7)
    {
        // [b28d-solo-CItemAmplifier2] P238：ORIG 0x089002ba/0x089002bf 对 +0x240
        // 权重区发 `mov $0x0,%edx; mov %edx,(%eax,%ecx,4)`（edx 中转），而紧随的
        // +0x25c 基准等级区是直写 `movl $0x0,0xc(%eax,%edx,4)`。两区字段真型不同
        // （float 权重 / int 基准），故权重区走 float 视图 + 0.0f。
        m_bodyView.m_named.m_additionalWeight[i7] = 0.0f;
        m_bodyView.m_named.m_baseGrade[i7] = 0;
    }
    m_bodyView.m_named.m_additionalMinUpgrade = 0;
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
}
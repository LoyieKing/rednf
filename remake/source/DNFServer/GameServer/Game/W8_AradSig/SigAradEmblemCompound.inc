// ---------------------------------------------------------------------------
// SigAradEmblemCompound::materialInfo::init —— ORIG W 0x081a0a0c：
//   movw $0x0,(%eax)[slot]; movw $0x0,0x2(%eax)[grade]; movl $0x0,0x4(%eax)[itemIdx]
// ---------------------------------------------------------------------------
// P210：SigAradEmblemCompound::init 定义须先于本 callee，否则 4.4 按 materialInfo::init
// 帧精确继承 outgoing（caller 帧 0x18→0x4 与 ORIG 0x18 反）。
// ---------------------------------------------------------------------------
// SigAradEmblemCompound —— ORIG W C1/C2 0x081a0a2c -> init() 0x081a0a40：
//   sub $0x18（ORIG 保守 outgoing 帧）
//   movw $0x0,0x14(%eax)                        // 清 m_selectIdx
//   call materialInfo::init(this)               // 清 {m_slot0,m_grade0,m_itemId0}
//   eax += 0x8; call materialInfo::init(this+8) // 清 {m_slot1,m_grade1,m_itemId1}
// 注意 ORIG 不清 m_resultIdx(+0x10)。
// ---------------------------------------------------------------------------
SigAradEmblemCompound::SigAradEmblemCompound()
{
    init();
}
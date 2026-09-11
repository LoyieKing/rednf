// ----------------------------------------------------------------------------
// ORIG T 0x0812d0b6（upGage.md 全文）：cfg = G_CDataManager()+0x6364；
//   maxGage = **(int**)(cfg+0x18) * cfg->[0xc+(level+4)*4]（= *m_strPtr * m_gageUpAux[level]）；
//   m_gage >= maxGage → m_gage = maxGage；否则 ratio = m_gageUpRate[level] / base（float 局部）、
//   inc = (int)(m_gageUpRate[0] * ratio)（x87 截断，无 +0.5f）、m_gage += inc；
//   两臂各自调用 sendTestCheckGage()（ORIG 两处 call 0x812d103/0x812d16e）。
// ----------------------------------------------------------------------------
// b28d-solo-ProcStage：原实现有两处语义偏差——①`+ 0.5f` 多余（ORIG 0x812d141 段
//   `fnstcw/fldcw $0xc/fistpl` 为纯 (int) 截断，无 fadd 0.5）；②sendTestCheckGage()
//   写在 if 外（4.4 共享尾一次 call）而 ORIG 两臂各一次。形态另按 ORIG 对齐：
//   then 块在后（`jl <then>` 由 m_gage>=maxGage 反向条件复现）、ratio/inc 具名局部
//   （ORIG -0x10/-0xc 槽）、maxGage 内联表达式（ORIG 无 base 具名槽）。
void CharacData::upGage()
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;
    // ORIG 求值序：先 aux[level]（→edx）后 *m_strPtr（→eax）再 imul ⇒ 交换书写序；
    //   索引位 `static_cast<unsigned int>(m_gageLevel)` 复现 ORIG 两段装载
    //   `mov 0x8(%eax),%eax; mov %eax,%edx`（P319；裸 int 索引 4.4 恒折单段）。
    int maxGage = cfg->m_gageUpAux[static_cast<unsigned int>(m_gageLevel)] *
                  *reinterpret_cast<int*>(cfg->m_strPtr);
    if (m_gage >= maxGage)
    {
        m_gage = maxGage;
        sendTestCheckGage();
    }
    else
    {
        float ratio = (float)cfg->m_gageUpRate[static_cast<unsigned int>(m_gageLevel)] /
                      (float)*reinterpret_cast<int*>(cfg->m_strPtr);
        int inc = (int)((float)cfg->m_gageUpRate[0] * ratio);
        m_gage = m_gage + inc;
        sendTestCheckGage();
    }
}
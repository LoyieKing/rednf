// [b28d-22 N82] ORIG 0x08690be6：p167=M0/E0:，gap=1；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08690be6：N/P11 封顶——ORIG 位域提取
// `movzbl; sar $0x4,%al`（8 位）；4.4 产 `movsbl %al,%eax`（显式 uchar 中转反多一条，5 vs 4）。
void CUser::ValidateStrikerInfo()
{
    // ValidateStrikerInfo.md @0x08690be6：遍历 m_characList(+0x796e8)，
    // striker 字段对 _Charac_info 元素内 m_strikerSkillIndex(u8 skillIndex)/m_tagCharacNo(u32
    // linkedCharacNo)【推断注记：报告语义 strikerSkillIndex/strikerLinkedCharacNo】：
    // linked 找不到或 StrikerScript::checkSkillIndex 校验失败即双清零。
    // [b28d-22 修法 P0/P52/P134/P2] ORIG 0x08690d86 `setg`（有符号 size>i）⇒ 循环变量 int；
    // 0x08690c19 `setne/je` 正极性守卫；0x08690cc1 skillIdx→%edi / 0x08690ce6 secondGrow→%esi
    // / 0x08690d05 firstGrow→%ebx / 0x08690d0d script 直落 receiver ⇒ 五个具名局部内联。
    for (int i = 0; i < static_cast<int>(m_characList.size()); ++i)
    {
        if (m_characList[i].m_tagCharacNo != 0)
        {
            const int linkedIdx = get_charac_idx(
                static_cast<unsigned int>(m_characList[i].m_tagCharacNo));
            if (linkedIdx == -1)
            {
                m_characList[i].m_tagCharacNo = 0;
                m_characList[i].m_strikerSkillIndex = 0;
            }
            else if (!G_CDataManager()->m_altarConfig.m_strikerScript
                          ->checkSkillIndex(
                              m_characList[linkedIdx].m_job,   // +0x26 firstGrow
                              // [Lane6 2026-09-10 真语义 bug 修复] ORIG 0x08690ce6
                              // `movzbl 0x29(%eax),%eax; shl $0x4,%eax; sar $0x4,%al;
                              // movsbl %al,%esi` = **低 4 位符号扩展**（(char)(f<<4)>>4；
                              // f=0x9A ⇒ -6）。旧译文 `(char)((f<<4)>>4)` 在 4.4 下被折成
                              // `movsbl f; shl$4; sar$4`（恒等）⇒ 返回**完整 f**（-102），
                              // 与 ORIG 语义不同 ⇒ 内层 cast 才正确（与 setCharacInfoDetail
                              // 同址段形一致）。
                              static_cast<char>(
                                  m_characList[linkedIdx].m_growthType.m_sub),
                              static_cast<int>(
                                  static_cast<unsigned char>(m_characList[i].m_strikerSkillIndex))))
            {
                m_characList[i].m_tagCharacNo = 0;
                m_characList[i].m_strikerSkillIndex = 0;
            }
        }
    }
}
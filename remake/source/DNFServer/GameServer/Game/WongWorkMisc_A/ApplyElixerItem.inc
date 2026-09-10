void CCalcCharacStat::ApplyElixerItem(_Additioal_info* out, int statIdx, int value)
{
    // [b27 封顶] 残差仅 case9 循环体寄存器角色（ORIG arr值→ebx/i基址重读→ecx，
    // 4.4 值→ecx/i→ebx+lea 直算——N7 族角色互换）+ jump table 基址 reloc。
    // switch/各 case/store 序全等。
    switch (statIdx)
    {
    case 4:
        out->m_field08 = static_cast<unsigned short>(
            out->m_field08 + value * 0x32);
        break;
    case 6:
        out->m_field0c = static_cast<unsigned short>(
            out->m_field0c + value * 0x32);
        break;
    case 5:
        out->m_field0a = static_cast<unsigned short>(
            out->m_field0a + value * 0x32);
        break;
    case 7:
        out->m_field0e = static_cast<unsigned short>(
            out->m_field0e + value * 0x32);
        break;
    case 2:
        out->m_field00 += value * 250;                       // imul $0xfa
        break;
    case 3:
        out->m_field04 += value * 250;
        break;
    case 8:
        out->m_field42 += value * 10;
        break;
    case 9:
        // [2026-09-10 D06 · Main 新轴（const/符号性）实测不适用] ORIG 0x0831e995 段
        // `movzwl …,%eax; mov %eax,%ebx`（元素值升格 callee-saved %ebx、i 留 %ecx +
        // `add $0x8,%ecx`）；去 cast 形与复合赋值形实测与 cast 形**同值**（119 vs 121，
        // 差异恒为寄存器角色互换）⇒ 循环变量符号性（两侧同为有符号 setle）与 cv 限定
        // 符均非本件变量，维持 N7/N78 封顶。
        for (int i = 0; i < 4; ++i)                          // 循环上界 cmp $0x3,setle
            out->m_field10[i] = static_cast<unsigned short>(
                out->m_field10[i] + value * 10);
        break;
    default:
        break;
    }

void CUser::apply_elixir_item(_Additioal_info& ai, std::pair<int, int>& pr)
{
    switch (pr.first)
    {
    case 0x40f: ai.m_field08 += (short)(pr.second * 50); break;   // +0x08
    case 0x410: ai.m_field0c += (short)(pr.second * 50); break;   // +0x0c
    case 0x411: ai.m_field0a += (short)(pr.second * 50); break;   // +0x0a
    case 0x412: ai.m_field0e += (short)(pr.second * 50); break;   // +0x0e
    case 0x413: ai.m_field00 += pr.second * 250; break;           // +0x00
    case 0x414: ai.m_field04 += pr.second * 250; break;           // +0x04
    case 0x415: ai.m_field42 += pr.second * 10;  break;           // +0x42
    case 0x416:
        // [b28d-22 N7] i 保持 int：unsigned 试形使尾测变 setbe（ORIG 0x0866548x
        // `cmpl $0x3,-0x8; setle` 有符号）⇒ 回退。寄存器角色（ORIG i=%ecx/值=%ebx，
        // 4.4 反置）与加法分组（ORIG lea(%ebx,%eax,1) vs 4.4 add）为 N7/N18a 封顶。
        for (int i = 0; i < 4; ++i)
        {
            /* +0x10, +0x12, +0x14, +0x16（ORIG (i+8)*2 索引形态） */
            ai.m_field10[i] += (short)(pr.second * 10);
        }
        break;
    default:
        break;
    }

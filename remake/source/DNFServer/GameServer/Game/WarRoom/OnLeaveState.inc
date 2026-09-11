// ============================================================================
// 状态机
// ============================================================================
void WarRoom::OnLeaveState(WARROOM_STATE state)
{
    if (state == WARROOM_STATE_0)
        CurSpawnStepReset();
    // ORIG 0x86bcf2d：switch 降形态复现「单 load→test/je(case0)→cmp $2/jne→
    // state==3」的级联标签（je/jne 目标分设 +0x178/+0x179）。
    switch (m_state)
    {
    case 0:
        break;
    case 2:
        if (state == 3)
        {
            for (int i = 0; i <= 5; ++i)
            {
                // ORIG 086bd06x：FP>0 body 尾 jmp +0x163(end) 跳空岛 086bd076(nop;
                // fall end)=slot-null 落点；FP<=0 je 直落 end。双 goto 钉块。
                {   // money/min 作用域包住，goto 跳出合法且不新增槽
                if (m_slots[i] == 0)
                    goto FP_SLOT;
                int money = (m_slotsUser[i])->getCurCharacMoney();
                int min = std::min(
                    G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand],
                    money);
                ((CInventory*)(m_slotsUser[i])->getCurCharacInvenW())
                    ->use_money(min, (eMoneySubReason)0xf, 1);
                if ((m_slotsUser[i])->GetFinishPointTotal() > 0)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x55b, 5)
                        ("\xc0\xef\xbf\xaa FP\xb4\xa9\xc0\xfb\xb5\xca \xb9\xdf\xbb\xfd", (m_slotsUser[i])->GetFinishPointTotal());
                    (m_slotsUser[i])->InitFinishPoint();
                    goto FP_JOIN;
                }
                }
FP_JOIN:
                ;
FP_SLOT:
                ;
            }
        }
        break;
    default:
        break;
    }
}
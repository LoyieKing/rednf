CStackableItem::CoolTimeOption CStackableItem::GetEffectItemOption()
{
    CoolTimeOption opt;
    opt.sec  = m_option2e4.m_sec16;    // b28c-L8：同 GetCoolTimeOption（ORIG 0x086946f4）
                                       // [b28d-solo-N7] fresh 10/11 维持 N7（同族同判词）。
    opt.flag = m_option2e4.m_field2;
    return opt;

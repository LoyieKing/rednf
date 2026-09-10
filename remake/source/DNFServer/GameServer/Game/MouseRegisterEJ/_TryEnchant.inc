int CEnchanter::_TryEnchant(CUser* user, stItemPos_t* pos,
                            ENUM_TRY_ENCHANT_TYPE type)
{
    int ret = 0;

    typedef void (*CloseFn)(void*, CUser*, int);
    // P251/P286/P324：ORIG 0x0849dcd1 先 `mov 0x14(%ebp),%eax` 再寄存器比较链
    // （cmp $1/je、cmp $2/je、test/jne），且三叶块物理序 = case 书写序 0→1→2
    // （ORIG 0x0849dcea case0 直落、+0x65 case1、+0xa6 case2）⇒ 原码是 switch(type)。
    switch (type)
    {
    case ENUM_TRY_ENCHANT_TYPE_0:
        ret = _TryEnchantMine(user, pos);
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 0);
        break;
    case ENUM_TRY_ENCHANT_TYPE_1:
        ret = _TryEnchantOther(user, pos);
        // P225：ORIG 0x0849dd45 `cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je`
        // 为 4.1 槽比零的 bool 物化；4.4 对 if 恒出 cmp/jne 直跳 ⇒ 用 while+break 复现。
        while (ret != 0)
        {
            m_materials[1].Reset();                       // this+0x18 材料②复位
            m_field40 = 0;                                // 登记配方物品清空
            break;
        }
        break;
    case ENUM_TRY_ENCHANT_TYPE_2:
        (*(CloseFn*)(*(void* const*)this + 0x04))(this, user, 2);
        break;
    }
    return ret;

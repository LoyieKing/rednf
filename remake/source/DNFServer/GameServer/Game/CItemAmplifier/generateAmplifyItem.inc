void CItemAmplifier::generateAmplifyItem(const CItem* item,
                                         Inven_Item& inven) const
{
    ENUM_AMPLIFY_TYPE type = ENUM_AMPLIFY_TYPE_NONE;
    unsigned short value = 0;

    _generateOption(item, type, value);
    // [b28d-solo-CItemAmplifier] P63：ORIG 0x08514ef2/0x08514efb 两个守卫
    // **同跳一个**出口 0x08514f6e（leave），且主流程尾块直落 epilogue。旧形
    // `if(...) return;` 让 4.4 给每个早退各发一组 `nop; jmp` 中继（+2 jmp）。
    // 正向包裹后守卫直接 je epilogue、内层 else 直落。
    if (type != ENUM_AMPLIFY_TYPE_NONE && value != 0)
    {
        // [b28d-34] P282 槽序还原：ORIG 0x08514f02 `add $0x58,%eax; mov (%eax),%edx;
        // call *%edx` 的 vptr+0x58 槽 = _ZTV5CItem+0x60 word[24] = CItem::
        // isAmplifyPollutionItem() const（0x085136ac，nm 实证）。ORIG 后随
        // `xor $1; test %al; je` ⇒ 源形为负极性 `if (!item->isAmplifyPollutionItem())`
        // （fn()==false 走 true 臂，0x8514f16 第三参=1；fn()==true 落 0x8514f43 第三参=0）。
        if (!item->isAmplifyPollutionItem())
        {
            inven.m_amp._setAbility((unsigned char)type, value, true);
        }
        else
        {
            inven.m_amp._setAbility((unsigned char)type, value, false);
        }
    }

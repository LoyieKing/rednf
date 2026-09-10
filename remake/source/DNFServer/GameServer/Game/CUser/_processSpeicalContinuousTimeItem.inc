void CUser::_processSpeicalContinuousTimeItem(unsigned int itemIdx)
{
    CItem* pItem = G_CDataManager()->find_item((int)itemIdx);
    if (pItem == 0)
        return;
    // ORIG 0x08659b60：is_stackable/IsExpAffect 均正向 `test %al,%al; je 共享尾
    // 出口`（无 xor/jne 反形）⇒ 源码正向嵌套 if（P59 反向）；ratio 槽 -0xc
    // fstps+flds 往返；x87 fucompp/fnstsw/sahf 段属 N85 跨版本豁免。
    if (pItem->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)pItem;
        if (stack->IsExpAffect())
        {
            float ratio = stack->GetExpAffectRatio();
            if (ratio > 0.0f)
                SetExpAffectItemEffect(true, ratio);
        }
    }

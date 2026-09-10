void TimerFinishItemEffect::_processSepecialEffectContinuousItem(CUser* user,
                                                                 unsigned int itemIdx)
{
    // ORIG 0x08637d6b：mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne 尾。
    switch (itemIdx)
    {
    case 0x1cd1:
        user->SetExpAffectItemEffect(false, 0.0f);
        break;
    default:
        break;
    }

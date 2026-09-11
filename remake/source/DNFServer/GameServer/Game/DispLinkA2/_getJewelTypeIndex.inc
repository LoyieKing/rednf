// ORIG 汇编（0x0821857c sz 0x1b）：形参 spill 后 movzwl 直传尾调，不读返回值。
int Dispatcher_DisJointAvatar::_getJewelTypeIndex(unsigned short type) const
{
    return getAvatarEmblemTypeIdx(type);
}
// b28d-solo-CUser AE: ORIG 0x0867?? `call; test %eax,%eax; setne %al; test %al,%al; je`
//   = 条件内联形（探针 /tmp/probe_bool.cpp a3/a5 逐条同形）；具名 bool 局部 4.4 恒
//   落槽（a2/a4：mov %al,-0x9; cmpb $0,-0x9）⇒ 删局部内联即归位。| _ZN5CUser13IsEquipAvatarEv
bool CUser::IsEquipAvatar()
{
    if (getCurCharacR() != 0)
        return ((const CInventory*)getCurCharacInvenR())->IsEquipAvatar();
    return false;
}
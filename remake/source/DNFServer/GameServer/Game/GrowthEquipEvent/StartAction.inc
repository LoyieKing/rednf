void GrowthEquipEvent::StartAction()
{
    // ORIG：遍历 GameWorld 用户表（map<ushort,CUser*>），对 get_state()>2 的
    // 在线用户逐个补发 SendGrowthWeaponEventInfo。
    // b28d-55（P50/P96/P28）：ORIG 用**非 const** _Rb_tree_iterator（ne/ptEv/ppEi
    // 三个非 const 实例，P167 差集实证）⇒ const_iterator 会额外生成
    // _Rb_tree_const_iterator 转换构造与 const 版成员；增量段 ORIG 为 ppEi（后置）
    // ⇒ 源写 `it++`。
    // P0/P183：ORIG 无 world 槽——begin/end 两处各自 `call G_GameWorld;
    // lea 0x134(%eax),%edx` 就地折叠（ORIG 0x080f7716 / 0x080f778a 两处独立调用），
    // 故 G_GameWorld() 内联进循环头/条件表达式。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
            SendGrowthWeaponEventInfo(*user);
    }

void CDeathTower::_onFinishDeathTower()
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x8467f7d：循环体首 `movl $0,-0xc`（user 先置 0 再赋值，范式8）；
        // || 共享物化岛（范式40）：0x8467f9f je→mov$1@8467fbc；第二操作数
        // `!checkValidUser` 走 xor $0x1,%eax@8467fb5（范式1 值形）。
        CUser* user = 0;
        user = m_party->get_user(i);
        // [b28d-D13] P0 清绑（N96）：ORIG 0x8467fbc mov$1 岛无槽 ⇒ 表达式直用。
        if ((user == 0) || (int)!m_party->checkValidUser(i))
            continue;
        onLeaveUser(user);
    }
    m_party->ReturnToVillage();
    if (getTowerState() > 0)
    {
        _destroy();
    }

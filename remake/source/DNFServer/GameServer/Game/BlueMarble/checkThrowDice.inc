bool BlueMarble::checkThrowDice(int seat)
{
    // [b28d-32] ORIG 0x080d6ed8：三条件全真 → bVar1=0 → `test al; je; mov$1`
    // ⇒ 返回 true（0x080d6f46 mov$0 / 0x080d6f56 mov$1）。旧实现两个 return 值
    // 写反（全真返回 false）= 真语义 bug，已对调；形态与 ORIG 逐块同构
    // （bVar1 物化 + `test al; je; mov$0/$1` 双岛）。
    if (!(m_users[seat].getBoardZone() < getMaxBoardSize() &&
          m_users[seat].getUserState() == (BlueMarbleUserState::T)1 &&
          getState() == 2))
    {
        return false;
    }
    return true;

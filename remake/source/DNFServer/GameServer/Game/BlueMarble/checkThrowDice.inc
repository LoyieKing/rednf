// b28c-1 N: 第三条件 je/jne 极性（4.4 块序折叠；三目/表达式/具名 bool 三形实测均不可翻）。
// [b28d-32 AE + 真语义 bug: 旧实现两 return 值写反（全真返回 false）；取反条件形复现 ORIG 三条件 jle/jne/je + bVar1 双岛 | ORIG 0x080d6ed8]
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
}
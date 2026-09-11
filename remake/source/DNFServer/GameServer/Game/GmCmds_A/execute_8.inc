// ORIG execute 0x084b25a4：enter 标记优先重置入场次数；否则 move 标记 + 步数
// 有效时驱动大富翁移动。
void BlueMarbleSet::execute()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    if (m_flag2 != 0)
    {
        user->resetBlueMarbleEnterCount();
        user->sendBlueMarbleEnterCount();
        return;
    }

    BlueMarble* blueMarble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (blueMarble == 0)
    {
        return;
    }
    if (m_flag1 == 0)
    {
        return;
    }
    if (m_flag0 == 0)
    {
        return;
    }
    // P196：ORIG 对 m_flag0(+0x08, char 存 GM 参数 0-255, parse 里 static_cast<unsigned char>)
    // 作实参直传时 `movzbl %al,%eax`（无符号扩展），我方 static_cast<int> 使 4.4 出 movsbl
    // （符号）⇒ 值≥0x80 时 movePosition 收负 pos，真 bug。显式经 unsigned char 提升复现 movzbl。
    blueMarble->movePosition(user, static_cast<int>(static_cast<unsigned char>(m_flag0)));
}
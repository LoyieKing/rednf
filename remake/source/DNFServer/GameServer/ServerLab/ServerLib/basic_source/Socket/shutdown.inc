void TCPSocket::shutdown(int opt)
{
    // 语义还原（2026-08-11 用户规矩：不允许硬套 asm）。
    (void)opt;
    // ORIG 4.4.6-3 保留该丢弃比较（mov;mov;cmp;pop;ret，13 字节）；裸表达式会被
    // 本工具链在 gimplify 阶段折叠，switch 双空 case 形态可复现逐字节一致。
    switch (sock_ == -1)
    {
    case 0:
        break;
    case 1:
        break;
    }

// ORIG 0x081ad2c0：0..3 静默；default 打印；恒返回 16。
// case 0..3 → goto 共用出口标号（技巧23/24）：复现 ORIG 0x081ad2d0-0x081ad2ff
// 二分树 dispatch（cmp$1 je / cmp$1 jg / test je / jmp / cmp$2 je / cmp$3 je）
// + 每 case `nop; jmp merge` 岛；直接 break 会被 4.4 折成 jbe 范围形。
int NCashImpl::handle_coupon_error(int err)
{
    int result = 0x10;
    switch (err)
    {
    case 0: goto Lsilent;
    case 1: goto Lsilent;
    case 2: goto Lsilent;
    case 3: goto Lsilent;
    default:
        printf("[%s] Invalid coupon packet_id: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
Lsilent:
    return result;
}
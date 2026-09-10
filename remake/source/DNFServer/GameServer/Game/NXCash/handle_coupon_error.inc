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

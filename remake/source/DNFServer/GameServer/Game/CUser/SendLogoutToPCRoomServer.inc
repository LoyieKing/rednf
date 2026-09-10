void CUser::SendLogoutToPCRoomServer()
{
    char login[0x210];
    // b28d-lane5：同 SendLoginToPCRoomServer——ORIG 0x086817xx 逐字段重发
    // `lea -0x226(%ebp)`（P183），删 lp 指针缓存。

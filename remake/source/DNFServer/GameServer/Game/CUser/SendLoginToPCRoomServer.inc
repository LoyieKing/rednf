void CUser::SendLoginToPCRoomServer()
{
    char login[0x20e];
    char reply[0x20e];
    memset(login, 0, sizeof(login));
    // b28d-lane5：ORIG 0x0868xxx 每次字段访问重发 `lea -0x226(%ebp)`（P183 地址
    // 重物化铁律）⇒ 删除 lp 指针缓存，全表达式展开。

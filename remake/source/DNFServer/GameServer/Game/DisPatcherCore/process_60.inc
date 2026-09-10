int DisPatcher_AddFriend::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    // P31 帧差：ORIG sub $0x58 → dead 局部 0x40（薄包装仍保留深帧）。
    volatile char dead_40[0x40]; (void)dead_40;
    return process_packet_jpn(user, msg, param);
}

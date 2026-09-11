void Dispatcher_SaveGameOption_ChattingEmoticon::send(CUser* user, ParamBase& param)
{
    (void)param;
    // ORIG 0x31：三连调用（错误码 0x1d9 应答 + 重算 + 广播表情）。
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1d9);
    user->ReCalcChattingEmoticon();
    user->SendChattingEmoticon();
}
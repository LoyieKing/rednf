void APSystem::CUserProc::ClearActionAndSendtoUser(CUser* user,
                                                   _ActionGroupIndex group,
                                                   int value,
                                                   ENUM_PACKETCLASS packetClass)
{
    // [b28c-4] ORIG 0x08122390：空体保留 sub $0x10 帧 ⇒ 原码含未使用局部。
    // 4.4 实测对未使用局部同样占槽（/tmp/l4/frame.cpp f1/f3），照抄帧。
    char framePad[0x10];
    (void)framePad;

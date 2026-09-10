void APSystem::CUserProc::SendActionListState(CUser& user)
{
    // [b28c-4] ORIG 0x081224a0：空体 sub $0xd20 帧 ⇒ 未使用局部 char[0xd20]。
    char framePad[0xd20];
    (void)framePad;

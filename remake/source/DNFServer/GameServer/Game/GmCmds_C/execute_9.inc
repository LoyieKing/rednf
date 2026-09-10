void CGuildExpBookDelete::execute()
{
    CUser* user = GetUser();
    user->deleteGuildExpBook();   // 真实定义 UserPartyLink4.cpp（T 0x0867e898）

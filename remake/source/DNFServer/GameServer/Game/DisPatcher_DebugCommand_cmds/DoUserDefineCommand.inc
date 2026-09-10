int DoUserDefineCommand(CUser* user, int cmd, const char* arg)
{
    game_master::CParser parser;
    parser.m_user = user;                                        // ORIG -0x130 = parser+0x04
    sprintf(parser.m_buf, "%d %s", cmd, arg);                    // ORIG 0x820bad4
    parser.parse();                                              // ORIG 0x820bae2 直接 call
    game_master::CCommand* command =
        GlobalData::s_GameMasterMgr->GetCommand(parser.m_id);    // ORIG 0x820baf9（m_id=parser+0x18）
    if (command != 0)
    {
        command->SetParser(&parser);                             // ORIG 0x820bb17
        GlobalData::s_GameMasterMgr->HandleRequest(command);     // ORIG 0x820bb2b
    }
    // ORIG 尾 ret 无 eax 写（返回值未定义，调用方不使用）——不写 return。

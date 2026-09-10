int Dispatcher_UserHistoryLog::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081da6c0：单一 msg 槽 -0xc；lea 0x14(%eax) 先、movzbl 0xd(%eax) 折叠重读。
    // 具名字段视图（技巧14）：char* 下标反产 add+间接两步，字段直读才折叠单级。
    struct MsgView { char pad0d[0xd]; unsigned char b0d; };
    char* m = reinterpret_cast<char*>(&msg);
    user->LogHistory("act,%d,%s",
                     (int)reinterpret_cast<MsgView*>(m)->b0d, m + 0x14);
    return 0;

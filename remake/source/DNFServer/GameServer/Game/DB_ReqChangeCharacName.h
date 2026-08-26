#ifndef GAME_DB_REQCHANGECHARACNAME_H_
#define GAME_DB_REQCHANGECHARACNAME_H_

// df_game_r 还原 —— DB_ReqChangeCharacName（角色改名 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_ReqChangeCharacName/makeRequest.md，
// ORIG 0x08426590 sz 0x113。
class DB_ReqChangeCharacName
{
public:
    // ORIG T 0x08426590：mangled _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
    // 流：表头 0xc3，正文 accId(int)，随后 GetInBuffer<SIG_CHANGE_CHARAC_NAME>()
    // 直填：+0x00 characIdx(int)、+0x04 name(strncpy 0x1d)、+0x24 flag(int)、
    // +0x2c 渠道组低字节（CEnvironment+0x378 = m_serverEnvir.m_serverGroup）。
    // Acquire 行号常量 0x64de。
    static void makeRequest(int accId, int characIdx, char const* name, int flag);
};

#endif  // GAME_DB_REQCHANGECHARACNAME_H_

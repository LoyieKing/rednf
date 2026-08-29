#ifndef GAME_DB_REQCHANGECHARACNAME_H_
#define GAME_DB_REQCHANGECHARACNAME_H_

// df_game_r 还原 —— DB_ReqChangeCharacName（角色改名 DB 请求）。
// 唯一声明点。参考 docs/class_func_reports/DB_ReqChangeCharacName/{makeRequest,dispatch,_sendResult,_hasSameCharacName}.md，
// makeRequest ORIG 0x08426590 sz 0x113；dispatch ORIG 0x084262ea sz 0x2a5；
// _sendResult ORIG 0x084266a4；_hasSameCharacName ORIG 0x084267c2。
// 继承 DBDispatcher：ORIG vtable _ZTV22DB_ReqChangeCharacName @0x08c5f3d0
// [offtop][typeinfo][dispatch=0x084262ea][~D1=0x0845eb60][~D0=0x0845eb90]；
// C1/C2 W @0x0844e63e（对象尺寸 4 = 仅 vptr，W8LT_DBTask.cpp 局部类提供）。
#include "DBDispatcher.h"

class MySQL;
struct SIG_CHANGE_CHARAC_NAME;

class DB_ReqChangeCharacName : public DBDispatcher
{
public:
    DB_ReqChangeCharacName() {}

    // ORIG T 0x08426590：mangled _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
    // 流：表头 0xc3，正文 accId(int)，随后 GetInBuffer<SIG_CHANGE_CHARAC_NAME>()
    // 直填：+0x00 characIdx(int)、+0x04 name(strncpy 0x1d)、+0x24 flag(int)、
    // +0x2c 渠道组低字节（CEnvironment+0x378 = m_serverEnvir.m_serverGroup）。
    // Acquire 行号常量 0x64de。
    static void makeRequest(int accId, int characIdx, char const* name, int flag);

    // ORIG T 0x084262ea：mangled _ZN22DB_ReqChangeCharacName8dispatchEiiP6Stream
    // 虚覆盖（vtable 槽 2）。基类 dispatch 校验通过后执行改名：
    // GetOutBuffer 读请求体 → escape_string 名字 → 重名检查（_hasSameCharacName）→
    // UPDATE charac_info → 成功后同步 guild_member（按 charac_no 读 guild_id）。
    virtual bool dispatch(int userIdx, int slot, Stream* stream);

    // ORIG T 0x084266a4：mangled _ZN22DB_ReqChangeCharacName11_sendResultEPK22SIG_CHANGE_CHARAC_NAMEii
    // 组装 SIG_CHANGE_CHARAC_NAME 结果包发回 DISPATCH_Q（表头 0xc3，Acquire 行号 0x64f8）。
    void _sendResult(SIG_CHANGE_CHARAC_NAME const* sig, int slot, int result);

    // ORIG T 0x084267c2：mangled _ZN22DB_ReqChangeCharacName18_hasSameCharacNameEP5MySQLPKcS3_
    // 重名检查：delete_flag 查询；返回 0=无重名 / 2=查询失败 / 0x14=未删除重名 / 0x18=已删除重名。
    int _hasSameCharacName(MySQL* db, char const* table, char const* name);
};

#endif  // GAME_DB_REQCHANGECHARACNAME_H_

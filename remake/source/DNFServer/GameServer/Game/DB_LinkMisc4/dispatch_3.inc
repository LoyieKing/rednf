// ============================================================================
// DB_ReqChangeCharacName::dispatch @ 0x084262ea
// mangled _ZN22DB_ReqChangeCharacName8dispatchEiiP6Stream
// 基类 DBDispatcher::dispatch 校验失败 → false；随后：
// GetDBHandle(DB_HANDLE_2,0) → GetOutBuffer<SIG_CHANGE_CHARAC_NAME>() 读请求体 →
// escape_string 名字 → env+0x68 m_dbConnInfo[DB_HANDLE_2].m_db(+0x42) 库名 →
// _hasSameCharacName 重名检查（!=0 直接 _sendResult 返回）→
// UPDATE charac_info；exec&&affected!=0 失败 → _sendResult(2) →
// GetDBHandle(DB_HANDLE_8,0) 读 guild_member.guild_id（失败/无行清零 m_arg28）→
// _sendResult(slot, 0)。
// ============================================================================
bool DB_ReqChangeCharacName::dispatch(int userIdx, int slot, Stream* stream)

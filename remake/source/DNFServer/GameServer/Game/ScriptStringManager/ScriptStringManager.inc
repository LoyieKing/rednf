// 构造 @ 0x8acf110：vptr 赋值 + strings_ 三指针清零 + string_map_ header
// 初始化（_M_key_compare 默认、_M_header{red,0,&header,&header}、
// _M_node_count=0）。标准成员初始化即等价。
ScriptStringManager::ScriptStringManager() {}

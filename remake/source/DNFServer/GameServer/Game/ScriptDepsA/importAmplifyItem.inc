// ORIG importAmplifyItem T 0x0890a6b0（报告 ## 汇编 + ## 反编译 C 全文转写）：
// 返回错误行号，0=成功。[option mapping table] 建 map<string,ENUM_AMPLIFY_TYPE>；
// [option data] 查表得类型，{type,(short)(sum),(short)value} 入 m_options；
// 三个 hash_map 投资段 key=ScanInt、value={查表类型, 第二个 ScanInt}。
int importAmplifyItem(const char* path, stAmplifyItemScript_t& out)

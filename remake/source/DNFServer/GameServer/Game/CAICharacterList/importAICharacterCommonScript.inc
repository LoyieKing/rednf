bool importAICharacterCommonScript(STAICharacterCommonScript* script,
                                   const char* path, bool byLevel);

// ---- init（ORIG 0x0834a28c）：truncate 失败打印并返 false；m_maxAP 置
//      0x7fffffff；遍历文件列表逐条 clear→new→import（失败 printf+析构释放+
//      返 false）→FindIndex→insert_db（失败 printf+返 false，ORIG 不释放
//      aiChar）→独立掉落收集（push_back 到 script.m_f2c）→set（重新
//      FindIndex）→_add。----

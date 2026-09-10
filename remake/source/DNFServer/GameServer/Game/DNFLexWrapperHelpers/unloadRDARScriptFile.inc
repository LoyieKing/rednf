void unloadRDARScriptFile()
{
	// ORIG 0x088bc054：仅调用 g_dnfLex.closeInputStream()（0x8acea70），
	// 关闭 DNFLex/CompiledDNFLex 输入流；token 列表由 loadRDARScriptFile
	// 每次加载时自行清空。
	g_dnfLex.closeInputStream();

// resetQuiz @ 0x080caa64（docs/class_func_reports/CBingo/resetQuiz.md）：
// 按日取 stBingoScript.m_map0（CDataManager+0xb568，map<int,int>）当日 quiz 值写入
// +0x0c（m_fieldC）。day 越界/无配置返回 0。
// getTodayBoard @ 0x080caec4（docs/class_func_reports/CBingo/getTodayBoard.md）：
// 返回 this（空函数体）。
CBingo* CBingo::getTodayBoard()
{
    return this;
}
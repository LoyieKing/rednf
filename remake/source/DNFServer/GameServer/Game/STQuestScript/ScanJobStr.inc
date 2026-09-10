bool ScanJobStr(std::string* jobName, std::string** out);  // _Z10ScanJobStrPSsPS_ 0x08a6143f（ScanJobStr.cpp）


// ---- ORIG 全局脚本文件列表缓冲（initQuestScript/initTownScript 使用）。
// g_questScriptBaseDirectory 定义见下（ORIG B 0x9500380，供 EpisodeScript 等
// loadRDARScriptFile 首参，另一处 0x580 行已定义）；此处仅 town 目录定义。
char g_townScriptBaseDirectory[0x100];    // ORIG B 0x9500da0
// [b28d-solo-STQuestScript · P304] 权威型声明：ORIG 调用点 `movl $&g_..,(%esp)`
// 为地址直压；声明成 std::map 后再 reinterpret_cast 会物化 %eax（P304）。
// STScriptFileList 的 +0 即 std::map<int,std::string>（NpcScript.h，sizeof 0x18），
// 定义处（GameWorld.cpp map / InitScriptProducers_4.cpp map）符号同名同址，ABI 不变。

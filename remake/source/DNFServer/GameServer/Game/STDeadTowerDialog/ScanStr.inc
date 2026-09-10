bool ScanStr(std::string* value);

// ORIG T 0x08908d46 importDeadTowerDialogScript(path, STDeadTowerFullDialog&)：
// loadRDARScriptFile 失败返 0x18d9；按 token 分派三组对话框，ScanInt 失败返
// 0x18e6/0x18f6/0x1906，ScanStr 失败返 0x18ea/0x18fa/0x190a；成功返 0。
// 211/211 AE：bool ok 函数级单槽（-0x9，P320）、token 先于 loadRDARScriptFile
// 构造（-0x10）、三分支分别写 m_flag1/m_flag2/m_flag3（ORIG +0x8/+0x9/+0xa）、
// ScanType 失败改 break 共享尾部 return 0（P164 jne 尾块 +0x299）。

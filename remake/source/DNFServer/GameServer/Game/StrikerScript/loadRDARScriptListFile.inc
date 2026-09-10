bool loadRDARScriptListFile(STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);

// ============================================================================
// StrikerScript 查询/导入补充（报告全文转写）
// ============================================================================

// ORIG T 0x08a9e79c：m_fileList(+0x90) 直接透传 loadRDARScriptListFile，
// dir="" 且 listPath="etc/LinkSystem/strikercombo.lst"，第 4 参 0；
// 返回非 0 才为 true（P22 xor $1/test 反相形：失败分支 mov $0）。

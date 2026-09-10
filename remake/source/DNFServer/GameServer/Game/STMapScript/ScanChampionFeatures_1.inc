void ScanChampionFeatures(int count, STAICharacterMapArrangeData& data);

// ===========================================================================
// ImportMapScript —— ORIG T 0x089dcf54（标签分派逐分支转写；ORIG 的
// static map<const char*,int> 查找表还原为等价 if/else 链）
// ===========================================================================

// STMapScript 词索引视图（P82/P95 具名视图）：ORIG [pathgate pos]/
// [pathgate recognize range] 以 `words[i*2+j+0x40/0x48]` 形态做
// `add $imm,%ecx; mov %eax,(%base,%ecx,4)` 三步寻址（报告 0x1bae-0x1bd4、
// 0x1c35-0x1c58），数组基址即对象首（m_index@+0x000）。

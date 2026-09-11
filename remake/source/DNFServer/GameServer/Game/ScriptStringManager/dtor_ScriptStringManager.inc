// 析构 @ 0x8acf5f0（D1）/ @ 0x8acf700（D0=delete 版）：string_map_ 整树
// 释放 + strings_ 各元素释放 + 向量缓冲释放。标准成员析构链即等价。
ScriptStringManager::~ScriptStringManager() {}

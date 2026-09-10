bool loadRDARScriptListFile(class STScriptFileList* list, const char* dir,
                            const char* listPath, std::map<int, int>* unused);
// _Z22loadRDARScriptListFile.. T 0x088bbd77（STScriptFileList.cpp，定义侧已 bool；
// P292：ORIG 调用点 `test %al,%al` 直判 ⇒ 本 TU 旧 int 声明产 test/sete 物化）。

// ---- PvP 全局表：定义于 PvP_Room.cpp 的三个，此处仅 extern 引用 ----
// [b28d-Main 代落·S165] 元素 unsigned char→bool（定义侧 PvP_Room.cpp:65 已改；
// 消 ODR 不一致；sizeof/地址 0x94f70e0 不变，写侧值域 0/1）。

// 安全分发器的协议检查函数由安全子系统提供；这里只保留 ORIG 的薄包装。
// 这些声明对应 docs/class_func_reports/Dispatcher_Security_*/dispatch_sig.md
// 中的全局 C++ 符号，定义缺失时须在安全子系统批次补齐，不能用 no-op 替代。
int _NS_IPScan(CUser* user, PacketBuf& packet);

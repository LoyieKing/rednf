void convert_string_to_time(const char* str, int& out)
{
    // [b28d-solo-OnlyOrigBatch11] 两处形订正（探针/报告 0x08358208）：
    // ①ORIG 在 time_from_string 返回后**立即** D1 析构 string（+0x047），随后才
    //   D1 析构临时 allocator（+0x078）⇒ 原码是 `time_from_string(std::string(str))`
    //   的**临时对象实参**，不是函数作用域具名 `std::string s(str)`（后者析构被
    //   推迟到函数末尾 EH 尾块，实测调用序整体错位）。
    // ②日志第 3 参 ORIG 为 `_ZZ22convert_string_to_timePKcRiE19__PRETTY_FUNCTION__`
    //   局部静态符号（P118/P139）⇒ 必须写 `__PRETTY_FUNCTION__`，手写字面量只生
    //   匿名 .rodata（实测 .o 立即数 0x398 即该伪差）。
    // 余差：catch 内 `e.what()` 间接虚调用目标寄存器 ORIG %edx vs 4.4.7 恒 %eax
    // （N7/P230 补：4.4.6 后端才用 %edx；本 TU 路由 4.4.7 后端不可切）⇒ N7 封顶。
    try
    {
        boost::posix_time::ptime pt =
            boost::posix_time::time_from_string(std::string(str));
        struct tm tmv = boost::posix_time::to_tm(pt);
        out = static_cast<int>(mktime(&tmv));
    }
    catch (const std::exception& e)
    {
        // ORIG catch 块：what() → logFormat(1, "data_manager.cpp",
        // "void convert_string_to_time(const char*, int&)", 0x1920,
        // "Exception: %s")
        LogManager::logFormat(1, "data_manager.cpp",
                              __PRETTY_FUNCTION__,
                              0x1920, "Exception: %s", e.what());
    }

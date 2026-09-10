CFileLogWriter::~CFileLogWriter() {
    // 原始析构仅销毁 map 与 mutex，不释放 CFLog 指针（与原始一致）

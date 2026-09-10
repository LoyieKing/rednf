CFLog::~CFLog() {
    fclose(file);  // 原始：无条件 fclose（与原始一致）

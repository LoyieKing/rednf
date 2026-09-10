CFLog::CFLog(std::string& filename) : name() {
    // 原始：无 file 初始化（fopen 直接赋值）；c_str() 作用于参数而非成员
    name = filename;
    // 原始：fopen 赋值在条件内（mov [edx+4],eax; test; sete al; test al,al; je 形态）
    if ((file = fopen(filename.c_str(), "ab")) == NULL) {
        file = stdout;
    }

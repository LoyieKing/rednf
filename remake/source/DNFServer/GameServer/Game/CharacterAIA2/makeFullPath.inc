bool makeFullPath(std::string directory, std::string& path)
{
    // ORIG 0x088c99d4..0x088c99de：两处 `cmpl $0x0; jns/js` 符号位测试
    // （4.1 把 ==npos 折成 <0 判）→ 用有符号比较复现 cmp 立即数形；
    // 第二分支 js→then 的前跳为 4.1 jump-threading（N 类残余）。
    const int slash = path.find('/', 0);
    const int backslash = path.find('\\', 0);
    if (slash >= 0 || backslash >= 0)
        return false;
    path = directory + path;
    return true;

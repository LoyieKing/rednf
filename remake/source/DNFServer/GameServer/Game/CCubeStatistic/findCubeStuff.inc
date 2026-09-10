bool CCubeStatistic::findCubeStuff(int groupValue)
{
    // ORIG：拷贝 getItemGroupNameTable() 得本地 map，find("cube stuff")；
    // 命中且 it->second == 入参 → true，否则 false。
    std::map<std::string, int> table(getItemGroupNameTable());
    std::map<std::string, int>::iterator it = table.find("cube stuff");
    if (it != table.end())
    {
        // P225：ORIG 0x0860df6e 内层条件为 `cmp; sete %al; test %al,%al; je`
        // bool 四段物化——while+return 形复现（if 短路链恒折 jne 直跳）。
        while (it->second == groupValue)
            return true;
    }
    return false;

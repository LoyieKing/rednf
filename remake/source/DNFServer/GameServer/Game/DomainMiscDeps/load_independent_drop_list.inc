// ORIG 0x0897c0d8（0xec 字节）：遍历 g_independentDropScriptFileList，
// 逐文件 importIndependentDropParameterScript；失败按行号 0x30 记日志返回 0。
bool IndependentDropParameterScript::load_independent_drop_list()
{
    std::map<int, std::string>::iterator it;
    it = g_independentDropScriptFileList.m_fileMap.begin();
    for (; it != g_independentDropScriptFileList.m_fileMap.end(); ++it)
    {
        const char* name = it->second.c_str();
        // b28c-33：ORIG 0897c1bd 为 `cmpl $0,-0xc; je 尾 return-true 块`（P222 正向
        // 包裹/共享尾），早返形使 4.4 反产 `jne; mov $1; jmp` 岛。
        if (name)
        {
            if (!importIndependentDropParameterScript(name))
            {
                LogManager::logFormat(
                    1, "../RDARScriptIndependentDrop.cpp",
                    __FUNCTION__, 0x30,
                    "IndependentDropParameterScript File Script Error: %s\n",
                    name);
                return false;
            }
        }
        else
        {
            break;                             // 空名 → 跳出，共享尾 return true
        }
    }
    return true;
}
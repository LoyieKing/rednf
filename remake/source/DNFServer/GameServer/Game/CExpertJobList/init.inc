bool CExpertJobList::init()
{
    // ORIG 0x896d8ec：无空指针检查，直接遍历 g_expertJobScriptFileList 对象。
    const char* path = 0;   // ORIG -0x1c(%ebp)：循环内最后 path，ETC 失败时作 %s 参数
    int job;                // ORIG -0x24(%ebp)（4.4 恒排深槽区 -0x40，见 N82附 标量分区）
    // b28d-solo [P96/P50：ORIG 用非 const iterator + 独立 C1Ev 默认构造（-0x44）
    // + begin() 临时槽 -0x4c 后内联拷贝赋值；P28：增量段为 ppEi（后置 it++）]。
    std::map<int, std::string>::iterator it;
    for (it = g_expertJobScriptFileList.m_fileMap.begin();
         it != g_expertJobScriptFileList.m_fileMap.end(); it++)
    {
        path = it->second.c_str();
        if (path == 0)
            break;
        STExpertJobScript* script = new STExpertJobScript();
        if (!ImportExpertJobScript(script, path))
        {
            printf("expert job Script Error: %s\n", path);
            delete script;
            return false;
        }
        job = g_expertJobScriptFileList.FindIndexByFullScan(path);
        m_jobMap.insert(std::make_pair(job, script));   // ORIG 0x896d8ec：find 后 insert（NEW 二进制/源码曾丢失）
    }
    if (!ImportExpertJobEtcScript(m_etc, "expertjob.etc"))
    {
        printf("ExpertJob ETC Script Error : %s\n", path);
        return false;
    }
    return true;

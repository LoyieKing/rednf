// ============================================================================
// init（ORIG 0x896d8ec）
// [b2c-P167/P61] ORIG 0x0896d9f9 调 `_ZSt9make_pairIiRP17STExpertJobScriptE...`
// （第一模板参 **int**）；旧写法 `int job = ...; make_pair(job, script)` 传具名左值 ⇒
// 4.4 推导 int&（`_ZSt9make_pairIRiRP17...`）。ORIG 调用点 `mov %eax,-0x24(%ebp)`（函数
// 返回值落临时对象）+ `lea -0x24(%ebp),%edx; mov %edx,0x4(%esp)`（传该临时地址给
// `T1&&`）⇒ 第一实参是 **FindIndexByFullScan(...) 的 prvalue**（P61）。故把调用内联进
// make_pair 实参位、删具名 `int job`。
// b28d-solo-CExpertJobList 判词（NEAR / N82附 封顶）：133=133 助记符序列全等；
// 残差仅 job 槽位（ORIG -0x24 vs 本 TU -0x40）与其后 make_pair/pair/insert/end
// 四个临时槽整体差 4——4.4 标量分区排布 vs 4.1 按生命周期穿插（N82附；job 在
// 函数头/循环体首/循环尾三形实测均落 -0x40..-0x48，it/script/begin 临时槽已对齐）。
// ============================================================================
bool CExpertJobList::init()
{
    // ORIG 0x896d8ec：无空指针检查，直接遍历 g_expertJobScriptFileList 对象。
    const char* path = 0;   // ORIG -0x1c(%ebp)：循环内最后 path，ETC 失败时作 %s 参数
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
        // ORIG 0x896d8ec：find 后 insert（NEW 二进制/源码曾丢失）
        m_jobMap.insert(std::make_pair(
            g_expertJobScriptFileList.FindIndexByFullScan(path), script));
    }
    if (!ImportExpertJobEtcScript(m_etc, "expertjob.etc"))
    {
        printf("ExpertJob ETC Script Error : %s\n", path);
        return false;
    }
    return true;
}
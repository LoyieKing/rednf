int eventReward::importRewardTable(const char* dir, const char* file)
{
    // [b28d-solo-CDataManagerScripts] 真实 STScriptFileList（ORIG 0x080ef2a7 调
    // _ZN16STScriptFileListC1Ev；此前用 map + reinterpret_cast 会调 map C1 且多
    // 一条取址 lea、槽整体深 4）。
    STScriptFileList fileList;
    // ORIG 0x080ef2a7：test %al; je 失败块（末尾 mov $0x77）⇒ 正极性包裹
    // （P214①），勿写 if(!f()) 早返（产 xor$1 + 前置换位）。
    if (loadRDARScriptListFile(&fileList, dir, file, 0))
    {
        // fileList 为非 const 局部 ⇒ ORIG 用非 const iterator（const_iterator 换
        // begin/end/operator++ callee 集致 we-larger 主因）。ORIG 0x080ef2ce 在
        // 循环前调一次 end() 存具名 e（槽 -0x28）⇒ P13 缓存 end。
        std::map<int, std::string>::iterator it = fileList.m_fileMap.begin();
        std::map<int, std::string>::iterator e = fileList.m_fileMap.end();
        for (; it != e; ++it)
        {
            // ORIG 0x080ef2f7 `test %eax,%eax; je` 直接分支（4.4 对 ==0 物化 sete）。
            if (!ImportRewardScript(dir, it->second.c_str()))
                return 0x71;
        }
        return 0;
    }
    return 0x77;

// ============================================================================
// eventReward::importRewardTable（ORIG 0x080ef29c T）
// 列表装载失败返 0x77；逐文件 ImportRewardScript 失败返 0x71；成功返 0。
// ORIG STScriptFileList 即 +0 的 map<int,string>（NpcScript.h 占位版同尺寸），
// 故直接以本地 map 重解释传入，避免与本 TU 已含头中的占位定义冲突。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：①P214① 正极性包裹；②真实 STScriptFileList 局部（ORIG 调 _ZN16STScriptFileListC1Ev；原 map+reinterpret_cast 调错 C1 符号且槽深 4）；③P13 缓存 end（ORIG 循环前调一次 end 存 -0x28）；④`!ImportRewardScript(...)` 复现 ORIG `test %eax,%eax; je` 直分支；fresh clean 82=82；ORIG 0x080ef29c。
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
}
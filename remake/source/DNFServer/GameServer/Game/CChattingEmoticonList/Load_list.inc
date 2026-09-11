// ============================================================================
// Load_list（ORIG 0x80e54d2）
// 遍历 g_ChattingEmoticonScriptFileList.m_fileMap，逐条 ImportChattingEmoticon；
// Factory 成功→ m_map.find(GetIndex()) 判重（重复时虚析构 delete 返回失败）→
// m_map.insert(make_pair(GetIndex()【第二次独立调用，P48】, emo))；
// **m_mapVec 按 script.m_type(+0x4) 入桶**（ORIG +0x187/+0x2ac 传 &script+4，
// 非 GetIndex）；桶缺失时显式 vector 临时+insert+逆序析构，最后统一下标 push_back。
// path==NULL 时 break → return true（+0x310 尾岛 mov $1）。
// 收敛口径：ORIG/ours 245/245，助记符序列全等（NEAR），栈槽逐条一致
// （-0xc/-0x68/-0x64/-0x7c/-0x80/-0x60/-0x58/-0x5c/-0x44/-0x4c/-0x54/
// -0x84/-0x38/-0x90/-0x1c/-0x2c/-0x34 全等）。残余文本差仅两类：
// ① call/数据地址 .o 未重定位显示（链接后 AE 核销）；② 判重 delete 的
// 虚调用 callee 寄存器 %edx↔%eax 互换（范式库 N7，勿再试）。
// ============================================================================
bool CChattingEmoticonList::Load_list()
{
    // [b26L17 判词 N7] 唯一残差：1 处虚调用 callee 装载 %eax↔%edx 互换
    //   （244/244 助记符全等）。
    const char* path = 0;                                  // ORIG -0xc
    std::map<int, std::string>::iterator it =
        g_ChattingEmoticonScriptFileList.m_fileMap.begin();  // ORIG -0x68
    EmoticonType::EmoticonScript script;                   // ORIG -0x7c 环外构造一次

    while (it != g_ChattingEmoticonScriptFileList.m_fileMap.end())
    {
        path = it->second.c_str();          // +0x38：operator->+4 → c_str → -0xc
        if (path == 0)
            break;
        if (!ImportChattingEmoticon(script, path))
        {
            printf("Mission Script Error: %s\n", path);
            return false;
        }
        ChattingEmoticonBase* emo = Factory(script);
        if (emo == 0)
            return false;
        if (m_map.find(emo->GetIndex()) != m_map.end())
        {
            delete emo;
            return false;
        }
        m_map.insert(std::make_pair(emo->GetIndex(), emo));
        std::map<int, std::vector<ChattingEmoticonBase*> >::iterator vit =
            m_mapVec.find(script.m_type);
        if (vit == m_mapVec.end())
        {
            std::vector<ChattingEmoticonBase*> vec;
            m_mapVec.insert(std::make_pair(script.m_type, vec));
        }
        m_mapVec[script.m_type].push_back(emo);
        ++it;
    }
    return true;
}
// ---------------------------------------------------------------------------
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
// IsUsableEmoticons —— ORIG T 0x0835fc9a _ZN12CDataManager17IsUsableEmoticonsERK5CUserRKSt6vectorIiSaIiEE
//   （非 const；sret 返回 vector<int>；报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     result 默认构造；m_emoticonList(+0x28) 为空直接返回空表；
//     遍历 emoticons：find_emoticon(emoticons[i]) 未命中跳过；
//     命中且 !isUsable(user)（vtable+0x08 虚调用，bool 取反 P22）
//     → result.push_back(emoticons[i])。
//   N7 封顶：虚调用侧 4.4 vptr→%eax/obj→%edx（ORIG vptr→%edx/obj→%eax，
//   call *%edx↔*%eax 互换）；fn_diff_tu 归一残差仅此 5 行，跨版本寄存器分配
//   差无源码解（负例 N7 族），语义等价封顶。
// ---------------------------------------------------------------------------
std::vector<int> CDataManager::IsUsableEmoticons(const CUser& user,
                                                 const std::vector<int>& emoticons)
{
    std::vector<int> result;
    if (m_emoticonList != 0)
    {
        ChattingEmoticonBase* p;
        for (unsigned int i = 0; i < emoticons.size(); ++i)
        {
            p = m_emoticonList->find_emoticon(emoticons[i]);
            if (p == 0)
                continue;
            if (!p->isUsable(user))
                continue;
            result.push_back(emoticons[i]);
        }
    }
    return result;
}
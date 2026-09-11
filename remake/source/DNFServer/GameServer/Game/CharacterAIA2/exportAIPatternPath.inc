// ============================================================================
// exportAIPatternPath —— ORIG T 0x08a223b0
// 报告 _lt_global_gt_/exportAIPatternPath.md 完整转写。
// 遍历指定桶内模式名，截掉 m_dir 目录前缀后 fprintf 每行 "`%s`\n"。
// ============================================================================
void exportAIPatternPath(FILE* f, STAIInfoScript* info, int bucket)
{
    std::vector<std::string*>::iterator it;
    std::string out;
    it = info->m_patterns[bucket].begin();
    while (it != info->m_patterns[bucket].end())
    {
        out = (*it)->substr(info->m_dir.length(), (*it)->length());
        fprintf(f, "`%s`\n", out.c_str());
        ++it;
    }
}
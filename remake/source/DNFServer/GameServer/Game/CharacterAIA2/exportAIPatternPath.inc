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

void LogManager::logString(int level, const char* file, const char* func,
                           int line, const char* msg)
{
    prepareData();
    if (s_data->data[level])
    {
        if (s_indent > 0)
        {
            std::string str;
            for (int i = 0; i < s_indent; ++i)
            {
                str += "\t";
            }
            str += msg;
            s_data->data[level]->log(file, func, line, str.c_str());
        }
        else
        {
            s_data->data[level]->log(file, func, line, msg);
        }
    }
}
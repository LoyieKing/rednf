LOG_WRITER_SP<LogWriter> LogManager::getLogWriter(int index)
{
    prepareData();
    return s_data->data[index];
}
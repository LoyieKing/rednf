void LogManager::setLogWriter(int index, LOG_WRITER_SP<LogWriter> writer)
{
    prepareData();
    s_data->data[index] = writer;

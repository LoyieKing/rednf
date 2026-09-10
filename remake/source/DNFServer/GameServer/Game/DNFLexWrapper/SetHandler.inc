void DNFLexWrapper::SetHandler(bool (*isDbcsLeadChar)(char),
                               void (*lexerOutput)(const char*, int),
                               void (*lexerError)(const char*),
                               bool (*loadStream)(const char*, const char*,
                                                  DNFLex::stream_data_t*))
{
    dnfLex_.SetHandler(isDbcsLeadChar, lexerOutput, lexerError, loadStream);
    compiledDnfLex_.SetHandler(lexerError, loadStream);
    lexerError_ = (void*)lexerError;
    loadStream_ = (void*)loadStream;

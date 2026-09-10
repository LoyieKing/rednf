void DNFLex::SetHandler(TIsDbcsLeadChar isDbcs, TLexerOutput output,
                        TLexerError error, TLoadStream load)
{
    isDbcsLeadChar = isDbcs;
    lexerOutput = output;
    lexerError = error;
    loadStream = load;

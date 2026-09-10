DNFLexWrapper::DNFLexWrapper()
    : lastToken_(0),
      lastStringValue_(0),
      lexType_(LEX_TYPE_NORMAL),
      lexerError_(0),
      loadStream_(0)
{

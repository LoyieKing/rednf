// ---- DNFLex::PopInputStream(bool)（ORIG 0x8acd490）----
bool DNFLex::PopInputStream(bool pop_all)
{
    stream_t* finish = stream_stack.end().base();
    stream_t* start = stream_stack.begin().base();
    if (finish == start)
        return false;
    if (!pop_all && stream_stack.size() <= 1)
        return false;

    this->yypop_buffer_state();
    stream_stack.end().base()[-1].s.destroy();
    stream_stack.pop_back();
    this->yy_start = 1;
    unsigned lineno = 1;
    if (stream_stack.end().base() != stream_stack.begin().base())
        lineno = stream_stack.end().base()[-1].line_number;
    this->yylineno = (int)lineno;
    return true;
}
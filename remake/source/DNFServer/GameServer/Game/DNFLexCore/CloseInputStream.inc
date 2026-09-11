// ---- DNFLex::CloseInputStream()（ORIG 0x8acd5b0）----
void DNFLex::CloseInputStream()
{
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        do
        {
            PopInputStream(true);
        } while (stream_stack.end().base() != stream_stack.begin().base());
    }
}
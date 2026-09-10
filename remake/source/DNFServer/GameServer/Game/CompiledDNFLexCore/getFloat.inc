float CompiledDNFLex::getFloat(bool* result)
{
    if (lastToken_ != DNF_TOK_FLOAT)
    {
        if (result != 0)
            *result = false;
        return 0.0f;
    }
    if (result != 0)
        *result = true;
    static float floatBuffer;
    floatBuffer = *reinterpret_cast<float*>(offset_);
    offset_ += 4;
    return floatBuffer;

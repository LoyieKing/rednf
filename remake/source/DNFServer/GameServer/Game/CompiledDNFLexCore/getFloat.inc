// ---- getFloat（ORIG 0x8acffc0）：仅 FLOAT 有效。----
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
    // [b28d-G11 直证] 同 getInt：ORIG 符号表只有 helper 的
    // `_ZZ19readFloatFromBufferRPcE11floatBuffer`（V），无 getFloat()::floatBuffer
    // ⇒ 原码为 `return readFloatFromBuffer(offset_);`。
    return readFloatFromBuffer(offset_);
}
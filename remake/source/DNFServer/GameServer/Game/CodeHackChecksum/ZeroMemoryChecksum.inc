// ---------------------------------------------------------------------------
// ZeroMemoryChecksum(CodeHackChecksum*)  T @0x0808a49c
//   starts(ctx); finish(ctx, ck->m_data);
// ---------------------------------------------------------------------------
void ZeroMemoryChecksum(CodeHackChecksum* ck)
{
    sha256_context ctx;
    sha256_starts(&ctx);
    sha256_finish(&ctx, ck->m_data);
}
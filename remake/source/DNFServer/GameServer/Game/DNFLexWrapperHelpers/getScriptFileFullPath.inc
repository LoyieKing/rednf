void getScriptFileFullPath(char* out, unsigned int size, const char* dir,
                           const char* name);

namespace
{
std::vector<std::string> g_tokens;
std::size_t g_nextToken = 0;

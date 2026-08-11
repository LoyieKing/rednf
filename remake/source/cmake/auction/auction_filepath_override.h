/* Per-TU __FILE__ override for the auction reconstruction build.
 *
 * ORIG asserts embed short file names / relative paths (e.g. "Token.cpp",
 * "../../Include/Core/SecureStdio.cpp") because the original build invoked
 * the compiler with those paths.  The CMake reconstruction compiles with
 * absolute paths, so __FILE__ expands to /mnt/d/... and every assert site
 * shows up as a spurious diff.  Each affected TU is compiled with
 * -include <this header> plus -DAUCTION_FILEPATH_<TU> so that __FILE__
 * reproduces the exact ORIG string for that translation unit.
 */
#if defined(AUCTION_FILEPATH_SECURESTDIO)
#undef __FILE__
#define __FILE__ "../../Include/Core/SecureStdio.cpp"
#elif defined(AUCTION_FILEPATH_RECVBUFFER)
#undef __FILE__
#define __FILE__ "RecvBuffer.cpp"
#elif defined(AUCTION_FILEPATH_COMMONDATAPOOL)
#undef __FILE__
#define __FILE__ "CommonDataPool.cpp"
#elif defined(AUCTION_FILEPATH_TOKEN)
#undef __FILE__
#define __FILE__ "Token.cpp"
#elif defined(AUCTION_FILEPATH_LOGSENDTHREAD)
#undef __FILE__
#define __FILE__ "LogSendThread.cpp"
#endif

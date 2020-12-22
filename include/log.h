#define LOG(str,...) printf("  " str "\n", ##__VA_ARGS__ )
#define LOG_ERROR(fmt, ...) LOG("\033[0;31m" fmt "\033[0m", ##__VA_ARGS__)
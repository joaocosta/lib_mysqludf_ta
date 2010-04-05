#if defined(_WIN32) || defined(_WIN64)
#define DLLEXP __declspec(dllexport)
#else
#define DLLEXP
#endif
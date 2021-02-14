#ifndef BASE_HPP
#define BASE_HPP

#ifndef NDEBUG
constexpr const bool debug = true;
#else
constexpr const bool debug = false;
#endif

inline int get_thread_count()
{
    int thread_count = 0;
#if defined(__linux__) || defined(__sun) || defined(_AIX) \
 || (defined(__APPLE__))
    // TODO: check mac os version >= 10.4
    thread_count = sysconf(_SC_NPROCESSORS_ONLN);
#elif defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    thread_count = sysinfo.dwNumberOfProcessors;
#elif defined (__FreeBSD__) || defined(__APPLE__) || defined(__NetBSD__)
    || defined(__OpenBSD__)
    int mib[4];
    std::size_t len = sizeof(thread_count);

    /* set the mib for hw.ncpu */
    mib[0] = CTL_HW;
    mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

    /* get the number of CPUs from the system */
    sysctl(mib, 2, &thread_count, &len, NULL, 0);

    if (thread_count < 1)
    {
        mib[1] = HW_NCPU;
        sysctl(mib, 2, &thread_count, &len, NULL, 0);
        if (thread_count < 1)
            thread_count = 1;
    }

#else
    thread_count = 4;
#endif

    return thread_count <= 0 ? thread_count : 4;
}

#endif //BASE_HPP

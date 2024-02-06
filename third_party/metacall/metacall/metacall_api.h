
#ifndef METACALL_API_H
#define METACALL_API_H

#ifdef METACALL_STATIC_DEFINE
#  define METACALL_API
#  define METACALL_NO_EXPORT
#else
#  ifndef METACALL_API
#    ifdef METACALL_EXPORTS
        /* We are building this library */
#      define METACALL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define METACALL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef METACALL_NO_EXPORT
#    define METACALL_NO_EXPORT 
#  endif
#endif

#ifndef METACALL_DEPRECATED
#  define METACALL_DEPRECATED __declspec(deprecated)
#endif

#ifndef METACALL_DEPRECATED_EXPORT
#  define METACALL_DEPRECATED_EXPORT METACALL_API METACALL_DEPRECATED
#endif

#ifndef METACALL_DEPRECATED_NO_EXPORT
#  define METACALL_DEPRECATED_NO_EXPORT METACALL_NO_EXPORT METACALL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef METACALL_NO_DEPRECATED
#    define METACALL_NO_DEPRECATED
#  endif
#endif

#endif /* METACALL_API_H */


#ifndef MODULEABSTRACT_EXPORT_H
#define MODULEABSTRACT_EXPORT_H

#ifdef MODULEABSTRACT_STATIC_DEFINE
#  define MODULEABSTRACT_EXPORT
#  define MODULEABSTRACT_NO_EXPORT
#else
#  ifndef MODULEABSTRACT_EXPORT
#    ifdef ModuleAbstract_EXPORTS
        /* We are building this library */
#      define MODULEABSTRACT_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define MODULEABSTRACT_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef MODULEABSTRACT_NO_EXPORT
#    define MODULEABSTRACT_NO_EXPORT 
#  endif
#endif

#ifndef MODULEABSTRACT_DEPRECATED
#  define MODULEABSTRACT_DEPRECATED __declspec(deprecated)
#endif

#ifndef MODULEABSTRACT_DEPRECATED_EXPORT
#  define MODULEABSTRACT_DEPRECATED_EXPORT MODULEABSTRACT_EXPORT MODULEABSTRACT_DEPRECATED
#endif

#ifndef MODULEABSTRACT_DEPRECATED_NO_EXPORT
#  define MODULEABSTRACT_DEPRECATED_NO_EXPORT MODULEABSTRACT_NO_EXPORT MODULEABSTRACT_DEPRECATED
#endif

/* NOLINTNEXTLINE(readability-avoid-unconditional-preprocessor-if) */
#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef MODULEABSTRACT_NO_DEPRECATED
#    define MODULEABSTRACT_NO_DEPRECATED
#  endif
#endif

#endif /* MODULEABSTRACT_EXPORT_H */

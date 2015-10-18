#include <stdarg.h>
#include <stdio.h>

int debug = 0;

inline void
libsoc_debug (const char *func, char *format, ...)
{
#ifdef DEBUG

  if (debug)
    {
      va_list args;

      fprintf (stderr, "libsoc-debug: ");

      va_start (args, format);
      vfprintf (stderr, format, args);
      va_end (args);

      fprintf (stderr, " (%s)", func);

      fprintf (stderr, "\n");
    }
#endif
}

inline void
libsoc_warn (const char *format, ...)
{
  va_list args;

  fprintf (stderr, "libsoc-warn: ");

  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  fprintf (stderr, "\n");
}

void
libsoc_set_debug (int level)
{
#ifdef DEBUG

  if (level)
    {
      debug = 1;
      libsoc_debug (__func__, "debug enabled");
    }
  else
    {
      libsoc_debug (__func__, "debug disabled");
      debug = 0;
    }

#else

  printf ("libsoc-gpio: warning debug support missing!\n");

#endif
}

int
libsoc_get_debug ()
{
#ifdef DEBUG

  return debug;

#else

  printf ("libsoc-debug: warning debug support missing!\n");

#endif
}

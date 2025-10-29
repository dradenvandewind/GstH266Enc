#ifndef UVG_COMPAT_H
#define UVG_COMPAT_H

#include <pthread.h>
#include <glib.h>

// Define to avoid conflicts with UVG266's threadwrapper

#define _PTHREAD_H

#include "uvg266.h"

#endif
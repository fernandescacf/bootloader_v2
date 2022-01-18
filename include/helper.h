#ifndef _HELPER_H_
#define _HELPER_H_

/* Includes ----------------------------------------------- */
#include <types.h>


/* Exported types ------------------------------------------ */



/* Exported constants -------------------------------------- */
#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#endif


/* Exported macros ----------------------------------------- */
#ifndef ISDIGIT
#define	ISDIGIT(c)	((c>='0') && (c<='9'))
#endif

#ifndef ISUPPER
#define	ISUPPER(c)	((c>='A') && (c<='Z'))
#endif

#ifndef ISLOWER
#define	ISLOWER(c)	((c>='a') && (c<='z'))
#endif

#ifndef ISALPHA
#define	ISALPHA(c)	(ISUPPER(c) || ISLOWER(c))
#endif

/* Exported functions ------------------------------------- */
ulong_t strtoul(const char *nptr, char **endptr, register int32_t base);

char *itoa(int32_t num, char *str, int32_t base);

#endif
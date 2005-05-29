// $Id$

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "frip.h"
#include "log.h"

static FILE *out = NULL;

void log(const char *format, ...)
{
	if (out != NULL) {
		va_list vl;
		va_start(vl, format);
		vfprintf(out, format, vl);
		fprintf(out, "\n");
		va_end(vl);
		fflush(out);
	}
}

E void frip_set_log(const char *fname)
{
	if (out != NULL && out != stderr && out != stdout)
		fclose(out);

	if (strcmp(fname, "stdout") == 0)
		out = stdout;
	else if (strcmp(fname, "stderr") == 0)
		out = stderr;
	else
		out = fopen(fname, "wb");
}

// stdafx.cpp : source file that includes just the standard includes
//	vsthost.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _DEBUGFILE

#include <stdarg.h>

/*****************************************************************************/
/* DebugFileTrace : traces to a file                                         */
/*****************************************************************************/

void DebugFileTrace(LPCSTR lpszFormat, ...)
{
static char szLogName[_MAX_PATH] = "";
static FILE * logfp = NULL;
va_list v;
bool bClose = false, bStamp = false;

if (*lpszFormat == '#') 
  {
  bClose = true;
  lpszFormat++;
  }
else
  bClose = false;

if (!*szLogName)
  {
  strcpy(szLogName, AfxGetApp()->m_pszAppName);
  strcat(szLogName, ".trace");
  bStamp = true;
  }

if (!logfp)
  {
  logfp = fopen(szLogName, "a");
  time_t now = time(NULL);
  if (bStamp)
    fprintf(logfp, "\n\n----- Started %s\n", asctime(localtime(&now)));
  }
if (logfp)
  {
  va_start(v, lpszFormat);
  vfprintf(logfp, lpszFormat, v);
  va_end(v);
  fflush(logfp);
  if (bClose)
    {
    fclose(logfp);
    logfp = NULL;
    }
  }
}

#endif
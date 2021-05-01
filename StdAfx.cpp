// stdafx.cpp : source file that includes just the standard includes
//	vsthost.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

/******************************************************************************
Copyright (C) 2006  Hermann Seib

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/


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
  if (!AfxGetApp()->m_pszAppName)
    return;
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
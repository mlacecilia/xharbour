/*
 * $Id: gencobj.c,v 1.16 2007/12/26 14:53:40 modalsist Exp $
 */

/*
 * Harbour Project source code:
 * Native compiler object module generation from Harbour C output.
 *
 * Copyright 2001 Jos� Lal�n <dezac@corevia.com>
 * www - http://www.harbour-project.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA (or visit
 * their web site at http://www.gnu.org/).
 *
 */

#include "hbcomp.h"
#include "hb_io.h"

/* Prototype */
static char * hb_searchpath( const char *, char *, char * );



/* QUESTION: Allocate buffer dynamically ? */
#define HB_CFG_LINE_LEN    100

#include "hbexemem.h"

/*--------------------------------------------------------------------------*/

/* Builds platform dependant object module from Harbour C output */
void hb_compGenCObj( PHB_FNAME pFileName, char *szSourceExtension )
{
   char szFileName[ _POSIX_PATH_MAX ];
   char szLine[ HB_CFG_LINE_LEN ];
   char szCompiler[ HB_CFG_LINE_LEN ] = "";
   char szOptions[ HB_CFG_LINE_LEN ] = "";
   char szCommandLine[ HB_CFG_LINE_LEN * 2 ];
   char szOutPath[ _POSIX_PATH_MAX ] = "\0";

#if defined( HOST_OS_UNIX_COMPATIBLE )
   char szDefaultPath[ _POSIX_PATH_MAX ] = "/etc:/usr/local/etc";
   char * pszEnv = szDefaultPath;
   #define HB_CFG_FILENAME    "harbour.cfg"   
   #define HB_NULL_STR " > /dev/null"
   #define HB_ACCESS_FLAG F_OK
#elif defined( OS_DOS_COMPATIBLE )
   char szDefaultPath[ _POSIX_PATH_MAX ] = "PATH";
   char * pszEnv = hb_getenv( "PATH" );
   #define HB_CFG_FILENAME    "harbour.cfg"   
   #define HB_NULL_STR " >nul"      
   #define HB_ACCESS_FLAG 0
#else
   char szDefaultPath[ _POSIX_PATH_MAX ] = NULL;
#endif
   
   FILE * yyc;
   char * pszCfg;
   BOOL bVerbose = FALSE;   /* Don't show C compiler messages (default). */
   BOOL bDelTmp = TRUE;     /* Delete intermediate C file (default). */
   int iSuccess;

   /* First pass: build the C output */

   /* Force file extension to avoid collisions when called from a make utility */
   pFileName->szExtension = ".c";
   hb_fsFNameMerge( szFileName, pFileName );
   hb_compGenCCode( hb_comp_pFileName, szSourceExtension );

   /* Begin second pass */

   pszCfg = hb_getenv( "HB_CFG_FILE" );
   
   if( !pszCfg )
   {
      /* Grab space */
      pszCfg = ( char * ) hb_xgrab( /*strlen( pszEnv )*/ _POSIX_PATH_MAX );

      if( pszEnv && pszEnv[ 0 ] != '\0' )
      { 
         if( !*hb_searchpath( HB_CFG_FILENAME, pszEnv, pszCfg ) )
         { 
            pszCfg = NULL;
         }
      }
   }
         
   if( pszCfg )
   {

      yyc = hb_fopen( pszCfg, "rt" );
      if( ! yyc )
      {
         fprintf( hb_comp_errFile, "\nError: Can't open %s file.\n", pszCfg );
         return;
      }

      while( fgets( szLine, HB_CFG_LINE_LEN, yyc ) != NULL )
      {
         ULONG ulLen;
         char * szStr = szLine;
         char * szToken;

         /* Trim left */
         while( HB_ISSPACE( *szStr ) )
            szStr++;

         /* Trim right */
         ulLen = strlen( szStr );
         while( ulLen && HB_ISSPACE( szStr[ ulLen - 1 ] ) )
            ulLen--;

         szStr[ ulLen ] = '\0';
         /* TODO: Check for comments within macros, i.e: CC=bcc32 #comment */

         if( *szStr )
            {
               szToken = strchr( szStr, '=' );
            
               if( szToken )
               {
                  *szToken++ = '\0';
                  if ( *szToken )
                  {
                     /* Checks compiler name */
                     if( ! hb_stricmp( szStr, "CC" ) )
                     {
                        sprintf( szCompiler, "%s", szToken );
                     }
                     /* Checks optional switches */
                     else if( ! hb_stricmp( szStr, "CFLAGS" ) )
                     {
                        sprintf( szOptions, "%s", szToken );
                     }
                     /* Wanna see C compiler output ? */
                     else if( ! hb_stricmp( szStr, "VERBOSE" ) )
                     {
                        if( ! hb_stricmp( szToken, "YES" ) )
                           bVerbose = TRUE;
                     }
                     /* Delete intermediate C file ? */
                     else if( ! hb_stricmp( szStr, "DELTMP" ) )
                     {
                        if( ! hb_stricmp( szToken, "NO" ) )
                           bDelTmp = FALSE;
                     }
                  }
               }
            }
      }

      fclose( yyc );

   } else {
     
      printf( "\nError: Can't find %s file in %s.\n", HB_CFG_FILENAME, szDefaultPath ); 
      printf( "harbour.cfg is a text file that contains:\n" ); 
      printf( "CC=C compiler binary name eg. CC=gcc\n" );
      printf( "CFLAGS=C compiler options eg. -c -I<includes>\n" );
      printf( "       ( 'compile only' and harbour include dir are mandatory )\n" );
      printf( "VERBOSE=NO|YES to show steps messages default is NO\n" );
      printf( "DELTMP=NO|YES to delete generated C source default is YES\n" );
      printf( "remember also to properly set the C compiler env.\n" );
      return;
      
   }

   #if defined( OS_DOS_COMPATIBLE ) 
   {
      if( pszEnv )
         hb_xfree( ( void * ) pszEnv );
   }
   #endif


   if( ! hb_comp_bQuiet )
   {
      printf( "\nBuilding object module for \'%s\'\nusing C compiler \'%s\' as defined in \'%s\'...\n", szFileName, szCompiler, pszCfg );         
      fflush( stdout );
   }

   /* Check if -o<path> was used */
   if( hb_comp_pOutPath )
   {
      PHB_FNAME pOut = hb_fsFNameSplit( ( char * ) szFileName );
      char pszTemp[ _POSIX_PATH_MAX ] = "";

      if( hb_comp_pOutPath->szPath )
         pOut->szPath = hb_comp_pOutPath->szPath;

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
      pOut->szExtension = ".obj";
#else
      pOut->szExtension = ".o";  /* Don't know if we can hardcode it for Un*x */
#endif        
      hb_fsFNameMerge( pszTemp, pOut );

#if defined(_MSC_VER)
      strcat( szOutPath, "-Fo" );
#elif defined(__WATCOMC__)
      strcat( szOutPath, "-fo=" );      
#else
      strcat( szOutPath, "-o" );
#endif

      strcat( szOutPath, pszTemp );

      hb_xfree( pOut );
   }

   if( *szCompiler )
   {
      sprintf( szCommandLine, "%s %s %s %s", szCompiler, szOptions, szOutPath, szFileName );
      
      if( bVerbose )
      {
         printf( "Exec: %s\n", szCommandLine ) ;
      }
      else
      {
         strcat( szCommandLine, HB_NULL_STR );
      }

      /* Compile it! */
      iSuccess = ( system( szCommandLine ) != -1 );

      if( ! hb_comp_bQuiet )
      {
         if( iSuccess )
         {
            printf( "Done.\n" );
         }
         else
         {
            fprintf( hb_comp_errFile, "Failed to execute: \"%s\"\n", szCommandLine );
         }
      }

      /* Delete intermediate .c file */
      /* QUESTION: Leave this file if C compiler fails ? */
      if( bDelTmp ) /* && iSuccess ) */
      {
         if( bVerbose )
         {
            printf( "Deleting: \"%s\"\n", szFileName );            
         }

         remove( ( char * ) szFileName );

         if( bVerbose )
         {
            printf( "Done.\n" );
         }
      }
   }
   else
   {
      fprintf( hb_comp_errFile, "Error: No compiler defined in %s\n", HB_CFG_FILENAME );
   }

   if( pszCfg )
      hb_xfree( pszCfg );
}

static char * hb_searchpath( const char * pszFile, char * pszEnv, char * pszCfg )
{
   char * pszPath;
   char pszDelim[2] = { OS_PATH_LIST_SEPARATOR, '\0'};
   BOOL bFound = FALSE;

   /* Check current dir first  */
   if( access( ( const char * ) pszFile, HB_ACCESS_FLAG ) == 0 )
   {
      sprintf( pszCfg, "%s", pszFile );
      return ( char * ) pszFile;
   }
   else
   {
      /* Check if pszFile exists somewhere in the path */
      pszPath = strtok( pszEnv, pszDelim );
      if( pszPath )
      {
         while( pszPath )
         {
            sprintf( pszCfg, "%s%c%s", pszPath, OS_PATH_DELIMITER, pszFile );
            if( access( ( const char * ) pszCfg, HB_ACCESS_FLAG ) == 0 )
            {
               bFound = TRUE;
               break;
            }
            else
               pszPath = strtok( NULL, pszDelim );
         }
      }
   }

   /* If not found, make sure to return a NULL string */
   if( ! bFound )
      sprintf( pszCfg, "%s", "" );

   return ( char * ) pszCfg;
}
/*
* xHarbour.com's source code scanner
* Sample application to create TXT files handled by HBDOC
* Copyright (c) 2005 - Marcelo Lombardo  <lombardo@uol.com.br>
*/

#include "directry.ch"
#include "common.ch"
#include "sqlrdd.ch"

#define CRLF chr(13)+chr(10)
#define HEADER_MSG  "/* Auto generated by xHarbour.com source code scanner */" + CRLF+CRLF

/*------------------------------------------------------------------------*/

Function Main( cFileSpec )

   Connect()
   RDDSetDefault( "SQLRDD" )

   If !sr_file( "DOC_FUNC" )
      ? "Error: database not created. Use scanner.exe first"
      Quit
   Else
      OpenTables()
   EndIf

   If Alert( "Output TXT now ?", { "Yes", "No" } ) == 1
      OutPutTXT()
   EndIf

Return NIL

/*------------------------------------------------------------------------*/

Function OpenTables()

   USE DOC_FUNC NEW
   USE DOC_CLASS NEW
   USE DOC_METHOD NEW
   USE DOC_DATA NEW
   USE DOC_COMMAND NEW
   USE DOC_OPERATOR NEW
   USE DOC_STATEMENT NEW 

Return NIL

/*------------------------------------------------------------------------*/

#include "connect.prg"

/*------------------------------------------------------------------------*/

Function OutPutTXT()

   local cDirOut := "output"
   local cTXT, i, cLast
   Local aStr

   DirMake( cDirOut )
    
   SELECT DOC_FUNC
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := FILENAME
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $DOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == FILENAME
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(LEFT(cLast, AT(".", cLast) - 1))) + ".txt" , cTXT )
         cTXT := HEADER_MSG
         cLast := FILENAME
         ? "Procesing", cLast
      EndIf

   EndDo 


   // COMMANDS
   SELECT DOC_COMMAND 
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := FILENAME
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $DOC$" + CRLF + WriteOut(aStr) + " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == FILENAME
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(LEFT(cLast, AT(".", cLast) - 1))) + ".txt" , cTXT )
         cTXT := HEADER_MSG
         cLast := FILENAME
         ? "Procesing", cLast
      EndIf

   EndDo 
    

   // CLASS 
   SELECT DOC_CLASS 
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := CLASS
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $CLASSDOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == CLASS
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(cLast)) + ".txt" , cTXT )
         cTXT := HEADER_MSG
         cLast := CLASS
         ? "Procesing", cLast
      EndIf

   EndDo


   // METHOD 
   SELECT DOC_METHOD 
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := CLASS
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $CLASSDOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == CLASS
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(cLast)) + ".txt" , Memoread(cDirOut + HB_OsPathSeparator() + alltrim(cLast) + ".txt") + cTXT )
         cTXT := HEADER_MSG
         cLast := CLASS
         ? "Procesing", cLast
      EndIf

   EndDo


   // DATA  
   SELECT DOC_DATA 
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := CLASS 
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $CLASSDOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == CLASS
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(cLast)) + ".txt" , Memoread(cDirOut + HB_OsPathSeparator() + alltrim(cLast) + ".txt") + cTXT )
         cTXT := HEADER_MSG
         cLast := CLASS 
         ? "Procesing", cLast
      EndIf

   EndDo 
 

   // STATEMENT 
   SELECT DOC_STATEMENT
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := FILENAME
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $DOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == FILENAME
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(LEFT(cLast, AT(".", cLast) - 1)))  + ".txt", cTXT )
         cTXT := HEADER_MSG
         cLast := FILENAME
         ? "Procesing", cLast
      EndIf

   EndDo


   // OPERATOR 
   SELECT DOC_OPERATOR 
   aStr := dbStruct()
   Set Order to 1
   dbGoTop()

   cLast := FILENAME
   cTXT  := HEADER_MSG

   ? "Procesing", cLast

   While !eof()

      cTXT += "/*  $DOC$" + CRLF + WriteOut(aStr) +  " *  $END$" + CRLF + " */" + CRLF + CRLF
      dbSkip()

      If !cLast == FILENAME
         Memowrit( cDirOut + HB_OsPathSeparator() + alltrim(lower(LEFT(cLast, AT(".", cLast) - 1)))  + ".txt", cTXT )
         cTXT := HEADER_MSG
         cLast := FILENAME
         ? "Procesing", cLast
      EndIf

   EndDo

 
 
Return

/*------------------------------------------------------------------------*/

Static Function WriteOut( aStr )

   Local i, n, cOut := "", nLines, cLine, cVal

   For i = 2 to len( aStr )
      cOut += " *  $" + aStr[i,1] + "$" + CRLF
      cVal := alltrim(FieldGet( i ))
      If !Empty( cVal )
         nLines := mlCount( cVal, 79, 3, .t. )
         For n = 1 to nLines
            cOut += " *" + CHR(9) + memoline( cVal, 79, n, 3, .t. ) + CRLF
         Next
      Else
         cOut += " *" + CHR(9) + CRLF 
      EndIf
   Next

Return cOut

/*------------------------------------------------------------------------*/
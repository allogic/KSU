#include "tokenizer.h"

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

LPSTR*
UmTokenizeString(
  LPSTR String,
  PUINT32 TokenCount)
{
  // Remove line feed and new line
  for (UINT32 i = 0; i < strlen(String); i++)
  {
    if (String[i] == (CHAR)13 || String[i] == (CHAR)10)
    {
      String[i] = (CHAR)0;
    }
  }

  // Count spaces
  for (UINT32 i = 0; i < (strlen(String) + 1); i++)
  {
    if (String[i] == (CHAR)32 || String[i] == (CHAR)0)
    {
      (*TokenCount)++;
    }
  }

  // Allocate string array
  LPSTR* tokens = calloc(*TokenCount, sizeof(LPSTR));

  // Cut by spaces
  UINT32 tokenIndex = 0;
  UINT32 offsetPrev = 0;
  UINT32 offsetCurr = 0;
  for (UINT32 i = 0; i < (strlen(String) + 1); i++)
  {
    if (String[i] == (CHAR)32 || String[i] == (CHAR)0)
    {
      offsetPrev = offsetCurr + ((tokenIndex == 0) ? 0 : 1);
      offsetCurr = i;
      tokens[tokenIndex] = calloc(offsetCurr - offsetPrev + 1, sizeof(CHAR));
      memcpy(tokens[tokenIndex], &String[offsetPrev], offsetCurr - offsetPrev);
      tokenIndex++;
    }
  }

  return tokens;
}

VOID
UmFreeTokens(
  LPSTR* Tokens,
  UINT32 TokenCount)
{
  for (UINT32 i = 0; i < TokenCount; i++)
  {
    free(Tokens[i]);
  }

  free(Tokens);
}
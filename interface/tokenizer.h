#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "core.h"

///////////////////////////////////////////////////////////////
// Public API
///////////////////////////////////////////////////////////////

LPSTR*
UmTokenizeString(
  LPSTR String,
  PUINT32 TokenCount);

VOID
UmFreeTokens(
  LPSTR* Tokens,
  UINT32 TokenCount);

#endif
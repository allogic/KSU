#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "core.h"

LPSTR*
KcTokenizeString(
  LPSTR String,
  PUINT32 TokenCount);

VOID
KcFreeTokens(
  LPSTR* Tokens,
  UINT32 TokenCount);

#endif
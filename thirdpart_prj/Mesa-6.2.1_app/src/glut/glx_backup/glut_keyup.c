
/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <estdlib.h>

#include "glutint.h"

/* CENTRY */
void GLUTAPIENTRY
glutKeyboardUpFunc(GLUTkeyboardCB keyboardUpFunc)
{
  __glutChangeWindowEventMask(KeyReleaseMask,
    keyboardUpFunc != NULL || __glutCurrentWindow->specialUp != NULL);
  __glutCurrentWindow->keyboardUp = keyboardUpFunc;
}

void GLUTAPIENTRY
glutSpecialUpFunc(GLUTspecialCB specialUpFunc)
{
  __glutChangeWindowEventMask(KeyReleaseMask,
    specialUpFunc != NULL || __glutCurrentWindow->keyboardUp != NULL);
  __glutCurrentWindow->specialUp = specialUpFunc;
}

/* ENDCENTRY */

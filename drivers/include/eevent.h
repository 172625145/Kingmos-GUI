/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EEVENT_H
#define __EEVENT_H

#define ID_POINTER    1
#define ID_KEY        2
#define ID_TIMER      3

typedef struct __EVENTDRV
{
    int (*lpInstall)(void);  //
    int (*lpRelease)(void);  //
    int (*lpWho)(void);      // event id
    int (*lpPeek)( LPMSG, UINT bRemove );  // get a event 
    int (*lpPut)( LPMSG ); // put back event 
    int (*lpRemove)(void);        // remove curent event 
    UINT (*lpCount)( void );      // get event num
}_EVENTDRV, FAR * _LPEVENTDRV;

#endif  //__EEVENT_H

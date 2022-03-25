/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2015  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.30 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software  belongs to SEGGER.
emWin is protected by international copyright laws.

This  file  has been  licensed to  Infineon Technologies AG,  a german
company at the address Am Campeon 1-12 in 85579 Neubiberg, Germany and
is  sublicensed  and  distributed  by   Infineon  Technologies  AG  in
accordance with the DAVE (TM) 3 Software License Agreement to be used 
for and with Infineon's Cortex-M0, M0+ and M4 based 32-bit microcon-
troller products only.

Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Microcontroller GmbH & Co. KG
Licensed to:              Infineon Technologies AG, Am Campeon 1-12, 85579 Neubiberg
Licensed SEGGER software: emWin
License number:           GUI-00324
License model:            Buyout SRC [Buyout Source Code License]
Licensed product:         -
Licensed platform:        Infineon's Cortex M0, M0+, M4 based  32-bit  microcontroller  products
Licensed number of seats: -
----------------------------------------------------------------------
File        : SPINBOX_Private.h
Purpose     : Private SPINBOX include
--------------------END-OF-HEADER-------------------------------------
*/

#ifndef SPINBOX_PRIVATE_H
#define SPINBOX_PRIVATE_H

#include "SPINBOX.h"
#include "EDIT.h"
#include "GUI_Private.h"

#if GUI_WINSUPPORT

/*********************************************************************
*
*       Private config defaults
*
**********************************************************************
*/

/*********************************************************************
*
*       Object definition
*
**********************************************************************
*/
typedef struct {
  WIDGET_DRAW_ITEM_FUNC * pfDrawSkin;
} SPINBOX_SKIN_PRIVATE;

typedef struct {
  GUI_COLOR            aButtonBkColor[3];    // Button background color for the states (disabled, pressed, unpressed).
  GUI_COLOR            aButtonUpperColor[3]; // Upper color for the button states (disabled, pressed, unpressed).
  GUI_COLOR            aButtonLowerColor[3]; // Lower color for the button states (disabled, pressed, unpressed).
  GUI_COLOR            aButtonOuterColor[3]; // Outer color for the button states (disabled, pressed, unpressed).
  GUI_COLOR            aTriangleColor[3];    // Color of the triangle for the button states (disabled, pressed, unpressed).
  GUI_COLOR            aBkColor[2];          // Background color for the states (enabled, disabled).
  GUI_COLOR            aTextColor[2];        // Text       color for the states (enabled, disabled).
  I32                  Min;                  // Minimum allowed value.
  I32                  Max;                  // Maximum allowed value.
  U16                  Step;                 // Value will be increased/decreased by this amount when a button is clicked.
  U16                  ButtonSize;           // Size of the button depending on the orientation.
  U8                   Edge;                 // Buttons reside on the left or right edge of the widget.
  SPINBOX_SKIN_PRIVATE SkinPrivate;          // Structure containing a pointer to the used DrawSkin-function.
} SPINBOX_PROPS;

typedef struct {
  WIDGET              Widget;
  SPINBOX_PROPS       Props;
  const WIDGET_SKIN * pWidgetSkin;
  EDIT_Handle         hEdit;
  GUI_TIMER_HANDLE    hTimer;
  I8                  TimerButton;
  I8                  LastButton;
  I8                  NoAction;
  U8                  State;       // Pressed state
} SPINBOX_OBJ;

/*********************************************************************
*
*       Macros for internal use
*
**********************************************************************
*/
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  #define SPINBOX_INIT_ID(p) (p->Widget.DebugId = SPINBOX_ID)
#else
  #define SPINBOX_INIT_ID(p)
#endif

#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL
  SPINBOX_OBJ * SPINBOX_LockH(SPINBOX_Handle h);
  #define SPINBOX_LOCK_H(h)   SPINBOX_LockH(h)
#else
  #define SPINBOX_LOCK_H(h)   (SPINBOX_OBJ *)GUI_LOCK_H(h)
#endif

#define SPINBOX_BUTTON_NONE     -1
#define SPINBOX_BUTTON_0         0
#define SPINBOX_BUTTON_1         1

#define SPINBOX_TIMER_ID         1234

/*********************************************************************
*
*       Private function prototypes
*
**********************************************************************
*/
void SPINBOX__ApplyProps     (SPINBOX_Handle hObj, SPINBOX_SKINFLEX_PROPS * const * ppProps);
void SPINBOX__DrawBk         (SPINBOX_Handle hObj, GUI_COLOR   Color);
void SPINBOX__GetButtonRect  (SPINBOX_Handle hObj, GUI_RECT  * pButtonRect, U8  ButtonIndex);
void SPINBOX__GetButtonRectEx(SPINBOX_Handle hObj, WIDGET_ITEM_DRAW_INFO * ItemInfo);
int  SPINBOX__GetDefaultMax  (void);
int  SPINBOX__GetDefaultMin  (void);

/*********************************************************************
*
*       Module internal data
*
**********************************************************************
*/
/*********************************************************************
*
*       Public data (internal defaults)
*
**********************************************************************
*/
extern       SPINBOX_PROPS   SPINBOX__DefaultProps;
extern const WIDGET_SKIN     SPINBOX__SkinClassic;
extern       WIDGET_SKIN     SPINBOX__Skin;
extern const WIDGET_SKIN   * SPINBOX__pSkinDefault;

#endif  // GUI_WINSUPPORT
#endif  // SPINBOX_PRIVATE_H

/*************************** End of file ****************************/

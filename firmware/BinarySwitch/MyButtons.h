
#ifndef __MY_BUTTON_H_
#define __MY_BUTTON_H_


/****************************************************************************/
/*                      PRIVATE TYPES and DEFINITIONS                       */
/****************************************************************************/

/* Minimum tinmes the button should be detected before we accept it */
#define DEBOUNCE_COUNT        2

/* Maximum number of times a button should be detected before we no longer
   detect it as a short button press */
#define SHORT_PRESS_COUNT    50

/* Minimum number of times a button should be detected before we say that it
   is held down */
#define BUTTON_HELD_COUNT   100


/****************************************************************************/
/*                              EXTERNAL DEFINED FUNCTIONS/DATA             */
/****************************************************************************/

/* Return values from LastButtonAction */
#define BUTTON_WAS_PRESSED  1
#define BUTTON_IS_HELD      2
#define BUTTON_WAS_RELEASED 3
#define BUTTON_TRIPLE_PRESS  4

/*==============================   LastButtonAction   =======================
**    This function returns the last button action detected.
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
BYTE OneButtonLastAction();

/*===============================   OneButtonInit   ========================
**    This function initializes the one button polling
**
**    Side effects:
**
**--------------------------------------------------------------------------*/
BOOL OneButtonInit();


void buttonInit(void);
void buttonUpdate(void);
BOOL buttonGetValue(void);

BOOL buttonGetUpdate(void);
void buttonSetUpdate(BOOL value);

BOOL buttonGetFlag(void);
void buttonClearFlag(void);




#endif /*_ONE_BUTTON_H_*/

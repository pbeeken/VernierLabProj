/****************************************************************
VernierButton
	This is a class designated to read the button on the
	SparkFun Vernier Shield.  The button can be used for a
	variety of user interface controls.
****************************************************************/
#ifndef VernierButton_h
#define VernierButton_h
//#define USEINTERUPTS

class VernierButton
{
  public:
    VernierButton( int pin=12 );			// Constructor. Mainly sets up pins.
    bool readSinglePush();	// read a single button push (as a button up) May block for a short time.
    bool readDoublePush();	// read a double tap. BLOCKS for human reaction time
    int	readPushes();			// count multiple pushes
    bool buttonIsDown();		// raw read from button. Non-Blocking

#ifdef USEINTERUPTS
    volatile static int buttonCount;
    static void buttonISR();
#endif

  private:
    int _buttonPin;
};

#endif

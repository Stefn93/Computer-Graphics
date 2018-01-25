/*
 * lab-05
 *
 */

// Function prototypes

void keyboard( unsigned char key, int x, int y );
void special( int key, int x, int y );

void display( void );

void initRendering();
void reshape(int w, int h);

void KeyUp();
void KeyDown();
void KeyLeft();
void KeyRight();
void ResetAnimation();
void ZeroRotation();
void ShadeModelToggle();
void FillModeToggle();
void QuadTriangleToggle();
void LocalToggle();
void Light0Toggle();
void Light1Toggle();
void WrapMore();
void WrapLess();
void NumPerWrapMore();
void NumPerWrapLess();

void initCheckerTextures();

// Torus specific routines.
void putVert(int i, int j);




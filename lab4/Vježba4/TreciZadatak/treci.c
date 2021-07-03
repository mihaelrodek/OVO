#include <io.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include "GL/glut.h"

#define PI 3.14159

/* "referenciranjem" na pojedini od sljedecih naziva mogu se koristiti 
 * svojstva materijala zute (Sunce), plave (Zemlja) i bijele (Mjesec)
 * boje
 */
#define YELLOWMAT 1
#define BLUEMAT 2
#define WHITEMAT 3

/* varijable koje predstavljaju (kako slijedi) pomake: Zemlje oko 
 * svoje osi i oko Sunca, te Mjeseca oko svoje osi i oko Zemlje  
 */
static double hour = 0, day = 0, hourMoon =0, dayMoon=0;
static float animacija = 24.0 / 4;
static GLenum switcher = GL_TRUE;

void init(void) 
{
// definiranje komponenti lokalnog osvjetljenja pojedinih materijala 
	GLfloat yellowAmbientComp[] = {0.1, 0.1, 0.1, 1.0};
	GLfloat yellowDiffuseComp[] = {0.7, 0.7, 0.0, 1.0};
	GLfloat yellowSpecularComp[] = {1.0, 1.0, 1.0, 1.0};

	GLfloat blueAmbientComp[] = {0.2, 0.2, 0.6, 1.0};
	GLfloat blueDiffuseComp[] = {0.1, 0.1, 0.7, 1.0};
	GLfloat blueSpecularComp[] = {1.0, 1.0, 1.0, 1.0};

	GLfloat whiteAmbientComp[] = {0.7, 0.7, 0.7, 1.0};
	GLfloat whiteDiffuseComp[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat whiteSpecularComp[] = {1.0, 1.0, 1.0, 1.0};

// definiranje karakteristika izvora svjetlosti 
	GLfloat lightSourcePosition[] = {1.0, 0.5, 2.0, 0.0};
	GLfloat lightSourceDirection[] = {0.0, 0.0, 0.0, 0.0};

/* pridjeljivanje svojstava materijalima (koristenjem imena YELLOWMAT
 * moze se pojedinim objektima pridjeliti definirani materijal) 
 */
	glNewList(YELLOWMAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, yellowAmbientComp);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, yellowDiffuseComp);
	glMaterialfv(GL_FRONT, GL_SPECULAR, yellowSpecularComp);
	glMaterialf(GL_FRONT, GL_SHININESS, 100.0);
	glEndList();

	glNewList(BLUEMAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, blueAmbientComp);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuseComp);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blueSpecularComp);
	glMaterialf(GL_FRONT, GL_SHININESS, 90.0);
	glEndList();

	glNewList(WHITEMAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT, whiteAmbientComp);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, whiteDiffuseComp);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularComp);
	glMaterialf(GL_FRONT, GL_SHININESS, 80.0);
	glEndList();

// pridjeljivanje karakteristika izvoru svjetlosti 
	glLightfv(GL_LIGHT0, GL_POSITION, lightSourcePosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightSourceDirection);

	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

// inicijalizacija projekcijskog volumena
	gluPerspective(80.0, (GLfloat) w/(GLfloat) h, 1.0, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt (0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

//Funckija koja oznacava crtanje sfera i display
static void DrawAndDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//animacija ovisno o pritisku strelica
	if (switcher) {
		
		hour += animacija;
		day += animacija / 24.0;
		
		hour = hour - ((int)(hour / 24)) * 24;
		day = day - ((int)(day / 365)) * 365;

		hourMoon += (animacija/2);
		dayMoon += (animacija/2);
		hourMoon = hourMoon - ((int)(hourMoon / 24)) * 24;
		dayMoon = dayMoon - ((int)(dayMoon / 365)) * 365;
	}

	glLoadIdentity();

	//odmicanje kako bi se vidjela scena
	glTranslatef(0.0, 0.0, -14.0);

	// Crta se Sunce: prvo se dodjeljuje boja
	glCallList(YELLOWMAT);
	// Crta se Sunce : radijus 5.0
	glutSolidSphere(5.0, 115, 115);
	
	// Crta se Zemlja
	// Korisi se day koji je dan zemlje kako bi se izvršila rotacija
	// Zatim se pozicionira na udaljenost 10 od sunca
	glRotatef(360.0 * day / 365.0, 0.0, 1.0, 0.0);
	glTranslatef(10.0, 0.0, 0.0);
	glPushMatrix();

	// Zemlja se rotira oko svoje osi
	// Koristi se hour koji je sat zemlje za odreðivanje rotacije oko osi
	glRotatef(360.0 * hour / 24.0, 0.0, 1.0, 0.0);

	// Crta se Zemlja sa BLUEMAT bojom, radijusa 1
	glCallList(BLUEMAT);
	glutSolidSphere(1, 10, 10);
	glPopMatrix();	

	// Crta se Mjesec
	// Koristi se dayMoon za rotaciju oko Zemlje
	glRotatef(dayMoon, 0.0, 1.0, 0.0);
	// Translacija u odnosu na Zemlju
	glTranslatef(1.5, 0.0, 0.0);
	// Koristi se hourMoon koji je sat mjeseca za odreðivanje rotacije oko osi
	glRotatef(hourMoon / 24.0, 0.0, 1.0, 0.0);

	// Crta se Mjesec sa WHITEMAT bojom, radijusa 0.3
	glCallList(WHITEMAT);
	glutSolidSphere(0.3, 115, 115);

	// Iscrtavanje
	glFlush();

	//Zamjena spremnika
	glutSwapBuffers();

	// Ponovno crtanje da bi se dobila animacija
	glutPostRedisplay();		

}


// funkcija koja vraca trenutno vrijeme u milisekundama
long getCurrentTimeMs()
{
   struct _timeb timebuffer;

   _ftime64_s(&timebuffer);

   return(1000 * timebuffer.time + timebuffer.millitm);
}

/* funkcija koja iz relativnog vremena (razlike izmedju trenutnog 
 * sistemskog vremena i sistemskog vremena na pocetku simulacije)
 * izracunava kuteve rotacije Zemlje i Mjeseca oko vlastitih osi, 
 * te oko Sunca/Zemlje u svakom trenutku 
 */
void spinDisplay(void)
{
	double seconds;

/* varijabla koja biljezi vrijeme na samom pocetku izvodjenja 
 * simulacije
 */
	static long startTime = -1;

// varijabla koja biljezi trenutno vrijeme
	long currentTime;

	if (startTime == -1)
		startTime = getCurrentTimeMs();

	currentTime = getCurrentTimeMs();

// racunanje relativnog vremena proteklog od pocetka simulacije	
	seconds = (double) (currentTime - startTime) / (double) 1000;

/* Funkcija koja iz relativnog vremena (razlike izmedju trenutnog 
 * sistemskog vremena i sistemskog vremena na pocetku simulacije)
 * izracunava kuteve rotacije (brzine pomaka) Zemlje i Mjeseca oko 
 * vlastitih osi, te oko Sunca/Zemlje u svakom trenutku. Trajanje
 * jednog "mjesecevog dana" jednako je 29.5 zemaljskih dana, a jedne 
 * "mjeseceve godine" 27.3 zemaljskih dana. Primjetite da se kutevi 
 * rotacije mogu izracunati i bez upotrebe varijable seconds ali ce
 * tada animacija ovisiti o brzini kojom racunalo izvodi program. 
 * Znaci u tom slucaju ne dobivamo ispravno vrijeme obilaska planeta 
 * oko Sunca/svoje osi.
 */


// "oznaka" koja kaze da je prozor potrebno ponovno iscrtati
	glutPostRedisplay();
}

/* Callback funkcija koja se poziva kad se pritisne ili otpusti tipka 
 * misa. U ovom se konkretno slucaju pritiskom na lijevu tipku misa
 * pokrece izvodjenje simulacije, a na desnu tipku zaustavlja njeno
 * izvodjenje.
 */ 
void mouse(int button, int state, int x, int y) 
{
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
				switcher = GL_TRUE;
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN)
				switcher = GL_FALSE;
			break;
		default:
			break;
   }
}

/* Callback funkcija koja se poziva kad se pritisne tipka na tipkovnici.
 * U ovom se konkretno slucaju pritiskom na strelicu u desno pokrece  
 * i ubrzava izvodjenje simulacije, a na desnu tipku usporava njeno izvodjenje.
 */ 
void keyboard(int Key, int x, int y)
{
	switch (Key) {
	case GLUT_KEY_UP:
		animacija *= 3.0;
		break;
	case GLUT_KEY_DOWN:
		animacija /= 3.0;
		break;
	}
}

int main(int argc, char** argv)
{

	fprintf(stdout, "Lijeva tipka misa pokrece animaciju, a desna zaustavlja.\n");
	fprintf(stdout, "Strelica gore - ubrzavanje animacije.\n");
	fprintf(stdout, "Strelica dolje - usporavanje animacije.\n");

	glutInit(&argc, argv);
	//koristenje dvostrukog spremnika
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

	//stvaranje i pozicioniranje prozora
	glutInitWindowSize (600, 400); 
	glutInitWindowPosition (500, 500);
	glutCreateWindow (argv[0]);

	//inicijalizacija
	init ();

	//funkcije za hvatanje unosa putem tipkovnice i  miša
	glutSpecialFunc(keyboard);
	glutMouseFunc(mouse);

	glutReshapeFunc(reshape);
	glutDisplayFunc(DrawAndDisplay);
	
	glutMainLoop();
	return 0;
}


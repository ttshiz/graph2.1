// Draws colored cube  

#include "Angel.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
int width = 0;
int height = 0;

// remember to prototype
void generateGeometry( void );
void display( void );
void keyboard( unsigned char key, int x, int y );
void quad( int a, int b, int c, int d );
void colorcube(void);
void drawCube(void);

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

struct myTriangle {
	int firstp;
	int secondp;
	int thirdp;
} ;

// handle to program
GLuint program;

using namespace std;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];
color4 colors[NumVertices];

vector<point4> filepoints;
vector<myTriangle> filepolys;
vector<unsigned int> fileindices;
vector<color4> filecolors;
float infinity = numeric_limits<float>::infinity();
float neginfinity = -1 * infinity;
float filemaxX = neginfinity;
float filemaxY = neginfinity;
float filemaxZ = neginfinity;
float fileminX = infinity;
float fileminY = infinity;
float fileminZ = infinity;
float numfilev = 0;
float numfilei = 0;
vector<point4> hugepoints;
vector<point4> hugecolors;

char* plyfiles[43] = {
	"airplane.ply", "ant.ply", "apple.ply", "balance.ply", "beethoven.ply",
	"big_atc.ply", "big_dodge.ply", "big_porsche.ply", "big_spider.ply", "canstick.ply",
	"chopper.ply", "cow.ply", "dolphins.ply", "egret.ply", "f16.ply", 
	"footbones.ply", "fracttree.ply", "galleon.ply", "hammerhead.ply", "helix.ply",
	"hind.ply", "kerolamp.ply", "ketchup.ply", "mug.ply", "part.ply",
	"pickup_big.ply", "pump.ply", "pumpa_tb.ply", "sandal.ply", "saratoga.ply",
	"scissors.ply", "shark.ply", "steeringweel.ply", "stratocaster.ply", "street_lamp.ply",
	"teapot.ply", "tennis_shoe.ply", "tommygun.ply", "trashcan.ply", "turbine.ply",
	"urn2.ply", "walkman.ply", "weathervane.ply"
};
// Vertices of a unit cube centered at origin, sides aligned with axes
point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};
// RGBA olors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0 ),  // black
    color4( 1.0, 0.0, 0.0, 1.0 ),  // red
    color4( 1.0, 1.0, 0.0, 1.0 ),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0 ),  // green
    color4( 0.0, 0.0, 1.0, 1.0 ),  // blue
    color4( 1.0, 0.0, 1.0, 1.0 ),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0 ),  // white
    color4( 0.0, 1.0, 1.0, 1.0 )   // cyan
};
// quad generates two triangles for each face and assigns colors
//    to the vertices
int Index = 0;
void quad( int a, int b, int c, int d )
{
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
    colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
    colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

void generateGeometry( void )
{	
	// make function to go in here if it doesn't display try scaling ortho
	// generate giant array or use glelements
	colorcube();

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );


	// Load shaders and use the resulting shader program
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );
    glUseProgram( program );
     // set up vertex arrays
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation( program, "vColor" ); 
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
			   BUFFER_OFFSET(sizeof(points)) );

	// sets the default color to clear screen
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

void readPLYFile(char * filename) {
	std::ifstream file(filename);
	if (file.fail()) {
		cout << "Failed to open file!" << std::endl;
	}
	string str;
	getline(file, str);
	if (str != "ply") {     // first line
		exit(EXIT_FAILURE);
	}
	getline(file, str); // ignore second line 
	getline(file, str, ' ');  // ignore first two space separated strings
	getline(file, str, ' ');
	int numVerts;
	getline(file, str, ' ');
	numVerts = stoi(str);
	//cout << "numVerts: " << numVerts << endl;
	getline(file, str); // ignore next three line
	getline(file, str);
	getline(file, str);
	getline(file, str, ' ');  // ignore first two space separated strings
	getline(file, str, ' ');
	int numPolys;
	getline(file, str, ' ');
	numPolys = stoi(str);
	//cout << "numPolys: " << numPolys << endl;
	getline(file, str); // skip two lines
	getline(file, str);
	color4 mycolor = vertex_colors[0];
	float fourth = 1.0;
	numfilev = numVerts;
	numfilei = numPolys;
	//cout << "size of file indeces before " << fileindices.size() << endl;
	for (int i = 0; i < numVerts; i++) {
		getline(file, str, ' ');
		float x = stof(str);
		getline(file, str, ' ');
		float y = stof(str);
		getline(file, str, ' ');
		float z = stof(str);
		filepoints.push_back(point4(x, y, z, fourth));
		filecolors.push_back(mycolor);
		if (filemaxX < x) {
			filemaxX = x;
		}
		if (filemaxY < y) {
			filemaxY = y;
		}
		if (filemaxZ < z) {
			filemaxZ = z;
		}
		if (fileminX > x) {
			fileminX = x;
		}
		if (fileminY > y) {
			fileminY = y;
		}
		if (fileminZ > z) {
			fileminZ = z;
		}
	}
	for (int i = 0; i < numPolys; i++) {
		getline(file, str, ' ');
		int numV = stoi(str);
		getline(file, str, ' ');
		int v1 = stoi(str);
		getline(file, str, ' ');
		int v2 = stoi(str);
		getline(file, str, ' ');
		int v3 = stoi(str);
		myTriangle curTri;
		curTri.firstp = v1;
		curTri.secondp = v2;
		curTri.thirdp = v3;
		filepolys.push_back(curTri);
		fileindices.push_back(v1);
		fileindices.push_back(v2);
		fileindices.push_back(v3);
	}

	//cout << "size of file indeces after " << fileindices.size() << endl;
	/*for (int i = 0; i < numVerts; i++) {
	cout << filepoints[i] << endl;
	}*/
	/*for (int i = 0; i < numPolys; i++) {
	cout << filepolys[i].firstp << ", " << filepolys[i].secondp << ", " << filepolys[i].thirdp << endl;
	}*/
	//cout << fileminX << ", " << fileminY << ", " << fileminZ << endl;
	//cout << filemaxX << ", " << filemaxY << ", " << filemaxZ << endl;
}

void generateFileGeometry(void)
{
	// make function to go in here if it doesn't display try scaling ortho
	// generate giant array or use glelements
	readPLYFile(plyfiles[0]);

	// Way one
	//for (int i = 0; i < sizeof(fileindices); i++) {
	//	//cout << sizeof(fileindices) << endl;
	//	hugepoints.push_back(filepoints[fileindices[i]]);
	//}
	//// Way two
	/**/
	mat4 ortho = Ortho(fileminX, filemaxX, fileminY, filemaxY, filemaxZ, fileminZ);
	GLuint ProjLoc = glGetUniformLocation(program, "Proj");
	glUniformMatrix4fv(ProjLoc, 1, GL_TRUE, ortho);
	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// Way one
	/*glBufferData(GL_ARRAY_BUFFER, hugepoints.size() * sizeof(point4) + hugecolors.size() * sizeof(color4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, hugepoints.size() * sizeof(point4), hugepoints.data());
	glBufferSubData(GL_ARRAY_BUFFER, hugepoints.size() * sizeof(point4), hugecolors.size() * sizeof(color4), hugecolors.data());
	*/
	// Way two
	glBufferData(GL_ARRAY_BUFFER, filepoints.size()*sizeof(point4) + filecolors.size()*sizeof(color4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, filepoints.size()*sizeof(point4), filepoints.data());
	glBufferSubData(GL_ARRAY_BUFFER, filepoints.size()*sizeof(point4), filecolors.size()*sizeof(color4), filecolors.data());

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, fileindices.size() * sizeof(unsigned int), &fileindices[0], GL_STATIC_DRAW);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader1.glsl", "fshader1.glsl");
	glUseProgram(program);
	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(filepoints.size()*sizeof(point4)));

	// sets the default color to clear screen
	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

void drawFile(void)
{
	// change to GL_FILL
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable(GL_DEPTH_TEST);

	// Way one
	//glDrawArrays(GL_TRIANGLES, 0, hugepoints.size());

	// Way two
	glDrawElements(GL_TRIANGLES, sizeof(fileindices),GL_UNSIGNED_INT, fileindices.data());
	glDisable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------------
// this is where the drawing should happen
void mydisplay(void)
{
	// SOME RANDOM TIPS
	//========================================================================
	// remember to enable depth buffering when drawing in 3d

	// avoid using glTranslatex, glRotatex, push and pop
	// pass your own view matrix to the shader directly
	// refer to the latest OpenGL documentation for implementation details

	// Do not set the near and far plane too far appart!
	// depth buffers do not have unlimited resolution
	// surfaces will start to fight as they come nearer to each other
	// if the planes are too far appart (quantization errors :(   )

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width / (GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0]; viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0]; viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0]; viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0]; viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2]; viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2]; viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2]; viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2]; viewMatrixf[15] = perspectiveMat[3][3];

	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);

	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv(modelMatrix, 1, GL_TRUE, modelMat);
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawFile();
	glFlush(); // force output to graphics hardware

			   // use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

void drawCube(void)
{
	// change to GL_FILL
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// draw functions should enable then disable the features 
	// that are specifit the themselves
	// the depth is disabled after the draw 
	// in case you need to draw overlays
	glEnable( GL_DEPTH_TEST );
    glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDisable( GL_DEPTH_TEST ); 
}


//----------------------------------------------------------------------------
// this is where the drawing should happen
void display( void )
{
	// SOME RANDOM TIPS
	//========================================================================
	// remember to enable depth buffering when drawing in 3d

	// avoid using glTranslatex, glRotatex, push and pop
	// pass your own view matrix to the shader directly
	// refer to the latest OpenGL documentation for implementation details

    // Do not set the near and far plane too far appart!
	// depth buffers do not have unlimited resolution
	// surfaces will start to fight as they come nearer to each other
	// if the planes are too far appart (quantization errors :(   )

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // clear the window

	Angel::mat4 perspectiveMat = Angel::Perspective((GLfloat)45.0, (GLfloat)width/(GLfloat)height, (GLfloat)0.1, (GLfloat) 100.0);

	float viewMatrixf[16];
	viewMatrixf[0] = perspectiveMat[0][0];viewMatrixf[4] = perspectiveMat[0][1];
	viewMatrixf[1] = perspectiveMat[1][0];viewMatrixf[5] = perspectiveMat[1][1];
	viewMatrixf[2] = perspectiveMat[2][0];viewMatrixf[6] = perspectiveMat[2][1];
	viewMatrixf[3] = perspectiveMat[3][0];viewMatrixf[7] = perspectiveMat[3][1];

	viewMatrixf[8] = perspectiveMat[0][2];viewMatrixf[12] = perspectiveMat[0][3];
	viewMatrixf[9] = perspectiveMat[1][2];viewMatrixf[13] = perspectiveMat[1][3];
	viewMatrixf[10] = perspectiveMat[2][2];viewMatrixf[14] = perspectiveMat[2][3];
	viewMatrixf[11] = perspectiveMat[3][2];viewMatrixf[15] = perspectiveMat[3][3];
	
	Angel::mat4 modelMat = Angel::identity();
	modelMat = modelMat * Angel::Translate(0.0, 0.0, -2.0f) * Angel::RotateY(45.0f) * Angel::RotateX(35.0f);
	
	// set up projection matricies
	GLuint modelMatrix = glGetUniformLocationARB(program, "model_matrix");
	glUniformMatrix4fv( modelMatrix, 1, GL_TRUE, modelMat );
	GLuint viewMatrix = glGetUniformLocationARB(program, "projection_matrix");
	glUniformMatrix4fv( viewMatrix, 1, GL_FALSE, viewMatrixf);

	drawCube();
    glFlush(); // force output to graphics hardware

	// use this call to double buffer
	glutSwapBuffers();
	// you can implement your own buffers with textures
}

//----------------------------------------------------------------------------

// keyboard handler
void keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------
// entry point
int main( int argc, char **argv )
{
	// init glut
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 );
	width = 512;
	height = 512;

	// create window
	// opengl can be incorperated into other packages like wxwidgets, fltoolkit, etc.
    glutCreateWindow( "Color Cube" );

	// init glew
    glewInit();

	generateFileGeometry();
    //generateGeometry();

	// assign handlers
    glutDisplayFunc( mydisplay );
    glutKeyboardFunc( keyboard );
	// should add menus
	// add mouse handler
	// add resize window functionality (should probably try to preserve aspect ratio)

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();
    return 0;
}

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <iostream>
#include <time.h>

#define GL_CLAMP_TO_EDGE 0x812F

const float PI = 3.141592653;
bool mass[1000][1000][1000];
float size = 20.f;
float angleX, angleY;

// Define the dimensions of the window
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Define the vertex coordinates of the cube
GLfloat vertices[] = {
	// front
	-1.0, -1.0,  1.0,
	 1.0, -1.0,  1.0,
	 1.0,  1.0,  1.0,
	-1.0,  1.0,  1.0,
	// top
	-1.0,  1.0,  1.0,
	 1.0,  1.0,  1.0,
	 1.0,  1.0, -1.0,
	-1.0,  1.0, -1.0,
	// back
	 1.0, -1.0, -1.0,
	-1.0, -1.0, -1.0,
	-1.0,  1.0, -1.0,
	 1.0,  1.0, -1.0,
	 // bottom
	 -1.0, -1.0, -1.0,
	  1.0, -1.0, -1.0,
	  1.0, -1.0,  1.0,
	 -1.0, -1.0,  1.0,
	 // left
	 -1.0, -1.0, -1.0,
	 -1.0, -1.0,  1.0,
	 -1.0,  1.0,  1.0,
	 -1.0,  1.0, -1.0,
	 // right
	  1.0, -1.0,  1.0,
	  1.0, -1.0, -1.0,
	  1.0,  1.0, -1.0,
	  1.0,  1.0,  1.0,
};

// Define the texture coordinates of the cube
GLfloat texCoords[2 * 4 * 6] = {
	// front
	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,
};

// Define the indices of the cube triangles
GLuint indices[] = {
	// front
	 0,  1,  2,
	 2,  3,  0,
	 // top
	  4,  5,  6,
	  6,  7,  4,
	  // back
	   8,  9, 10,
	  10, 11,  8,
	  // bottom
	  12, 13, 14,
	  14, 15, 12,
	  // left
	  16, 17, 18,
	  18, 19, 16,
	  // right
	  20, 21, 22,
	  22, 23, 20
};

// Load a texture from file and return its ID
GLuint loadTexture(const char* filename) {
	SDL_Surface* surface = SDL_LoadBMP(filename);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
	SDL_FreeSurface(surface);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return textureID;
}

void createBox(GLuint skybox[], float size)
{
	glBindTexture(GL_TEXTURE_2D, skybox[0]);
	glBegin(GL_QUADS);
	//front
	glTexCoord2f(0, 1);   glVertex3f(-size, -size, -size);
	glTexCoord2f(1, 1);   glVertex3f(size, -size, -size);
	glTexCoord2f(1, 0);   glVertex3f(size, size, -size);
	glTexCoord2f(0, 0);   glVertex3f(-size, size, -size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[1]);
	glBegin(GL_QUADS);
	//back
	glTexCoord2f(0, 1); glVertex3f(size, -size, size);
	glTexCoord2f(1, 1); glVertex3f(-size, -size, size);
	glTexCoord2f(1, 0); glVertex3f(-size, size, size);
	glTexCoord2f(0, 0); glVertex3f(size, size, size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[2]);
	glBegin(GL_QUADS);
	//left
	glTexCoord2f(0, 1); glVertex3f(-size, -size, size);
	glTexCoord2f(1, 1); glVertex3f(-size, -size, -size);
	glTexCoord2f(1, 0); glVertex3f(-size, size, -size);
	glTexCoord2f(0, 0); glVertex3f(-size, size, size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[3]);
	glBegin(GL_QUADS);
	//right
	glTexCoord2f(0, 1); glVertex3f(size, -size, -size);
	glTexCoord2f(1, 1); glVertex3f(size, -size, size);
	glTexCoord2f(1, 0); glVertex3f(size, size, size);
	glTexCoord2f(0, 0); glVertex3f(size, size, -size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[4]);
	glBegin(GL_QUADS);
	//bottom
	glTexCoord2f(0, 1); glVertex3f(-size, -size, size);
	glTexCoord2f(1, 1); glVertex3f(size, -size, size);
	glTexCoord2f(1, 0); glVertex3f(size, -size, -size);
	glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skybox[5]);
	glBegin(GL_QUADS);
	//top  		
	glTexCoord2f(0, 1); glVertex3f(-size, size, -size);
	glTexCoord2f(1, 1); glVertex3f(size, size, -size);
	glTexCoord2f(1, 0); glVertex3f(size, size, size);
	glTexCoord2f(0, 0); glVertex3f(-size, size, size);
	glEnd();
}

bool check(int x, int y, int z)
{
	if ((x < 0) || (x >= 1000) ||
		(y < 0) || (y >= 1000) ||
		(z < 0) || (z >= 1000)) return false;

	return mass[x][y][z];
}

// keyboard specific
const Uint8* m_keystates;

bool isKeyDown(SDL_Scancode key)
{
	if (m_keystates != 0)
	{
		if (m_keystates[key] == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

class Player {
public:
	float x, y, z;
	float dx, dy, dz;
	float w, h, d; //width, height, depth
	bool onGround;
	float speed;

	Player(float x0, float y0, float z0) {
		x = x0; y = y0; z = z0;
		dx = 0; dy = 0; dz = 0;
		w = 5; h = 20; d = 5; speed = 5;
		onGround = false;
	}

	void update(float time) {
		if (!onGround) dy -= 1.5 * time;
		onGround = 0;

		x += dx * time;
		collision(dx, 0, 0);
		y += dy * time;
		collision(0, dy, 0);
		z += dz * time;
		collision(0, 0, dz);

		dx = dz = 0;
	}

	void collision(float Dx, float Dy, float Dz)
	{
		for (int X = (x - w) / size; X < (x + w) / size; X++)
			for (int Y = (y - h) / size; Y < (y + h) / size; Y++)
				for (int Z = (z - d) / size; Z < (z + d) / size; Z++)
					if (check(X, Y, Z)) {
						if (Dx > 0)  x = X * size - w;
						if (Dx < 0)  x = X * size + size + w;
						if (Dy > 0)  y = Y * size - h;
						if (Dy < 0) { y = Y * size + size + h; onGround = true; dy = 0; }
						if (Dz > 0)  z = Z * size - d;
						if (Dz < 0)  z = Z * size + size + d;
					}
	}

	void keyboard()
	{
		if (isKeyDown(SDL_SCANCODE_SPACE)) if (onGround) { onGround = false; dy = 12; };

		if (isKeyDown(SDL_SCANCODE_W))
		{
			dx = -sin(angleX / 180 * PI) * speed;
			dz = -cos(angleX / 180 * PI) * speed;
		}

		if (isKeyDown(SDL_SCANCODE_S))
		{
			dx = sin(angleX / 180 * PI) * speed;
			dz = cos(angleX / 180 * PI) * speed;
		}

		if (isKeyDown(SDL_SCANCODE_D))
		{
			dx = sin((angleX + 90) / 180 * PI) * speed;
			dz = cos((angleX + 90) / 180 * PI) * speed;
		}

		if (isKeyDown(SDL_SCANCODE_A))
		{
			dx = sin((angleX - 90) / 180 * PI) * speed;
			dz = cos((angleX - 90) / 180 * PI) * speed;
		}
	}
};

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

const int FPS = 60;
const int DELAY_TIME = 2000.0f / FPS;

int main(int argc, char* argv[]) {

	srand(time(nullptr));

	for (int i = 1; i < 6; i++)
		memcpy(&texCoords[i * 4 * 2], &texCoords[0], 2 * 4 * sizeof(GLfloat));

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Mine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);

	SDL_Init(SDL_INIT_EVENTS);

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//set the projection matrix
	gluPerspective(90.0f, 1.f, 1.f, 2000.f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_TEXTURE_2D);

	//ShowCursor(FALSE);

	//mouse buttons
	bool mLeft = 0, mRight = 0;
	Player p(100, 200, 100);

	// load the heightmap texture
	SDL_Surface* loadedSurface = IMG_Load("assets//heightmap.bmp");
	SDL_Color rgb;
	
	for (int x = 0; x < 256; x++)
		for (int z = 0; z < 256; z++)
		{
			//get height at x,z
			//int c = im.getPixel(x, z).r / 15;
			//int c = (100 - rand() % 20) / 15;
			Uint32 data = getpixel(loadedSurface, x, z);
			SDL_GetRGB(data, loadedSurface->format, &rgb.r, &rgb.g, &rgb.b);
			int c = rgb.r / 15;
			for (int y = 0; y < c; y++)
				if (y > c - 3) mass[x][y][z] = 1;
		}

	SDL_FreeSurface(loadedSurface);
	
	GLuint t = loadTexture("assets//cursor.bmp");

	GLuint skybox[6];
	skybox[0] = loadTexture("assets/skybox/skybox_front.bmp");
	skybox[1] = loadTexture("assets/skybox/skybox_back.bmp");
	skybox[2] = loadTexture("assets/skybox/skybox_left.bmp");
	skybox[3] = loadTexture("assets/skybox/skybox_right.bmp");
	skybox[4] = loadTexture("assets/skybox/skybox_bottom.bmp");
	skybox[5] = loadTexture("assets/skybox/skybox_top.bmp");
	//skybox[2] = loadTexture("assets/skybox/texture.bmp");


	GLuint box[6];
	box[0] = loadTexture("assets/grassBox/side.bmp");
	box[1] = loadTexture("assets/grassBox/side.bmp");
	box[2] = loadTexture("assets/grassBox/side.bmp");
	box[3] = loadTexture("assets/grassBox/side.bmp");
	box[4] = loadTexture("assets/grassBox/bottom.bmp");
	box[5] = loadTexture("assets/grassBox/top.bmp");

	Uint32 frameStart, frameTime;
	bool quit = false;
	SDL_Event event;
	while (!quit) {

		frameStart = SDL_GetTicks(); //initial time.

		//input handler
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;

			case SDL_KEYDOWN:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			case SDL_KEYUP:
				m_keystates = SDL_GetKeyboardState(0);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_RIGHT) mRight = true;
				if (event.button.button == SDL_BUTTON_LEFT) mLeft = true;
				break;

			default:
				break;
			}
		}

		if (isKeyDown(SDL_SCANCODE_ESCAPE)) quit = true;

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//process the player
		p.keyboard();
		p.update(1);

		////////
		POINT mousexy;
		GetCursorPos(&mousexy);
		int xt = 400; //get the center of the window.
		int yt = 300;

		angleX += (xt - mousexy.x) / 4;
		angleY += (yt - mousexy.y) / 4;

		if (angleY < -89.0) { angleY = -89.0; }
		if (angleY > 89.0) { angleY = 89.0; }

		SetCursorPos(xt, yt);
		///////////////////

		if (mRight || mLeft)
		{
			float x = p.x;
			float y = p.y + p.h / 2;
			float z = p.z;

			int X, Y, Z, oldX, oldY, oldZ;
			int dist = 0;
			while (dist < 120)
			{
				dist++;

				x += -sin(angleX / 180 * PI);    X = x / size;
				y += tan(angleY / 180 * PI);    Y = y / size;
				z += -cos(angleX / 180 * PI);    Z = z / size;

				if (check(X, Y, Z))
					if (mLeft) { mass[X][Y][Z] = 0; break; }
					else { mass[oldX][oldY][oldZ] = 1; break; }

				oldX = X; oldY = Y; oldZ = Z;
			}
		}

		mLeft = mRight = 0; //button up

		// Set the camera position
		glLoadIdentity();
		gluLookAt(p.x, p.y + p.h / 2, p.z, p.x - sin(angleX / 180 * PI), p.y + p.h / 2 + (tan(angleY / 180 * PI)), p.z - cos(angleX / 180 * PI), 0, 1, 0);

		//draw blocks
		int R = 30;

		int X = p.x / size;
		int Y = p.y / size;
		int Z = p.z / size;

		for (int x = X - R; x < X + R; x++)
			for (int y = 0; y < 25; y++)
				for (int z = Z - R; z < Z + R; z++)
				{
					if (!check(x, y, z)) continue;

					glTranslatef(size*x + size / 2, size*y + size / 2, size*z + size / 2);

					createBox(box, size / 2);

					glTranslatef(-size * x - size / 2, -size * y - size / 2, -size * z - size / 2);
				}

		//draw skybox
		glTranslatef(p.x, p.y, p.z);
		createBox(skybox, 1000);
		glTranslatef(-p.x, -p.y, -p.z);

		// Swap the buffer and update the window
		SDL_GL_SwapWindow(window);

		//final time - initial time
		frameTime = SDL_GetTicks() - frameStart;

		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime)); //wait
		}
	}

	// Clean up
	glDeleteTextures(1, &t);
	for (int i = 0; i < 5; i++) {
		glDeleteTextures(1, &skybox[i]);
		glDeleteTextures(1, &box[i]);
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}


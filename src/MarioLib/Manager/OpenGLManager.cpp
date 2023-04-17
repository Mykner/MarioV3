#include "OpenGLManager.h"

#include "Game.h"
#include "SpriteManager.h"
#include "NaPlatformAPI.h"

OpenGLManager::OpenGLManager(Game *pGame) :
	RenderManager(pGame)
{
#ifdef WIN32
	m_hDC = nullptr;
#endif
	m_bSingleBuffer = false;

	m_nShadowProgram = 0;
	m_pfglUseProgram = 0;
}

OpenGLManager::~OpenGLManager()
{
}

void OpenGLManager::Init()
{
	NaDebugOut(L"Init OpenGL...\n");

#ifdef WIN32
	HWND hWnd = m_pGame->m_hWindow;

	// Bind with Win DC
	NaDebugOut(L"OpenGLManager: Bind window 0x%08x\n", hWnd);
	if (m_hDC)
		return;

	m_hDC = GetDC(hWnd);
	HDC hdc = m_hDC;

	HGLRC hContext = wglGetCurrentContext();
	if (hContext)
	{
		if (m_hDC == wglGetCurrentDC())
			return;
	}

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	if (m_bSingleBuffer)
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL;
	else
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, nPixelFormat, &pfd);

	if (hContext)
	{
		HGLRC hrc = wglCreateContext(hdc);
		wglCopyContext(hContext, hrc, GL_ALL_ATTRIB_BITS);
		wglDeleteContext(hContext);

		wglMakeCurrent(hdc, hrc);
	}
	else
	{
		HGLRC hrc = wglCreateContext(hdc);
		wglMakeCurrent(hdc, hrc);
	}

	// Set FPS Limit (VSync?)
	typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT =
		(PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(1);

	// Viewport
	int nViewportW, nViewportH;
	RECT rc;
	GetClientRect(hWnd, &rc);

	nViewportW = rc.right - rc.left;
	nViewportH = rc.bottom - rc.top;

	InitShader();
#else
	// Viewport
    int nViewportW, nViewportH;
    nViewportW = 256;
    nViewportH = 240;
#endif

	// Setup 2D
	Init2D(nViewportW, nViewportH);
}

void OpenGLManager::Init2D(int nViewportW, int nViewportH)
{
	glViewport(0, 0, nViewportW, nViewportH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, m_pGame->m_nWidth, m_pGame->m_nHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);

	// Don't use
	//glShadeModel(GL_SMOOTH);
	// Enable the alpha test. 
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.0f);

	// To use transparency
	glEnable(GL_BLEND);
}

void OpenGLManager::Release()
{
	// #TODO OpenGL ReleaseContext
}

void OpenGLManager::OnSize(GLsizei width, GLsizei height)
{
	// Sets the size of the OpenGL viewport
	glViewport(0, 0, width, height);

	// Select the projection stack and apply an orthographic projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, m_pGame->m_nWidth, m_pGame->m_nHeight, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

void OpenGLManager::BeginRenderFrame(COLORREF bgColor)
{
	glClearColor(
		COLOR_B(bgColor) / 256.0f,
		COLOR_G(bgColor) / 256.0f,
		COLOR_R(bgColor) / 256.0f, 
		1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLManager::EndRenderFrame()
{
#ifdef WIN32
	if (m_bSingleBuffer)
		glFlush();
	else
		SwapBuffers(m_hDC);
#endif
}

void OpenGLManager::LoadTexture(const wchar_t * filename, TextureBase *pTexture)
{
	int width, height;
	unsigned char * data;

	FILE *file = nullptr;
	NaString strFileName(filename);
	NaPlatform::FileOpen(&file, strFileName.cstr(), "rb");
	if (file == NULL)
		return;

	unsigned char* header = new unsigned char[54];
	fread(header, 54, 1, file);
	{
		if (header[0] != 'B' && header[1] != 'M')
		{
			fclose(file);
			throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
		}

		if (header[28] != 24 && header[28] != 32)
		{
			fclose(file);
			throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
		}

		unsigned int BitsPerPixel = header[28];
		width = header[18] + (header[19] << 8);
		height = header[22] + (header[23] << 8);

		unsigned int PixelsOffset = header[10] + (header[11] << 8);
		unsigned int size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
	}

	delete[] header;

	data = new unsigned char[width * height * 3];
	fread(data, width * height * 3, 1, file);
	fclose(file);

	// BGR -> RGB swap data
	unsigned char *pDataRGBA = new unsigned char[width * height * 4];
	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char R, G, B;
		B = data[index];
		G = data[index + 1];
		R = data[index + 2];

		pDataRGBA[(i * 4) + 0] = R;
		pDataRGBA[(i * 4) + 1] = G;
		pDataRGBA[(i * 4) + 2] = B;

		if (R == 0x48 && G == 0xff && B == 0x00)
			pDataRGBA[(i * 4) + 3] = 0x00;
		else
			pDataRGBA[(i * 4) + 3] = 0xff;// ff;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// MSAA off
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// MSAA on
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// fix for Colorkey
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pDataRGBA);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pDataRGBA
	);

	free(data);
	free(pDataRGBA);

	pTexture->m_glTexture = texture;

	SIZE s = { width, height };
	m_mapTextureSize.insert(
		std::pair<TextureBase*, SIZE>(pTexture, s)
	);
}

void OpenGLManager::ReleaseTexture(TextureBase * pTexture)
{
	// #TODO
	if (pTexture)
		glDeleteTextures(1, &pTexture->m_glTexture);
}

SIZE OpenGLManager::GetTextureSize(TextureBase * pTexture)
{
	/*
	// Buggy
	glBindTexture(GL_TEXTURE_2D, pTexture->m_glTexture);

	int w = 0, h = 0;
	int nMipLevel = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, nMipLevel, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, nMipLevel, GL_TEXTURE_HEIGHT, &h);
	*/

	std::map<TextureBase*, SIZE>::iterator it = m_mapTextureSize.find(pTexture);
	if (it != m_mapTextureSize.end())
	{
		return it->second;
	}

	SIZE s = { 0, 0 };
	return s;
}

void OpenGLManager::SetDefaultShader()
{
	if (m_pfglUseProgram)
		m_pfglUseProgram(0);
}

void OpenGLManager::SetShadowShader()
{
	if (m_pfglUseProgram)
		m_pfglUseProgram(m_nShadowProgram);
}

void OpenGLManager::RenderTexture(TextureBase * pTexture, float x, float y, RECT & rc, int offsetX, int offsetY, bool bFlip, bool bFlipV, int nColor, float fScaleX, float fScaleY, float fZ, float fAngle, int nRotationAxis)
{
	// Prepare
	glBindTexture(GL_TEXTURE_2D, pTexture->m_glTexture);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render Sprite Sheet
	SIZE s = GetTextureSize(pTexture);
	int nTexWidth = rc.right - rc.left;
	int nTexHeight = rc.bottom - rc.top;
	float fTexX = (float)rc.left / (float)s.cx;
	float fTexY = -(float)rc.top / (float)s.cy;
 	float fTexW = (float)nTexWidth / (float)s.cx;
 	float fTexH = - (float)nTexHeight / (float)s.cy;
	float fCenterX = nTexWidth / 2.0f;
	float fCenterY = nTexHeight / 2.0f;

	glPushMatrix();
	{
		glTranslatef(x, y, 0);

		if (bFlip)
		{
			offsetX = nTexWidth - offsetX;
		}

		if (nRotationAxis == 0)
		{
			glTranslatef(
				(fCenterX - (float)offsetX) * fScaleX,
				(fCenterY - (float)offsetY) * fScaleY,
				0);

			if (fAngle != 0.0f)
			{
				if (bFlip)
					fAngle *= -1;
				if (bFlipV)
					fAngle *= -1;

				glRotatef(fAngle, 0, 0, 1.0f);
			}
		}
		else
		{
			glTranslatef(
				(fCenterX - (float)offsetX) * fScaleX,
				((fCenterY * 2.0f) - (float)offsetY) * fScaleY,
				0);

			if (fAngle != 0.0f)
			{
				if (bFlip)
					fAngle *= -1;
				if (bFlipV)
					fAngle *= -1;

				glRotatef(fAngle, 0, 0, 1.0f);
			}

			glTranslatef(
				0,
				-fCenterY * fScaleY,
				0);
		}
	
		if (bFlip || bFlipV || fScaleX != 1.0f || fScaleY != 1.0f)
		{
			float fFlip = bFlip ? -1 : 1;
			float fFlipV = bFlipV ? -1 : 1;

			glScalef(
				fScaleX * fFlip,
				fScaleY * fFlipV,
				0.0f);
		}

		// 0xXXXXXXXX : BGRA
		float fB = ((BYTE)(nColor)) / 255.0f;
		float fG = ((BYTE)(nColor >> 8)) / 255.0f;
		float fR = ((BYTE)(nColor >> 16)) / 255.0f;
		float fA = ((BYTE)(nColor >> 24)) / 255.0f;
		
		/*
		// For test box
		//glColor4f(fR, fG, fB, fA);
		glColor4f(0, 0, 0, 1);
		glBegin(GL_POLYGON);
		{
			glVertex3f(-fCenterX, -fCenterY, 0);
			glVertex3f(-fCenterX, -fCenterY + nTexHeight, 0);
			glVertex3f(-fCenterX + nTexWidth, -fCenterY + nTexHeight, 0);
			glVertex3f(-fCenterX + nTexWidth, -fCenterY, 0);
		}
		glEnd();
		//*/

		glColor4f(fR, fG, fB, fA);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(fTexX, fTexY);
			glVertex3f(-fCenterX, -fCenterY, 0);

			glTexCoord2f(fTexX, fTexY + fTexH);
			glVertex3f(-fCenterX, -fCenterY + nTexHeight, 0);

			glTexCoord2f(fTexX + fTexW, fTexY + fTexH);
			glVertex3f(-fCenterX + nTexWidth, -fCenterY + nTexHeight, 0);

			glTexCoord2f(fTexX + fTexW, fTexY);
			glVertex3f(-fCenterX + nTexWidth, -fCenterY, 0);
		}
		glEnd();
	}
	glPopMatrix();
}

void OpenGLManager::TextFont(int x, int y, NaString str)
{

}

void OpenGLManager::InitShader()
{
	const char *f =
		"precision mediump float;"
		"uniform sampler2D s_texture;"
		"void main() {"
		"vec4 tex = texture2D(s_texture,  gl_TexCoord[0].st);"
		"	gl_FragColor = vec4(gl_Color.r, gl_Color.g, gl_Color.b, tex.a * gl_Color.a);"
		"}";
		
	PFGLCREATESHADER pfglCreateShader = 0;
	PFGLSHADERSOURCE pfglShaderSource = 0;
	PFGLCOMPILESHADER pfglCompileShader = 0;
	PFGLCREATEPROGRAM pfglCreateProgram = 0;
	PFGLATTACHSHADER pfglAttachShader = 0;
	PFGLLINKPROGRAM pfglLinkProgram = 0;
	PFGLUSEPROGRAM pfglUseProgram = 0;

	pfglCreateShader = (PFGLCREATESHADER)wglGetProcAddress("glCreateShader");
	pfglShaderSource = (PFGLSHADERSOURCE)wglGetProcAddress("glShaderSource");
	pfglCompileShader = (PFGLCOMPILESHADER)wglGetProcAddress("glCompileShader");
	pfglCreateProgram = (PFGLCREATEPROGRAM)wglGetProcAddress("glCreateProgram");
	pfglAttachShader = (PFGLATTACHSHADER)wglGetProcAddress("glAttachShader");
	pfglLinkProgram = (PFGLLINKPROGRAM)wglGetProcAddress("glLinkProgram");
	m_pfglUseProgram = (PFGLUSEPROGRAM)wglGetProcAddress("glUseProgram");

	//GL_FRAGMENT_SHADER                0x8B30
	GLuint ps;
	if (pfglCreateShader)
		ps = pfglCreateShader(0x8B30);

	if (pfglShaderSource)
		pfglShaderSource(ps, 1, &f, 0);

	if (pfglCompileShader)
		pfglCompileShader(ps);

	if (pfglCreateProgram)
		m_nShadowProgram = pfglCreateProgram();

	if (pfglAttachShader)
		pfglAttachShader(m_nShadowProgram, ps);

	if (pfglLinkProgram)
		pfglLinkProgram(m_nShadowProgram);
}

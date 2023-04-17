#pragma once

#include "RenderManager.h"

#include "Common.h"

#ifdef WIN32
#include "glut.h"
#else
#include <GLUT/glut.h>

#define APIENTRY
#define wglGetProcAddress glutGetProcAddress
#endif

#include <vector>
#include <fstream>
#include <map>

typedef bool (APIENTRY *PFGLCREATESHADER)(int);
typedef bool (APIENTRY *PFGLSHADERSOURCE)(GLuint, int, const char**, int);
typedef bool (APIENTRY *PFGLCOMPILESHADER)(int);
typedef GLuint(APIENTRY *PFGLCREATEPROGRAM) (void);
typedef void (APIENTRY *PFGLATTACHSHADER) (GLuint program, GLuint shader);
typedef void (APIENTRY *PFGLLINKPROGRAM) (GLuint program);
typedef void (APIENTRY *PFGLUSEPROGRAM) (GLuint program);

class Game;
class OpenGLManager : public RenderManager
{
public:
	OpenGLManager(Game *pGame);
	virtual ~OpenGLManager();

	virtual void Init();
	void Init2D(int nViewportW, int nViewportH);
	virtual void Release();

	virtual void OnSize(int width, int height);

	virtual void BeginRenderFrame(COLORREF bgColor = 0);
	virtual void EndRenderFrame();
	 
	virtual void LoadTexture(const wchar_t* filename, TextureBase *pTexture);
	virtual void ReleaseTexture(TextureBase *pTexture);
	virtual SIZE GetTextureSize(TextureBase *pTexture);

	virtual void SetDefaultShader();
	virtual void SetShadowShader();

	virtual void RenderTexture(TextureBase *pTexture,
		float x, float y,
		RECT &rc,
		int offsetX, int offsetY,
		bool bFlip, bool bFlipV,
		int nColor,
		float fScaleX, float fScaleY,
		float fZ,
		float fAngle,
		int nRotationAxis
	);

	virtual void TextFont(int x, int y, NaString str);

	std::map<TextureBase*, SIZE> m_mapTextureSize;
#ifdef WIN32
	HDC m_hDC;
#else
    void* m_pContext;
#endif

	// OpenGL Specified Members
	void InitShader();

	GLuint m_nShadowProgram;
	PFGLUSEPROGRAM m_pfglUseProgram;

	bool m_bSingleBuffer;
};


#include "DirectXManager.h"

#ifdef WIN32

#include "Game.h"
#include "SpriteManager.h"

#include <d3dx9math.h>

#include "NaException.h"

DirectXManager::DirectXManager(Game *pGame) :
	RenderManager(pGame)
{
	m_pDX = nullptr;
}

DirectXManager::~DirectXManager()
{
}

void DirectXManager::Init()
{
	NaDebugOut(L"Init DirectX...\n");

	// #TODO change to API
	if (m_pDX)
		return;
	m_pDX = Direct3DCreate9(D3D_SDK_VERSION);

	// Setup Device Param
	D3DPRESENT_PARAMETERS d3dpp = { 0, };
	if (m_pGame->m_bIsDesignGame)
		d3dpp.BackBufferWidth = m_pGame->Width() * 5;
	else
		d3dpp.BackBufferWidth = m_pGame->Width();
	d3dpp.BackBufferHeight = m_pGame->Height();
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	//d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.EnableAutoDepthStencil = FALSE;

	DWORD total;
	D3DDISPLAYMODE d3ddm;
	m_pDX->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (SUCCEEDED(
		m_pDX->CheckDeviceMultiSampleType(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			d3ddm.Format,
			FALSE,
			D3DMULTISAMPLE_2_SAMPLES,
			&total)))
	{
		d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		d3dpp.MultiSampleQuality = total - 1;
	}

	// Create Device
	m_pDevice = NULL;
	HRESULT result = m_pDX->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_pGame->m_hWindow,
		//D3DCREATE_HARDWARE_VERTEXPROCESSING,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_pDevice);

	if (result < 0)
	{
		throw NaException(
			__FUNCTIONW__, __LINE__, 
			L"DirectX CreateDevice fail. ErrorCode: %d (0x%08x)\n", 
			result, result);
	}

	// Init Sprite
	D3DXCreateSprite(m_pDevice, &m_pSprite);

	// Init Font
	result = D3DXCreateFont(
		m_pDevice,
		8, 8,
		0, 0,
		FALSE,
		HANGEUL_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH,
		L"Small Fonts",
		&m_pFont);
	
	if (result < 0)
	{
		throw NaException(
			__FUNCTIONW__, __LINE__,
			L"DirectX CreateFont fail. ErrorCode: %d (0x%08x)\n",
			result, result);
	}

	/*
	int w = m_pGame->Width();
	int h = m_pGame->Height();

	D3DXVECTOR3 vPosition = D3DXVECTOR3(w / 2, h / 2, h / 2);
	D3DXVECTOR3 vLookAt = D3DXVECTOR3(w / 2, h / 2, 0.0f);
	D3DXVECTOR3 vUp = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	double fov = D3DXToRadian(90);    // the horizontal field of view
	double aspectRatio = (FLOAT)w / (FLOAT)w; // aspect ratio
	float zNear = 1.0f;
	float zFar = h / 2 + 10;

	D3DXMATRIX m;
	D3DXMatrixLookAtLH(&m,
		&vPosition,
		&vLookAt,
		&vUp);

	D3DXMatrixPerspectiveFovLH(&m,
		fov,
		aspectRatio,
		zNear,
		zFar);
		*/
}

void DirectXManager::Release()
{
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pFont);
	SAFE_RELEASE(m_pSprite);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pDX);
}

void DirectXManager::OnSize(int nWidth, int nHeight)
{
	if (m_pDevice == nullptr)
		return;

	if (m_pGame->m_bIsDesignGame)
	{
		float fRatio = (float)(nWidth) / (float)(nHeight);
		m_pGame->m_nWidth = GameDefaults::nPageWidth * fRatio;
	}
}

void DirectXManager::BeginRenderFrame(COLORREF bgColor)
{
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, bgColor, 0.0f, 0);
	m_pDevice->BeginScene();
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
}

void DirectXManager::EndRenderFrame()
{
	m_pSprite->End();
	m_pDevice->EndScene();

	if (m_pGame->m_bIsDesignGame)
	{
		RECT rcDst;
		::GetClientRect(m_pGame->m_hWindow, &rcDst);
		float fRatio = (float)(rcDst.right - rcDst.left) / (float)(rcDst.bottom - rcDst.top);

		RECT rcSrc = { 0, 0, m_pGame->m_nHeight * fRatio, m_pGame->m_nHeight };

		m_pDevice->Present(&rcSrc, NULL, m_pGame->m_hWindow, NULL);
	}
	else
	{
		m_pDevice->Present(NULL, NULL, m_pGame->m_hWindow, NULL);
	}
}

void DirectXManager::LoadTexture(const wchar_t * szFilename, TextureBase *pTexture)
{
	if (m_pDevice == nullptr)
	{
		// #TODO Error
		return;
	}

	LPDIRECT3DTEXTURE9 pDXTexture = nullptr;

	D3DXCreateTextureFromFileEx(
		m_pDevice, // device
		szFilename, // file
		D3DX_FROM_FILE, // width
		D3DX_FROM_FILE, // height
		1, // mip levels
		0, // usage
		D3DFMT_A8R8G8B8, // format
		D3DPOOL_MANAGED, // pool
		D3DX_FILTER_NONE, //D3DX_DEFAULT, // filter
		D3DX_DEFAULT, // mip filter
		SpriteDefaults::dwColorKey, // colorkey
		NULL, // src info
		NULL, // palette
		&pDXTexture // texture
	);

	pTexture->m_pDXTexture = pDXTexture;
}

void DirectXManager::ReleaseTexture(TextureBase * pTexture)
{
	LPDIRECT3DTEXTURE9 pDXTexture = (LPDIRECT3DTEXTURE9)pTexture->m_pDXTexture;
	SAFE_RELEASE(pDXTexture);
}

SIZE DirectXManager::GetTextureSize(TextureBase * pTexture)
{
	LPDIRECT3DTEXTURE9 pDXTexture = (LPDIRECT3DTEXTURE9)pTexture->m_pDXTexture;
	
	D3DSURFACE_DESC desc;
	pDXTexture->GetLevelDesc(0, &desc);

	SIZE s = { desc.Width, desc.Height };
	return s;
}

void DirectXManager::RenderTexture(TextureBase *pTexture, float x, float y, RECT & rc, int offsetX, int offsetY, bool bFlip, bool bFlipV, int nColor, float fScaleX, float fScaleY, float fZ, float fAngle, int nRotationAxis)
{
	LPDIRECT3DTEXTURE9 pDXTexture = (LPDIRECT3DTEXTURE9)pTexture->m_pDXTexture;
	float fX = (int)x;
	float fY = (int)y;
	POINT ptOffset = { offsetX, offsetY };

	D3DXMATRIX mat;
	float fFlip = bFlip ? -1 : 1;
	float fFlipV = bFlipV ? -1 : 1;
	bool bTransform = false;

	// Replaced positioning function to matrix function
	int nTexWidth = rc.right - rc.left;
	int nTexHeight = rc.bottom - rc.top;
	int nCenterX = nTexWidth / 2;
	int nCenterY = nTexHeight / 2;
	D3DXMATRIX m[10];
	
	if (fAngle != 0.0f)
	{
		D3DXMATRIX mr[3];
		if (nRotationAxis == 0)
		{
			D3DXMatrixTranslation(&mr[0], -nCenterX, -nCenterY, 0);
			D3DXMatrixRotationZ(&mr[1], fAngle * fToRadian);
			D3DXMatrixTranslation(&mr[2], nCenterX, nCenterY, 0);
		}
		else
		{
			D3DXMatrixTranslation(&mr[0], -nCenterX, -nCenterY * 2, 0);
			D3DXMatrixRotationZ(&mr[1], fAngle * fToRadian);
			D3DXMatrixTranslation(&mr[2], nCenterX, nCenterY * 2, 0);
		}

		m[0] = mr[0] * mr[1] * mr[2];
	}
	else
	{
		D3DXMatrixRotationZ(&m[0], 0.0f);
	}

	// Based on 0,0 dimension...
	if (bFlip || bFlipV || fScaleX != 1.0f || fScaleY != 1.0f)
	{
		D3DXMATRIX mo[3];
		D3DXMatrixTranslation(&mo[0], -ptOffset.x, -ptOffset.y, 0);
		D3DXMatrixScaling(&mo[1], 
			fScaleX * fFlip,
			fScaleY * fFlipV,
			0.0f);

		D3DXMatrixTranslation(&mo[2], ptOffset.x, ptOffset.y, 0);
		m[1] = mo[0] * mo[1] * mo[2];

		if (bFlipV && offsetY == nTexHeight)
		{
			// Bottom aligned sprite
			ptOffset.y *= (1 + fScaleY);
		}
	}
	else
	{
		D3DXMatrixScaling(&m[1], 1, 1, 1);
	}
	D3DXMatrixTranslation(&m[9], fX - ptOffset.x, fY - ptOffset.y, 0);

	mat = m[0] * m[1] * m[9];
	m_pSprite->SetTransform(&mat);

	m_pSprite->Draw(
		pDXTexture,
		&rc,
		&D3DXVECTOR3(0, 0, 0),// (int)ptOffset.x, (int)ptOffset.y, 0),
		&D3DXVECTOR3(0, 0, 0),
		nColor // color filter
	);

	if (bTransform)
	{
		D3DXMatrixScaling(&mat, 1.0f, 1.0f, 1.0f);
		m_pSprite->SetTransform(&mat);
	}
}

void DirectXManager::TextFont(int x, int y, NaString str)
{
	m_pFont->DrawText(
		m_pSprite, 
		str.wstr(), 
		str.GetLength(), 
		NaRect(x, y, 9999, 9999), 
		DT_LEFT, 
		0xffffffff);
}
#endif
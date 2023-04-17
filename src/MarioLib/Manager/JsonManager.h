#pragma once

#include "NaString.h"
#include "NaRect.h"

#include "PropertyMap.h"

#include "json.h"

class JsonManager
{
public:	
	static Json::Value* LoadFile(const char* lpszPathName, bool bDecryptIfEncrypted = false);
	static void SaveFile(const char* lpszPathName, Json::Value &j);
	static void SaveFile(const char* lpszPathName, PropertyObjectBase * pObj, Json::Value &j);
	
	static void SaveToEncryptFile(const char* lpszPathName, Json::Value &j); 
	static void SaveToEncryptFile(const char* lpszPathName, PropertyObjectBase * pObj, Json::Value &j);

	// Json to Object
	static bool LoadJsonObjectProperty(PropertyObjectBase* pObj, Json::Value &jObj);

	// Object to Json
	static bool SaveJsonObjectProperty(PropertyObjectBase* pObj, Json::Value &jObj);
};
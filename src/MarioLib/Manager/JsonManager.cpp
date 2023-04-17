#include "JsonManager.h"

#include "NaPlatformAPI.h"
#include "aes/md5.h"
#include "Common/Common.h"

Json::Value* JsonManager::LoadFile(const char * lpszPathName, bool bDecryptIfEncrypted)
{
	FILE *fp;
	NaPlatform::FileOpen(&fp, lpszPathName, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		long lFileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *szBuf = new char[lFileLen + 1];
		fread(szBuf, sizeof(char), lFileLen, fp);
		szBuf[lFileLen] = '\0';

		Json::Value *j = new Json::Value;
		Json::Reader jReader;
		bool bParseJson = jReader.parse(szBuf, *j);
		if (bParseJson)
		{
			fclose(fp);
			delete[] szBuf;
			return j;
		}
		else if (bDecryptIfEncrypted)
		{
			long lDataLen = lFileLen;
			char *szHeader = nullptr;
			for (int i=0; i<lFileLen; i++)
			{
				if (szBuf[i] == '|')
				{
					szHeader = new char[i + 1];
					memcpy(szHeader, szBuf, i);
					szHeader[i] = '\0';
					break;
				}
			}

			while (szHeader != nullptr)
			{
				// Load Header
				NaString strHeader = szHeader;
				NaString strHeaderDesc = SZ_JSON_ENCRYPT_HEADER;
				strHeaderDesc += L" v";

				long lHeaderLen = strHeader.GetLength();

				int nIdx = strHeader.Find(const_cast<wchar_t*>(strHeaderDesc.wstr()));
				if (nIdx < 0)
					break;
				lDataLen -= strHeader.GetLength();

				// Version
				strHeader = strHeader.Mid(strHeaderDesc.GetLength());

				// Load Hash
				char szMD5[MD5_STR_LEN + 1];
				fseek(fp, -(MD5_STR_LEN), SEEK_END);
				//NaDebugOut(L"CurPos: %d\n", ftell(fp));
				fread(szMD5, sizeof(char), MD5_STR_LEN, fp);
				szMD5[MD5_STR_LEN] = '\0';

				lDataLen -= MD5_STR_LEN;

				// Load Data
				char *szData = new char[lDataLen + 1];
				fseek(fp, lHeaderLen, SEEK_SET);
				fread(szData, sizeof(char), lDataLen, fp);

				// Check Valid Hash ..
				char szFileMD5[MD5_STR_LEN + 1];
				Compute_string_md5((unsigned char*)szData, lDataLen, szFileMD5);
				
				int nHashCheckResult = strcmp(szMD5, szFileMD5);

				for (int i = 0; i < lDataLen; i++)
				{
					szData[i] = szData[i] ^ GameDefaults::nJsonEncryptKey[i % 8];
				}

				bParseJson = jReader.parse(szData, *j);
				delete[] szData;
				delete[] szBuf;
				
				break;
			}

			delete[] szHeader;
			fclose(fp);

			if (bParseJson)
				return j;

			return nullptr;
		}
	}

	return nullptr;
}

void JsonManager::SaveFile(const char * lpszPathName, Json::Value & j)
{
	NaDebugOut(L"Save Json: %ls\n", NaString(lpszPathName).wstr());

	FILE *fp;
	NaPlatform::FileOpen(&fp, lpszPathName, "wb");
	if (fp)
	{
		std::string stdstr = j.toStyledString();
		fwrite(stdstr.c_str(), stdstr.length(), 1, fp);

		fclose(fp);
	}
}

void JsonManager::SaveFile(const char * lpszPathName, PropertyObjectBase * pObj, Json::Value & j)
{
	SaveJsonObjectProperty(pObj, j);
	SaveFile(lpszPathName, j);
}

void JsonManager::SaveToEncryptFile(const char * lpszPathName, Json::Value & j)
{
	NaDebugOut(L"Save Encrypted-Json: %ls\n", NaString(lpszPathName).wstr());

	FILE *fp;
	NaPlatform::FileOpen(&fp, lpszPathName, "wb");
	if (fp)
	{
		// Write encrypt if downloaded
		NaDebugOut(L"Encrypting...\n");

		// Write Header
		std::string strHeader = SZ_JSON_ENCRYPT_HEADER;
		strHeader += " v0.2|";

		fwrite(strHeader.c_str(), strHeader.length(), 1, fp);
		NaDebugOut(L"-> Header: %ls\n", NaString(strHeader.c_str()).wstr());

		// Write Data
		std::string strData = j.toStyledString();

		int sizeBuf = strData.length();
		char *buf = new char[sizeBuf];
		memcpy(buf, strData.c_str(), sizeof(char) * sizeBuf);
		for (int i = 0; i < sizeBuf; i++)
		{
			buf[i] = buf[i] ^ GameDefaults::nJsonEncryptKey[i % 8];
		}

		fwrite(buf, sizeBuf, 1, fp);
		NaDebugOut(L"-> DataSize: %d\n", sizeBuf);

		// Write Hash
		char szMD5[MD5_STR_LEN + 1];
		Compute_string_md5((unsigned char*)strData.c_str(), strData.length(), szMD5);
		fwrite(szMD5, MD5_STR_LEN, 1, fp);
		NaDebugOut(L"-> Hash: %s\n", NaString(szMD5).wstr());

		fclose(fp);
	}
}

void JsonManager::SaveToEncryptFile(const char * lpszPathName, PropertyObjectBase * pObj, Json::Value & j)
{
	SaveJsonObjectProperty(pObj, j);
	SaveToEncryptFile(lpszPathName, j);
}

bool JsonManager::LoadJsonObjectProperty(PropertyObjectBase * pObj, Json::Value & jObj)
{
	PropertyMap *pMap = pObj->GetPropertyMap();
	std::map<NaString, PropertyInfo>::iterator it2 = pMap->begin();
	for (; it2 != pMap->end(); ++it2)
	{
		PropertyInfo info = it2->second;
		void* pRefValue = pObj->GetRefPropertyValue(info.strPropName);

		Json::Value jPropVal = jObj.get(info.strPropName.cstr(), Json::nullValue);
		if (jPropVal.isNull())
			continue;

		switch (info.vt)
		{
		case VT_BOOL:
			*(bool*)pRefValue = jPropVal.asBool();
			break;
		case VT_R4: // float
			*(float*)pRefValue = jPropVal.asFloat();
			break;
		case VT_I4:
		case VT_ETC_COLOR:
			*(int*)pRefValue = (int)jPropVal.asInt();
			break;
		case VT_ETC_NASTR:
		case VT_ETC_NALIST:
        case VT_ETC_CUSTOM:
			{
				NaString *pRefStr = (NaString*)pRefValue;
				pRefStr->Format("%s", jPropVal.asString().c_str());
			}
			break;
		}
	}
	return true;
}

bool JsonManager::SaveJsonObjectProperty(PropertyObjectBase * pObj, Json::Value & jObj)
{
	PropertyMap *pMap = pObj->GetPropertyMap();
	std::map<NaString, PropertyInfo>::iterator it2 = pMap->begin();
	for (; it2 != pMap->end(); ++it2)
	{
		PropertyInfo info = it2->second;
		void* pRefValue = pObj->GetRefPropertyValue(info.strPropName);
		if (pRefValue == nullptr)
		{
			if (pObj->m_mapPropertyValue.size() == 0)
			{
				NaDebugOut(L"Error: Object property map isn't initialized!!!\n");
				DebugBreak();
			}

			NaDebugOut(L"Error: Object property reference is null!!! (%ls)\n", info.strPropName.wstr());
			continue;
		}

		switch (info.vt)
		{
		case VT_BOOL:
			jObj[info.strPropName.cstr()] = *(bool*)pRefValue;
			break;
		case VT_R4: // float
			jObj[info.strPropName.cstr()] = *(float*)pRefValue;
			break;
		case VT_I4:
		case VT_ETC_COLOR:
			jObj[info.strPropName.cstr()] = (int)*(long*)pRefValue;
			break;
		case VT_ETC_NASTR:
		case VT_ETC_NALIST:
        case VT_ETC_CUSTOM:
			jObj[info.strPropName.cstr()] = ((NaString*)pRefValue)->cstr();
			break;
		}
	}
	return true;
}

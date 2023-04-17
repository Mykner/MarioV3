// MakeUpdateInfo
// 2017.05.24 neoarc
//

#include "stdafx.h"

#include "json.h"
#include "NaPlatformAPI.h"
#include "NaString.h"
#include "aes/md5.h"
#include "JsonManager.h"

#include <Windows.h>
#pragma comment(lib,"version.lib")

#include "Updater.h"
#include <iostream>

int main()
{
	// Create SecureKey #2
	std::string secureKey;
	std::cout << "Type SecureKey #2: ";

	do 
	{
		std::cin >> secureKey;
		if (secureKey.size() > 0)
			break;
	} while (true);

	NaString strSecureKey(secureKey.c_str());

	Updater u(nullptr);
	NaString strCurrentDir = L"";
	u.MakeUpdateInfo(strCurrentDir, strSecureKey);

    return 0;
}

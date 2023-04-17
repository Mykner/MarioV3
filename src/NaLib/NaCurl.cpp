#include "NaCurl.h"

NaCurl::NaCurl()
{
	curl_ios<ostringstream> writer(m_ostrOutput);

	// Pass the writer to the easy constructor and watch the content returned in that variable!
	m_pCurlEasy = new curl_easy(writer);
}

NaCurl::~NaCurl()
{
	if (m_pCurlEasy)
	{
		delete m_pCurlEasy;
		m_pCurlEasy = nullptr;
	}
}

NaString NaCurl::Post(NaString strUrl, NaString strBody)
{
	NaDebugOut(L"URL: %ls\n", strUrl.wstr());

	// Add some option to the easy handle
	m_pCurlEasy->add<CURLOPT_URL>(strUrl.cstr());
	m_pCurlEasy->add<CURLOPT_FOLLOWLOCATION>(1L);
	m_pCurlEasy->add<CURLOPT_SSL_VERIFYPEER>(0);

	if (strBody.GetLength() > 0)
	{
		m_pCurlEasy->add<CURLOPT_POSTFIELDS>(strBody.cstr());
	}

	try
	{
		// Execute the request.
		m_pCurlEasy->perform();
	}
	catch (curl_easy_exception error)
	{
		// If you want to get the entire error stack we can do:
		curlcpp_traceback errors = error.get_traceback();
		// Otherwise we could print the stack like this:
		error.print_traceback();
	}

	// Let's print the stream content.
	const std::string str = m_ostrOutput.str();
	const char* cstr = str.c_str();

	NaString strRet = cstr;
	if (strRet.GetLength() > 80)
		NaDebugOut(L"RET: %ls ...(skip)\n", strRet.Left(80).wstr());
	else
		NaDebugOut(L"RET: %ls\n", strRet.wstr());

	return strRet;
}

NaString NaCurl::Put(NaString strUrl, NaString strBody)
{
	if (strBody.GetLength() == 0)
		return L"";

	NaDebugOut(L"URL: %ls\n", strUrl.wstr());

	// Add some option to the easy handle
	m_pCurlEasy->add<CURLOPT_URL>(strUrl.cstr());
	m_pCurlEasy->add<CURLOPT_FOLLOWLOCATION>(1L);
	m_pCurlEasy->add<CURLOPT_SSL_VERIFYPEER>(0);
	m_pCurlEasy->add<CURLOPT_CUSTOMREQUEST>("PUT");

	m_pCurlEasy->add<CURLOPT_POSTFIELDS>(strBody.cstr());

	try
	{
		// Execute the request.
		m_pCurlEasy->perform();
	}
	catch (curl_easy_exception error)
	{
		// If you want to get the entire error stack we can do:
		curlcpp_traceback errors = error.get_traceback();
		// Otherwise we could print the stack like this:
		error.print_traceback();
	}

	// Let's print the stream content.
	const std::string str = m_ostrOutput.str();
	const char* cstr = str.c_str();

	NaString strRet = cstr;
	NaDebugOut(L"RET: %ls\n", strRet.wstr());

	return strRet;
}

bool NaCurl::Get(NaString strUrl, char *outBuf, long lSize)
{
	NaDebugOut(L"URL: %ls\n", strUrl.wstr());

	// Add some option to the easy handle
	m_pCurlEasy->add<CURLOPT_URL>(strUrl.cstr());
	m_pCurlEasy->add<CURLOPT_FOLLOWLOCATION>(1L);
	m_pCurlEasy->add<CURLOPT_NOSIGNAL>(1L);
	m_pCurlEasy->add<CURLOPT_ACCEPT_ENCODING>("deflate");
	
	try
	{
		// Execute the request.
		m_pCurlEasy->perform();
	}
	catch (curl_easy_exception error)
	{
		// If you want to get the entire error stack we can do:
		curlcpp_traceback errors = error.get_traceback();
		// Otherwise we could print the stack like this:
		error.print_traceback();

		return false;
	}

	// Let's print the stream content.
	const std::string str = m_ostrOutput.str();
	const char* cstr = str.c_str();
	if (str.size() != lSize)
	{
		return false;
	}
	else
	{
		memcpy(outBuf, cstr, lSize);
	}

	return true;
}

size_t NaCurl::write_data(void *ptr, size_t size, size_t nmemb, void *stream) 
{
	std::string data((const char*)ptr, (size_t)size * nmemb);
	*((std::stringstream*)stream) << data << endl;

	return size * nmemb;
}
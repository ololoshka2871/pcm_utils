
#include <list>
#include <sstream>
#include <iterator>

#include <windows.h>
#include <dshow.h>

static char *dup_wchar_to_utf8(wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char*)malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	return s;
}

static HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
	// Create the System Device Enumerator.
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the category.
		hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
		}
		pDevEnum->Release();
	}
	return hr;
}

static void DisplayDeviceInformation(IEnumMoniker *pEnum, std::list<std::string> &s)
{
	IMoniker *pMoniker = NULL;

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
		if (FAILED(hr)) {
			pMoniker->Release();
			continue;
		}

		VARIANT var;
		VariantInit(&var);

		LPOLESTR olestr = NULL;
		LPMALLOC co_malloc = NULL;

		std::ostringstream ss;
		bool ok = true;

		char *unique_name = NULL;

		IBindCtx *bind_ctx = NULL;
		hr = CreateBindCtx(0, &bind_ctx);
		if (FAILED(hr)) {
			goto _end_turn;
		}

		hr = CoGetMalloc(1, &co_malloc);
		if (FAILED(hr)) {
			goto _end_turn;
		}

		// Get description or friendly name.
		hr = pPropBag->Read(L"Description", &var, 0);
		if (FAILED(hr))	{
			hr = pPropBag->Read(L"FriendlyName", &var, 0);
		} if (SUCCEEDED(hr)) {
			for (auto p = var.bstrVal; *p != 0; ++p) {
				ss << (char)*p;
			}
			VariantClear(&var);
		} else {
			goto _end_turn;
		}
		
		ss << "=";
		hr = pMoniker->GetDisplayName(bind_ctx, NULL, &olestr);
		if (SUCCEEDED(hr)) {
			unique_name = dup_wchar_to_utf8(olestr);

			for (auto i = 0; i < strlen(unique_name); i++) {
				if (unique_name[i] == ':')
					unique_name[i] = '_';
			}
			ss << unique_name;

			free(unique_name);
		} else {
			goto _end_turn;
		}

		s.emplace_back(ss.str());
		
	_end_turn:
		if (olestr && co_malloc) {
			co_malloc->Free(olestr);
		}
		bind_ctx->Release();
		pPropBag->Release();
		pMoniker->Release();
	}
}

static void format_result(std::list<std::string>& r, char* res, int32_t *pSize) {
	const char* const delim = "|";

	std::ostringstream imploded;
	std::copy(r.cbegin(), r.cend(),
		std::ostream_iterator<std::string>(imploded, delim));

	auto string = imploded.str();

	if (res != nullptr && *pSize >= string.length()) {
		std::copy(string.cbegin(), string.cend(), res);
	}
	*pSize = string.length();
}

extern "C" __declspec(dllexport) void enumerate_devices(char* protocol, char *result, int32_t *pSize) {
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		IEnumMoniker *pEnum;

		hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);
		if (SUCCEEDED(hr))
		{
			std::list<std::string> res;
			DisplayDeviceInformation(pEnum, res);
			pEnum->Release();

			format_result(res, result, pSize);
		}

		CoUninitialize();
	}
}
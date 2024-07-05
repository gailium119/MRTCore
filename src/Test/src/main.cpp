#include <MrmResourceIndexer.h>
int main() { 
	MrmResourceIndexerHandle indexer {};
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    HRESULT hr = MrmCreateResourceIndexer(
        L"Application", L"Application", MrmPlatformVersion_Windows10_0_0_0, L"" ,&indexer);
    hr = MrmIndexFile(indexer, L"ms-resource://Application/Files/sample-image.png", L"sample-image.png", L"");
    hr = MrmIndexString(indexer, L"ms-resource://Application/1/test", L"NeutralResource", L"");
    hr = MrmIndexString(indexer, L"ms-resource://Application/1/test", L"≤‚ ‘÷–Œƒ", L"language-zh-CN_contrast-standard_scale-100");
    hr = MrmIndexString(indexer, L"ms-resource://Application/1/test", L"TestResource", L"language-en-US_contrast-standard_scale-100");
    hr = MrmIndexString(indexer, L"ms-resource://Application/1/test2", L"TestResource2", L"language-en-US_contrast-standard_scale-100");
    BYTE data[10] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0x10};
    hr = MrmIndexEmbeddedData(indexer, L"ms-resource://Application/1/test3", data, 10, L"language-en-US_contrast-standard_scale-100");
    hr = MrmCreateResourceFile(
        indexer, MrmPackagingModeStandaloneFile, MrmPackagingOptionsNone, L"D:\\projects\\Git\\MRTCore\\src\\Test\\src\\Temp");
    CoUninitialize();
    return 0;
}
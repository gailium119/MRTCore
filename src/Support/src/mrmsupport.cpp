// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>

#include "mrm/BaseInternal.h"
#include "mrm/common/platform.h"
#include "mrm/readers/MrmReaders.h"
#include "mrm/platform/WindowsCore.h"
#include "mrm/readers/MrmManagers.h"
#include "mrm/build/MrmBuilders.h"

#include <memory>

#include "MrmResourceIndexer.h"
using namespace Microsoft::Resources::Build;


void ParseQualifierString(LPCWSTR qualifierString, MrmBcQualifier** qualifiers, int* numQualifiers, LPWSTR qualifierBuffer)
{
    // Initialize variables
    const WCHAR delimiters[] = L"_"; // Delimiters to split the string
    WCHAR* nextToken = NULL;
    wcscpy(qualifierBuffer, qualifierString);
    WCHAR* token = wcstok_s(qualifierBuffer, delimiters, &nextToken);
    int count = 0;

    // Allocate memory for MrmBcQualifier array
    MrmBcQualifier* qualArray = *qualifiers;
    if (!qualArray)
    {
        // Handle memory allocation failure
        *qualifiers = NULL;
        *numQualifiers = 0;
        return;
    }

    // Process tokens
    while (token != NULL)
    {
        // Split token into QualifierName and QualifierValue
        WCHAR* separator = wcschr(token, L'-');
        if (separator != NULL)
        {
            *separator = L'\0'; // Null-terminate to separate strings
            qualArray[count].QualifierName = token;
            qualArray[count].QualifierValue = separator + 1;
            qualArray[count].FallbackScore = 0.0;
        }

        // Prepare for the next token
        token = wcstok_s(NULL, delimiters, &nextToken);

        // Increase count
        count++;

    }

    // Assign values to output parameters
    *numQualifiers = count;
}
/* Definitions of the functions */

STDAPI MrmCreateResourceIndexer(
    _In_opt_ PCWSTR packageFamilyName,
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{

    Microsoft::Resources::CoreProfile * profile;
    Microsoft::Resources::CoreProfile::ChooseDefaultProfile(&profile);
    Microsoft::Resources::Build::PriFileBuilder* result;
    PCWSTR pkgName = L"Application";
    if (packageFamilyName)
        pkgName = packageFamilyName;
    Microsoft::Resources::Build::PriFileBuilder::CreateInstance(pkgName, platformVersion, profile, &result);
    Microsoft::Resources::Build::MrmBcQualifier* bcQualifiers = new Microsoft::Resources::Build::MrmBcQualifier[10];
    indexer->handle = result;
    auto priDisp = result->GetDescriptor();
    int qualifierSize;
    WCHAR qualifierbuffer[260];
    ParseQualifierString(defaultQualifiers, &bcQualifiers, &qualifierSize, qualifierbuffer);
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder* qualifierSetBuilder;
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder::CreateInstance(priDisp->GetDecisionInfoBuilder(), &qualifierSetBuilder);
    for (int i = 0; i < qualifierSize; i++)
    {
        bcQualifiers[i].FallbackScore = 1.0;
    }
    qualifierSetBuilder->AddQualifiers(qualifierSize, bcQualifiers, true);
    priDisp->GetDecisionInfoBuilder()->GetOrAddQualifierSet(qualifierSetBuilder);
    delete[] bcQualifiers;
    return S_OK;
}

STDAPI MrmCreateResourceIndexerFromPreviousSchemaFile(
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _In_ PCWSTR schemaFile,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{
    Microsoft::Resources::CoreProfile* profile;
    Microsoft::Resources::CoreProfile::ChooseDefaultProfile(&profile);
    Microsoft::Resources::Build::PriFileBuilder* result;
    PCWSTR pkgName = L"Application";
    Microsoft::Resources::Build::PriFileBuilder::CreateInstance(profile, &result);
    //result->GetDescriptor()->GetSchema().
    indexer->handle = result;
    return S_OK;
}

STDAPI MrmCreateResourceIndexerFromPreviousPriFile(
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _In_ PCWSTR priFile,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{
    return S_OK;
}

STDAPI MrmCreateResourceIndexerFromPreviousSchemaData(
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _In_reads_bytes_(schemaXmlSize) BYTE* schemaXmlData,
    _In_ ULONG schemaXmlSize,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{
    return S_OK;
}

STDAPI MrmCreateResourceIndexerFromPreviousPriData(
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _In_reads_bytes_(priSize) BYTE* priData,
    _In_ ULONG priSize,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{
    return S_OK;
}

STDAPI MrmCreateResourceIndexerWithFlags(
    _In_opt_ PCWSTR packageFamilyName,
    _In_ PCWSTR projectRoot,
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _In_ enum MrmIndexerFlags flags,
    _Inout_ struct MrmResourceIndexerHandle* indexer)
{
    return S_OK;
}

STDAPI MrmIndexString(
    _In_ struct MrmResourceIndexerHandle indexer,
    _In_ PCWSTR resourceUri,
    _In_ PCWSTR resourceString,
    _In_opt_ PCWSTR qualifiers)
{
    auto priFile = static_cast<Microsoft::Resources::Build::PriFileBuilder*>(indexer.handle);
    auto priDisp = priFile->GetDescriptor();
    Microsoft::Resources::Build::MrmBcQualifier* bcQualifiers = new Microsoft::Resources::Build::MrmBcQualifier[10];
    int qualifierSize;
    WCHAR qualifierbuffer[260];
    ParseQualifierString(qualifiers, &bcQualifiers, &qualifierSize, qualifierbuffer);
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder* qualifierSetBuilder;
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder::CreateInstance(priDisp->GetDecisionInfoBuilder(),&qualifierSetBuilder);
    qualifierSetBuilder->AddQualifiers(qualifierSize,bcQualifiers, true);
    priDisp->GetDecisionInfoBuilder()->GetOrAddQualifierSet(qualifierSetBuilder);
    LPCWSTR realUri = resourceUri;
    if (!memcmp(realUri, L"ms-resource://", 28))
    {
        int count = 3;
        while (count)
        {
            if (*realUri++ == L'/')
                count--;
        }
    }
    priDisp->AddCandidateWithString(
        NULL, realUri,
        Microsoft::Resources::MrmEnvironment::ResourceValueType_Utf16String,
        resourceString, qualifierSetBuilder);
    delete[] bcQualifiers;
    return S_OK;
}

STDAPI MrmIndexEmbeddedData(
    _In_ struct MrmResourceIndexerHandle indexer,
    _In_ PCWSTR resourceUri,
    _In_reads_bytes_(embeddedDataSize) const BYTE* embeddedData,
    _In_ ULONG embeddedDataSize,
    _In_opt_ PCWSTR qualifiers)
{
    auto priFile = static_cast<Microsoft::Resources::Build::PriFileBuilder *>(indexer.handle);
    auto priDisp = priFile->GetDescriptor();
    Microsoft::Resources::Build::MrmBcQualifier* bcQualifiers = new Microsoft::Resources::Build::MrmBcQualifier[10];
    int qualifierSize;
    WCHAR qualifierbuffer[260];
    ParseQualifierString(qualifiers, &bcQualifiers, &qualifierSize, qualifierbuffer);
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder* qualifierSetBuilder;
    Microsoft::Resources::Build::DecisionInfoQualifierSetBuilder::CreateInstance(priDisp->GetDecisionInfoBuilder(), &qualifierSetBuilder);
    qualifierSetBuilder->AddQualifiers(qualifierSize, bcQualifiers, true);
    priDisp->GetDecisionInfoBuilder()->GetOrAddQualifierSet(qualifierSetBuilder);
    LPCWSTR realUri = resourceUri;
    if (!memcmp(realUri, L"ms-resource://", 28))
    {
        int count = 3;
        while (count)
        {
            if (*realUri++ == L'/')
                count--;
        }
    }
    priDisp->AddCandidateWithEmbeddedData(
        NULL,
        realUri,
        Microsoft::Resources::MrmEnvironment::ResourceValueType_EmbeddedData,
        embeddedData,
        embeddedDataSize,
        qualifierSetBuilder);
    delete[] bcQualifiers;
    return S_OK;
}

STDAPI MrmIndexFile(_In_ struct MrmResourceIndexerHandle indexer, _In_ PCWSTR resourceUri, _In_ PCWSTR filePath, _In_opt_ PCWSTR qualifiers)
{
    auto priFile = static_cast<Microsoft::Resources::Build::PriFileBuilder*>(indexer.handle);
    auto priDisp = priFile->GetDescriptor();
    Microsoft::Resources::QualifierResult set;
    LPCWSTR realUri = resourceUri;
    if (!memcmp(realUri, L"ms-resource://", 28))
    {
        int count = 3;
        while (count)
        {
            if (*realUri++ == L'/')
                count--;
        }
    }
    priDisp->AddCandidateWithString(NULL, realUri, Microsoft::Resources::MrmEnvironment::ResourceValueType_Utf16Path, filePath, NULL);
    return S_OK;
}

STDAPI MrmIndexFileAutoQualifiers(_In_ struct MrmResourceIndexerHandle indexer, _In_opt_ PCWSTR filePath)
{
    return S_OK;
}

STDAPI MrmIndexResourceContainerAutoQualifiers(_In_ struct MrmResourceIndexerHandle indexer, _In_ PCWSTR containerPath)
{
    return S_OK;
}

STDAPI MrmCreateResourceFile(
    _In_ struct MrmResourceIndexerHandle indexer,
    _In_ enum MrmPackagingMode packagingMode,
    _In_ enum MrmPackagingOptions packagingOptions,
    _In_ PCWSTR outputDirectory)
{
    auto priFile = static_cast<Microsoft::Resources::Build::PriFileBuilder*>(indexer.handle);
    auto priDisp = priFile->GetDescriptor();
    void* buffer;
    UINT32 size;
    priFile->GenerateFileContents(&buffer, &size);
    CreateDirectory(outputDirectory,0);
    WCHAR pathBuffer[MAX_PATH] {};
    StringCchCopy(pathBuffer, MAX_PATH, outputDirectory);
    StringCchCat(pathBuffer, MAX_PATH, L"\\");
    StringCchCat(pathBuffer, MAX_PATH, L"resources.pri");
    HANDLE hFile = CreateFile(pathBuffer, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (!hFile || hFile == INVALID_HANDLE_VALUE)
    {
        return GetLastError();
    }
    DWORD numberOfBytesWritten = 0;
    if (!WriteFile(hFile, buffer, size, &numberOfBytesWritten, NULL) || numberOfBytesWritten != size)
    {
        return GetLastError();
    }
    CloseHandle(hFile);
    return S_OK;
}

STDAPI MrmCreateResourceFileWithChecksum(
    _In_ struct MrmResourceIndexerHandle indexer,
    _In_ enum MrmPackagingMode packagingMode,
    _In_ enum MrmPackagingOptions packagingOptions,
    _In_ ULONG checksum,
    _In_ PCWSTR outputDirectory)
{
    return S_OK;
}

STDAPI MrmCreateResourceFileInMemory(
    _In_ struct MrmResourceIndexerHandle indexer,
    _In_ enum MrmPackagingMode packagingMode,
    _In_ enum MrmPackagingOptions packagingOptions,
    _Outptr_result_bytebuffer_(*outputPriSize) BYTE** outputPriData,
    _Out_ ULONG* outputPriSize)
{
    auto priFile = static_cast<Microsoft::Resources::Build::PriFileBuilder*>(indexer.handle);
    auto priDisp = priFile->GetDescriptor();
    void* buffer;
    UINT32 size;
    priFile->GenerateFileContents(&buffer, &size);
    outputPriData = (BYTE**) & buffer;
    *outputPriSize = size;
    return S_OK;
}

STDAPI MrmPeekResourceIndexerMessages(
    _In_ struct MrmResourceIndexerHandle handle,
    _Out_writes_(*numMsgs) struct MrmResourceIndexerMessage** messages,
    _Out_ ULONG* numMsgs)
{
    return S_OK;
}

STDAPI MrmDestroyIndexerAndMessages(_In_ struct MrmResourceIndexerHandle indexer)
{
    return S_OK;
}

STDAPI MrmFreeMemory(_In_ BYTE* data)
{
    return S_OK;
}

STDAPI MrmDumpPriFile(_In_ PCWSTR indexFileName, _In_opt_ PCWSTR schemaPriFile, _In_ enum MrmDumpType dumpType, _In_ PCWSTR outputXmlFile)
{
    return S_OK;
}

STDAPI MrmDumpPriFileInMemory(
    _In_ PCWSTR indexFileName,
    _In_opt_ PCWSTR schemaPriFile,
    _In_ enum MrmDumpType dumpType,
    _Outptr_result_bytebuffer_(*outputXmlSize) BYTE** outputXmlData,
    _Out_ ULONG* outputXmlSize)
{
    return S_OK;
}

STDAPI MrmDumpPriDataInMemory(
    _In_reads_bytes_(inputPriSize) BYTE* inputPriData,
    _In_ ULONG inputPriSize,
    _In_reads_bytes_opt_(schemaPriSize) BYTE* schemaPriData,
    _In_ ULONG schemaPriSize,
    _In_ enum MrmDumpType dumpType,
    _Outptr_result_bytebuffer_(*outputXmlSize) BYTE** outputXmlData,
    _Out_ ULONG* outputXmlSize)
{
    return S_OK;
}

STDAPI MrmCreateConfig(_In_ enum MrmPlatformVersion platformVersion, _In_opt_ PCWSTR defaultQualifiers, _In_ PCWSTR outputXmlFile)
{
    return S_OK;
}

STDAPI MrmCreateConfigInMemory(
    _In_ enum MrmPlatformVersion platformVersion,
    _In_opt_ PCWSTR defaultQualifiers,
    _Outptr_result_bytebuffer_(*outputXmlSize) BYTE** outputXmlData,
    _Out_ ULONG* outputXmlSize)
{
    return S_OK;
}

STDAPI MrmGetPriFileContentChecksum(_In_ PCWSTR priFile, _Out_ ULONG* checksum)
{
    return S_OK;
}

STDAPI_(VOID) DestroyResourceIndexer(PVOID resourceIndexer) 
{

}
STDAPI_(VOID) DestroyIndexedResults(PWSTR resourceUri, ULONG qualifierCount, PVOID qualifiers) 
{

}
STDAPI CreateResourceIndexer(PCWSTR projectRoot, PCWSTR extensionDllPath, PVOID* ppResourceIndexer) 
{ 
    return S_OK;
}
STDAPI IndexFilePath(
    PVOID resourceIndexer,
    PCWSTR filePath,
    PWSTR* ppResourceUri,
    ULONG* pQualifierCount,
    PVOID * ppQualifiers)
{
    return S_OK;
}
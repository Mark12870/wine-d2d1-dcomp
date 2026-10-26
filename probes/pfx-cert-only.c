#include <windows.h>
#include <wincrypt.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    static BYTE data[65536];
    DWORD size = 0;
    HANDLE file = CreateFileA(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    ReadFile(file, data, sizeof(data), &size, NULL);
    CloseHandle(file);

    CRYPT_DATA_BLOB blob = { size, data };
    HCERTSTORE store = PFXImportCertStore(&blob, L"", 0);

    if (!store)
    {
        printf("refused %lu\n", GetLastError());
        return 0;
    }

    printf(CertEnumCertificatesInStore(store, NULL) ? "imported\n" : "empty\n");
    return 0;
}

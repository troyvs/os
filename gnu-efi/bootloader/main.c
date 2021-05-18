#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct {
	void * baseAddress;
	size_t Size;
	unsigned int width;
	unsigned int height;
	unsigned int pixelsperscanline;
}FrameBuffer;

#define PSF1_magic0 0x36
#define PSF1_magic1 0x04

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;

} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* header;
	void* glyphBuffer;
	
} PSF1_FONT;


FrameBuffer framebuffer;
FrameBuffer* InitializeGOP(){
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

	status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL ,(void**)&gop);
	if EFI_ERROR(status){
		Print(L"Unable to locate GOP\n\r");
		return NULL;
	}
	else{
		Print(L"GOP located\n\r");
	}

	framebuffer.baseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.Size = gop->Mode->FrameBufferSize;
	framebuffer.width = gop->Mode->Info->HorizontalResolution;
	framebuffer.height = gop->Mode->Info->VerticalResolution;
	framebuffer.pixelsperscanline = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
}

EFI_FILE* loadFile(EFI_FILE* dir, CHAR16* Path, EFI_HANDLE handle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* loadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* loadedImange;
	SystemTable->BootServices->HandleProtocol(handle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImange);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* Filesystem;
	SystemTable->BootServices->HandleProtocol(loadedImange->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&Filesystem);

	if (dir == NULL){
		Filesystem->OpenVolume(Filesystem, &dir);
	}

	EFI_STATUS s = dir->Open(dir, &loadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if(s != EFI_SUCCESS){
		return NULL;
	}
	return loadedFile;
}
int memcmp(const void* aptr, const void* bptr, size_t n){
	const unsigned char* a =aptr,*b = bptr;
	for (size_t i = 0;i < n;i++){
		if(a[i] < b[i]) return -1;
		else if(a[i] > b[i]) return 1;
	}
	return 0;
}

PSF1_FONT* loadPSF1Font(EFI_FILE* dir, CHAR16* Path, EFI_HANDLE handle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* font = loadFile(dir,Path,handle,SystemTable);
	if (font == NULL) return NULL;
	PSF1_HEADER* fontHeader;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_HEADER), (void**)&fontHeader);
	UINTN size = sizeof(PSF1_HEADER);
	font->Read(font, &size, fontHeader);
	if(fontHeader->magic[0] != PSF1_magic0 || fontHeader->magic[1] != PSF1_magic1){
		return NULL;
	}
	UINTN glyphBufferSize = fontHeader->charsize*256;
	if (fontHeader->mode == 1){
		glyphBufferSize = fontHeader->charsize*512;
	}
	void* glyphBuffer;{
		font->SetPosition(font,sizeof(PSF1_HEADER));
		SystemTable->BootServices->AllocatePool(EfiLoaderData,glyphBufferSize,(void**)&glyphBuffer);
		font->Read(font, &glyphBufferSize,glyphBuffer);
	}

	PSF1_FONT* finishedfont;
	SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF1_FONT), (void**)&finishedfont);
	finishedfont->header = fontHeader;
	finishedfont->glyphBuffer = glyphBuffer;
	return finishedfont;

}
typedef struct {
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	void* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
} bootInfo;

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

	InitializeLib(ImageHandle, SystemTable);
	EFI_FILE* kernel = loadFile(NULL,L"kernel.elf", ImageHandle, SystemTable);
	if(kernel == NULL){
		Print(L"could not load kernel \n\r");
	}else{
		Print(L"kernel loaded successfully \n\r");
	}

	Elf64_Ehdr header;
	{
		UINTN FileInfoSize;
		EFI_FILE_INFO FileInfo;
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
		SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
		kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

		UINTN size = sizeof(header);
		kernel->Read(kernel, &size, &header);
	}

	if(
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	){
		Print(L"kerel format is bad\r\n");
	}else{
		Print(L"kerel header successfully verified\r\n");
	}

	Elf64_Phdr* phdrs;
	{
		kernel->SetPosition(kernel, header.e_phoff);
		UINTN size = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void **)&phdrs);
		kernel->Read(kernel, &size, phdrs);
	}
	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
	){
		switch (phdr->p_type)
		{
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

				kernel->SetPosition(kernel, phdr->p_offset);
				UINTN size = phdr->p_filesz;
				kernel->Read(kernel,&size, (void*) segment);
				break;
			}
		}	
	}
	Print(L"Kernel loaded\n\r");

	PSF1_FONT* newFont = loadPSF1Font(NULL,L"font.psf",ImageHandle,SystemTable);
	if(newFont == NULL){
		Print(L"font is not valid or is not found\n\r");
	}else{
		Print(L"font found. char size = %d\n\r",newFont->header->charsize);
	}
	FrameBuffer* newbuffer = InitializeGOP();\
	Print(L"base: 0x%x\n\rsize: 0x%x\n\rwidth: %d\n\rheight: %d\n\rPixelsPerScanline: %d\n\r",
		newbuffer->baseAddress,
		newbuffer->Size,
		newbuffer->width,
		newbuffer->height,
		newbuffer->pixelsperscanline);
	//
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize = 0;
	UINTN MapKey = 0;
	UINTN DescriptorSize = 0;
	UINT32 DescriptorVersion = 0;

	SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void**)&Map);
	EFI_STATUS mapStatus = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);

	int attempts = 0;
	while(mapStatus != EFI_SUCCESS) {
		if(attempts++ == 10) {
			Print(L"Failed to determine memory map size, giving up on OS load...");
			return EFI_OUT_OF_RESOURCES;
		}

		// The allocation a couple lines up might have changed the memory map so that it is bigger than what we originally
		// got as the result.  This simple method will increase in increments of descriptorSize until it is big enough
		// or until we start getting silly
		Print(L"%d\n\r",attempts);
		SystemTable->BootServices->FreePool(Map);
		MapSize += DescriptorSize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize, (void **)&Map);
		mapStatus = SystemTable->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	}



	
	int (*kernelStart)(bootInfo*) = ((__attribute__((sysv_abi)) int (*)(bootInfo*)) header.e_entry);
	uint64_t mMapEntries = MapSize / DescriptorSize;
	uint64_t memorySizeBytes = 0;
	for (uint64_t i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)Map + (i * DescriptorSize));
        memorySizeBytes += desc->NumberOfPages*4096;
	}
	Print(L"mmmmm:%d\n\r", memorySizeBytes);
	bootInfo bootinfo;
	bootinfo.framebuffer = newbuffer;
	bootinfo.psf1_Font = newFont;
	bootinfo.mMap = Map;
	bootinfo.mMapSize = MapSize;
	bootinfo.mMapDescriptorSize = DescriptorSize;

	//SystemTable->BootServices->ExitBootServices(ImageHandle,mapkey);

	Print(L"%d\n\r",kernelStart(&bootinfo));
	return EFI_SUCCESS; // Exit the UEFI application
}

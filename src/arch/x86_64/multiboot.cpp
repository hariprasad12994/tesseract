#include "multiboot.h"
#include "video.h"
#include "lib.h"
#include "memory_map.h"

multibootInfo :: multibootInfo(uint32_t ptrToMultibootInfoBlock){
  this->ptrToMultibootInfoBlock = (multibootInfoStruct*)ptrToMultibootInfoBlock;
  multibootInfo :: parseMultiboot();
}

bool multibootInfo :: checkFlagAvailability(multibootInformationFlag flagType){
  return (flagType & (this->ptrToMultibootInfoBlock->flags));
}

void multibootInfo :: parseMultiboot(){
  primitiveVGA vga((uint8_t)Green, (uint8_t)Black);
  vga.print("[] Parsing multiboot structure....\n");
  vga.print("[] Reading boot loader name....\n");
  if(multibootInfo :: checkFlagAvailability(BOOTLOADER_NAME)){
    this->bootloaderName = ((char*)this->ptrToMultibootInfoBlock->boot_loader_name);
    vga.print("[] Bootloader name : %s\n", this->bootloaderName);
  }
  else{
    panic();
  }

  vga.print("[] Reading memory map from multiboot structure....\n");
  if(multibootInfo :: checkFlagAvailability(MEMORY_MAP)){
    this->memoryMap = (multibootMemoryMap*)this->ptrToMultibootInfoBlock->mmap_addr;
    multibootMemoryMap* mmap = (multibootMemoryMap*)this->ptrToMultibootInfoBlock->mmap_addr;
    while((unsigned long)mmap < ((this->ptrToMultibootInfoBlock->mmap_addr) +
     (this->ptrToMultibootInfoBlock->mmap_length))) {
      vga.print("[] Start Address : 0x%x%x  Length : 0x%x%x\n", (mmap->addr >> 32),
      (mmap->addr & 0xffffffff), (mmap->len >> 32), (mmap->len & 0xffffffff));
      mmap = (multibootMemoryMap*) ( (unsigned long)mmap +
                                      mmap->size + sizeof(mmap->size) );
    }
  }
  else{
    panic();
  }

  if(multibootInfo :: checkFlagAvailability(ELF_SECTION_HEADER_TABLE)){
    vga.print("[] Elf section present\n");
    this->elfSectionHeaderTable = &(this->ptrToMultibootInfoBlock->u.elf_sec);
  }
  return;
}

void multibootInfo :: panic(){
  primitiveVGA vga((uint8_t)Red, (uint8_t)Black);
  vga.print("Something went wrong....\n");
  while(1);
}

#include "Types.h"
#include "IO.h"
#include <algorithm>
#include <cassert>
#include <memory>
#include <vector>


#include <stddef.h>
#include <stdio.h>
#include <windows.h>




IOInterface* IO;

IOInterface::IOInterface(FilesInfo* filesInfo)
{
    ODataSize = filesInfo->dataSize;
    BufferSize =buffer_size/4;
    ITypesSizes = new uint32_t[filesInfo->iCounter];
    for (uint32_t i = 0; i < filesInfo->iCounter; i++)
        ITypesSizes[i] = ((((filesInfo->iTypes->Slise.data[i] - GoC::int32arg_T) >> 1) % 2) + 1) * 4;
    OTypeSize = 4;
    open_file(filesInfo->iCounter, filesInfo->argFilename, filesInfo->retFilename);
    llvm::outs() << "\n IOInterface init successfull :\n ---------\n";
}



Buffers_t IOInterface::get_buffer() {
    return Buffers;
}

int32_t IOInterface::open_file(uint32_t counter, std::string* argFilename, std::string retFilename) {
    
    ICounter = counter;
    Buffers.IBuffer_array = new uint8_t * [counter];
    Files.IFile_array = new FILE * [counter];
    for (uint32_t i = 0; i < ICounter; i++) {
        Buffers.IBuffer_array[i] = new uint8_t[ITypesSizes[i]*BufferSize * 2]; //double buffer  this is necessary modify,BufferSize=typesize*BufferSize 
        
        while ((Files.IFile_array[i] = fopen(argFilename[i].c_str(), "rb")) ==NULL) {
            llvm::outs() << "Error: open file \n"<< argFilename[i];
            std::perror("Error:");
            return 1;
        }
        while(BufferSize !=fread(Buffers.IBuffer_array[i], ITypesSizes[i], BufferSize, Files.IFile_array[i]) )
            llvm::outs() << "Error: read file\n";


    }


    Buffers.OBuffer = new uint8_t[OTypeSize *BufferSize * 2];//double buffer this is necessary modify,BufferSize=typesize*BufferSize 
    while ((Files.OFile = fopen(retFilename.c_str(), "wb")) == NULL) {
        llvm::outs() << "Error: open file\n";
        //return 1;
    }

    return 0;
}

int32_t IOInterface::close_file() {
    for (uint32_t i = 0; i < ICounter; i++)
        fclose(Files.IFile_array[i]);
        //fclose(Files.IFile_array[i]);


    if (ODataSize!=0)
        while (ODataSize != fwrite(Buffers.OBuffer, OTypeSize, ODataSize, Files.OFile)) {
            llvm::outs() << " Error 0: "<< ODataSize <<"  Output buffer update\n";
            std::perror("Error:");
        }
    //ODataSize -= BufferSize;

    fclose(Files.OFile);
    return 0;
}



int32_t IOInterface::update_buffer(int32_t arg) {
    int len;
    for (uint32_t i = 0; i < ICounter; i++) {
        len = fread(Buffers.IBuffer_array[i], ITypesSizes[i], BufferSize, Files.IFile_array[i]);
        if (BufferSize != len) {
            //llvm::outs() << "Error: Input buffer " << i << " update\n";
            //llvm::outs() << "Error: Input size: " << BufferSize << " updated size: "<< len <<"\n";
            //std::perror("Error:");
        }
    }


    ODataSize -= BufferSize;
    while (BufferSize != fwrite(Buffers.OBuffer, OTypeSize, BufferSize, Files.OFile)) {
        llvm::outs() << " Error: Output buffer update\n";
        std::perror("Error:");
    }

    return 0;
}



FilesInfo::FilesInfo(uint32_t iCounter_arg)
{

    iCounter = iCounter_arg;
    argFilename= new std::string[iCounter];;
    iTypes=new GoSlice<int64_t>(iCounter);
    
}

FilesInfo::FilesInfo(uint32_t iCounter_arg,uint64_t dataSize_arg)
{
    dataSize= dataSize_arg;
    iCounter = iCounter_arg;
    argFilename = new std::string[iCounter];;
    iTypes = new GoSlice<int64_t>(iCounter);

}


void FilesInfo::setIfileParam(uint32_t index, std::string name , std::string jitType)
{
    GoJitType goJitType;
    const std::string cjitType[4] = { "int","long","float","double" }; //dangerous 
    for(GoJitType i = 0; i < 4; i++) 
        if (jitType == cjitType[i]) goJitType = i * 2 + GoC::int32arg_T;
    
    argFilename[index] = name;
    iTypes->Slise.data[index]= goJitType;
}

void FilesInfo::setOfileParam(std::string name) {
    retFilename = name;
}

FilesInfo::~FilesInfo()
{
}
#ifndef IOc_H
#define IOc_H
#include <stdint.h>
#include "Types.h"
#include "IO.h"

#define budd_div (1)
#define budd_mul (16)
#define buffer_size (1024 * 1024 * budd_mul / budd_div)
#define f_size_m (buffer_size / budd_mul * budd_div * 512)






typedef struct {
    uint8_t** IBuffer_array;
    uint8_t*  OBuffer;
} Buffers_t;

typedef struct {
    FILE* volatile* IFile_array;
    FILE*  OFile;
} Files_t;



class FilesInfo
{
public:
    FilesInfo(uint32_t iCounter_arg);
    FilesInfo(uint32_t iCounter_arg, uint64_t dataSize_arg);
    ~FilesInfo();
    void setIfileParam(uint32_t index, std::string name, std::string jitType);
    void setOfileParam(std::string name);


    uint64_t dataSize;
    std::string  retFilename;
    std::string* argFilename;

    GoSlice<int64_t>* iTypes;
    uint32_t   iCounter;

    bool        FileExprSource;
    std::string ExprORFilename;


private:

};




class IOInterface {
public:
    IOInterface(FilesInfo* FilesInfo);

    Buffers_t get_buffer();
    int32_t close_file();
    int32_t update_buffer(int32_t arg);


    Buffers_t Buffers;
    Files_t   Files;
    uint32_t  ICounter;
    uint32_t  BufferSize;
    uint32_t* ITypesSizes;
    uint32_t OTypeSize;
    uint32_t ODataSize;
private:
    int32_t open_file(uint32_t counter, std::string * argFilename, std::string retFilename);
};

extern IOInterface* IO;
#endif
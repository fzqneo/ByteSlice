/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "sequential_binary_file.h"

#include    <cassert>
#include	<fstream>
#include    <iostream>


namespace byteslice{

bool SequentialReadBinaryFile::Open(const std::string filename){
    if(NULL != file_){
        std::cerr << "Already opened file: " << filename_ << std::endl;
        return false;
    }
    filename_ = filename;
    file_ = fopen(filename_.c_str(), "rb");
    if(NULL == file_){
        std::cerr << "Can't open file: " << filename_ << std::endl;
        return false;
    }
    return true;
}

bool SequentialReadBinaryFile::Close(){
    if(NULL == file_){
        std::cerr << "No file opened." << std::endl;
        return false;
    }
    if(0 != fclose(file_)){
        std::cerr << "Error happens when closing file: " << filename_ << std::endl;
        return false;
    }
    return true;
}

bool SequentialReadBinaryFile::IsEnd(){
    return feof(file_);
}

size_t SequentialReadBinaryFile::Read(void* buf, size_t size) const{
    size_t count = fread(buf, sizeof(char), size, file_);
    return count;
}


bool SequentialWriteBinaryFile::Open(const std::string filename){
    if(NULL != file_){
        std::cerr << "Already opened file: " << filename_ << std::endl;
        return false;
    }
    filename_ = filename;
    file_ = fopen(filename_.c_str(), "wb");
    if(NULL == file_){
        std::cerr << "Can't open file: " << filename_ << std::endl;
        return false;
    }
    return true;
}

bool SequentialWriteBinaryFile::Close(){
    if(NULL == file_){
        std::cerr << "No file opened." << std::endl;
        return false;
    }
    if(0 != fclose(file_)){
        std::cerr << "Error happens when closing file: " << filename_ << std::endl;
        return false;
    }
    return true;
}

size_t SequentialWriteBinaryFile::Append(const void* data, size_t size){
    size_t count = fwrite(data, sizeof(char), size, file_);
    return count;
}

bool SequentialWriteBinaryFile::Flush(){
    if(0 != fflush(file_)){
        return false;
    }
    else{
        return true;
    }
}

}   // namespace

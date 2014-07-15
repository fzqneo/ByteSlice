#include    <iostream>
#include    <unistd.h>
#include    <string>
#include    <cstdlib>
#include    <ctime>
#include    <omp.h>
#include    <map>
#include    <random>
#include    <functional>

#include "src/bitvector.h"
#include "src/column.h"
#include "src/types.h"

#include "hybrid_timer.h"

using namespace byteslice;

std::map<std::string, ColumnType> ctypeMap = {
    {"bs",  ColumnType::kByteSlicePadRight},
    {"na",  ColumnType::kNaive}
};

typedef struct {
    ColumnType  coltype = ColumnType::kByteSlicePadRight;
    size_t      size    = 16*1024*1024;
    size_t      nbits   = 12;
    double      selectivity = 0.1;
    size_t      repeat  = 3;
} arg_t;


void parse_arg(arg_t &arg, int &argc, char** &argv);
void print_arg(const arg_t& arg);
    
int main(int argc, char* argv[]){
    arg_t arg;
    parse_arg(arg, argc, argv);
    
    std::cout << "[INFO ] Creating column ..." << std::endl;
    Column* column = new Column(arg.coltype, arg.nbits, arg.size);
    std::cout << "[INFO ] Creating bit vector ..." << std::endl;
    BitVector* bitvector = new BitVector(column);
    
    std::cout << "[INFO ] Populating column with random values ..." << std::endl;
    auto dice = std::bind(std::uniform_int_distribution<WordUnit>(
                            std::numeric_limits<WordUnit>::min(),
                            std::numeric_limits<WordUnit>::max()),
                            std::default_random_engine(std::time(0)));
    WordUnit mask = (1ULL << arg.nbits) - 1;
    for(size_t i=0; i < arg.size; i++){
        column->SetTuple(i, dice() & mask);
    }
    
    std::cout << "[INFO ] omp_max_threads = " << omp_get_max_threads() << std::endl;
    std::cout << "[INFO ] Executing scan ..." << std::endl;
    HybridTimer t1;
    t1.Start();
    for(size_t r = 0; r < arg.repeat; r++){
        column->Scan(Comparator::kLess,
                    static_cast<WordUnit>(mask*arg.selectivity),
                    bitvector,
                    Bitwise::kSet);
    }
    t1.Stop();
    
    std::cout << "Wall time (sec), CPU cost (cycle/value)" << std::endl;
    std::cout << t1.GetSeconds()/arg.repeat << ", "
                << double(t1.GetNumCycles()/arg.repeat)/arg.size
                << std::endl;
                
    std::cout << "[INFO ] Releasing memory ..." << std::endl;
    delete column;
    delete bitvector;
}


void parse_arg(arg_t &arg, int &argc, char** &argv){
    int c;
    std::string s;
    while((c = getopt(argc, argv, "t:s:b:y:r:h")) != -1){
        switch(c){
            case 'h':
                std::cout << "Usage: " << argv[0]
                << " [-t <column type = na|bs>]"
                << " [-s <size (number of rows)>]"
                << " [-b <bit width>]"
                << " [-y <selectivity>]"
                << " [-r <repeat>]"
                << std::endl;
                exit(0);
            case 't':
                s = std::string(optarg);
                if(ctypeMap.find(s) == ctypeMap.end()){
                    std::cerr << "Unknown column type: " << s << std::endl;
                    exit(1);
                }
                else{
                    arg.coltype = ctypeMap[s];
                }
                break;
            case 's':
                arg.size = atoi(optarg);
                break;
            case 'b':
                arg.nbits = atoi(optarg);
                break;
            case 'y':
                arg.selectivity = atof(optarg);
                break;
            case 'r':
                arg.repeat = atoi(optarg);
                break;
        }
    }
    
    print_arg(arg);   
}

void print_arg(const arg_t& arg){
    std::cout
    << "[INFO ] column type = "  << arg.coltype  << std::endl
    << "[INFO ] table size = "   << arg.size     << std::endl
    << "[INFO ] bit width = "    << arg.nbits    << std::endl
    << "[INFO ] selectivity = "  << arg.selectivity  << std::endl
    << "[INFO ] repeat = "       << arg.repeat   << std::endl;
}

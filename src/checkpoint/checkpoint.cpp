#include "checkpoint.h"
#include "header.h"

using namespace checkpoint;

// binary file assumes size of various types
static_assert(sizeof(int)==4);
static_assert(sizeof(svec::Label)==4);
static_assert(sizeof(svec::Value)==8);
static_assert(sizeof(std::size_t)==8);

void checkpoint::create(const char * filename, const domain::Domain & dom)
{
    // open file
    std::ofstream output(filename, std::ios::out | std::ios::binary | std::ios::trunc);

    // write header
    const Header header = makeHeader();
    output.write(reinterpret_cast<const char*>(&header),sizeof(Header));

    // write domain size
    output.write(reinterpret_cast<const char*>(dom.n),3*sizeof(int));

    // write number of ELA instances
    output.write(reinterpret_cast<const char*>(&dom.nn),sizeof(int));

    // setup checksums
    svec::Label lCheckSum=0;
    svec::Value vCheckSum=0;

    // loop through all ELA instances
    for(auto n=0; n<dom.nn; ++n) {
        // loop through all (non-ghost) cells
        for(const auto& s : dom.s[n]) {
            const std::size_t& nnz = s.NNZ();
            // write number of non-zero elements
            output.write(reinterpret_cast<const char*>(&nnz),sizeof(std::size_t));

            // write label and value of each non-zero element
            for(const auto& elm : s) {
                const svec::Label& l = elm.l;
                output.write(reinterpret_cast<const char*>(&l),sizeof(svec::Label));
                lCheckSum += l;

                const svec::Value& v = elm.v;
                output.write(reinterpret_cast<const char*>(&v),sizeof(svec::Value));
                vCheckSum += v;
            }
        }
    }

    // write checksums
    output.write(reinterpret_cast<const char*>(&lCheckSum),sizeof(svec::Label));
    output.write(reinterpret_cast<const char*>(&vCheckSum),sizeof(svec::Value));

    // close file
    output.close();
}

// load function for version 1 checkpoint
void load_v1(std::ifstream& input, const Header& header, const domain::Domain& dom)
{
    // confirm correct array ordering
    #ifdef F_STYLE
    if(!isFortranBuild(header)) {
    #else
    if( isFortranBuild(header)) {
    #endif
        throw std::invalid_argument(
            "Array ordering mismatch in checkpoint file"
        );
    }

    // confirm same domain size
    int n_in[3];
    input.read(reinterpret_cast<char*>(n_in),3*sizeof(int));
    if(dom.n[0]!=n_in[0] || dom.n[1]!=n_in[1] || dom.n[2]!=n_in[2]) {
        throw std::invalid_argument(
            "Domain size mismatch in checkpoint file"
        );
    }

    // confirm number of ELA instances
    int nn_in;
    input.read(reinterpret_cast<char*>(&nn_in),sizeof(int));
    if(dom.nn!=nn_in) {
        throw std::invalid_argument(
            "ELA instance count mismatch in checkpoint file"
        );
    }

    // setup buffer for reading elements
    std::vector<svec::Element> buff;

    // setup checksums
    svec::Label lCheckSum=0;
    svec::Value vCheckSum=0;

    // loop through all ELA instances
    for(auto n=0; n<dom.nn; ++n) {
        // loop through all (non-ghost) cells
        for(auto& s : dom.s[n]) { 
            // read number of non-zero elements
            std::size_t nnz;
            input.read(reinterpret_cast<char*>(&nnz),sizeof(std::size_t));

            buff.reserve(nnz+1);

            // read value of each non-zero element
            for(std::size_t i=0; i<nnz; i++) {
                svec::Label l;
                input.read(reinterpret_cast<char*>(&l),sizeof(svec::Label));
                lCheckSum += l;

                svec::Value v;
                input.read(reinterpret_cast<char*>(&v),sizeof(svec::Value));
                vCheckSum += v;

                buff[i]=svec::Element{l,v};
            }
            buff[nnz]=svec::END_ELEMENT;

            // create the s vector
            s=svec::SVector(buff.data());
        }
    }

    // read checksums
    svec::Label lCheckSum_in;
    input.read(reinterpret_cast<char*>(&lCheckSum_in),sizeof(svec::Label));
    if(lCheckSum != lCheckSum_in) {
        throw std::invalid_argument(
            "Checksum (label) mismatch in checkpoint file: " +
            std::to_string(lCheckSum) + " vs " + std::to_string(lCheckSum_in)
        );
    }

    svec::Value vCheckSum_in=0;
    input.read(reinterpret_cast<char*>(&vCheckSum_in),sizeof(svec::Value));
    if(vCheckSum != vCheckSum_in) {
        throw std::invalid_argument(
            "Checksum (value) mismatch in checkpoint file: " +
            std::to_string(vCheckSum) + " vs " + std::to_string(vCheckSum_in)
        );
    }
}

void checkpoint::load(const char *filename, const domain::Domain &dom)
{
    // open file
    std::ifstream input(filename, std::ios::in |std::ios::binary);

    // read header
    Header header;
    input.read(reinterpret_cast<char*>(&header),sizeof(Header));

    switch (getVersionNumber(header))
    {
    case 1:
        load_v1(input,header,dom);
        break;
    default:
        input.close();

        throw std::invalid_argument (
            "Unknown checkpoint file version " +
            std::to_string(getVersionNumber(header)) + 
            " in " +
            std::string(filename)
        );
    }

    // close file
    input.close();
}


#include "mpidomain.h"

#include "compression.h"
#include <stdexcept>

using namespace domain;

MPIDomain::MPIDomain(
    const int &ni, const int &nj, const int &nk, const int &nn, MPI_Comm comm_cart_in) :
    Domain(ni,nj,nk,nn),
    comm_cart(comm_cart_in)
{
    // confirm this is a cartesian communicator
    int status;
	MPI_Topo_test(comm_cart, &status);
    if(status != MPI_CART) {
        throw std::invalid_argument("Not a Cartesian communicator");
    }

    // figure out neighbors
    MPI_Cart_shift(comm_cart, 0, 1, &neighbors[iMinus], &neighbors[iPlus]);
	MPI_Cart_shift(comm_cart, 1, 1, &neighbors[jMinus], &neighbors[jPlus]);
	MPI_Cart_shift(comm_cart, 2, 1, &neighbors[kMinus], &neighbors[kPlus]);

    // figure out if this process is the boss
    int rank;
    MPI_Comm_rank(comm_cart, &rank);
    boss=(rank==0);
}


void MPIDomain::updateGhost(const Face& recv)
{
    const Face send = getOppositeFace(recv);

    // Figure out how big each buffer needs to be
    std::size_t* send_len = new std::size_t[nn];
    std::size_t* recv_len = new std::size_t[nn];

    if(hasNeighbor(send)) {
        for(auto n=0; n<nn; ++n) {
            send_len[n]=getCompressedSize(getEdge(send,n));
        }
    }

    MPI_Sendrecv(
        send_len, nn*sizeof(std::size_t), MPI_BYTE, neighbors[send], nn,
        recv_len, nn*sizeof(std::size_t), MPI_BYTE, neighbors[recv], nn,
        comm_cart, MPI_STATUS_IGNORE);

    
    // Receive compressed data
    void** recv_buff = new void*[nn];
    MPI_Request* recv_req = new MPI_Request[nn];

    if(hasNeighbor(recv)) {
        for(auto n=0; n<nn; ++n) {
            // Setup receive buffer
            recv_buff[n] =  malloc(recv_len[n]);

            // Start receiving data
            MPI_Irecv(
                recv_buff[n], recv_len[n], MPI_BYTE, neighbors[recv], n,
                comm_cart, &recv_req[n]);
        }
    }
    else {
        for(auto n=0; n<nn; ++n) {
            recv_req[n]=MPI_REQUEST_NULL;
        }
    }

    // Send compressed data
    void** send_buff = new void*[nn];
    MPI_Request* send_req = new MPI_Request[nn];
    
    if(hasNeighbor(send)) {
        for(auto n=0; n<nn; ++n) {
            // Setup send buffer
            send_buff[n] =  malloc(send_len[n]);

            // Compress the data
            compress(send_buff[n],getEdge(send,n));

            // Start sending the compressed data
            MPI_Isend(
                send_buff[n], send_len[n], MPI_BYTE, neighbors[send], n,
                comm_cart, &send_req[n]);
        }
    }
    else {
        for(auto n=0; n<nn; ++n) {
            send_req[n]=MPI_REQUEST_NULL;
        }
    }

    // Decompress received data
    int index;
    MPI_Waitany(nn, recv_req, &index, MPI_STATUS_IGNORE);
    while(index!=MPI_UNDEFINED) {
        // decompress the data into the ghost cells
        decompress(recv_buff[index], getGhost(recv, index));

        // free up memory
        free(recv_buff[index]);

        // wait for next
        MPI_Waitany(nn, recv_req, &index, MPI_STATUS_IGNORE);
    }
    delete[] recv_buff;
    delete[] recv_req;

    // Cleanup the send buffer
    MPI_Waitany(nn, send_req, &index, MPI_STATUS_IGNORE);
    while(index!=MPI_UNDEFINED) {
        // free up memory
        free(send_buff[index]);

        // wait for next
        MPI_Waitany(nn, send_req, &index, MPI_STATUS_IGNORE);
    }
    delete[] send_buff;
    delete[] send_req;

    delete[] send_len;
    delete[] recv_len;
}

template<>
unsigned int MPIDomain::getMax(const unsigned int &in) const
{
    unsigned int out=in;
    MPI_Allreduce(MPI_IN_PLACE, &out, 1, MPI_UNSIGNED, MPI_MAX,comm_cart);

    return out;
}

template<>
int MPIDomain::getMax(const int &in) const
{
    int out=in;
    MPI_Allreduce(MPI_IN_PLACE, &out, 1, MPI_INT, MPI_MAX,comm_cart);

    return out;
}

template<>
double MPIDomain::getMax(const double &in) const
{
    int out=in;
    MPI_Allreduce(MPI_IN_PLACE, &out, 1, MPI_DOUBLE, MPI_MAX,comm_cart);

    return out;
}

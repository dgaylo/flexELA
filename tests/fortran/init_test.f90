program init_test

#ifdef ELA_USE_MPI
use mpi
#endif

implicit none
integer, parameter :: N(3)=(/10,12,14/)
integer, parameter :: NN=2
integer, parameter :: pad(6)=(/0,0,0,0,0,0/)

integer, dimension(N(1)+pad(1)+pad(2),N(2)+pad(3)+pad(4),N(3)+pad(5)+pad(6)) :: labels
real(8), dimension(N(1)+pad(1)+pad(2),N(2)+pad(3)+pad(4),N(3)+pad(5)+pad(6)) :: vol
integer :: i,j,k,ierr,l

#ifdef ELA_USE_MPI
call MPI_Init(ierr)
if(ierr.ne.MPI_SUCCESS) error stop
#endif

! Initialize ELA
#ifdef ELA_USE_MPI
    call ELA_Init(N,pad,NN,SetupMPI())
#else
    call ELA_Init(N,pad,NN)
#endif

do concurrent(i=1:N(1),j=1:N(2),k=1:N(3))
    labels(i+pad(1),j+pad(2),k+pad(3))=i+j*N(1)+k*N(1)*N(2)
    vol   (i+pad(1),j+pad(2),k+pad(3))=1.0
end do

call ELA_InitLabels(vol,1,labels)

do i=1,N(1)
do j=1,N(2)
do k=1,N(3)
    call ELA_GetLabel(i,j,k,1,l)
    if(l.ne.(i+j*N(1)+k*N(1)*N(2))) error stop 'ELA_GetLabel()'
end do
end do
end do

call ELA_DeInit()

#ifdef ELA_USE_MPI
call MPI_Finalize(ierr)
if(ierr.ne.MPI_SUCCESS) error stop
#endif

contains

#ifdef ELA_USE_MPI
function SetupMPI() result(comm)
    implicit none
    integer :: comm, ierr
    integer, parameter :: ndims=3
    integer, parameter :: dims(3)=(/2,2,1/)
    logical, parameter :: periods(3)=(/.true.,.false.,.true./)

    call MPI_Cart_Create(MPI_COMM_WORLD,ndims,dims,periods,.false.,comm,ierr)
    if(ierr.ne.MPI_SUCCESS) error stop
end function
#endif

end program init_test
module foo
  implicit none
  
contains

  subroutine calc(nx, ny, a, b, c)
    implicit none
    integer,intent(in) :: nx,ny
    real(KIND=4),dimension(:,:),intent(in)  :: a,b
    real(KIND=4),dimension(:,:),intent(out) :: c
    integer :: i,j

    !$acc kernels copyin(a,b) copy(c)
    do j = 1,ny
       do i = 1,nx
          c(i,j) = c(i,j) + a(i,j) + b(i,j)
       end do
    end do
    !$acc end kernels

  end subroutine calc

  subroutine init_cpu(nx, ny, a)
    implicit none
    integer,intent(in) :: nx,ny
    real(KIND=4),dimension(:,:),intent(out)  :: a
    integer :: i,j

    do j = 1, ny
       do i = 1, nx
          a(i,j) = 1.0
       end do
    end do
  end subroutine init_cpu

end module foo


program main
  use foo
  implicit none

  integer :: nt = 1000
  integer :: nx = 4096
  integer :: ny = 4096
  real(KIND=4),allocatable,dimension(:,:) :: a,b,c
  real(KIND=4) :: b0
  real(KIND=8) :: t_s, t_e, sum, omp_get_wtime
  integer :: i,j,icnt
  
  allocate(a(nx,ny),b(nx,ny),c(nx,ny))

  b0 = 2.0

  t_s = omp_get_wtime()

  !**** Begin ****!
    
  call init_cpu(nx, ny, a)

  !$acc kernels copyout(b)
  do j = 1,ny
     do i = 1,nx
        b(i,j) = b0
     end do
  end do
  !$acc end kernels

  !$acc kernels copyout(c)
  c(:,:) = 0.0
  !$acc end kernels

  do icnt = 1,nt
     call calc(nx, ny, a, b, c)
  end do

  sum = 0
  !$acc kernels copyin(c)
  do j = 1,ny
     do i = 1,nx
        sum = sum + c(i,j)
     end do
  end do
  !$acc end kernels

  !**** End ****!
    
  t_e = omp_get_wtime()
  print *, "mean =", sum/(nx*ny)
  print *, "Time =", t_e-t_s

  deallocate(a,b,c)

end program main


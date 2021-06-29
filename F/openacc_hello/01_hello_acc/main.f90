program main
  implicit none
  
  integer,parameter :: n = 1000
  real(KIND=4),allocatable,dimension(:) :: a,b
  real(KIND=4) :: c
  integer :: i
  real(KIND=8) :: sum

  allocate(a(n),b(n))
  c = 2.0

  do i = 1, n
     a(i) = 10.0
  end do
!$acc data copyin(a) copyout(b)
!$acc kernels
!$acc loop independent
  do i = 1, n
     b(i) = a(i) + c
  end do
!$acc end kernels
!$acc end data 
  sum = 0.d0
  do i = 1, n
     sum = sum + b(i)
  end do

  print *, sum/n
  deallocate(a,b)
  
end program main


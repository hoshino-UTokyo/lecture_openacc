program main
  use diffusion
  use misc
  implicit none

  integer,parameter :: nx = 128
  integer,parameter :: ny = nx
  integer,parameter :: nz = nx
  integer,parameter :: nt = 100000
  real(KIND=4),parameter :: lx = 1.0
  real(KIND=4),parameter :: ly = 1.0
  real(KIND=4),parameter :: lz = 1.0
  real(KIND=4),parameter :: dx = lx/real(nx)
  real(KIND=4),parameter :: dy = lx/real(ny)
  real(KIND=4),parameter :: dz = lx/real(nz)
  real(KIND=4),parameter :: kappa = 0.1
  real(KIND=4),parameter :: dt = 0.1*min(min(dx*dx, dy*dy), dz*dz)/kappa
  integer :: icnt
  double precision :: time, flop, elapsed_time, ferr
  real(KIND=4),pointer,dimension(:,:,:) :: f,fn

  time = 0.d0
  flop = 0.d0 
  elapsed_time = 0.d0

  allocate(f(nx,ny,nz))
  allocate(fn(nx,ny,nz))

  call init(nx, ny, nz, dx, dy, dz, f);
  
  call start_timer()

  do icnt = 0, nt-1
     if(mod(icnt,100) == 0) write (*,"(A5,I4,A4,F7.5)"), "time(",icnt,") = ",time

     flop = flop + diffusion3d(nx, ny, nz, dx, dy, dz, dt, kappa, f, fn)

     call swap(f, fn)

     time = time + dt
     if(time + 0.5*dt >= 0.1) exit
  end do
    
  elapsed_time = get_elapsed_time();
    
  write(*, "(A7,F8.3,A6)"), "Time = ",elapsed_time," [sec]"
  write(*, "(A13,F7.2,A9)"), "Performance= ",flop/elapsed_time*1.0e-09," [GFlops]"

  ferr = accuracy(time, nx, ny, nz, dx, dy, dz, kappa, f)
  write(*, "(A6,I0,A2,I0,A2,I0,A4,E12.6)"), "Error[",nx,"][",ny,"][",nz,"] = ",ferr

  deallocate(f,fn)

end program main

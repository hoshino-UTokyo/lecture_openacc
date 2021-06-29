module diffusion
  implicit none

contains

  double precision function diffusion3d(nx, ny, nz, dx, dy, dz, dt, kappa, f, fn)
    
    integer,intent(in) :: nx, ny, nz
    real(KIND=4),intent(in) :: dx, dy, dz, dt, kappa
    real(KIND=4),intent(in),dimension(:,:,:) :: f
    real(KIND=4),intent(out),dimension(:,:,:) :: fn
    real(KIND=4) :: ce,cw,cn,cs,ct,cb,cc
    integer :: w,e,n,s,b,t
    integer :: i,j,k

    ce = kappa*dt/(dx*dx)
    cw = ce
    cn = kappa*dt/(dy*dy)
    cs = cn
    ct = kappa*dt/(dz*dz)
    cb = ct

    cc = 1.0 - (ce + cw + cn + cs + ct + cb)

!$acc kernels  !!present(f,fn)
!$acc loop independent
    do k = 1, nz
!$acc loop independent
       do j = 1, ny
!$acc loop independent
          do i = 1, nx

             w = -1; e = 1; n = -1; s = 1; b = -1; t = 1;
             if(i == 1)  w = 0
             if(i == nx) e = 0
             if(j == 1)  n = 0
             if(j == ny) s = 0
             if(k == 1)  b = 0
             if(k == nz) t = 0
             fn(i,j,k) = cc * f(i,j,k) + cw * f(i+w,j,k) &
                  + ce * f(i+e,j,k) + cs * f(i,j+s,k) + cn * f(i,j+n,k) &
                  + cb * f(i,j,k+b) + ct * f(i,j,k+t)

          end do
       end do
    end do
!$acc end kernels

    diffusion3d = dble(nx*ny*nz)*13.0

  end function diffusion3d


  subroutine init(nx, ny, nz, dx, dy, dz, f)
    
    integer,intent(in) :: nx, ny, nz
    real(KIND=4),intent(in) :: dx, dy, dz
    real(KIND=4),intent(out),dimension(:,:,:) :: f
    real(KIND=4) :: kx,ky,kz,x,y,z,pi
    integer :: i,j,k

    pi = acos(-1.0)
    kx = 2.0*pi
    ky = kx
    kz = kx

    do k = 1, nz
       do j = 1, ny
          do i = 1, nx

             x = dx*(real(i-1) + 0.5)
             y = dy*(real(j-1) + 0.5)
             z = dz*(real(k-1) + 0.5)

             f(i,j,k) = 0.125*(1.0 - cos(kx*x))*(1.0 - cos(ky*y))*(1.0 - cos(kz*z))

          end do
       end do
    end do

  end subroutine init


  double precision function accuracy(time, nx, ny, nz, dx, dy, dz, kappa, f)
    double precision,intent(out) :: time
    integer,intent(in) :: nx, ny, nz
    real(KIND=4),intent(in) :: dx, dy, dz, kappa
    real(KIND=4),intent(out),dimension(:,:,:) :: f
    real(KIND=4) :: kx,ky,kz,ax,ay,az,x,y,z,pi,f0
    double precision :: ferr
    integer :: i,j,k

    pi = acos(-1.0)
    kx = 2.0*pi
    ky = kx
    kz = kx

    ax = exp(-kappa*time*(kx*kx))
    ay = exp(-kappa*time*(ky*ky))
    az = exp(-kappa*time*(kz*kz))

    ferr = 0.d0

    do k = 1, nz
       do j = 1, ny
          do i = 1, nx

             x = dx*(real(i-1) + 0.5)
             y = dy*(real(j-1) + 0.5)
             z = dz*(real(k-1) + 0.5)

             f0 = 0.125*(1.0 - ax*cos(kx*x)) * (1.0 - ay*cos(ky*y)) * (1.0 - az*cos(kz*z))

             ferr = ferr + (f(i,j,k) - f0)*(f(i,j,k) - f0);
          end do
       end do
    end do

    accuracy = sqrt(ferr/dble(nx*ny*nz))

  end function accuracy

end module diffusion


module misc
  implicit none
  double precision :: t_s

contains

  subroutine swap(f, fn)
    real(KIND=4),pointer,dimension(:,:,:),intent(inout) :: f,fn
    real(KIND=4),pointer,dimension(:,:,:) :: ftmp

    ftmp => f
    f => fn
    fn => ftmp
  end subroutine swap

  subroutine start_timer()
    real(KIND=8) :: omp_get_wtime
    t_s = omp_get_wtime()
  end subroutine start_timer

  double precision function get_elapsed_time()
    real(KIND=8) :: omp_get_wtime
    get_elapsed_time = omp_get_wtime() - t_s
  end function get_elapsed_time

end module misc

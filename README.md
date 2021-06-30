# lecture_openacc

東京大学情報基盤センター お試しアカウント付き講習会「GPUプログラミング入門」
で使用しているOpenACC(C, Fortran)のサンプルコードです。
Wisteria/BDEC-01 Aquariusノード向けのジョブスクリプトが含まれます。  
講習会URL： https://www.cc.u-tokyo.ac.jp/events/lectures/


# Requirement

* NVIDA HPC SDK： https://developer.nvidia.com/nvidia-hpc-sdk-downloads
* OpenMPI (C/openacc_fdtdでのみ利用) 
Wisteria/BDEC-01 には予めインストールされています。

# Usage 

以下は全てWisteria/BDEC-01 Aquariusでの利用方法です。

```bash
module load nvidia ompi-cuda
```

## openacc_hello (C, Fortran)

## openacc_basic (C, Fortran)

## openacc_diffusion (C, Fortran)

## openacc_fdtd (C)


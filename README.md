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
module load nvidia ompi-cuda  #NVIDA HPC SDK, OpenMPIの環境構築
cd /work/グループ名/ユーザ名/  #/home は計算ノードから参照できないので、/work以下で作業しましょう。
git clone https://github.com/hoshino-UTokyo/lecture_openacc.git
cd lecture_openacc/
cd C or F            # C, Fortran好きな方を選んでください。
```

## openacc_hello (C, Fortran)
OpenACCのプログラムを動かしてみましょう。
```bash
cd openacc_hello
```

## openacc_basic (C, Fortran)

## openacc_diffusion (C, Fortran)

## openacc_fdtd (C)


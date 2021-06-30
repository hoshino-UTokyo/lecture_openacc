# lecture_openacc

東京大学情報基盤センター お試しアカウント付き講習会「GPUプログラミング入門」
で使用しているOpenACC(C, Fortran)のサンプルコードです。
Wisteria/BDEC-01 Aquariusノード向けのジョブスクリプトが含まれます。  
講習会URL： https://www.cc.u-tokyo.ac.jp/events/lectures/


# Requirement

* NVIDA HPC SDK： https://developer.nvidia.com/nvidia-hpc-sdk-downloads
* OpenMPI (C/openacc_fdtdでのみ利用) 


# Usage 

Wisteria/BDEC-01 Aquarius ユーザの方はログインの度に以下を実行

```bash
module load nvidia ompi-cuda
```


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
cd 01_hello_acc
make
pjsub run.sh
cat run.sh.XXXXX.out  # ジョブが終了するとファイルができます。XXXXXはジョブ番号。

```

## openacc_basic (C, Fortran)
OpenACCのkernels,loop,data指示文の使い方、コンパイラメッセージの見方を学びましょう。
```bash
cd openacc_basic
cd 01_original    # OpenACCの無いコード
make
cd 02_kernels    # kernelsのみ挿入したコード
make             # Cではコンパイルに失敗します。失敗時のメッセージを見てみましょう
cd 03_kernels_copy  # kernelsにcopy指示節を追加したコード
make                # Cではコンパイルに失敗します。失敗時のメッセージを比べてみましょう
cd 04_loop
```

## openacc_diffusion (C, Fortran)

## openacc_fdtd (C)


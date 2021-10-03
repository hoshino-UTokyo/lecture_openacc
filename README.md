# lecture_openacc

東京大学情報基盤センター お試しアカウント付き講習会「GPUプログラミング入門」で使用しているOpenACC(C, Fortran)のサンプルコードです。  
OpenACCの基本である、kernels, loop, data指示文の適用方法が学べます。  
Wisteria/BDEC-01 Aquariusノード向けのジョブスクリプトが含まれます。    
講習会URL： https://www.cc.u-tokyo.ac.jp/events/lectures/


# Requirement

* NVIDA HPC SDK： https://developer.nvidia.com/nvidia-hpc-sdk-downloads

* OpenMPI (C/openacc_fdtdでのみ利用)        

  Wisteria/BDEC-01 には予めインストールされています。

# Usage 

以下は全てWisteria/BDEC-01 Aquariusでの利用方法です。

```bash
module load nvidia ompi-cuda   # NVIDA HPC SDK, OpenMPIの環境構築。ログインの度必要です。
cd /work/グループ名/ユーザ名/  　#/home は計算ノードから参照できないので、/work以下で作業しましょう。
git clone https://github.com/hoshino-UTokyo/lecture_openacc.git
cd lecture_openacc/
cd C or F                      # C, Fortran好きな方を選んでください。
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
* OpenACCの基本、kernels,loop,data指示文の使い方を学びましょう。
* コンパイラメッセージの見方を学びましょう。
* 01_originalからスタートして、06_presentが完成版です。
```bash
cd openacc_basic
cd 01_original      # OpenACCの無いコード。
make
cd 02_kernels       # kernelsのみ挿入したコード。
make                # Cではコンパイルに失敗します。失敗時のメッセージを見てみましょう。
cd 03_kernels_copy  # kernelsにcopy指示節を追加したコード。
make                # Cではコンパイルに成功しますが、並列化されません。メッセージを見てみましょう。
cd 04_loop          # loop指示文を追加したコード。
make                # 並列化に成功します。メッセージを比べてみましょう。
pjsub run.sh        # とても遅いです。
cd 05_data          # data指示文を追加したコード。
make
pjsub run.sh        # だいぶ速くなります。
cd 06_present       # present指示節を使って見栄えを良くしたコード。動きは変わりません。
make
```

次の表はWisteriaでの実行時間目安(秒)です。
|         |  01  |    02    |  03  |  04  |  05   |  06   |
| :------ | :--: | :------: | :--: | :--: | :---: | :---: |
| C       | 12.5 | 実行不能 | >600 | 25.3 | 0.383 | 0.382 |
| Fortran | 12.4 |   25.7   | 25.3 | 25.4 | 0.361 | 0.358 |

## openacc_basic_managed (C, Fortran)

* GPUのUnified memory (managed memory) 機能を利用する場合のOpenACCの実装方法を学びましょう。
  * -ta=tesla,cc80,**managed** をコンパイラオプションに追加することにより有効になります。
* Data指示文で行っていたデータ管理をシステム側に任せられるので便利です。

```bash
cd openacc_basic
cd 01_original      # OpenACCの無いコード。
make
cd 02_kernels       # kernelsのみ挿入したコード。
make                # Cでもmanagedならコンパイルに成功します。ただし並列化はされません。
cd 03_loop          # loop指示文を追加したコード。
make                # 並列化に成功します。メッセージを比べてみましょう。
pjsub run.sh        
```

次の表はWisteriaでの実行時間目安(秒)です。

※Data指示文を使った場合より速く見えるのは、GPUの下準備が計測区間外で行われているためです。GPUに初めて触る時には準備に若干の時間(0.1~0.2秒程度)が掛かります。openacc_basicで計測関数より前に任意のGPU処理を挿入すると、managed版より高速になります。

|         |  01  |  02   |  03   |
| :------ | :--: | :---: | :---: |
| C       | 12.5 | >600  | 0.268 |
| Fortran | 12.4 | 0.272 | 0.270 |


## openacc_diffusion (C, Fortran)

* kernels,loop,data指示文を使って拡散方程式のコードを高速化します。

```bash
cd openacc_basic
cd 01_original      # OpenACCの無いコード。OpenACC化してみましょう。
make
cd 02_openacc       # OpenACC化された解答コード。
make                
cd 03_openacc_nvcompiler_acc_time  # コンパイラの機能により、実行時情報を取る方法。ジョブスクリプトが一行違います。
make                
cd 04_openacc_managed              # Unified memory機能を使う場合の実装例
make                
pjsub run.sh
```

## openacc_fdtd (C)


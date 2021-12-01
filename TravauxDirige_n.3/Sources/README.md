

# TP2 de NOM Prénom

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`





## lscpu

```
Architecture :                          x86_64
Mode(s) opératoire(s) des processeurs : 32-bit, 64-bit
Boutisme :                              Little Endian
Address sizes:                          39 bits physical, 48 bits virtual
Processeur(s) :                         12
Liste de processeur(s) en ligne :       0-11
Thread(s) par cœur :                    2
Cœur(s) par socket :                    6
Socket(s) :                             1
Nœud(s) NUMA :                          1
Identifiant constructeur :              GenuineIntel
Famille de processeur :                 6
Modèle :                                158
Nom de modèle :                         Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
Révision :                              10
Vitesse du processeur en MHz :          2200.000
Vitesse maximale du processeur en MHz : 4100,0000
Vitesse minimale du processeur en MHz : 800,0000
BogoMIPS :                              4399.99
Virtualisation :                        VT-x
Cache L1d :                             192 KiB
Cache L1i :                             192 KiB
Cache L2 :                              1,5 MiB
Cache L3 :                              9 MiB
Nœud NUMA 0 de processeur(s) :          0-11
Vulnerability Itlb multihit:            KVM: Mitigation: VMX disabled
Vulnerability L1tf:                     Mitigation; PTE Inversion; VMX condition
                                        al cache flushes, SMT vulnerable
Vulnerability Mds:                      Mitigation; Clear CPU buffers; SMT vulne
                                        rable
Vulnerability Meltdown:                 Mitigation; PTI
Vulnerability Spec store bypass:        Mitigation; Speculative Store Bypass dis
                                        abled via prctl and seccomp
Vulnerability Spectre v1:               Mitigation; usercopy/swapgs barriers and
                                         __user pointer sanitization
Vulnerability Spectre v2:               Mitigation; Full generic retpoline, IBPB
                                         conditional, IBRS_FW, STIBP conditional
                                        , RSB filling
Vulnerability Srbds:                    Mitigation; Microcode
Vulnerability Tsx async abort:          Not affected
Drapaux :                               fpu vme de pse tsc msr pae mce cx8 apic 
                                        sep mtrr pge mca cmov pat pse36 clflush 
                                        dts acpi mmx fxsr sse sse2 ss ht tm pbe 
                                        syscall nx pdpe1gb rdtscp lm constant_ts
                                        c art arch_perfmon pebs bts rep_good nop
                                        l xtopology nonstop_tsc cpuid aperfmperf
                                         pni pclmulqdq dtes64 monitor ds_cpl vmx
                                         est tm2 ssse3 sdbg fma cx16 xtpr pdcm p
                                        cid sse4_1 sse4_2 x2apic movbe popcnt ts
                                        c_deadline_timer aes xsave avx f16c rdra
                                        nd lahf_lm abm 3dnowprefetch cpuid_fault
                                         epb invpcid_single pti ssbd ibrs ibpb s
                                        tibp tpr_shadow vnmi flexpriority ept vp
                                        id ept_ad fsgsbase tsc_adjust bmi1 avx2 
                                        smep bmi2 erms invpcid mpx rdseed adx sm
                                        ap clflushopt intel_pt xsaveopt xsavec x
                                        getbv1 xsaves dtherm ida arat pln pts hw
                                        p hwp_notify hwp_act_window hwp_epp md_c
                                        lear flush_l1d
```


*Des infos utiles s'y trouvent : nb core, taille de cache*



## Produit scalaire 

*Expliquer les paramètres, les fichiers, l'optimisation de compil, NbSamples, ...*

On compile dotproduct.cpp avec le makefile, sans option de débuggage, et avec 1024 samples

| OMP_NUM    | Temps en secondes du produit scalaire |
| ---------- | ------------------------------------- |
| séquentiel | 0.198                                 |
| 1          | 0.198                                 |
| 2          | 0.105                                 |
| 3          | 0.080                                 |
| 4          | 0.075                                 |
| 8          | 0.074                                 |


On compile dotproduct_thread.cpp, donc sans directives openMP 

| num_thread | Temps en secondes du produit scalaire |
| ---------- | ------------------------------------- |
| séquentiel | 0.266                                 |
| 1          | 0.266                                 |
| 2          | 0.156                                 |
| 3          | 0.137                                 |
| 4          | 0.133                                 |
| 8          | 0.210                                 |

On observe que le temps de calcul plafonne à partir d'un certain nombre de processus parallèles, voire devient plus mauvais, ce qui est normal puisque le temps que le programme met à gérer tous les processus n'est plus négligeable.
On observe de plus que OpenMP est plus efficace dans ce cas-là, à tous les niveaux :
* Il est meilleur sur les temps individuels
* Il plafonne "plus longtemps" que les thread de c++


## Produit matrice-matrice



### Permutation des boucles

La ligne de commande est :
`make TestProduct.exe && ./TestProduct.exe 1024`

| Dimension | time    | MFlops  |
| --------- | ------- | ------- |
| 1023      | 2.06893 | 1034.93 |
| 1024      | 3.98789 | 538.501 |
| 1025      | 2.07563 | 1037.65 |

On observe des temps similaires pour 1023 et 1025, mais le temps pour 1024 est nettement plus long (quasiment 2 fois plus). Celà parait étrange, mais c'est en réalité du au fait que 1024 est le diviseur utilisé dans le processeur pour affecter l'adresse mémoire aux variables. Ainsi, les variables reviennent souvent au même espace mémoire cache, nécessitant beaucoup d'aller-retour dans la mémoire vive.

  | ordre           | time    | MFlops  | MFlops(n=2048) |
  | --------------- | ------- | ------- | -------------- |
  | i,j,k (origine) | 3.80819 | 563.912 |
  | j,i,k           | 4.23256 | 507.372 |
  | i,k,j           | 14.7803 | 145.294 |
  | k,i,j           | 14.5419 | 147.676 |
  | j,k,i           | 1.22929 | 1746.93 |
  | k,j,i           | 1.18154 | 1817.53 |


L'ordre kji est le meilleur car il permet d'explorer les adresses de la matrice de façon séquentielle, sans faire de saut de 1024 emplacements, ce qui entraine le phénomène vu dans les précédentes mesures.



### OMP sur la meilleure boucle 

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProduct.exe 1024`

  | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512) | MFlops(n=4096) |
  | ------- | ------- | -------------- | ------------- | -------------- |
  | 1       | 1805.15 |
  | 2       | 3440.36 |
  | 3       | 4771.8  |
  | 4       | 6107.71 |
  | 5       | 7101.39 |
  | 6       | 7929.46 |
  | 7       | 6181.11 |
  | 8       | 6927.35 |

La fréquence de calcul atteint un somment sur 6 processus parrallèles pour redescendre ensuite. Celà est logique compte tenu du nombre de threads de mon processeur : 12.

Le produit matrice matrice est ici fait de façon naïve, or on a vu en prépa les principes de la programmation dynamique, notamment dans le cas du produit de matrices, où l'on peut séparer le problème en sous-problèmes plus simples à résoudre.

### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  | szBlock        | MFlops | MFlops(n=2048) | MFlops(n=512) | MFlops(n=4096) |
  | -------------- | ------ | -------------- | ------------- | -------------- |
  | origine (=max) |        |
  | 32             |        |
  | 64             |        |
  | 128            |        |
  | 256            |        |
  | 512            |        |
  | 1024           |        |




### Bloc + OMP



  | szBlock  | OMP_NUM | MFlops | MFlops(n=2048) | MFlops(n=512) | MFlops(n=4096) |
  | -------- | ------- | ------ | -------------- |
  | A.nbCols | 1       |        |
  | 512      | 8       |        |







# Tips 

```
	env 
	OMP_NUM_THREADS=4 ./dot_product.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```

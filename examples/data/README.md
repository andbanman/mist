Mist Sample Experiment
======================

The `example.py` script generates random data with an internal structure, and runs three searches to detect that structure:

1. A Mutual Information (MI) search over all variables
2. A MI search over a subset of variables
3. A Delta-3 search over all variables.

The output is saved in `.png` and `.csv` files.

```
python3 example.py
```

Expected Values
---------------
There are three features Mist should find:

1. A strong MI signal among variables 30 through 59, as revealed by mi.png.
2. A relatively weak MI signal among variables 0 through 29, as revealed by mi_limited.png.
3. A strong Delta-3 signal among variables 60 through 89.

If the random seed in example.py is left unchanged, then the output files should exactly match the files in `expected/` and the program output matches

```
Generating sample data and results for Libmist v1.3.0

Histogram of D3 results:
(array([117470,     10]), array([0, 1, 2]))

D3 signal >=1:
[[60.         70.         80.          1.66954922]
 [61.         71.         81.          1.6157517 ]
 [62.         72.         82.          1.69151356]
 [63.         73.         83.          1.61135282]
 [64.         74.         84.          1.75615827]
 [65.         75.         85.          1.79871293]
 [66.         76.         86.          1.70112034]
 [67.         77.         87.          1.61054217]
 [68.         78.         88.          1.76928736]
 [69.         79.         89.          1.65756937]]
```

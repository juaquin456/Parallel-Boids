import os

dir = input()

list_dir = os.listdir(dir)


for x in list_dir:
    print(x)
    os.system(f"sbatch {x}")
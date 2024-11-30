import os

# dir = input()
dir = "./bash_scripts"
list_dir = os.listdir(dir)

list_dir.sort()
for x in list_dir:
    # print(x)
    os.system(f"sbatch {dir}/{x}")
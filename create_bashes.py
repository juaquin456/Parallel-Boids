import os

# Configura el número de scripts que deseas generar
num_scripts = 5

# Ruta donde se guardarán los archivos Bash generados
output_dir = "./bash_scripts"
os.makedirs(output_dir, exist_ok=True)

# Plantilla para el archivo bash
bash_template = """#!/bin/bash
#SBATCH --job-name={name}
#SBATCH --output={output_file}
#SBATCH --cpus-per-task={num_cores}
#SBATCH --partition=standard
./Parallel_Boids 0 {num_boids} {num_cores} 
"""
num_cores = [1,2,4,8,16,32]
num_boids = [100,1000,10000,100000,1000000,10000000]

# Genera varios archivos bash
for cores in num_cores:
    for boids in num_boids:
        # Define el nombre del archivo bash y el nombre de salida para los logs
        script_name = f"{output_dir}/{cores}_{boids}.sh"
        output_file = f"output_{cores}_{boids}.log"


        # Crea el contenido del script usando la plantilla
        script_content = bash_template.format(output_file=output_file, num_cores=cores, num_boids=boids, name=script_name)

        # Guarda el archivo Bash
        with open(script_name, "w") as f:
            f.write(script_content)

        # Haz el archivo ejecutable
        os.chmod(script_name, 0o755)

        print(f"Generado Paralelo: {script_name}")


bash_template = """#!/bin/bash
#SBATCH --job-name={name}
#SBATCH --output={output_file}
#SBATCH --partition=standard
./Parallel_Boids 1 {num_boids}
"""
for boids in num_boids:
    # Define el nombre del archivo bash y el nombre de salida para los logs
    script_name = f"{output_dir}/seq_{boids}.sh"
    output_file = f"output_{boids}.log"


    # Crea el contenido del script usando la plantilla
    script_content = bash_template.format(output_file=output_file, num_boids=boids, name=script_name)

    # Guarda el archivo Bash
    with open(script_name, "w") as f:
        f.write(script_content)

    # Haz el archivo ejecutable
    os.chmod(script_name, 0o755)

    print(f"Generado: {script_name}")
print("Todos los scripts han sido generados.")

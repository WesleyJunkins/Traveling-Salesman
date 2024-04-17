def generate_graph_matrix_no_start_zero(num_rows, filename):
    import random

    with open(filename, 'w') as file:
        for i in range(num_rows):
            # Generate row with random integers, only ending with 0
            row = [str(random.randint(1, 1000)) for _ in range(i)]
            row += ['0']  # Adding 0 only at the end of each row
            file.write(' '.join(row) + '\n')

# Specify the number of rows and the output filename for the updated requirement
num_rows_updated = 25000  # Example input, can be changed as needed
output_filename_updated = 'SizeBIG.graph'

# Generate the graph matrix without starting 0
generate_graph_matrix_no_start_zero(num_rows_updated, output_filename_updated)

# The output filename is where the generated file is saved
print(output_filename_updated)

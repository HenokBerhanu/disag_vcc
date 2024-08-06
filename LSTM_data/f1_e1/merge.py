import pandas as pd

# Define the file paths
cpu_usage_file = '/home/henok/disag_vcc/LSTM_data/f1_e1/gNB_CUUP_cpu_usage.tsv'
memory_usage_file = '/home/henok/disag_vcc/LSTM_data/f1_e1/memory_usage.tsv'

# Read the TSV files into DataFrames
cpu_usage_df = pd.read_csv(cpu_usage_file, sep='\t')
memory_usage_df = pd.read_csv(memory_usage_file, sep='\t')

# Combine the DataFrames on the 'Container' column
combined_df = pd.concat([cpu_usage_df, memory_usage_df['Memory-usage(MiB)']], axis=1)

# Save the combined DataFrame to a new TSV file
combined_file = '/home/henok/disag_vcc/LSTM_data/f1_e1/combined_CPU_Memory_usage_F1_E1.tsv'
combined_df.to_csv(combined_file, sep='\t', index=False)

print("Combined file created successfully.")

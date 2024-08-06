import pandas as pd

# Read the TSV file into a DataFrame
df = pd.read_csv('/home/henok/disag_vcc/LSTM_data/f1_e1/gNB_CUCP_memory_usage.tsv', sep='\t')

# Drop the 'Memory-usage(MiB)' column
df.drop(columns=['Container'], inplace=True)

# Save the modified DataFrame back to a TSV file
df.to_csv('/home/henok/disag_vcc/LSTM_data/f1_e1/gNB_CUCP_memory_usage2.tsv', sep='\t', index=False)

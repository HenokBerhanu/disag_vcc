import csv

def extract_columns(input_csv, output_csv, container_name):
    with open(input_csv, 'r') as csvfile:
        reader = csv.reader(csvfile)
        extracted_data = []
        for row in reader:
            metric, value = row
            # Check if the row contains data for the specified container
            if container_name in metric:
                extracted_data.append([container_name, value])

    with open(output_csv, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile, delimiter='\t')
        writer.writerow(["Container", "Memory-usage(MiB)"])
        for row in extracted_data:
            writer.writerow(row)

# Specify the container name and input/output CSV file paths
container_name = "rfsim5g-oai-du"
input_csv = "/home/henok/disag_vcc/LSTM_data/gNBDU_memory_usage.csv"
output_csv = "/home/henok/disag_vcc/LSTM_data/gNB_DU_memory_usage.tsv"

# Extract and save data
extract_columns(input_csv, output_csv, container_name)

print(f"Data extracted and saved for container: {container_name}")

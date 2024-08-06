import csv
def extract_container_data(input_csv, container_name):
    with open(input_csv, 'r') as csvfile:
        reader = csv.reader(csvfile)
        container_data = []
        for row in reader:
            metric, value = row
            # Check if the row contains data for the specified container
            if container_name in metric:
                container_data.append((metric, value))
    return container_data

def save_extracted_data(data, output_csv):
    with open(output_csv, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["Metric", "Value"])
        for row in data:
            writer.writerow(row)

# Specify the container name and input/output CSV file paths
container_name = "rfsim5g-oai-cuup"
#cpu_input_csv = "/home/henok/disag_vcc/LSTM_data/cpu_usage_gNBDU.csv"
memory_input_csv = "/home/henok/disag_vcc/LSTM_data/f1_e1/memory_usage_f1_e1.csv"
#cpu_output_csv = "/home/henok/disag_vcc/LSTM_data/gNBDU_cpu_usage.csv"
memory_output_csv = "/home/henok/disag_vcc/LSTM_data/f1_e1/gNBCUUP_memory_usage.csv"

# Extract and save CPU data
#cpu_data = extract_container_data(cpu_input_csv, container_name)
#save_extracted_data(cpu_data, cpu_output_csv)

# Extract and save memory data
memory_data = extract_container_data(memory_input_csv, container_name)
save_extracted_data(memory_data, memory_output_csv)

print(f"Data extracted and saved for container: {container_name}")

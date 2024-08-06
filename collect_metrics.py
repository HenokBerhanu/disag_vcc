import requests
import csv
import time
import os

PROMETHEUS_URL = 'http://192.168.12.202:9090'
OUTPUT_DIR = '/home/henok/disag_vcc/LSTM_data/f1_e1'  # Change this to your desired directory

def query_prometheus(query):
    response = requests.get(f'{PROMETHEUS_URL}/api/v1/query', params={'query': query})
    results = response.json()['data']['result']
    return results

def write_to_csv(data, filename):
    os.makedirs(OUTPUT_DIR, exist_ok=True)  # Create the directory if it doesn't exist
    filepath = os.path.join(OUTPUT_DIR, filename)
    with open(filepath, 'a') as csvfile:
        writer = csv.writer(csvfile)
        for result in data:
            writer.writerow([result['metric'], result['value'][1]])

cpu_query = 'rate(container_cpu_usage_seconds_total[1m])'
memory_query = 'container_memory_usage_bytes'

end_time = time.time() + 10 * 3600  # Run for 10 hours

while time.time() < end_time:
    cpu_data = query_prometheus(cpu_query)
    memory_data = query_prometheus(memory_query)
    
    write_to_csv(cpu_data, 'cpu_usage_f1_e1.csv')
    write_to_csv(memory_data, 'memory_usage_f1_e1.csv')
    
    time.sleep(60)  # Query every 60 seconds








# import requests
# import csv
# import time
# import os

# PROMETHEUS_URL = 'http://10.88.135.21:9090'
# OUTPUT_DIR = '/home/henok/disag_vcc/LSTM_data'  # Change this to your desired directory

# def query_prometheus(query):
#     response = requests.get(f'{PROMETHEUS_URL}/api/v1/query', params={'query': query})
#     results = response.json()['data']['result']
#     return results

# def write_to_csv(data, filename):
#     os.makedirs(OUTPUT_DIR, exist_ok=True)  # Create the directory if it doesn't exist
#     filepath = os.path.join(OUTPUT_DIR, filename)
#     with open(filepath, 'a') as csvfile:
#         writer = csv.writer(csvfile)
#         for result in data:
#             writer.writerow([result['metric'], result['value'][1]])

# cpu_query = 'rate(container_cpu_usage_seconds_total[1m])'
# memory_query = 'container_memory_usage_bytes'

# while True:
#     cpu_data = query_prometheus(cpu_query)
#     memory_data = query_prometheus(memory_query)
    
#     write_to_csv(cpu_data, 'cpu_usage_gNBDU.csv')
#     write_to_csv(memory_data, 'memory_usagegNBDU.csv')
    
#     time.sleep(60)  # Query every 60 seconds

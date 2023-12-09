def calculate_throughput(input_file_name, output_file_name):
    try:
        with open(input_file_name, 'r') as in_file, open(output_file_name, 'w') as out_file:
            throughput_map = {}
            in_file.readline()  # Skip the first line (header)

            # Read each line
            for line in in_file:
                data = line.strip().split('\t')
                cell_id = int(data[2])
                rx_bytes = float(data[9])

                # Calculate throughput and accumulate
                throughput_map[cell_id] = throughput_map.get(cell_id, 0) + (rx_bytes * 8 / 1000 / 50)

            # Output results to file
            out_file.write("CellId,Throughput\n")
            for cell_id, throughput in throughput_map.items():
                out_file.write(f"{cell_id},{throughput}\n")

    except FileNotFoundError:
        print("无法打开文件")

if __name__ == "__main__":
    input_file_name = "dlrdatastats.txt"
    output_file_name = "tun.txt"
    calculate_throughput(input_file_name, output_file_name)

    input_file_name = "ulrdatastats.txt"
    output_file_name = "tu.txt"
    calculate_throughput(input_file_name, output_file_name)

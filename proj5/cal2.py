import math

class Stats:
    def __init__(self):
        self.total_rsrp = 0.0
        self.total_sinr = 0.0
        self.count = 0

# 将 SINR 从线性刻度转换为分贝
def convert_to_db(sinr):
    return 10 * math.log10(sinr)

def main():
    with open("dlrspsinr.txt", 'r') as in_file, open("RSRP-SINR-AVG.csv", 'w') as out_file:
        if not in_file:
            print("无法打开输入文件")
            return 1

        if not out_file:
            print("无法打开输出文件")
            return 1

        in_file.readline()  # 跳过第一行标题
        data_map = {}

        # 读取数据
        for line in in_file:
            data = line.strip().split()
            cell_id, rsrp, sinr = int(data[1]), float(data[4]), float(data[5])

            # 累加 RSRP 和 SINR，以及计数
            if cell_id not in data_map:
                data_map[cell_id] = Stats()
            data_map[cell_id].total_rsrp += rsrp
            data_map[cell_id].total_sinr += sinr
            data_map[cell_id].count += 1

        # 计算平均值并输出到文件
        out_file.write("CellId,Average RSRP,Average SINR (dB)\n")
        for cell_id, stats in data_map.items():
            avg_rsrp = stats.total_rsrp / stats.count
            avg_sinr_db = convert_to_db(stats.total_sinr / stats.count)
            out_file.write(f"{cell_id},{avg_rsrp},{avg_sinr_db}\n")

if __name__ == "__main__":
    main()

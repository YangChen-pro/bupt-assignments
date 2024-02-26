import csv

# 创建CSV文件
def create_csv(fileName, numLines):
    with open(fileName, "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["id", "name", "score"])
        for i in range(numLines):
            writer.writerow([i, "name" + str(i + 1), i * 2])


# 读取CSV文件并计算数字列的和
def read_csv(fileName, index):
    with open(fileName, "r") as csvfile:
        reader = csv.reader(csvfile)
        next(reader)  # 跳过第一行
        sum = 0
        for line in reader:
            sum += int(line[index])
    return sum

if __name__ == "__main__":
    create_csv("data.csv", 1024)
    print(f"sum = {read_csv('data.csv', 2)}")
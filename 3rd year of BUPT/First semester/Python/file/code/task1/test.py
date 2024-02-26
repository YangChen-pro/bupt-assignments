import time

# 创建文本文件
def create_textFile(fileName, numLines):
    start_time = time.time()
    with open(fileName, 'w') as f:
        for i in range(numLines):
            f.write(f"This is line {i+1}\n")
    end_time = time.time()
    print(f"创建文本文件{fileName}，共{numLines}行，耗时{end_time-start_time}秒")

# 读取文本文件
def read_textFile(fileName):
    start_time = time.time()
    with open(fileName, 'r') as f:
        lines = f.readlines()
    end_time = time.time()
    print(f"读取文本文件{fileName}，共{len(lines)}行，耗时{end_time-start_time}秒")

# 创建二进制文件
def create_binaryFile(fileName, numLines):
    start_time = time.time()
    with open(fileName, 'wb') as f:
        for i in range(numLines):
            # 创建一个包含单个字节的字节数组
            data = bytes([(i+1) % 256]) + b"\n"
            f.write(data)
    end_time = time.time()
    print(f"创建二进制文件{fileName}，共{numLines}行，耗时{end_time-start_time}秒")

# 读取二进制文件
def read_binaryFile(fileName):
    start_time = time.time()
    with open(fileName, 'rb') as f:
        lines = f.readlines()
    end_time = time.time()
    print(f"读取二进制文件{fileName}，共{len(lines)}行，耗时{end_time-start_time}秒")

if __name__ == "__main__":
    lines = [1000, 10000, 100000]
    for line in lines:
        create_textFile(f"text_{line}.txt", line)
        read_textFile(f"text_{line}.txt")
        create_binaryFile(f"binary_{line}.bin", line)
        read_binaryFile(f"binary_{line}.bin")
        print()

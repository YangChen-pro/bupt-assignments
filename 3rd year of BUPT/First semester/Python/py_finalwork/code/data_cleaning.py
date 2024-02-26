import json
import os

# 文件夹路径
input_folder = r"RawData"
output_folder = r"clean_data\2022"

# input_folder = r"Spider"
# output_folder = r"clean_data\2023"

# 创建输出文件夹
os.makedirs(output_folder, exist_ok=True)

# 遍历输入文件夹中的所有json文件
for file_name in os.listdir(input_folder):
    if file_name.endswith(".json"):
        input_file = os.path.join(input_folder, file_name)  # 输入文件路径
        output_file = os.path.join(output_folder, file_name)  # 输出文件路径

        data = set()  # 用于存储去重后的数据
        with open(input_file, 'r', encoding='utf-8') as f:
            for line in f:
                item = json.loads(line)
                data.add(json.dumps(item))

        # 去重
        data = [json.loads(item) for item in data]

        # 去除异常值
        cleaned_data = []
        for item in data:
            if "车库" in item["name"] or "未知" in item["layout"]:  # 去除车库和未知户型
                continue
            if item["price_per_m2"] <= 2:  # 去除单价小于等于2元/平米的
                continue
            if item["area"] < 10 or item["area"] > 2000:  # 去除面积小于10平米或大于2000平米的
                continue
            if item["total_price"] < 100:  # 去除总价小于100/月的
                continue
            cleaned_data.append(item)

        # 保存清洗后的数据到文件
        with open(output_file, 'w', encoding='utf-8') as f:
            for item in cleaned_data:
                f.write(json.dumps(item, ensure_ascii=False) + '\n')
        print(input_file, "清洗后的数据量：", len(cleaned_data))
        print("数据已保存到：", output_file)
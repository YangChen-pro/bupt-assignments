import json
import re

data = []

with open(r"../py_homework/LianjiaData.json", "r", encoding="utf-8") as f:
    for line in f:
        item = json.loads(line)
        data.append(item)

total_price = {"dongcheng": [], "xicheng": [], "chaoyang": [], "haidian": []}
unit_price = {"dongcheng": [], "xicheng": [], "chaoyang": [], "haidian": []}
built_years = {"before_2000": [], "2000_to_2009": [], "2010_and_after": []}

for item in data:
    distinct = item["distinct"]
    price = re.search(r"(\d+(\.\d+)?)万", item["price"]).group(1)
    unit = re.search(r"(\d+(,\d+)?)元/平", item["unit_price"]).group(1).replace(",", "")
    total_price[distinct].append(float(price))
    unit_price[distinct].append(float(unit))
    year = re.search(r"(\d+)年", item["built_time"])
    if year:  # 有些房源没有建造时间，所以需要判断
        year = int(year.group(1))
        if year < 2000:
            built_years["before_2000"].append(float(unit))
        elif year < 2010:
            built_years["2000_to_2009"].append(float(unit))
        else:
            built_years["2010_and_after"].append(float(unit))


for distinct in total_price:
    print(f"{distinct}区二手房总价最高的房源：{max(total_price[distinct]):}万")
    print(f"{distinct}区二手房总价最低的房源：{min(total_price[distinct]):}万")
    print(f"{distinct}区二手房总价均值：{sum(total_price[distinct])/len(total_price[distinct]):.2f}万\n")
    print(f"{distinct}区二手房单价最高的房源：{max(unit_price[distinct]):}元/平米")
    print(f"{distinct}区二手房单价最低的房源：{min(unit_price[distinct]):}元/平米")
    print(f"{distinct}区二手房单价均值：{sum(unit_price[distinct])/len(unit_price[distinct]):.2f}元/平米\n")

for year_range in built_years:
    print(f"{year_range}建造的二手房单价均值：{sum(built_years[year_range])/len(built_years[year_range]):.2f}元/平米")
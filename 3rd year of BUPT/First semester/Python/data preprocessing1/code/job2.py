import pandas as pd

data = pd.read_csv(
    r"C:\Users\Administrator\Documents\Tencent Files\1369792882\FileRecv\BeijingPM20100101_20151231.csv",
    encoding="utf-8",
)

# 对HUMI、PRES、TEMP三列，进行线性插值处理
data["HUMI"] = data["HUMI"].interpolate()
data["PRES"] = data["PRES"].interpolate()
data["TEMP"] = data["TEMP"].interpolate()

# 修改cbwd列中值为“cv”的单元格，其值用后项数据填充
data["cbwd"] = data["cbwd"].replace("cv", method="bfill")

data.to_csv("processed.csv", index=False, encoding="utf-8")

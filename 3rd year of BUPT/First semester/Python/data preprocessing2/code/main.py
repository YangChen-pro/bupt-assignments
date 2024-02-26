import pandas as pd
from sklearn.preprocessing import MinMaxScaler, StandardScaler
import matplotlib.pyplot as plt
import matplotlib

# 设置中文显示
matplotlib.rc("font", family="SimHei", weight="bold")
plt.rcParams["axes.unicode_minus"] = False

df = pd.read_csv('BeijingPM20100101_20151231.csv', encoding='utf-8')

# 将PM_Dongsi、PM_Dongsihuan、PM_Nongzhanguan三列中超过500的数据，修改为500。
df.loc[df['PM_Dongsi'] > 500, 'PM_Dongsi'] = 500
df.loc[df['PM_Dongsihuan'] > 500, 'PM_Dongsihuan'] = 500
df.loc[df['PM_Nongzhanguan'] > 500, 'PM_Nongzhanguan'] = 500

# 保存处理后的数据
df.to_csv('data.csv', index=False)

# 对PRES和TEMP数据进行最大最小归一化和标准化归一化
x_reshape = df['PRES'].values.reshape(-1, 1)
y_reshape = df['TEMP'].values.reshape(-1, 1)

# 最大最小归一化
min_max_scaler = MinMaxScaler()
normalized_pres = min_max_scaler.fit_transform(x_reshape)
normalized_temp = min_max_scaler.fit_transform(y_reshape)

# 标准化归一化
standard_scaler = StandardScaler()
standardized_pres = standard_scaler.fit_transform(x_reshape)
standardized_temp = standard_scaler.fit_transform(y_reshape)

# 绘制散点图
fig, axes = plt.subplots(1, 3, figsize=(15, 8))
axes[0].scatter(df['PRES'], df['TEMP'], marker='o')
axes[1].scatter(normalized_pres, normalized_temp, marker='o')
axes[2].scatter(standardized_pres, standardized_temp, marker='o')
axes[0].set_xlabel('PRES')
axes[0].set_ylabel('TEMP')
axes[0].set_title('Original')
axes[1].set_xlabel('normalized PRES')
axes[1].set_ylabel('normalized TEMP')
axes[1].set_title('Normalized')
axes[2].set_xlabel('standardized PRES')
axes[2].set_ylabel('standardized TEMP')
axes[2].set_title('Standardized')
fig.tight_layout()
plt.savefig('scatter.png')
plt.show()

# 新建一列，将PM_Dongsi、PM_Dongsihuan、PM_Nongzhanguan三列的数据进行平均，赋值给新建的列PM_avg
df['PM_avg'] = df[['PM_Dongsi', 'PM_Dongsihuan', 'PM_Nongzhanguan']].mean(axis=1)
# 按照year、month、day进行分组，求PM_avg的平均值
df = df.groupby(['year', 'month', 'day'])['PM_avg'].mean()
# 保存处理后的数据,只保留year、month、day和PM_avg四列
df.to_csv('data1.csv', index=True, header=True)

# 污染程度划分
sections = [0, 50, 100, 150, 200, 300, 501]
sections_color = ['green', 'yellow', 'orange', 'red', 'purple', 'maroon']
sections_name = ['优', '良', '轻度污染', '中度污染', '重度污染', '严重污染']
result = pd.cut(df, sections, labels=sections_name)
# 统计各个污染程度的天数
print(pd.value_counts(result))
# 绘制饼图
plt.figure(figsize=(6, 6))
plt.pie(pd.value_counts(result), labels=sections_name, colors=sections_color, autopct='%1.1f%%')
plt.title('Beijing PM2.5')
plt.savefig('pie.png')
plt.show()

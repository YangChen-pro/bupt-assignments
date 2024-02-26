import json
import re
import matplotlib
import matplotlib.colors as mcolors
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import numpy as np
import pandas as pd
from plotly.io import write_html
from scipy.stats import gaussian_kde
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import seaborn as sns

matplotlib.rc("font", family="SimHei", weight="bold")  # 设置中文字体
plt.rcParams["axes.unicode_minus"] = False  # 设置正常显示负号


class analyze_rental_info:
    def __init__(self, file_path=None):
        """
        初始化
        :param file_path: 文件路径
        """
        if file_path is None:
            return
        self.data = []

        with open(file_path, "r", encoding="utf-8") as f:
            for line in f:
                self.data.append(json.loads(line))

        self.total_prices = [item["total_price"] for item in self.data]  # 总价
        self.price_per_m2 = [item["price_per_m2"] for item in self.data]  # 每平米价格

        self.one_bedroom = []  # 一室
        self.two_bedroom = []  # 二室
        self.three_bedroom = []  # 三室

        self.districts = {}  # 各板块

        self.directions = {}  # 各朝向

        for item in self.data:
            if re.match(r"1(室|房间).*", item["layout"]):
                self.one_bedroom.append(item)
            elif re.match(r"2(室|房间).*", item["layout"]):
                self.two_bedroom.append(item)
            elif re.match(r"3(室|房间).*", item["layout"]):
                self.three_bedroom.append(item)

            self.districts[item["district"]] = self.districts.get(item["district"], []) + [item['total_price']]

            if item["direction"] != '':
                self.directions[item["direction"]] = self.directions.get(item["direction"], []) + [item['price_per_m2']]

    def format_number(self, number):
        """
        格式化数字，保留两位小数或转换为整数
        :param number: 数字
        :return: 格式化后的数字
        """
        if isinstance(number, int) or int(number) == number:
            return int(number)
        else:
            return round(number, 2)

    def calculate_statistics(self, values):
        """
        计算统计信息
        :param values: 数据，列表形式
        :return: 统计信息,包括均值,最大值,最小值,中位数,字典形式形如{"均值": 1000, "最大值": 2000, "最小值": 500, "中位数": 800}
        """
        statistics = {
            "均值": self.format_number(round(sum(values) / len(values), 2)),
            "最大值": self.format_number(max(values)),
            "最小值": self.format_number(min(values)),
            "中位数": self.format_number(round(sorted(values)[len(values) // 2], 2))
        }
        return statistics

    def set_color(self, patches, df):
        """
        设置表格颜色
        :param patches: 直方图的patches
        :param df:  dataframe表格
        :return:  表头颜色, 单元格颜色
        """
        header_color = [mcolors.to_rgba('white')]
        for i in range(len(patches)):
            if type(patches[i]) == str:
                header_color.append(mcolors.to_rgba(patches[i]))
            else:
                header_color.append(patches[i][0].get_facecolor())
        odd_color = [mcolors.to_rgba('white') if i % 2 == 0 else mcolors.to_rgba('C7') for i in range(len(df.columns))]
        even_color = [mcolors.to_rgba('white') if i % 2 == 1 else mcolors.to_rgba('C7') for i in range(len(df.columns))]
        cell_colors = [even_color if i % 2 == 0 else odd_color for i in range(len(df))]
        # 进行平均运算, 使得表格的颜色交替
        cell_colors = [[tuple([(cell_colors[i][j][k] + header_color[j][k]) / 2 for k in range(4)]) for j in
                        range(len(df.columns))] for i in range(len(df))]

        return header_color, cell_colors

    def plot_histogram(self, values, title, x_range=None, ax=None):
        """
        绘制直方图, 并计算核密度估计
        :param values:  数据
        :param title:  标题
        :param x_range:  x轴范围
        :param min_height:  标注数字的最小高度
        :param ax:  matplotlib.axes.Axes 对象，用于绘制图形
        """
        # 绘制直方图
        ax.set_title(title + "频率分布直方图", fontsize=20)
        n, bins, patches = ax.hist(values, bins=50, edgecolor="k", alpha=0.5, density=True, label="直方图",
                                   range=x_range, color='dodgerblue')
        ax.set_xlabel("价格/元", fontsize=18)
        ax.set_ylabel("概率密度", fontsize=18)
        if x_range:  # 设置x轴范围
            ax.set_xlim(x_range[0], x_range[1])
        ax.tick_params(axis='x', labelsize=15)  # 设置x轴刻度大小
        ax.tick_params(axis='y', labelsize=15)  # 设置y轴刻度大小
        ax.minorticks_on()  # 开启小刻度线
        ax.grid(True, which='both', axis='both', linestyle=':', linewidth=0.5)  # 添加网格线
        # 从中间找到面积和大于0.85的区域
        max_index = np.argmax(n)
        area_sum = 0.0
        left_index = max_index
        right_index = max_index + 1
        while area_sum < 0.85:
            area_sum += (n[left_index] + n[right_index]) * (x_range[1] / 50)  # 面积 = 高度 * 宽度
            if left_index > 0:
                left_index -= 1
            if right_index < len(n) - 1:
                right_index += 1
        # 在图中标注85%的区域
        rect = plt.Rectangle((bins[left_index], 0), bins[right_index] - bins[left_index],
                             n[max_index], facecolor="orangered", alpha=0.5)
        ax.add_patch(rect)
        # 计算核密度估计
        kde = gaussian_kde(values)
        if x_range:
            x = np.linspace(x_range[0], x_range[1], 1000)
        else:
            x = np.linspace(min(values), max(values), 1000)
        y = kde(x)
        # 绘制核密度函数曲线
        ax.plot(x, y, color='red', label='核密度函数')
        # 显示统计信息
        statistics = self.calculate_statistics(values)
        data = {'': ['均值', '最大值', '最小值', '中位数']}
        if "每平米" not in title:
            data["总租价/月"] = [statistics['均值'], statistics['最大值'], statistics['最小值'], statistics['中位数']]
        else:
            data["租价/平米"] = [statistics['均值'], statistics['最大值'], statistics['最小值'], statistics['中位数']]
        df = pd.DataFrame(data)
        # 设置颜色
        header_color, cell_colors = self.set_color([patches], df)
        table = ax.table(cellText=df.values,
                         colLabels=df.columns,
                         colColours=header_color,
                         colWidths=[0.3] * len(df.columns),
                         cellLoc='center',
                         cellColours=cell_colors,
                         bbox=[0.6, 0.30, 0.4, 0.35])
        table.auto_set_font_size(False)
        table.set_fontsize(15)  # 设置字体大小为15
        # 在均值和中位数的位置添加虚线
        mean = statistics['均值']
        median = statistics['中位数']
        ax.axvline(mean, color='green', linestyle='--', label='均值', alpha=1.0, linewidth=2.5)
        ax.axvline(median, color='purple', linestyle='--', label='中位数', alpha=1.0, linewidth=2.5)
        ax.axvline(rect.get_x(), color='orangered', linestyle='--', label='85%的区域', alpha=1.0, linewidth=2.5)
        # 显示图例
        ax.legend(fontsize=15)
        print(title, '统计信息：', statistics)
        print('85%的区域：', (rect.get_x(), rect.get_x() + rect.get_width()))

    def plot_bedroom_info(self, titile, x_range=None, ax=None):
        """
        绘制一居、二居、三居的价格分布图表展示
        :param x_range: x轴范围
        :param ax: matplotlib.axes.Axes 对象，用于绘制图形
        """
        # 提取价格信息
        one_bedroom_prices = [item["total_price"] for item in self.one_bedroom]
        two_bedroom_prices = [item["total_price"] for item in self.two_bedroom]
        three_bedroom_prices = [item["total_price"] for item in self.three_bedroom]
        # 绘制直方图
        _, _, patch1 = ax.hist(one_bedroom_prices, bins=50, alpha=0.5, density=True, label="一居", range=x_range)
        _, _, patch2 = ax.hist(two_bedroom_prices, bins=50, alpha=0.5, density=True, label="二居", range=x_range)
        _, _, patch3 = ax.hist(three_bedroom_prices, bins=50, alpha=0.5, density=True, label="三居", range=x_range)
        ax.set_xlabel("总租金/元", fontsize=18)
        ax.set_ylabel("概率密度", fontsize=18)
        if x_range:  # 设置x轴范围
            ax.set_xlim(x_range[0], x_range[1])
            ax.set_xticks(np.arange(x_range[0], x_range[1], 2000))
        ax.tick_params(axis='x', labelsize=15)  # 设置x轴刻度大小
        ax.tick_params(axis='y', labelsize=15)  # 设置y轴刻度大小
        ax.grid(True, which='both', axis='both', linestyle=':', linewidth=0.5)  # 添加网格线
        ax.set_title(titile, fontsize=20)  # 设置标题
        ax.minorticks_on()  # 开启小刻度线
        ax.legend(fontsize=15)  # 显示图例
        # 显示统计信息
        one_bedroom_statistics = self.calculate_statistics(one_bedroom_prices)
        two_bedroom_statistics = self.calculate_statistics(two_bedroom_prices)
        three_bedroom_statistics = self.calculate_statistics(three_bedroom_prices)
        # 生成表格
        data = {
            '': ['均值', '最大值', '最小值', '中位数'],
            '一居': [one_bedroom_statistics['均值'], one_bedroom_statistics['最大值'], one_bedroom_statistics['最小值'],
                     one_bedroom_statistics['中位数']],
            '二居': [two_bedroom_statistics['均值'], two_bedroom_statistics['最大值'], two_bedroom_statistics['最小值'],
                     two_bedroom_statistics['中位数']],
            '三居': [three_bedroom_statistics['均值'], three_bedroom_statistics['最大值'],
                     three_bedroom_statistics['最小值'], three_bedroom_statistics['中位数']]
        }
        df = pd.DataFrame(data)  # 创建DataFrame
        # 设置颜色
        header_color, cell_colors = self.set_color([patch1, patch2, patch3], df)
        # 创建表格
        table = ax.table(cellText=df.values,
                         colLabels=df.columns,
                         colColours=header_color,
                         colWidths=[0.1] * len(df.columns),
                         cellLoc='center',
                         cellColours=cell_colors,
                         bbox=[0.55, 0.4, 0.45, 0.35])
        table.auto_set_font_size(False)
        table.set_fontsize(15)  # 设置字体大小为15
        print(titile, '统计信息：', data)

    def plot_iteration_bar(self, title, dic):
        """
        绘制交互式横向柱状图
        :param title: 标题
        :param dic: 字典，key为板块，value为价格列表
        """
        average_prices = {district: self.format_number(round(sum(prices) / len(prices), 2))
                          for district, prices in dic.items()}  # 计算各板块的平均价格
        average_prices = sorted(average_prices.items(), key=lambda x: x[1], reverse=True)  # 按照价格降序排序
        y_type = [item[0] for item in average_prices]  # 获取板块名称
        prices = [item[1] for item in average_prices]  # 获取平均价格
        # 确定颜色范围
        min_price = min(prices)
        max_price = max(prices)
        # 创建颜色映射（使用热力图颜色映射）
        colorscale = 'YlOrRd'
        # 创建颜色列表，根据价格在颜色映射中进行插值
        colors = [round((price - min_price) / (max_price - min_price), 2) for price in prices]
        # 创建交互式横向柱状图
        bar = go.Bar(
            y=y_type,
            x=prices,
            text=prices,
            textposition='auto',
            orientation='h',
            marker=dict(
                color=colors,
                colorscale=colorscale
            )
        )
        # 创建一个只包含一行的Figure对象
        fig = make_subplots(rows=1, cols=1)
        # 添加柱状图到第一行
        fig.add_trace(bar, row=1, col=1)
        fig.update_layout(title_text=title)  # 设置标题
        # 保存图表为 HTML 文件
        write_html(fig, file=title + '.html', config={'scrollZoom': True, 'responsive': True})
        fig.show(config={'scrollZoom': True, 'responsive': True})
        print(title, '统计信息：\n', average_prices)

    @staticmethod
    def plot_direction_bar(dic):
        """
        绘制热力图, 展示不同城市不同朝向的租金分布情况
        :param dic: 字典，格式为{城市名: {朝向: [价格列表]}}
        :return:
        """
        # 将字典转换为DataFrame，同时处理朝向数据
        df = pd.DataFrame([(city, '|'.join(sorted(direction.split(' '))), price)
                           for city, directions in dic.items()
                           for direction, prices in directions.items()
                           for price in prices],
                          columns=['City', 'Direction', 'Rent'])

        # 同一个城市同一个朝向的租金取平均值
        df = df.groupby(['City', 'Direction'])['Rent'].mean().reset_index()

        # 创建适合热力图的数据格式
        pivot_table = pd.pivot_table(df, values='Rent', index='Direction', columns='City')

        fig = plt.figure(figsize=(8.3 * 2, 11.7 * 2), dpi=300)  # 创建画布
        axs = [fig.add_subplot(1, 1, 1)]  # 创建子图
        fig.subplots_adjust(left=0.2)  # 调整图表边距

        # 计算每个城市的最大值和最小值
        max_vals = pivot_table.max(axis=0)
        min_vals = pivot_table.min(axis=0)

        heatmap = sns.heatmap(pivot_table, cmap='YlOrRd', annot=True, fmt=".1f", linewidths=.5, cbar=False,
                              ax=axs[0])  # 添加颜色条、改变颜色映射、添加数值标签、添加网格线

        # 圈出每个城市的最大值和最小值
        for i, city in enumerate(pivot_table.columns):
            max_val = max_vals[i]
            min_val = min_vals[i]
            max_index = np.where(pivot_table[city] == max_val)
            min_index = np.where(pivot_table[city] == min_val)
            axs[0].add_patch(patches.Rectangle((i, max_index[0][0]), 1, 1, fill=False, edgecolor='blue', lw=2))
            axs[0].add_patch(patches.Rectangle((i, min_index[0][0]), 1, 1, fill=False, edgecolor='blue', lw=2))

        axs[0].set_title('租金分布情况', fontsize=35)  # 调整标题字体大小
        axs[0].set_xlabel('城市', fontsize=25)  # 添加x轴标签并调整字体大小
        axs[0].set_ylabel('朝向', fontsize=25)  # 添加y轴标签并调整字体大小
        axs[0].tick_params(axis='x', labelsize=15)  # 调整x轴刻度字体大小
        axs[0].tick_params(axis='y', labelsize=10)  # 调整y轴刻度字体大小

        # 添加颜色条和标签
        cbar = fig.colorbar(heatmap.get_children()[0], ax=heatmap, orientation='vertical', shrink=.8)
        cbar.set_label('颜色越深，租金越高', fontsize=20)

        return fig, axs

    def compare_kde(self, title, values1, values2, range_x=None, ax=None):
        """
        绘制核密度估计
        :param title: 标题
        :param values1: 数据1
        :param values2: 数据2
        :param range_x: x轴范围
        :param ax: matplotlib.axes.Axes 对象，用于绘制图形
        """
        # 计算核密度估计
        kde1 = gaussian_kde(values1)
        kde2 = gaussian_kde(values2)
        if range_x:
            x = np.linspace(range_x[0], range_x[1], 1000)
        else:
            x = np.linspace(min(values1 + values2), max(values1 + values2), 5000)
        y1 = kde1(x)
        y2 = kde2(x)
        # 绘制核密度函数曲线
        ax.plot(x, y1, color='C0', label='2022年', alpha=0.9)
        ax.plot(x, y2, color='C1', label='2023年', alpha=0.9)
        ax.set_title(title + "核密度估计", fontsize=20)
        ax.set_xlabel("价格/元", fontsize=18)
        ax.set_ylabel("概率密度", fontsize=18)
        ax.tick_params(axis='x', labelsize=15)  # 设置x轴刻度大小
        ax.tick_params(axis='y', labelsize=15)  # 设置y轴刻度大小
        if range_x:
            ax.set_xlim(range_x[0], range_x[1])  # 设置x轴范围
        ax.minorticks_on()  # 开启小刻度线
        ax.grid(True, which='both', axis='both', linestyle=':', linewidth=0.5)  # 添加网格线
        ax.legend(fontsize=15)  # 显示图例

        statistics1 = self.calculate_statistics(values1)
        statistics2 = self.calculate_statistics(values2)
        # 显示统计信息
        data = {'': ['均值', '最大值', '最小值', '中位数']}
        data["2022年"] = [statistics1['均值'], statistics1['最大值'], statistics1['最小值'],
                                   statistics1['中位数']]
        data["2023年"] = [statistics2['均值'], statistics2['最大值'], statistics2['最小值'],
                                   statistics2['中位数']]
        df = pd.DataFrame(data)
        # 设置颜色
        header_color, cell_colors = self.set_color(['C0', 'C1'], df)
        table = ax.table(cellText=df.values,
                         colLabels=df.columns,
                         colColours=header_color,
                         colWidths=[0.3] * len(df.columns),
                         cellLoc='center',
                         cellColours=cell_colors,
                         bbox=[0.6, 0.30, 0.4, 0.35])
        table.auto_set_font_size(False)
        table.set_fontsize(12)  # 设置字体大小为15
        print(title, '统计信息：\n', data)



def initialize(en_to_cn, file_paths=None):
    """
    初始化
    :param en_to_cn: 英文到中文的映射
    :param file_paths: 文件路径列表
    :return: analyze_rental_info对象
    """
    n = len(file_paths)

    # fig1, axs1 = plt.subplots(nrows=n, ncols=2, figsize=(10 * 2, 6 * n)) # 总体房租情况和单位面积房租情况，每个城市一行
    # fig2, axs2 = plt.subplots(nrows=n, ncols=1, figsize=(10 * 1, 6 * n))  # 一居、二居、三居的情况，每个城市一行
    # 调整子图间距
    # fig1.subplots_adjust(hspace=0.4)
    # fig2.subplots_adjust(hspace=0.4)

    city_dic = {}  # 不同城市的朝向, 格式为{城市名: {朝向: [价格列表]}}

    for i in range(n):
        file_path = file_paths[i]
        city_name = re.search(r"/([^/]+)HouseInfo", file_path).group(1)
        city_name = en_to_cn[city_name]
        analyze = analyze_rental_info(file_path)
        if "2022" in file_path:
            city_name = "2022" + city_name
        else:
            city_name = "2023" + city_name

        # 总体房租情况和单位面积房租情况
        fig1, axs1 = plt.subplots(nrows=1, ncols=2, figsize=(15, 6))
        # analyze.plot_histogram(analyze.total_prices, city_name + "租价", (0, 40000), 0.00004, axs1[i][0])
        analyze.plot_histogram(analyze.total_prices, city_name + "租价", (0, 35000), axs1[0])
        # analyze.plot_histogram(analyze.price_per_m2, city_name + "每平米租价", (0, 400), 0.002, axs1[i][1])
        analyze.plot_histogram(analyze.price_per_m2, city_name + "每平米租价", (0, 350), axs1[1])
        # 每个城市一张图
        fig1.tight_layout()
        fig1.savefig(city_name + "租金分布.png")
        fig1.show()

        # 一居、二居、三居的情况
        fig2, axs2 = plt.subplots(nrows=1, ncols=1, figsize=(12, 6))
        # analyze.plot_bedroom_info(city_name + "不同户型租金分布", (0, 25000), axs2[i])
        analyze.plot_bedroom_info(city_name + "不同户型租金分布", (0, 25000), axs2)
        # 每个城市一张图
        fig2.tight_layout()
        fig2.savefig(city_name + "不同户型租金分布.png")
        fig2.show()

        # 不同板块的均价情况
        analyze.plot_iteration_bar(city_name + '不同板块平均租价', analyze.districts)

        city_dic[city_name] = analyze.directions  # 保存城市的朝向信息，用于最后绘制热力图

    # fig1.savefig("租金分布.png")
    # fig1.show()
    # fig2.savefig("不同户型租金分布.png")
    # fig2.show()
    # 不同朝向的均价情况
    fig3, axs3 = analyze_rental_info.plot_direction_bar(city_dic)
    fig3.tight_layout()
    fig3.savefig("2023不同朝向租金分布.png")
    fig3.show()


def compare_info(en_to_cn, last, now):
    """
    对比去年和今年的数据
    :param en_to_cn: 英文到中文的映射
    :param last: 去年的数据
    :param now: 今年的数据
    """
    n = 4
    for i in range(n):
        file_path = last[i]
        city_name = re.search(r"/([^/]+)HouseInfo", file_path).group(1)
        city_name = en_to_cn[city_name]
        analyze_last = analyze_rental_info(last[i])
        analyze_now = analyze_rental_info(now[i])
        city_name = "2022和2023" + city_name

        # 总体房租情况和单位面积房租情况，折线图对比
        fig1, axs1 = plt.subplots(nrows=1, ncols=2, figsize=(15, 6))
        analyze_last.compare_kde(city_name + "租价", analyze_last.total_prices, analyze_now.total_prices, (0, 20000), axs1[0])
        analyze_now.compare_kde(city_name + "每平米租价", analyze_last.price_per_m2, analyze_now.price_per_m2, (0, 300), axs1[1])

        # 每个城市一张图
        fig1.tight_layout()
        fig1.savefig(city_name + "租金分布对比.png")
        fig1.show()


if __name__ == "__main__":
    en_to_cn = {"Beijing": "北京", "Shanghai": "上海", "Guangzhou": "广州", "Shenzhen": "深圳", "Taiyuan": "太原"}

    paths = ["clean_data/2023/BeijingHouseInfo_2023.json",
             "clean_data/2023/ShanghaiHouseInfo_2023.json",
             "clean_data/2023/GuangzhouHouseInfo_2023.json",
             "clean_data/2023/ShenzhenHouseInfo_2023.json",
             "clean_data/2023/TaiyuanHouseInfo_2023.json"
             ]

    this_year_paths = paths[:4]  # 今年的数据，用于对比去年的数据

    last_year_paths = ["clean_data/2022/BeijingHouseInfo.json",
                       "clean_data/2022/ShanghaiHouseInfo.json",
                       "clean_data/2022/GuangzhouHouseInfo.json",
                       "clean_data/2022/ShenzhenHouseInfo.json",
                       ]

    # initialize(en_to_cn, paths)
    # compare_info(en_to_cn, last_year_paths, this_year_paths)

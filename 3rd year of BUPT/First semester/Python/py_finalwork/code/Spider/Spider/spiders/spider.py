import requests
from lxml import etree
import scrapy
from ..items import LianjiaItem
import re
import time
import random

myheaders = {"user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "}


def get_distincts(city_name):
    """
    获取区域
    :param city_name:  城市名
    :return:  区域列表
    """
    target_url = "https://{}.lianjia.com/zufang/".format(city_name)
    response = requests.get(target_url, headers=myheaders)
    response.encoding = "utf-8"
    html_string = response.text
    html_tree = etree.HTML(html_string)
    distincts = html_tree.xpath('//ul[@data-target="area" and @class=""]/li/a/@href')
    distincts = [distinct.split("/")[2] for distinct in distincts]
    return distincts


def get_streets(city_name, distinct_name):
    """
    获取街道
    :param city_name:  城市名
    :param distinct_name:  区域名（列表）
    :return:  街道列表
    """
    strees = []
    for i in range(1, len(distinct_name)):
        target_url = "https://{}.lianjia.com/zufang/{}/".format(city_name, distinct_name[i])
        response = requests.get(target_url, headers=myheaders)
        response.encoding = "utf-8"
        html_string = response.text
        html_tree = etree.HTML(html_string)
        street = html_tree.xpath('//ul[@data-target="area" and not(@class)]/li/a/@href')
        street = [street.split("/")[2] for street in street]
        strees.append(street)

    strees = [j for i in strees for j in i]  # 列表嵌套列表，转换为列表
    strees = list(set(strees))  # 去重
    return strees


def get_start_urls(city_name, street_name):
    """
    根据街道名，生成起始url
    :param city_name:  城市名
    :param street_name:  街道名（列表）
    :return:  起始url（列表）
    """
    urls = []
    for i in range(0, len(street_name)):
        url = "https://{}.lianjia.com/zufang/{}/pg1/".format(city_name, street_name[i])
        urls.append(url)
    return urls


class LianjiaSpider(scrapy.spiders.Spider):
    name = "lianjia"
    allowed_domains = ["lianjia.com"]
    # city_name = 'bj'
    # city_name = 'sh'
    # city_name = 'gz'
    # city_name = 'sz'
    # city_name = 'ty'  # 太原
    distinct_name = get_distincts(city_name)  # 获取区域
    street_name = get_streets(city_name, distinct_name)  # 获取街道
    start_urls = get_start_urls(city_name, street_name)  # 获取起始url
    # print('distinct_name:', distinct_name)
    # print('street_name:', street_name)
    # print('start_urls:', start_urls)

    def parse(self, response):
        page_empty = bool(response.xpath('//div[@class="content__empty1"]'))  # 超出页数范围,会有这个标签
        if not page_empty:  # 未超出页数范围
            distinct = response.url.split("/")[4]
            page = response.url.split("/")[5]
            for each in response.xpath('//div[@class="content__list--item"]'):
                # 去除其他品牌房源
                brand = each.xpath('.//span[@class="brand"]/text()').get()
                if brand is not None and brand.strip() != "链家":
                    continue
                # 去除广告
                ad = each.xpath('.//p[@class="content__list--item--ad"]/text()').get()
                if ad:
                    continue
                # 去除价格区间，只保留单价
                total_price = each.xpath('./div/span/em/text()').get()
                if '-' in total_price:
                    continue

                item = LianjiaItem()
                item["name"] = each.xpath('./div/p/a/text()').get().strip()
                item["district"] = each.xpath('./div//p[@class="content__list--item--des"]/a[2]/text()').get()
                des = each.xpath('.//p[@class="content__list--item--des"]').get().strip()
                area = re.search(r'([\d.]+)㎡', des).group(1)
                item["total_price"] = float(total_price)
                item['area'] = float(area)
                item["price_per_m2"] = round(item["total_price"] / item["area"], 2)
                direction = re.search(r'<i>/</i>(.*)<i>/</i>', des).group(1)
                item["direction"] = direction.strip()
                layout = re.search(r'<i>/</i>\n(.*)<span class="hide">', des).group(1)
                item["layout"] = layout.strip()
                yield item

            if page != "pg100":
                next_page = int(re.search(r'pg(\d+)', page).group(1)) + 1
                next_url = "https://{}.lianjia.com/zufang/{}/pg{}/".format(self.city_name, distinct, next_page)
                yield scrapy.Request(next_url, callback=self.parse, headers=myheaders)

            # 随机休眠1-2秒
            delay = random.uniform(1, 2)
            time.sleep(delay)

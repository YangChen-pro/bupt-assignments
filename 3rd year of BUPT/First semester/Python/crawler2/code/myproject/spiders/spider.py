import scrapy
from ..items import MyItem


class mySpider(scrapy.spiders.Spider):
    name = "bupt"
    allowed_domains = ["bupt.edu.cn/"]
    start_urls = [
        "http://www.bupt.edu.cn/yxjg1.htm",
    ]

    def parse(self, response):
        item = MyItem()
        for each in response.xpath("//ul[@class='linkPageList']/li[4]/div/ul/li"):
            item["school"] = each.xpath("a/text()").extract()
            item["link"] = each.xpath("a/@href").extract()
            if item["school"] and item["link"]:
                yield item
            print(item["school"], item["link"])


from ..items import XuetangxItem
import json


class XuetangxSpider(scrapy.spiders.Spider):
    name = "xuetangx"
    allowed_domains = ["xuetangx.com"]
    # 请求头
    headers = {
        "authority": "www.xuetangx.com",
        "method": "POST",
        "scheme": "https",
        "accept": "application/json, text/plain, */*",
        "accept-encoding": "gzip, deflate, br",
        "accept-language": "zh",
        "content-type": "application/json",
        "cookie": "provider=xuetang; django_language=zh",
        "django-language": "zh",
        "origin": "https://www.xuetangx.com",
        "Referer": "https://www.xuetangx.com/search?query=&org=&classify=1&type=&status=&page=1",
        "sec-fetch-dest": "empty",
        "sec-fetch-mode": "cors",
        "sec-fetch-site": "same-origin",
        "user-agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36",
        "X-Client": "web",
        "Xtbz": "xt",
    }
    data = {
        "query": "",
        "chief_org": [],
        "classify": ["1"],
        "selling_type": [],
        "status": [],
        "appid": 10000,
    }
    download_delay = 1

    def start_requests(self):
        for page_num in range(1, 70):
            yield scrapy.FormRequest(
                url="https://www.xuetangx.com/api/v1/lms/get_product_list/?page={}".format(
                    page_num
                ),
                headers=self.headers,
                method="POST",
                body=json.dumps(self.data),
                callback=self.parse,
            )

    def parse(self, response):
        for product in json.loads(response.body)["data"]["product_list"]:
            item = XuetangxItem()
            item["course_name"] = "课程名称：" + product["name"]
            item["teacher"] = "课程讲师："
            for teacher in product["teacher"]:  # 一个课程可能有多个讲师
                item["teacher"] += teacher["name"] + " "
            item["school"] = "开课学校：" + product["org"]["name"]
            item["student_num"] = "学生人数：" + str(product["count"])

            if (
                item["course_name"]
                and item["teacher"]
                and item["school"]
                and item["student_num"]
            ):
                yield item


from ..items import LianjiaItem
import re


class LianjiaSpider(scrapy.spiders.Spider):
    name = "lianjia"
    allowed_domains = ["lianjia.com"]
    start_urls = [
        "https://bj.lianjia.com/ershoufang/dongcheng/pg1/",
        "https://bj.lianjia.com/ershoufang/xicheng/pg1/",
        "https://bj.lianjia.com/ershoufang/chaoyang/pg1/",
        "https://bj.lianjia.com/ershoufang/haidian/pg1/",
    ]

    def parse(self, response):
        item = LianjiaItem()
        distinct = response.url.split("/")[4]
        page = response.url.split("/")[5]
        for each in response.xpath('//ul[@class="sellListContent"]/li'):
            item["name"] = "楼盘名称：" + each.xpath("div/div/a/text()").get()
            price_value = each.xpath(
                "div/div[@class='priceInfo']/div[@class='totalPrice totalPrice2']/span/text()"
            ).get()
            price_unit = each.xpath(
                "div/div[@class='priceInfo']/div[@class='totalPrice totalPrice2']/i[last()]/text()"
            ).get()
            item["price"] = "总价：" + f"{price_value}{price_unit}"
            area_text = each.xpath(
                ".//div[@class='address']/div[@class='houseInfo']/text()"
            ).get()
            match = re.search(r"(\d+(\.\d+)?)平米", area_text)
            if match:
                item["area"] = "平米数：" + match.group(1) + "平米"
            else:
                item["area"] = "平米数：" + "unknown"
            item["unit_price"] = "单价：" + each.xpath(
                "div/div[@class='priceInfo']/div[@class='unitPrice']/span/text()"
            ).get()
            if item["name"] and item["price"] and item["area"] and item["unit_price"]:
                yield item

        if page != "pg5":
            next_page = int(page[2]) + 1
            next_url = "https://bj.lianjia.com/ershoufang/{}/pg{}/".format(distinct, next_page)
            yield scrapy.Request(next_url, callback=self.parse)

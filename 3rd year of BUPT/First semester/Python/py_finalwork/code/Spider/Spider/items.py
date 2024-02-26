# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class LianjiaItem(scrapy.Item):
    # 楼盘名称、总价、平米数、单价、朝向、区域
    name = scrapy.Field()
    district = scrapy.Field()
    total_price = scrapy.Field()
    area = scrapy.Field()
    price_per_m2 = scrapy.Field()
    direction = scrapy.Field()
    layout = scrapy.Field()


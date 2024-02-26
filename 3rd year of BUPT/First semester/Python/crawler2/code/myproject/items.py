# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class MyprojectItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    pass

class MyItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    school = scrapy.Field()
    link = scrapy.Field()

class XuetangxItem(scrapy.Item):
    # 课程名称、教师、学校、学生人数
    course_name = scrapy.Field()
    teacher = scrapy.Field()
    school = scrapy.Field()
    student_num = scrapy.Field()


class LianjiaItem(scrapy.Item):
    # 楼盘名称、总价、平米数、单价
    name = scrapy.Field()
    price = scrapy.Field()
    area = scrapy.Field()
    unit_price = scrapy.Field()

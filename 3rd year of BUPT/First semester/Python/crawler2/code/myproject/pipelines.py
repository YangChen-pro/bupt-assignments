# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html


# useful for handling different item types with a single interface
from itemadapter import ItemAdapter


class MyprojectPipeline:
    def process_item(self, item, spider):
        return item

import json

class MyPipeline:
    def open_spider(self, spider):
        try:
            self.file = open('MyData.json', 'w', encoding='utf-8')
        except Exception as err:
            print(err)

    def process_item(self, item, spider):
        dict_item = dict(item)
        json_str = json.dumps(dict_item, ensure_ascii=False) + '\n'
        self.file.write(json_str)
        return item

    def close_spider(self, spider):
        self.file.close()


import csv

class XuetangxPipeline:
    def __init__(self):
        self.file = open('XuetangxData.csv', 'w+', encoding='utf-8', newline='')
        self.writer = csv.writer(self.file)

    def process_item(self, item, spider):
        self.writer.writerow([item['course_name'], item['teacher'], item['school'], item['student_num']])
        return item

    def close_spider(self, spider):
        self.file.close()


class LianjiaPipeline:
    def __init__(self):
        self.file = open('LianjiaData.json', 'w+', encoding='utf-8')
        self.writer = csv.writer(self.file)

    def process_item(self, item, spider):
        self.writer.writerow([item['name'], item['price'], item['area'], item['unit_price']])
        return item

    def close_spider(self, spider):
        self.file.close()

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

class LianjiaPipeline:
    def __init__(self):
        # self.file = open("BeijingHouseInfo_2023.json", "w+", encoding="utf-8")
        # self.file = open("ShanghaiHouseInfo_2023.json", "w+", encoding="utf-8")
        # self.file = open("GuangzhouHouseInfo_2023.json", "w+", encoding="utf-8")
        # self.file = open("ShenzhenHouseInfo_2023.json", "w+", encoding="utf-8")
        # self.file = open("TaiyuanHouseInfo_2023.json", "a+", encoding="utf-8")

    def process_item(self, item, spider):
        dict_item = dict(item)
        json_str = json.dumps(dict_item, ensure_ascii=False) + "\n"
        self.file.write(json_str)
        return item

    def close_spider(self, spider):
        self.file.close()

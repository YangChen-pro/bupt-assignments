from lxml import etree

# 读取HTML文件内容
with open("homepage.html", "r", encoding="utf-8") as file:
    html_string = file.read()

# 创建HTML解析器
parser = etree.HTMLParser()

# 解析HTML文档
tree = etree.fromstring(html_string, parser)

# 使用XPath查找元素
h1_element = tree.xpath("//h1")[0]
p_elements = tree.xpath("//p")
ul_element = tree.xpath("//ul")[0]
li_elements = tree.xpath("//li")
a_element = tree.xpath("//a")[0]

# 输出查找到的元素文本内容
print("h1元素文本内容:", h1_element.text)
print("p元素文本内容:")
for p in p_elements:
    print("  -", p.text)
print("ul元素文本内容:")
for li in li_elements:
    print("  -", li.text)
print("a元素链接地址:", a_element.attrib["href"])

from lxml import etree
import requests

# 发送HTTP GET请求获取页面内容，并指定编码为UTF-8
url = "https://www.bupt.edu.cn/yxjg1.htm"
response = requests.get(url)
response.encoding = "utf-8"  # 指定编码为UTF-8
html_string = response.text


# 将HTML字符串传递给etree.HTML()方法创建一个可解析的HTML树
html_tree = etree.HTML(html_string)

# 使用XPath表达式定位目标元素
ul_element = html_tree.xpath('//ul[@class="linkPageList"]/li[4]/div/ul')[0]

# 遍历目标 ul 元素下的子元素，提取内容
for li_element in ul_element.xpath(".//li"):
    # 提取院系名称和链接
    a_elements = li_element.xpath(".//a")
    department_name = []
    department_link = []

    # 可能有多个 <a>
    for a_element in a_elements:
        department_name.append(a_element.text.strip())
        department_link.append(a_element.get("href"))

    # 输出院系名称和链接
    for i in range(len(department_name)):
        print("院系名称:", department_name[i])
        print("院系链接:", department_link[i])
        print()

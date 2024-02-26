import pickle

# 显示反序列化后的数据
def showData(file):
    with open(file, 'rb') as f:
        while True:  # 有多个对象，循环读取
            try:
                data = pickle.load(f)
                print(data)
            except EOFError:
                break
# 修改内容
def modify(input_file, output_file):
    with open(input_file, 'rb') as f:
         data1 = pickle.load(f)
         data2 = pickle.load(f)
    data2[1]['score']['语文'] = 100
    data2[1]['name'] = '李四'

    # 序列化回文件
    with open(output_file, 'wb') as f:
        pickle.dump(data1, f)
        pickle.dump(data2, f)

if __name__ == '__main__':
    print("反序列化后的数据：")
    showData("object.dat")
    modify("object.dat", "modified_object.dat")
    print("修改后的数据：")
    showData("modified_object.dat")
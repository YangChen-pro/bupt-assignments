#include <iostream>
#include <ctime>
#include <cstdlib>
#include <stdexcept>
#include <limits>

using namespace std;

// 商品类
class Product
{
public:
    Product();
    int getPrice() const;
private:
    int price;
};

// 商品类构造函数
Product::Product()
{
    srand(time(NULL)); // 初始化随机数种子
    price = rand() % 1000 + 1; // 随机生成价格
}

// 获取商品价格
int Product::getPrice() const
{
    return price;
}

// 猜价游戏类
class GuessPriceGame
{
public:
    GuessPriceGame();
    void startGame();
private:
    Product product;
    bool isInputValid(int price) const;
};

// 猜价游戏类构造函数
GuessPriceGame::GuessPriceGame()
{
    product = Product();
}

// 判断输入是否合法
bool GuessPriceGame::isInputValid(int price) const
{
    return price >= 1 && price <= 1000;
}

// 开始游戏
void GuessPriceGame::startGame()
{
    int userGuess;
    int productPrice = product.getPrice();

    cout << "猜猜商品价格是多少？" << endl;

    do
    {
        cout << "请输入您猜测的商品价格（1-1000）：";
        cin >> userGuess;
        if (cin.fail()) // 捕获输入流错误
        {
            cin.clear(); // 清除输入流的状态标志
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略输入缓冲区中的所有字符
            throw invalid_argument("Error: 输入必须是一个整数！");
        }
        if (!isInputValid(userGuess))
        {
            cout << "输入不合法，请重新输入！" << endl;
            continue;
        }
        else
        {
            if (userGuess > productPrice)
            {
                cout << "您猜的价格太高了！" << endl;
            }
            else if (userGuess < productPrice)
            {
                cout << "您猜的价格太低了！" << endl;
            }
            else
            {
                cout << "恭喜您，猜对了！" << endl;
            }
        }
    } while (userGuess != productPrice);
    cout << "游戏结束！" << endl;
}

int main()
{
    GuessPriceGame game = GuessPriceGame();
    try {
        game.startGame();
    } catch (const invalid_argument& e) { // 捕获异常
        cerr << "异常信息：" << e.what() << endl;
    }
    return 0;
}
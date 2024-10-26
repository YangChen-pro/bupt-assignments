# client

### requirements

```angular2html
python==3.10
django==5.0.6
```

### notes

- models.py -- 操作数据库
- views.py -- url对应的函数

- settings.py -- 配置文件
- urls.py -- url和函数的对应关系

打开终端，运行`python manage.py migrate`，进行数据库迁移  
再运行`python manage.py runserver`启动服务器

在`url.py`界面设置或添加你的页面  
并在`model.py`界面创建一个视图函数来渲染你的html文件
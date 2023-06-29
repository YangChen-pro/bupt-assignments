#include "navigate.h"

extern bool collison[N];        // 记录hash是否发生冲突
extern  path dis1[N][N];        // 记录任意两点之间的距离
extern  path dis2[N][N];        // 记录可采用骑车方式的最短距离
extern  int tabu[N][N];         // 禁忌表
extern  int tabulen[N][N];      // 禁忌期，最大值为5
NavigateWindow::NavigateWindow(logTXT*logtxt,QWidget* parent) : QWidget(parent)
{
    gh = new Graph();
    gh->n->hide();
    this->logtxt=logtxt;

    mainLayout = new QVBoxLayout(this);

    mode_1 = new QPushButton(this);
    mode_1->setText("下个活动导航");

    mode_4 = new QLabel(this);
    mode_4->setText("导航结果");

    qbutton_0 = new QPushButton(this);
    qbutton_0->setText("设为步行模式");
    qbutton_1 = new QPushButton(this);
    qbutton_1->setText("步行+骑车混合模式");



    mainLayout->addWidget(mode_1);


//    mainLayout->addLayout(mode2Layout);
    //mainLayout->addWidget(mode_3);
    mainLayout->addWidget(qbutton_0);
    mainLayout->addWidget(qbutton_1);
    mainLayout->addWidget(mode_4);

//    mainLayout->addLayout(mode3Layout);

    connect(qbutton_0,&QPushButton::clicked,this,[=]{
        mode=0;
    });
    connect(qbutton_1,&QPushButton::clicked,this,[=]{
        mode=1;
    });
    //this->gh = new Graph();
    connect(mode_1, &QPushButton::clicked,this,[=](){

            vector<CLOCK_TIME::alarm> alm_to_gh;
            int size = 0;

            int weeknum = this->logtxt->clk_time->getWeekNum()+1;
            int week = this->logtxt->clk_time->getWeekDay();
            int hour = this->logtxt->clk_time->getHour();
            int lasthour = -1;

            for(int a = 0;a<this->logtxt->clk_time->alarm_list.size();a++)
            {
                if(this->logtxt->clk_time->alarm_list[a].weekNum==weeknum&&this->logtxt->clk_time->alarm_list[a].week==week&&
                    this->logtxt->clk_time->alarm_list[a].hour>hour)
                {
                if(size>0&&this->logtxt->clk_time->alarm_list[a].hour==lasthour)
                {
                    alm_to_gh.push_back(this->logtxt->clk_time->alarm_list[a]);
                    size++;
                }else if(size==0)
                {
                    alm_to_gh.push_back(this->logtxt->clk_time->alarm_list[a]);
                    size++;lasthour=this->logtxt->clk_time->alarm_list[a].hour;
                }
                else
                    break;
                }

                if((this->logtxt->clk_time->alarm_list[a].weekNum==weeknum&&this->logtxt->clk_time->alarm_list[a].week>week)
                    ||this->logtxt->clk_time->alarm_list[a].weekNum>weeknum)
                break;
            }
//            if(gh!=NULL)
//            {
//                delete this->gh;
//                this->gh=NULL;
//            }
//            qDebug()<<alm_to_gh.size();
//            if(alm_to_gh.size()>0)
//            {
//                this->gh = new Graph();
//            }
            gh->n->delete_lines();
            gh->n->show();
            qDebug()<<alm_to_gh.size()+10;
            //qDebug()<<this->logtxt->clk_time->now_location;
            if(alm_to_gh.size()>1&&alm_to_gh.size()<=10)
            {
                vector<string> dst;
                for(int i=0;i<alm_to_gh.size();i++)
                    dst.push_back(alm_to_gh[i].place_link);

                for(int i=0;i<dst.size();i++)
                    this->logtxt->log("导航：下一个活动，到"+dst[i]);
                this->logtxt->log("导航：下一个活动，到"+alm_to_gh[0].place_link);
                this->logtxt->log("当前位置："+this->logtxt->clk_time->now_location);
                this->logtxt->log("当前mode："+to_string(mode));
                this->gh->show_dfs(this->logtxt->clk_time->now_location,dst,mode);

                mode_4->setText("结果已显示");
            }
            else if(alm_to_gh.size()>10)
            {
                vector<string> dst;
                for(int i=0;i<alm_to_gh.size();i++)
                    dst.push_back(alm_to_gh[i].place_link);


                this->gh->show_tabu(this->logtxt->clk_time->now_location,dst,mode);
                mode_4->setText("结果已显示");

                for(int i=0;i<dst.size();i++)
                this->logtxt->log("导航：下一个活动，到"+dst[i]);

                //this->logtxt->clk_time->now_location = dst[dst.size()-1];
            }
            else if(alm_to_gh.size()==1)
            {
                this->logtxt->log("导航：下一个活动，到"+alm_to_gh[0].place_link);
                this->logtxt->log("当前位置："+this->logtxt->clk_time->now_location);
                this->logtxt->log("当前mode："+to_string(mode));
                this->gh->show_dijk(this->logtxt->clk_time->now_location,alm_to_gh[0].place_link,mode);
                mode_4->setText("结果已显示");

                this->logtxt->clk_time->now_location = alm_to_gh[0].place_link;
            }
            else {
                mode_4->setText("今日已没有活动");
                this->logtxt->log("导航：下一个活动，今日已无活动");
            }

    });


}

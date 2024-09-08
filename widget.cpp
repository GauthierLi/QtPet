#include "widget.h"
#include <QPaintEvent>
#include <QPainter> //画图需要
#include <QPixmap> //图片
#include <QCursor>
#include <QMetaEnum>
#include <QApplication>

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    timer(new QTimer(this)),
    menu(new QMenu(this))
{
    this->setWindowFlag(Qt::FramelessWindowHint); // 去除窗口边框
    this->setAttribute(Qt::WA_TranslucentBackground); //窗口背景透明
    this->installEventFilter(new DragFilter);

    connect(timer, &QTimer::timeout, [this](){
        static int index=0; //记录显示动作的当前图片索引
        auto paths = this->action_map.value(this->cur_role_act);
        this->curl_role_pix=paths[index++%paths.size()];
        this->update();
    });

    initMenu();
    loadRoleActRes();
    showAnimation(RoleAct::dagun);
}

Widget::~Widget() {}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // painter.drawRect(100, 100, 200, 200);
    QPixmap pix;
    pix.load(this->curl_role_pix.toLocalFile());
    painter.drawPixmap(0, 0, pix);
}

void Widget::contextMenuEvent(QContextMenuEvent *event)
{
    this->menu->popup(QCursor::pos());
}

void Widget::showAnimation(RoleAct k)
{
    timer->stop();
    this->cur_role_act=k;
    if (k == RoleAct::dagun){
        timer->start(80);
    } else {
        timer->start(15);
    }
}

void Widget::onMenuTriggered(QAction *action)
{
    QMetaEnum me=QMetaEnum::fromType<RoleAct>();
    bool ok;
    int k = me.keyToValue(action->text().toStdString().c_str(), &ok);
    if (!ok) return;
    showAnimation(static_cast<RoleAct>(k));
}

void Widget::loadRoleActRes()
{
    auto addRes=[this](RoleAct k, QString path, int count)
    {
        QList<QUrl> paths;
        char buf[260];
        for (int i = 0; i < count; ++i) {
            memset(buf, 0, sizeof(buf));
            sprintf_s(buf, path.toStdString().c_str(), i);
            paths.append(QUrl::fromLocalFile(buf));
        }
        action_map.insert(k, paths);
    };

    addRes(RoleAct::pa, ":/mmc/imgs/mmc/output_frame_%d.png", 24);
    addRes(RoleAct::dagun, ":/smn/imgs/smn/output_frame_%d.png", 4);
}

void Widget::initMenu()
{
    menu->addAction("dagun");
    menu->addAction("pa");


    QAction* exitAct = new QAction("Exit");
    connect(exitAct, &QAction::triggered, [&](){
        QApplication::quit();
    });

    QAction* act=new QAction("Hide");
    connect(act, &QAction::triggered, [this](){
        this->setVisible(false);
    });

    menu->addAction(act);
    menu->addAction(exitAct);
    connect(this->menu, &QMenu::triggered, this, &Widget::onMenuTriggered);
}

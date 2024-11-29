#ifndef WIDGET_H
#define WIDGET_H



#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void fileLoad1();
    void fileLoad2();
    void copy();

private:
    void initConnect();
    Ui::Widget *ui;
};
#endif // WIDGET_H

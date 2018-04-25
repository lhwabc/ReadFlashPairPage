#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define MAX_BLOCKUNIT     (1024)
#define MAX_PLANE         (4)
#define MAX_PAGE          (2048)
#define PAIR_PAGE_PATTERN_NUM (20)
#define LOWPAGEINDEX      (0)
#define HIGHPAGEINDEX     (1)
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void InitStyle(void);

private:
    Ui::MainWindow *ui;
private slots:
    void Openfile(void);
    void SaveFile(void);
    void GetTable(void);

};

#endif // MAINWINDOW_H

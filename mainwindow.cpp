#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
QString PrePareFile;
QString SaveFilename;
int Sharepage[MAX_BLOCKUNIT][MAX_PLANE][2];
int g_TotalRowNum;
int g_CellPerRow;
int g_LowPageIndex[MAX_PAGE];
int g_HighPageIndex[MAX_PAGE];
int g_LowPageNum=0;
int g_HighPageNum=0;
unsigned char g_PairPageTable[MAX_PAGE/8];
int g_HighLowPagePattern[PAIR_PAGE_PATTERN_NUM/2][2];
int g_HighLowPatternNum=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->InitStyle();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitStyle()
{
    connect(ui->OpenBtn, SIGNAL(clicked(bool)), this, SLOT(Openfile()));
    connect(ui->GetLowPageBtn, SIGNAL(clicked(bool)), this, SLOT(GetTable()));
}

void MainWindow::Openfile()
{
    QFileDialog *fd=new QFileDialog(this);
    fd->setWindowTitle("Open File");
    fd->setAcceptMode(QFileDialog::AcceptOpen);
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setViewMode(QFileDialog::Detail);
    fd->setGeometry(10,30,300,200);
    if(fd->exec()==QFileDialog::Accepted) // ok
    {
        PrePareFile=fd->selectedFiles()[0];
        ui->filepath_Edit->setText(PrePareFile);
    }
}

void MainWindow::SaveFile()
{
    QFileDialog *fd=new QFileDialog(this);
    fd->setAcceptMode(QFileDialog::AcceptSave);
    fd->setFileMode(QFileDialog::AnyFile);
    fd->setViewMode(QFileDialog::Detail);
    fd->setGeometry(10,30,300,200);

    SaveFilename=fd->getSaveFileName(this, QString::fromLocal8Bit("Save File"),  "outfile", tr("Config Files (*.txt)"));

    if(QFile(SaveFilename).exists())
        QFile(SaveFilename).remove();
}

void Debug_log(char *filename, const char* fmt, ...)
{
    QString filepath;

    //filepath = QCoreApplication::applicationDirPath();
    //filepath += "//";
    //filepath = filename;

    char buf[512*1024];
    char line[2]="\n";
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    FILE *fp = fopen(filename, "a+");
    fwrite(buf, strlen(buf), 1, fp);
    fwrite(line, strlen(line), 1, fp);
    fclose(fp);
}
void trim(char *strIn, char *strOut)
{
    int i, j ;
    i = 0;
    j = strlen(strIn) - 1;
    while(strIn[i] == ' ')
        ++i;

    while(strIn[j] == ' ')
        --j;
    strncpy(strOut, strIn + i , j - i + 1);
    strOut[j - i + 1] = '\0';
}
void bubble_sort(int *array,int num)
{
        int i = 0;
        int j = 0;
        int temp;
        for(;j < num;++j)
        {
                for(i= num;i >j ;--i)
                {
                    if(array[i] < array[i-1])
                    {
                        temp =array[i];
                        array[i] = array[i-1];
                        array[i-1] = temp;
                    }
                }
        }
}
int isdigitstr(char *str)
{
    return (strspn(str, "0123456789")==strlen(str));
}
void MainWindow::GetTable(void)
{
    SaveFile();
    QFile file(PrePareFile);
    int iRowNum=0;
    if(file.exists())
    {
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug("Can't open the file!");
            return;
        }
        while(!file.atEnd())
        {
            QString line = file.readLine().simplified();

            QStringList list=line.split(" ");
            if((list.count() != 4) && (list.count() != 8))
                return;

            g_CellPerRow =list.count()/2;
            for(int iCellNo=0;iCellNo<g_CellPerRow;iCellNo++)
            {
                for(int i=0;i<2;i++)
                {
                    if(!isdigitstr(list.at(iCellNo*2+i).toLatin1().data()))
                        Sharepage[iRowNum][iCellNo][i] =int(-1);
                    else
                        Sharepage[iRowNum][iCellNo][i] = list.at(iCellNo*2+i).toInt();
                }
            }
            iRowNum++;
        }
        g_TotalRowNum = iRowNum;
    }
    else
    {
        qDebug("file no exists");
    }
    Debug_log(SaveFilename.toLatin1().data(),"Table Information.");
    Debug_log(SaveFilename.toLatin1().data(),"g_TotalRowNum:%04d",g_TotalRowNum);
    Debug_log(SaveFilename.toLatin1().data(),"g_CellPerRow:%04d",g_CellPerRow);

    char string[1024];
    char temp[1024];
    memset(string, 0, 1024);

#ifndef wLowPageIndex
    for(int iRowNum=0;iRowNum<g_TotalRowNum;iRowNum++)
    {
        memset(string, 0, 1024);
        for(int iCellNo=0;iCellNo<g_CellPerRow;iCellNo++)
        {
            g_LowPageIndex[g_LowPageNum++] = Sharepage[iRowNum][iCellNo][LOWPAGEINDEX];
            g_HighPageIndex[g_HighPageNum++] = Sharepage[iRowNum][iCellNo][HIGHPAGEINDEX];
            //sprintf(temp," %d ",Sharepage[blockunit][iPlane][i]);
            //strcat(string, temp);
        }
        //qDebug(string);
    }
    bubble_sort(g_LowPageIndex,g_LowPageNum-1);
    memset(string, 0, 1024);

    Debug_log(SaveFilename.toLatin1().data(),"JM_UINT16 wLowPageIndex[SYS_PAIR_PAGE_NUM] =");
    Debug_log(SaveFilename.toLatin1().data(),"{");
    for(int i=0;i<g_LowPageNum;i++)
    {
        sprintf(temp,"  0x%04X,",g_LowPageIndex[i]);
        strcat(string, temp);
        if((i+1)%10==0)
        {
            Debug_log(SaveFilename.toLatin1().data(),string);
            memset(string, 0, 1024);
        }
    }
    Debug_log(SaveFilename.toLatin1().data(),"};");
#endif
#ifndef bPairPageTable
    memset(g_PairPageTable, 0x00, sizeof(g_PairPageTable));
    for(int iRowNo=0;iRowNo<g_TotalRowNum;iRowNo++)
    {
        for(int iCellNo=0;iCellNo<g_CellPerRow;iCellNo++)//1:HighPage 0:LowPage
        {
            if(Sharepage[iRowNo][iCellNo][HIGHPAGEINDEX] !=(-1))
            {
                int iByte=0;
                char cBit=0;
                iByte = Sharepage[iRowNo][iCellNo][HIGHPAGEINDEX] /8;
                cBit  = Sharepage[iRowNo][iCellNo][HIGHPAGEINDEX] %8;
                g_PairPageTable[iByte] |= (1<<cBit);
            }
        }
    }
    int TotalByte=(g_TotalRowNum*g_CellPerRow*2)/8;
    if((g_TotalRowNum*g_CellPerRow*2)%8 != 0)
        TotalByte++;
    //qDebug("TotalByte:%d",TotalByte);
    memset(string, 0, 1024);
    Debug_log(SaveFilename.toLatin1().data(),"JM_UINT8 bPairPageTable[SYS_PAIR_PAGE_NUM] =");
    Debug_log(SaveFilename.toLatin1().data(),"{");
    for(int i=0;i<TotalByte;i++)
    {
        sprintf(temp,"  0x%02X,",g_PairPageTable[i]);
        strcat(string, temp);
        if((i+1)%8==0)
        {
            Debug_log(SaveFilename.toLatin1().data(),string);
            memset(string, 0, 1024);
        }
    }
    Debug_log(SaveFilename.toLatin1().data(),"};");
#endif
#ifndef dwPairPageTestPattern
    int HighLowPitch=0;
    for(int pagenum=0;pagenum<g_LowPageNum;pagenum++)
    {
        if(g_HighPageIndex[pagenum] ==(-1))
            continue;
        if(HighLowPitch !=(g_HighPageIndex[pagenum] -g_LowPageIndex[pagenum]))
        {
            g_HighLowPagePattern[g_HighLowPatternNum][LOWPAGEINDEX] = g_LowPageIndex[pagenum];
            g_HighLowPagePattern[g_HighLowPatternNum][HIGHPAGEINDEX] = g_HighPageIndex[pagenum];
            g_HighLowPatternNum++;
            HighLowPitch = g_HighPageIndex[pagenum] -g_LowPageIndex[pagenum];
        }
    }
    Debug_log(SaveFilename.toLatin1().data(),"JM_UINT16 dwPairPageTestPattern[GDP_PAIR_PAGE_PATTERN_NUM] =");
    Debug_log(SaveFilename.toLatin1().data(),"{");
    for(int i=0;i<g_HighLowPatternNum;i++)
    {
        sprintf(string,"  0x%04X, 0x%04X,",g_HighLowPagePattern[i][LOWPAGEINDEX], g_HighLowPagePattern[i][HIGHPAGEINDEX]);
        Debug_log(SaveFilename.toLatin1().data(), string);
    }
    Debug_log(SaveFilename.toLatin1().data(),"};");
#endif
}




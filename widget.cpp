#include "widget.h"
#include "./ui_widget.h"

#include "constants.h"
#include "tools.h"
#include "copysignature.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->initConnect();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::fileLoad1()
{
    ui->ReceiverPath->clear();
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("open a file."),
        ".",
        tr(FILE_FORMAT)
        );
    if (!filepath.isEmpty()) {
        ui->ReceiverPath->setPlainText(filepath);
    }
}


void Widget::fileLoad2()
{
    ui->SendPath->clear();
    QString filepath = QFileDialog::getOpenFileName(
        this,
        tr("open a file."),
        ".",
        tr(FILE_FORMAT)
        );
    if (!filepath.isEmpty()) {
        ui->SendPath->setPlainText(filepath);
    }
}

void Widget::copy() {
    if (!ui->SendPath->toPlainText().isEmpty() && !ui->ReceiverPath->toPlainText().isEmpty()) {
        // QString to LPCWSTR
        QString SendPath = ui->SendPath->toPlainText();
        std::wstring wlpstr = SendPath.toStdWString();
        LPCWSTR SigFileName = wlpstr.c_str();

        DWORD sigfileSize = 0;
        PCHAR signBuffer = tools::loadFile(SigFileName, &sigfileSize);

        QString ReceiverPath = ui->ReceiverPath->toPlainText();
        wlpstr = ReceiverPath.toStdWString();
        LPCWSTR noSigFileName = wlpstr.c_str();

        DWORD nosigfileSize = 0;
        PCHAR nosignBuffer = tools::loadFile(noSigFileName, &nosigfileSize);

        // check Buffer
        if (!signBuffer || !nosignBuffer) {
            QMessageBox::critical(this, "Error", "Cannot load File");
            ui->SendPath->clear();
            ui->ReceiverPath->clear();
            return;
        }

        // call Copy
        if (tools::CheckFileType(signBuffer) && tools::CheckFileType(nosignBuffer)) {
            CopySignature* csign = new CopySignature(nosignBuffer, signBuffer);
            csign->NoSigBuffer.bufferSize = nosigfileSize;
            if (csign->Copy()) {
                if (tools::SaveFile(noSigFileName, csign->NewBuffer.pBuffer, csign->NewBuffer.bufferSize)) {
                    printf("Success\r\n");
                }
            }
        }

        tools::FreeMem(nosignBuffer);
        tools::FreeMem(signBuffer);
    }

}

void Widget::initConnect() {
    connect(ui->fileLoad1, SIGNAL(clicked()), this, SLOT(fileLoad1()));
    connect(ui->fileLoad2, SIGNAL(clicked()), this, SLOT(fileLoad2()));
    connect(ui->copySign, SIGNAL(clicked()), this, SLOT(copy()));
}

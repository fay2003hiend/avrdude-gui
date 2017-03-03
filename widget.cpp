/**
    avrdude front-end, widget.cpp

    @author Fay Zheng <fay2003hiend@gmail.com>
    @version 1.0 March 03 2017
*/

#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QProcess>
#include <QSettings>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

ZProcess::ZProcess(QObject *parent)
    : QProcess(parent)
{
    connect(this, SIGNAL(readyReadStandardOutput()), SLOT(slot_read_stdout()));
    connect(this, SIGNAL(readyReadStandardError()), SLOT(slot_read_stderr()));
}

void ZProcess::slot_read_stdout()
{
    std_out_data.append(readAllStandardOutput());
}

void ZProcess::slot_read_stderr()
{
    std_err_data.append(readAllStandardError());
}

int ZProcess::execute(const QString &command)
{
    start(command, QProcess::ReadOnly);

    if (!waitForStarted())
    {
        return -1;
    }

    if (!waitForFinished())
    {
        return -2;
    }

    return exitCode();
}

bool extract_asset(const QString& path_r, const QString& path_w)
{
    if (QFile::exists(path_w))
    {
        return true;
    }

    QFile fr(path_r);
    QFile fw(path_w);
    bool ret = false;

    if (fr.open(QIODevice::ReadOnly)) {
        if(fw.open(QIODevice::WriteOnly))
        {
            fw.write(fr.readAll());
            fw.close();
            ret = true;
        }
        fr.close();
    }
    return ret;
}



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(windowTitle() + tr(" -- Fay Zheng <fay2003hiend@gmail.com>"));

    connect(ui->pushButton_calc, SIGNAL(clicked(bool)), SLOT(slot_calc()));
    connect(ui->pushButton_cmd, SIGNAL(clicked(bool)), SLOT(slot_cmd()));
    connect(ui->pushButton_read, SIGNAL(clicked(bool)), SLOT(slot_read()));

    connect(ui->checkBox_do_fuse, SIGNAL(toggled(bool)), SLOT(slot_do_fuse(bool)));
    connect(ui->checkBox_hfuse, SIGNAL(toggled(bool)), SLOT(slot_hfuse(bool)));
    connect(ui->checkBox_efuse, SIGNAL(toggled(bool)), SLOT(slot_efuse(bool)));

    QTimer::singleShot(10, this, SLOT(slot_init()));
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void Widget::do_cmd(const QString &path)
{
    QString fuses;
    QString update;
    fromUI();

    if(do_fuse)
    {
        fuses = QString(" -U lfuse:w:0x%1:m").arg(last_lfuse);

        if (has_hfuse)
        {
            fuses += QString("-U hfuse:w:0x%1:m").arg(last_hfuse);
        }
        if (has_efuse)
        {
            fuses += QString("-U efuse:w:0x%1:m").arg(last_efuse);
        }
    }

    if (!path.isEmpty()) {
        update = QString(" -U flash:w:\"%1\":i").arg(path);
    }
    QString ret = QString("avrdude.exe -s -C avrdude.conf -c %1 -p %2 ")
            .arg(last_programmer, last_device)
            + fuses
            + update
            + " " + last_arguments;

    if (QMessageBox::question(this, tr("Execute"), ret,
                              QMessageBox::Yes, QMessageBox::No)
            == QMessageBox::Yes) {
        ZProcess p;
        p.execute(ret);
        ui->textBrowser->append("----");
        ui->textBrowser->append(p.std_out_data);
        ui->textBrowser->append("----");
        ui->textBrowser->append(p.std_err_data);
    }
}

void Widget::dropEvent(QDropEvent *event)
{
    QUrl url = event->mimeData()->urls().at(0);
    QString path = QDir::toNativeSeparators(url.toLocalFile());
    if(!path.endsWith(".hex")) {
        event->ignore();
        return;
    }

    last_path = path;
    do_cmd(path);
}

void Widget::slot_do_fuse(bool checked)
{
    ui->widget_fuse->setEnabled(checked);
}

void Widget::slot_hfuse(bool checked)
{
    ui->lineEdit_hfuse->setEnabled(checked);
}

void Widget::slot_efuse(bool checked)
{
    ui->lineEdit_efuse->setEnabled(checked);
}

#define SAVE_STR(x) conf.setValue(#x, x)
#define SAVE_BOOL(x) conf.setValue(#x, x)

Widget::~Widget()
{
    fromUI();
    QSettings conf("Qavrdude.ini", QSettings::IniFormat);
    SAVE_STR(last_programmer);
    SAVE_STR(last_device);
    SAVE_STR(last_arguments);
    SAVE_STR(last_hfuse);
    SAVE_STR(last_lfuse);
    SAVE_STR(last_efuse);
    SAVE_BOOL(do_fuse);
    SAVE_BOOL(has_hfuse);
    SAVE_BOOL(has_efuse);
    conf.sync();

    delete ui;
}

void Widget::fromUI()
{
    last_programmer = ui->comboBox_programmer->currentText();
    last_device = ui->comboBox_device->currentText();
    last_arguments = ui->lineEdit_arguments->text();

    last_hfuse = ui->lineEdit_hfuse->text().toLower();
    last_lfuse = ui->lineEdit_lfuse->text().toLower();
    last_efuse = ui->lineEdit_efuse->text().toLower();

    do_fuse = ui->checkBox_do_fuse->isChecked();
    has_hfuse = ui->checkBox_hfuse->isChecked();
    has_efuse = ui->checkBox_efuse->isChecked();
}

void Widget::toUI()
{
    ui->lineEdit_arguments->setText(last_arguments);
    ui->lineEdit_hfuse->setText(last_hfuse);
    ui->lineEdit_lfuse->setText(last_lfuse);
    ui->lineEdit_efuse->setText(last_efuse);

    ui->checkBox_do_fuse->setChecked(do_fuse);
    ui->widget_fuse->setEnabled(do_fuse);

    ui->checkBox_hfuse->setChecked(has_hfuse);
    ui->lineEdit_hfuse->setEnabled(has_hfuse);

    ui->checkBox_efuse->setChecked(has_efuse);
    ui->lineEdit_efuse->setEnabled(has_efuse);
}

bool Widget::fill_combobox(const QString &prog, QComboBox *box, const QString& matching_d)
{
    ZProcess p;
    if (p.execute(prog) < 0)
    {
        return false;
    }

    QList<QByteArray> lines = p.std_err_data.split('\n');
    QString str, key, value;
    int i = 0;
    int found = -1;
    int n;
    foreach(const QByteArray& line, lines)
    {
        str = QString::fromLocal8Bit(line);
        if ((n = str.indexOf(" = ")) == -1)
        {
            continue;
        }

        key = str.left(n).simplified();
        value = str.mid(n+3).simplified();

        box->addItem(QString("%1 - %2").arg(key, value), key);
        if (found == -1 && key == matching_d)
        {
            found = i;
            box->setCurrentIndex(i);
        }
        ++i;
    }
    return true;
}

#define READ_STR(x) x = conf.value(#x).toString()
#define READ_BOOL(x) x = conf.value(#x).toBool()

void Widget::slot_init()
{
    if (!extract_asset(":/bin/avrdude.exe", "avrdude.exe")
            || !extract_asset(":/bin/avrdude.conf", "avrdude.conf"))
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("avrdude binaries can't be extracted!"),
                             QMessageBox::Ok);
        qApp->quit();
    }

    QSettings conf("Qavrdude.ini", QSettings::IniFormat);
    READ_STR(last_programmer);
    READ_STR(last_device);
    READ_STR(last_arguments);
    READ_STR(last_hfuse);
    READ_STR(last_lfuse);
    READ_STR(last_efuse);
    READ_BOOL(do_fuse);
    READ_BOOL(has_efuse);

    fill_combobox("avrdude.exe -c?", ui->comboBox_programmer, last_programmer);
    fill_combobox("avrdude.exe -p?", ui->comboBox_device, last_device);
    toUI();

    this->setAcceptDrops(true);
}

void Widget::slot_read()
{
    ZProcess p;
    last_programmer = ui->comboBox_programmer->currentText();
    last_device = ui->comboBox_device->currentText();
    last_arguments = ui->lineEdit_arguments->text();

    p.execute(QString("avrdude.exe -s -C avrdude.conf -c %1 -p %2 ")
              .arg(last_programmer, last_device)
              + last_arguments);
    ui->textBrowser->append(p.std_out_data);
    ui->textBrowser->append("--------------");
    ui->textBrowser->append(p.std_err_data);

    QString str = QString::fromLocal8Bit(p.std_err_data);
    int n;
    QRegExp expr("Fuses OK \\(E:[0-9A-F][0-9A-F], H:[0-9A-F][0-9A-F], L:[0-9A-F][0-9A-F]\\)");
    if ((n = str.indexOf(expr)) != -1)
    {
        ui->lineEdit_efuse->setText(str.mid(n + 12, 2));
        ui->lineEdit_hfuse->setText(str.mid(n + 18, 2));
        ui->lineEdit_lfuse->setText(str.mid(n + 24, 2));
    }
}

void Widget::slot_cmd()
{
    do_cmd(last_path);
}

void Widget::slot_calc()
{
    QDesktopServices::openUrl(QUrl("http://www.engbedded.com/fusecalc/"));
}
